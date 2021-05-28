//# VisImagingWeight.h: Calculate Imaging Weights for a buffer from weight
//# Copyright (C) 2009-2018
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//# License for more details.
//#
//# You should have received a copy of the GNU  General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef VISIMAGINGWEIGHT_H
#define VISIMAGINGWEIGHT_H
#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta/Quantum.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

template<class T> class ImageInterface;
template<class T> class TempLattice;

}

namespace casa { //# NAMESPACE CASA - BEGIN
//#forward
class ROVisibilityIterator;

// <summary>
// Object to hold type of imaging weight scheme to be used on the fly and to provide
// facilities to do that.
// </summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// </todo>

 class VisImagingWeight {
    public:
     //empty constructor
     VisImagingWeight();
     //Constructor to calculate natural and radial weights
     VisImagingWeight(const casacore::String& type);
     //Constructor to calculate uniform weight schemes; include Brigg's and super/uniform
     //If multiField=true, the weight density calcution is done on a per field basis, 
     //else it is all fields combined
     VisImagingWeight(ROVisibilityIterator& vi, const casacore::String& rmode, const casacore::Quantity& noise,
                               const casacore::Double robust, const casacore::Int nx, const casacore::Int ny,
                               const casacore::Quantity& cellx, const casacore::Quantity& celly,
		      const casacore::Int uBox, const casacore::Int vBox, const casacore::Bool multiField=false);
     //Constructor for uniform style weighting when the weight density is calculated 
     //elsewhere
     VisImagingWeight(ROVisibilityIterator& vi, casacore::Block<casacore::CountedPtr<casacore::TempLattice<casacore::Float> > >& grids, const casacore::String& rmode, const casacore::Quantity& noise,
                                     const casacore::Double robust, const casacore::Quantity& cellx, const casacore::Quantity& celly,
		      const casacore::Bool multiField=false) ;
     //VisibilityIterator2 version of the above....
     // Note the VisibilityIterator can be readonly...thus recommended if you can
     // as that will prevent unnecessary locks
     VisImagingWeight(vi::VisibilityIterator2& vi, const casacore::String& rmode, const casacore::Quantity& noise,
                               const casacore::Double robust, const casacore::Int nx, const casacore::Int ny,
                               const casacore::Quantity& cellx, const casacore::Quantity& celly,
		      const casacore::Int uBox, const casacore::Int vBox, const casacore::Bool multiField=false);
     ///Construct uniform weight density from save grid on an image
     VisImagingWeight(casacore::ImageInterface<casacore::Float>& densityImages) ;
     virtual ~VisImagingWeight();


     // reference semantically = operator
     VisImagingWeight& operator=(const VisImagingWeight& imwgt);

     //you have to make sure the imageinterface passed has the shape of gwt
     //on the first 2 axex and last axis
     virtual void toImageInterface(casacore::ImageInterface<casacore::Float>& im);
     // casacore::Function to calculate the  uniform style weights, include Brigg's for example
     // imagingWeight should be sized by (nchan, row) already
     // The fieldid and msid parameters must correspond to what VisBuffer  or VisIter fieldId() and msId() returns
     virtual void weightUniform(casacore::Matrix<casacore::Float>& imagingWeight, const casacore::Matrix<casacore::Bool>& flag, const casacore::Matrix<casacore::Double>& uvw,
                                const casacore::Vector<casacore::Double>& frequency, const casacore::Matrix<casacore::Float>& weight, const casacore::Int msid, const casacore::Int fieldid ) const;

     //Natural weighting scheme
     //imagingWeight should be sized by (nchan, row) already
     virtual void weightNatural(casacore::Matrix<casacore::Float>& imagingWeight, const casacore::Matrix<casacore::Bool>& flag,
                                const casacore::Matrix<casacore::Float>& weight) const;

   /*  unused version?
     //weight as casacore::Matrix version
     virtual void weightNatural(casacore::Matrix<casacore::Float>& imagingWeight, const casacore::Matrix<casacore::Bool>& flag,
                                const casacore::Matrix<casacore::Float>& weight) const;

   */
     //Radial weighting
     //imagingWeight should be sized by (nchan, row) already
     virtual void weightRadial(casacore::Matrix<casacore::Float>& imagingWeight, const casacore::Matrix<casacore::Bool>& flag,
                               const casacore::Matrix<casacore::Double>& uvw, const casacore::Vector<casacore::Double>& frequency,
                               const casacore::Matrix<casacore::Float>& weight) const;

     //Get the type of weighting this object is on..will return one of "natural", "uniform", "radial"
     virtual casacore::String getType() const;
     
     //setting uv filtering
     virtual void setFilter(const casacore::String& type, const casacore::Quantity& bmaj,
			    const casacore::Quantity& bmin, const casacore::Quantity& bpa);

     //returns if uv filtering is set
     virtual casacore::Bool doFilter() const;

     //do uvfiltering...to be called after imaging weight is calculated
     virtual void filter(casacore::Matrix<casacore::Float>& imWeight, const casacore::Matrix<casacore::Bool>& flag, 
			 const casacore::Matrix<casacore::Double>& uvw,
			 const casacore::Vector<casacore::Double>& frequency, const casacore::Matrix<casacore::Float>& weight) const;

     // This is to  get/set uniform style weight density...e.g if the caller wants to
     // add densities from different pieces of data distributed via different 
     // VisibilityIterators
     virtual casacore::Bool getWeightDensity (casacore::Block<casacore::Matrix<casacore::Float> >& density);
     virtual void setWeightDensity(const casacore::Block<casacore::Matrix<casacore::Float> >& density);

     // Form corr-indep weight by averaging parallel-hand weights
     static void unPolChanWeight(casacore::Matrix<casacore::Float>& chanRowWt, const casacore::Cube<casacore::Float>& corrChanRowWt);

     casacore::Vector<casacore::Int> shapeOfdensityGrid();
     
    private:
     void cube2Matrix(const casacore::Cube<casacore::Bool>& fcube, casacore::Matrix<casacore::Bool>& fMat);

     std::map<casacore::String, casacore::Int> multiFieldMap_p;
     casacore::Block<casacore::CountedPtr<casacore::TempLattice<casacore::Float> > > gwt_p;
     mutable casacore::Matrix<casacore::Float>  a_gwt_p;
     casacore::String wgtType_p;
     casacore::Float uscale_p, vscale_p;
     casacore::Vector<casacore::Float> f2_p, d2_p;
     casacore::Int uorigin_p, vorigin_p;
     casacore::Int nx_p, ny_p;
     casacore::Bool doFilter_p;
     casacore::Double cospa_p;
     casacore::Double sinpa_p;
     casacore::Double rbmaj_p;
     casacore::Double rbmin_p;
     casacore::Double robust_p;
     casacore::String rmode_p;
     casacore::Quantity noise_p;
     mutable casacore::Int activeFieldIndex_p;


 };
} //# NAMESPACE CASA - END
#endif // VISIMAGINGWEIGHT_H
