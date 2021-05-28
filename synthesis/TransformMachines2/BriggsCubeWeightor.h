//# BriggsCubeWeight.h: Definition for Briggs weighting for cubes
//# Copyright (C) 2018-2019
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
//# You should have received a copy of the GNU  General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
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

#ifndef SYNTHESIS_TRANSFORM2_BRIGGSCUBEWEIGHTOR_H
#define SYNTHESIS_TRANSFORM2_BRIGGSCUBEWEIGHTOR_H

#include <synthesis/TransformMachines2/GridFT.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

namespace casacore{
  class RecordInterface;
}
namespace casa{ //# namespace casa

 namespace vi{ class VisBuffer2;
                  class VisibilityIterator2;
  }
namespace refim{ //#	 namespace for refactored imaging code with vi2/vb2
class BriggsCubeWeightor{
  public:

   BriggsCubeWeightor();
   BriggsCubeWeightor(vi::VisibilityIterator2& vi, const casacore::String& rmode,
		       const casacore::Quantity& noise, const casacore::Double robust,
		       const casacore::ImageInterface<casacore::Complex>& templateimage,
                      const casacore::RecordInterface& inrec, const casacore::Double& fracBW,
		       const casacore::Int superUniformBox=0,
		      const casacore::Bool multiField=false);
   //This constructor will require that init be called at a later stage
   BriggsCubeWeightor(const casacore::String& rmode,
		       const casacore::Quantity& noise,
		      const casacore::Double robust, const casacore::Double& fracBW, const
		      casacore::Int superUniformBox=0,
		      const casacore::Bool multiField=false);
    
    void weightUniform(casacore::Matrix<casacore::Float>& imweight, const vi::VisBuffer2& vb);
    //initialize
    // inRec is a record interface from FTMachine that is used to set the state of the weight gridder ftmachines
    //contains frame info etc.
    void init(vi::VisibilityIterator2& vi,const casacore::ImageInterface<casacore::Complex>& templateimage, const casacore::RecordInterface& inRec);
    casacore::String initImgWeightCol(vi::VisibilityIterator2& vi,const casacore::ImageInterface<casacore::Complex>& templateimage, const casacore::RecordInterface& inRec);
	
  private:
    void initializeFTMachine(const casacore::uInt index, const casacore::ImageInterface<casacore::Complex>& templateimage, const casacore::RecordInterface& inrec);
    void cube2Matrix(const casacore::Cube<casacore::Bool>& fcube, casacore::Matrix<casacore::Bool>& fMat);

    casacore::String makeScratchImagingWeightTable(casacore::CountedPtr<casacore::Table>& weightTable, const casacore::String& postfix);
	void getWeightUniform(const casacore::Array<casacore::Float>& wgtDensity, casacore::Matrix<casacore::Float>& imweight, const vi::VisBuffer2& vb);
	void readWeightColumn(casacore::Matrix<casacore::Float>& imweight, const vi::VisBuffer2& vb);
  casacore::Int estimateSwingChanPad(vi::VisibilityIterator2& vi, const casacore::CoordinateSystem& cs, const casacore::Int imNChan, const casacore::String& ephemtab="");

    casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > > grids_p;
    
    casacore::Block<casacore::CountedPtr<refim::GridFT> >  ft_p;
    std::map<casacore::String, casacore::Int> multiFieldMap_p;
    casacore::Block<casacore::Vector<casacore::Float> > f2_p, d2_p;
    casacore::Float uscale_p, vscale_p;
    casacore::Int uorigin_p, vorigin_p;
    casacore::Int nx_p, ny_p;
    casacore::String rmode_p;
    casacore::Quantity noise_p;
    casacore::Double robust_p;
    casacore::Int superUniformBox_p;
    casacore::Bool multiField_p;
    casacore::Bool initialized_p;
    casacore::Double refFreq_p;
    casacore::InterpolateArray1D<casacore::Double,casacore::Complex>::InterpolationMethod freqInterpMethod_p;
    casacore::Bool freqFrameValid_p;
    VisImagingWeight visWgt_p;
    
    casacore::Double fracBW_p;
    casacore::CountedPtr<casacore::Table> wgtTab_p;
    std::map<std::pair<casacore::uInt, casacore::rownr_t>, casacore::rownr_t> vbrowms2wgtrow_p;
    casacore::String imWgtColName_p;
 };
   }//# end namespace refim
} // end namespace casa


#endif


