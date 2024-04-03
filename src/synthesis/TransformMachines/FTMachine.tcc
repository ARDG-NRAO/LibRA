//# FTMachine.tcc //For templated functions of FTMachine class 
//# Copyright (C) 2015-2019
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify
//# it under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or
//# (at your option) any later version.
//#
//# This library is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//# 
//# You should have received a copy of the GNU General Public License
//# along with this library; if not, write to the Free Software
//# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

 template <class T> void FTMachine::getGrid(casacore::Array<T>& thegrid){

    thegrid.resize();
    void * ptr = nullptr;
    casacore::Array<casacore::Float> tmpFloat;
    casacore::Array<casacore::Double> tmpDouble;
    casacore::IPosition shp;
    bool del;
    if((whatType(&thegrid)==casacore::TpArrayComplex)){
	if(useDoubleGrid_p){
	  griddedData.resize(griddedData2.shape());
	  convertArray(griddedData, griddedData2);
	  griddedData2.resize();
	  useDoubleGrid_p=false;
	}
	 ptr=griddedData.getStorage(del);
	 shp=griddedData.shape();
    }
    else if(whatType(&thegrid)==casacore::TpArrayDComplex){
      if(!useDoubleGrid_p){
	griddedData2.resize(griddedData.shape());
	convertArray(griddedData2, griddedData);
	griddedData.resize();
	useDoubleGrid_p=true;
      }
      ptr=griddedData2.getStorage(del);
      shp=griddedData2.shape();
	

    }
    else if(((whatType(&thegrid)==casacore::TpArrayFloat))){
      if(!useDoubleGrid_p){
	tmpFloat.resize(griddedData.shape());
	
      }
      else{
	tmpFloat.resize(griddedData2.shape());
	griddedData.resize(griddedData2.shape());
	convertArray(griddedData, griddedData2);
	griddedData2.resize();
	useDoubleGrid_p=false;
	//convertArray(tmpFloat, real(griddedData2));
      }


      tmpFloat.set(0.0);
      shp=tmpFloat.shape();
      Int end_indx = 0;
      //cerr << "getgrid "  << sum(griddedData) << endl;
      casacore::IPosition in(4, 0, 0, 0, 0);
      casacore::IPosition out(4, 0,0,0,0);

      tmpFloat = real(griddedData);
      //cerr << "tmpFloat "  << sum(tmpFloat) << endl;
      for (casacore::Int cc=0; cc< shp[3]; ++cc){
	in[3]=cc;
	out[3]=cc;
	for(casacore::Int pp=0; pp< shp[2]; ++pp){
	  in[2]=pp;
	  out[2]=pp;

	  for (casacore::Int yy=-shp[1]/2; yy< shp[1]-shp[1]/2; ++yy){

	    for(casacore::Int xx=-shp[0]/2; xx< shp[0]-shp[0]/2; ++xx){
	      in[0]=xx + shp[0]/2;
	      in[1]=yy + shp[1]/2;
	      out[0]=-xx + shp[0]/2;
	      out[1]=-yy + shp[1]/2;
	      
	      if((out[0] < shp[0]) && (out[1] < shp[1])) 
		{
		  tmpFloat(in)+=real(griddedData(out));
		}
	    }

	  }
	}
      }
       //cerr << "getgrid "  << sum(griddedData) << endl;
      //tmpFloat = real(griddedData);
      //cerr << "tmpFloat " << sum(tmpFloat) << endl;
      ptr=tmpFloat.getStorage(del);
      //ptr=griddedData.getStorage(del);
    }
    else if(((whatType(&thegrid)==casacore::TpArrayDouble))){
       if(useDoubleGrid_p){
	 tmpDouble.resize(griddedData2.shape());	
       }
       else{
	 tmpDouble.resize(griddedData.shape());
	 griddedData2.resize(griddedData.shape());
	 convertArray(griddedData2, griddedData);
	 griddedData.resize();
	 useDoubleGrid_p=true;
	 //convertArray(tmpFloat, real(griddedData2));
       }
      
      tmpDouble=real(griddedData2);
      shp=tmpDouble.shape();
      casacore::IPosition in(4, 0, 0, 0, 0);
      casacore::IPosition out(4, 0,0,0,0);

      for (casacore::Int cc=0; cc< shp[3]; ++cc){
	in[3]=cc;
	out[3]=cc;
	for(casacore::Int pp=0; pp< shp[2]; ++pp){
	  in[2]=pp;
	  out[2]=pp;

	  for (casacore::Int yy=-shp[1]/2; yy< shp[1]-shp[1]/2; ++yy){

	    for(casacore::Int xx=-shp[0]/2; xx< shp[0]-shp[0]/2; ++xx){
	      in[0]=xx + shp[0]/2;
	      in[1]=yy + shp[1]/2;
	      out[0]=-xx + shp[0]/2;
	      out[1]=-yy + shp[1]/2;
	      
	      if((out[0] < shp[0]) && (out[1] < shp[1])) 
		{
		  tmpDouble(in)+=real(griddedData(out));
		}
	    }

	  }
	}
      }
      ptr=tmpDouble.getStorage(del);


    }  
    thegrid=casacore::Array<T>(shp, (T*)(ptr));
    griddedData.resize();
    griddedData2.resize();
  };
