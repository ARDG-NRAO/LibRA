//# VisImagingWeight.cc: imaging weight calculation for a give buffer
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
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  General Public
//# License for more details.
//#
//# You should have received a copy of the GNU General Public License
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
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <casa/Quanta/MVAngle.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <lattices/Lattices/TempLattice.h>
#include <images/Images/ImageInterface.h>
#include<coordinates/Coordinates/DirectionCoordinate.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


  VisImagingWeight::VisImagingWeight() : wgtType_p("none"), doFilter_p(false), robust_p(0.0), rmode_p("norm"), noise_p(Quantity(0.0, "Jy")) , activeFieldIndex_p(-1){

    }

  VisImagingWeight::VisImagingWeight(const String& type) : doFilter_p(false),  robust_p(0.0), rmode_p("norm"), noise_p(Quantity(0.0, "Jy")) , activeFieldIndex_p(-1){
  

        wgtType_p=type;
        wgtType_p.downcase();
        if (wgtType_p != "natural" && wgtType_p != "radial"){

            throw(AipsError("Programmer error...wrong constructor used"));
        }
  
    }


  VisImagingWeight::VisImagingWeight(ROVisibilityIterator& vi, const String& rmode, const Quantity& noise,
                                     const Double robust, const Int nx, const Int ny,
                                     const Quantity& cellx, const Quantity& celly,
                                     const Int uBox, const Int vBox, const Bool multiField) : doFilter_p(false), robust_p(robust), rmode_p(rmode), noise_p(noise), activeFieldIndex_p(-1) {

      LogIO os(LogOrigin("VisSetUtil", "VisImagingWeight()", WHERE));

      VisBufferAutoPtr vb (vi);
      wgtType_p="uniform";
      // Float uscale, vscale;
      //Int uorigin, vorigin;
      Vector<Double> deltas;
      uscale_p=(nx*cellx.get("rad").getValue());
      vscale_p=(ny*celly.get("rad").getValue());
      uorigin_p=nx/2;
      vorigin_p=ny/2;
      nx_p=nx;
      ny_p=ny;
      // Simply declare a big matrix
      //Matrix<Float> gwt(nx,ny);
      gwt_p.resize(1);
      multiFieldMap_p.clear();
      vi.originChunks();
      vi.origin();
      
      // Detect usable WEIGHT_SPECTRUM
      Bool doWtSp=(vb->weightSpectrum().nelements()>0);

      String mapid=String::toString(vb->msId())+String("_")+String::toString(vb->fieldId());
      multiFieldMap_p.insert(std::pair<casacore::String, casacore::Int>(mapid, 0));
      gwt_p[0]=new TempLattice<Float>(IPosition(2, nx,ny), 0);
      gwt_p[0]->set(0.0);
      a_gwt_p.resize(nx_p, ny_p);
      a_gwt_p.set(0.0);

      Int fields=0;
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
          for (vi.origin();vi.more();vi++) {
        
              if(vb->newFieldId()){
                  mapid=String::toString(vb->msId())+String("_")+String::toString(vb->fieldId());
                  if(multiField){
                      if( multiFieldMap_p.find(mapid) == multiFieldMap_p.end( ) ){
                          fields+=1;
                          gwt_p.resize(fields+1);
                          gwt_p[fields]=new TempLattice<Float>(IPosition(2, nx_p,ny_p),0);
                          gwt_p[fields]->set(0.0);
                      }
                  }

                  if( multiFieldMap_p.find(mapid) == multiFieldMap_p.end( ) )
                      multiFieldMap_p.insert(std::pair<casacore::String, casacore::Int>(mapid, fields));
              }
          }
      }

      Float u, v;
      Vector<Double> sumwt(fields+1,0.0);
      f2_p.resize(fields+1);
      d2_p.resize(fields+1);
      Int fid=0;
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
          for (vi.origin();vi.more();vi++) {
        if(vb->newFieldId()){
                  mapid=String::toString(vb->msId())+String("_")+String::toString(vb->fieldId());

              
          if(multiField){
            if(activeFieldIndex_p >=0)
              gwt_p[activeFieldIndex_p]->put(a_gwt_p);
          }
          activeFieldIndex_p=multiFieldMap_p.find(mapid) !=multiFieldMap_p.end( ) ? multiFieldMap_p[mapid] : -1;
          if(multiField && activeFieldIndex_p >=0)
            gwt_p[activeFieldIndex_p]->get(a_gwt_p);
          
        }
            auto mapvp = multiFieldMap_p.find(mapid);
            fid= mapvp != multiFieldMap_p.end( ) ? mapvp->second : -1;
              Int nRow=vb->nRow();
              Int nChan=vb->nChannel();

          // Extract weights correctly (use WEIGHT_SPECTRUM, if available)
          Matrix<Float> wtm;
          ////////////////
          doWtSp=(vb->weightSpectrum().nelements()>0);
          //////////////
          if (doWtSp)
        // WS available,
        unPolChanWeight(wtm,vb->weightSpectrum());               // collapse corr axis (parallel-hand average)
          else
        // WS UNavailable
        wtm.reference(vb->weight().reform(IPosition(2,1,nRow))); // use vb.weight() (corr-collapsed, w/ 1 channel)

          // Use this to mod the channel indexing below
          Int nChanWt=wtm.shape()(0);

              for (Int row=0; row<nRow; row++) {
                  for (Int chn=0; chn<nChan; chn++) {
                      if(!vb->flag()(chn,row)) {
              Float currwt=wtm(chn%nChanWt,row);  // the weight for this chan,row
                          Float f=vb->frequency()(chn)/C::c;
                          u=vb->uvw()(row)(0)*f;
                          v=vb->uvw()(row)(1)*f;
                          Int ucell=Int(uscale_p*u+uorigin_p);
                          Int vcell=Int(vscale_p*v+vorigin_p);
             
                          if(((ucell-uBox)>0)&&((ucell+uBox)<nx)&&((vcell-vBox)>0)&&((vcell+vBox)<ny)) {
                              for (Int iv=-vBox;iv<=vBox;iv++) {
                                  for (Int iu=-uBox;iu<=uBox;iu++) {
                      a_gwt_p(ucell+iu,vcell+iv)+=currwt;
                                      sumwt[fid]+=currwt;
                                  }
                              }
                          }
                          ucell=Int(-uscale_p*u+uorigin_p);
                          vcell=Int(-vscale_p*v+vorigin_p);
                          if(((ucell-uBox)>0)&&((ucell+uBox)<nx)&&((vcell-vBox)>0)&&((vcell+vBox)<ny)) {
                              for (Int iv=-vBox;iv<=vBox;iv++) {
                                  for (Int iu=-uBox;iu<=uBox;iu++) {
                                      a_gwt_p(ucell+iu,vcell+iv)+=currwt;
                                      sumwt[fid]+=currwt;
                                  }
                              }
                          }
                      }
                  }
              }
          }
      }
      // make sure last active plane is saved
      gwt_p[activeFieldIndex_p]->put(a_gwt_p);
      // We use the approximation that all statistical weights are equal to
      // calculate the average summed weights (over visibilities, not bins!)
      // This is simply to try an ensure that the normalization of the robustness
      // parameter is similar to that of the ungridded case, but it doesn't have
      // to be exact, since any given case will require some experimentation.

      //Float f2, d2;
      for(fid=0; fid < Int(gwt_p.nelements()); ++fid){
    gwt_p[fid]->get(a_gwt_p);
    activeFieldIndex_p=fid;
    if (rmode=="norm") {
              os << "Normal robustness, robust = " << robust << LogIO::POST;
              Double sumlocwt = 0.;
              for(Int vgrid=0;vgrid<ny;vgrid++) {
                  for(Int ugrid=0;ugrid<nx;ugrid++) {
                      if(a_gwt_p(ugrid, vgrid)>0.0) sumlocwt+=square(a_gwt_p(ugrid,vgrid));
                  }
              }
              f2_p[fid] = square(5.0*pow(10.0,Double(-robust))) / (sumlocwt / sumwt[fid]);
              d2_p[fid] = 1.0;

          }
          else if (rmode=="abs") {
              os << "Absolute robustness, robust = " << robust << ", noise = "
                      << noise.get("Jy").getValue() << "Jy" << LogIO::POST;
              f2_p[fid] = square(robust);
              d2_p[fid] = 2.0 * square(noise.get("Jy").getValue());

          }
          else {
              f2_p[fid] = 1.0;
              d2_p[fid] = 0.0;
          }
      }

  }

  VisImagingWeight::VisImagingWeight(ROVisibilityIterator& vi, Block<CountedPtr<TempLattice<Float> > >& grids, const String& rmode, const Quantity& noise,
                                     const Double robust, const Quantity& cellx, const Quantity& celly,
                                     const Bool multiField) : doFilter_p(false), robust_p(robust), rmode_p(rmode), noise_p(noise) {

   LogIO os(LogOrigin("VisSetUtil", "VisImagingWeight()", WHERE));

      VisBufferAutoPtr vb (vi);
      wgtType_p="uniform";
      gwt_p.resize(grids.nelements(), true, false);
      for (uInt k =0; k < gwt_p.nelements(); ++k)
    gwt_p[k]=grids[k];
      nx_p=gwt_p[0]->shape()[0];
      ny_p=gwt_p[0]->shape()[1];
      uscale_p=(nx_p*cellx.get("rad").getValue());
      vscale_p=(ny_p*celly.get("rad").getValue());
      uorigin_p=nx_p/2;
      vorigin_p=ny_p/2;
      
      multiFieldMap_p.clear();
      Int fields=0;
      String mapid;
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    for (vi.origin();vi.more();vi++) {
      if(vb->newFieldId()){
        mapid=String::toString(vb->msId())+String("_")+String::toString(vb->fieldId());
        if(multiField){
          if( multiFieldMap_p.find(mapid) == multiFieldMap_p.end( ) ){
        fields+=1;
                   
          }
        }
        if( multiFieldMap_p.find(mapid) == multiFieldMap_p.end( ) )
          multiFieldMap_p.insert(std::pair<casacore::String, casacore::Int>(mapid, fields));
      }
    }
      }
      f2_p.resize(gwt_p.nelements());
      d2_p.resize(gwt_p.nelements());
      for(uInt fid=0; fid < gwt_p.nelements(); ++fid){
    activeFieldIndex_p=fid;
    gwt_p[fid]->get(a_gwt_p);
    if (rmode_p=="norm") {
      os << "Normal robustness, robust = " << robust_p << LogIO::POST;
      Double sumlocwt = 0.;
      for(Int vgrid=0;vgrid<ny_p;vgrid++) {
        for(Int ugrid=0;ugrid<nx_p;ugrid++) {
          if(a_gwt_p(ugrid, vgrid)>0.0) sumlocwt+=square(a_gwt_p(ugrid,vgrid));
        }
      }
      Double sumwt_fid=sum(a_gwt_p);
      f2_p[fid] = square(5.0*pow(10.0,Double(-robust_p))) / (sumlocwt / sumwt_fid);
      d2_p[fid] = 1.0;

    }
          else if (rmode=="abs") {
              os << "Absolute robustness, robust = " << robust_p << ", noise = "
                      << noise_p.get("Jy").getValue() << "Jy" << LogIO::POST;
              f2_p[fid] = square(robust_p);
              d2_p[fid] = 2.0 * square(noise_p.get("Jy").getValue());

          }
          else {
              f2_p[fid] = 1.0;
              d2_p[fid] = 0.0;
          }
      }
}

  VisImagingWeight::VisImagingWeight(vi::VisibilityIterator2& visIter,
                     const String& rmode, const Quantity& noise,
                                     const Double robust, const Int nx, const Int ny,
                                     const Quantity& cellx, const Quantity& celly,
                                     const Int uBox, const Int vBox, const Bool multiField) : doFilter_p(false), robust_p(robust), rmode_p(rmode), noise_p(noise), activeFieldIndex_p(-1) {

      LogIO os(LogOrigin("VisSetUtil", "VisImagingWeight()", WHERE));



      vi::VisBuffer2* vb=(visIter.getVisBuffer());
      wgtType_p="uniform";
      // Float uscale, vscale;
      //Int uorigin, vorigin;
      Vector<Double> deltas;
      uscale_p=(nx*cellx.get("rad").getValue());
      vscale_p=(ny*celly.get("rad").getValue());
      uorigin_p=nx/2;
      vorigin_p=ny/2;
      nx_p=nx;
      ny_p=ny;
      // Simply declare a big matrix
      //Matrix<Float> gwt(nx,ny);
      gwt_p.resize(1);
      multiFieldMap_p.clear();
      visIter.originChunks();
      visIter.origin();
      String mapid=String::toString(vb->msId())+String("_")+String::toString(vb->fieldId()[0]);

      multiFieldMap_p.insert( std::pair<casacore::String, casacore::Int>(mapid, 0) );
      gwt_p[0]=new TempLattice<Float>(IPosition(2,nx_p, ny_p), 0);
      gwt_p[0]->set(0.0);
      a_gwt_p.resize(nx_p, ny_p);
      a_gwt_p.set(0.0);


      // Discover if weightSpectrum non-trivially available
      Bool doWtSp=visIter.weightSpectrumExists();

      Int fields=0;
      for (visIter.originChunks();visIter.moreChunks();visIter.nextChunk()) {
          for (visIter.origin();visIter.more();visIter.next()) {
              if(vb->isNewFieldId()){
                  mapid=String::toString(vb->msId())+String("_")+String::toString(vb->fieldId()[0]);
                  if(multiField){
                      if( multiFieldMap_p.find(mapid) == multiFieldMap_p.end( ) ){
                          fields+=1;
                          gwt_p.resize(fields+1);
                          gwt_p[fields]=new TempLattice<Float>(IPosition(2,nx_p, ny_p), 0);
                          gwt_p[fields]->set(0.0);
                      }
                  }
                  if( multiFieldMap_p.find(mapid) == multiFieldMap_p.end( ) )
                      multiFieldMap_p.insert(std::pair<casacore::String, casacore::Int>(mapid, fields));
              }
          }
      }

      Float u, v;
      Vector<Double> sumwt(fields+1,0.0);
      f2_p.resize(fields+1);
      d2_p.resize(fields+1);
      Int fid=0;
      for (visIter.originChunks();visIter.moreChunks();visIter.nextChunk()) {
          for (visIter.origin();visIter.more();visIter.next()) {
        if(vb->isNewFieldId()){
                  mapid=String::toString(vb->msId())+String("_")+String::toString(vb->fieldId()[0]);

          if(multiField){
            if(activeFieldIndex_p >=0)
              gwt_p[activeFieldIndex_p]->put(a_gwt_p);
          }
          activeFieldIndex_p= multiFieldMap_p.find(mapid) != multiFieldMap_p.end( ) ? multiFieldMap_p[mapid] :  -1;
          if(multiField && activeFieldIndex_p >=0)
            gwt_p[activeFieldIndex_p]->get(a_gwt_p);
        }
           
              auto mapvp = multiFieldMap_p.find(mapid);
              fid= mapvp != multiFieldMap_p.end( ) ? mapvp->second : -1;
              Int nRow=vb->nRows();
              Int nChan=vb->nChannels();

          // Extract weights correctly (use WEIGHT_SPECTRUM, if available)
          Matrix<Float> wtm;
          Cube<Float> wtc;
          if (doWtSp)
        // WS available [nCorr,nChan,nRow]
        wtc.reference(vb->weightSpectrum());
          else {
        Int nCorr=vb->nCorrelations();
        // WS UNavailable weight()[nCorr,nRow] --> [nCorr,nChan,nRow]
            wtc.reference(vb->weight().reform(IPosition(3,nCorr,1,nRow)));  // unchan'd weight as single-chan
          }
          unPolChanWeight(wtm,wtc);   // Collapse on corr axis

          // Used for mod of chn index below
          Int nChanWt=wtm.shape()(0);

          //Oww !!! temporary implementation of old vb.flag just to see if things work
          Matrix<Bool> flag;
          cube2Matrix(vb->flagCube(), flag);
              for (Int row=0; row<nRow; row++) {
                  for (Int chn=0; chn<nChan; chn++) {
                      
            Float currwt=wtm(chn%nChanWt,row);  // the weight for this chan,row
            
                      if(!flag(chn,row)) {
                          Float f=vb->getFrequency(row, chn)/C::c;
                          u=vb->uvw()(0,row)*f;
                          v=vb->uvw()(1,row)*f;
                          Int ucell=Int(uscale_p*u+uorigin_p);
                          Int vcell=Int(vscale_p*v+vorigin_p);
                          if(((ucell-uBox)>0)&&((ucell+uBox)<nx)&&((vcell-vBox)>0)&&((vcell+vBox)<ny)) {
                              for (Int iv=-vBox;iv<=vBox;iv++) {
                                  for (Int iu=-uBox;iu<=uBox;iu++) {
                      a_gwt_p(ucell+iu,vcell+iv)+=currwt;
                                      sumwt[fid]+=currwt;
                                  }
                              }
                          }
                          ucell=Int(-uscale_p*u+uorigin_p);
                          vcell=Int(-vscale_p*v+vorigin_p);
                          if(((ucell-uBox)>0)&&((ucell+uBox)<nx)&&((vcell-vBox)>0)&&((vcell+vBox)<ny)) {
                              for (Int iv=-vBox;iv<=vBox;iv++) {
                                  for (Int iu=-uBox;iu<=uBox;iu++) {
                                      a_gwt_p(ucell+iu,vcell+iv)+=currwt;
                                      sumwt[fid]+=currwt;
                                  }
                              }
                          }
                      }
                  }
              }
          }
      }

      // make sure last active plane is saved
      gwt_p[activeFieldIndex_p]->put(a_gwt_p);
      // We use the approximation that all statistical weights are equal to
      // calculate the average summed weights (over visibilities, not bins!)
      // This is simply to try an ensure that the normalization of the robustness
      // parameter is similar to that of the ungridded case, but it doesn't have
      // to be exact, since any given case will require some experimentation.

      //Float f2, d2;
      for(fid=0; fid < Int(gwt_p.nelements()); ++fid){
    gwt_p[fid]->get(a_gwt_p);
    activeFieldIndex_p=fid;
    if (rmode=="norm") {
              os << "Normal robustness, robust = " << robust << LogIO::POST;
              Double sumlocwt = 0.;
              for(Int vgrid=0;vgrid<ny;vgrid++) {
                  for(Int ugrid=0;ugrid<nx;ugrid++) {
                      if(a_gwt_p(ugrid, vgrid)>0.0) sumlocwt+=square(a_gwt_p(ugrid,vgrid));
                  }
              }
              f2_p[fid] = square(5.0*pow(10.0,Double(-robust))) / (sumlocwt / sumwt[fid]);
              d2_p[fid] = 1.0;

          }
          else if (rmode=="abs") {
              os << "Absolute robustness, robust = " << robust << ", noise = "
                      << noise.get("Jy").getValue() << "Jy" << LogIO::POST;
              f2_p[fid] = square(robust);
              d2_p[fid] = 2.0 * square(noise.get("Jy").getValue());

          }
          else {
              f2_p[fid] = 1.0;
              d2_p[fid] = 0.0;
          }
      }
  }


  VisImagingWeight::VisImagingWeight(ImageInterface<Float>& im) :  robust_p(0.0), rmode_p(""), noise_p(Quantity(0.0, "Jy")) {

      LogIO os(LogOrigin("VisSetUtil", "VisImagingWeight()", WHERE));

      doFilter_p=False;

      wgtType_p="uniform";
      nx_p=im.shape()(0);
      ny_p=im.shape()(1);
      DirectionCoordinate dc=im.coordinates().directionCoordinate(0);
      dc.setWorldAxisUnits(Vector<String>(2, "rad"));
      Double cellx=dc.increment()(0);
      Double celly=dc.increment()(1);
      uscale_p=nx_p*cellx;
      vscale_p=ny_p*celly;
      uorigin_p=nx_p/2;
      vorigin_p=ny_p/2;
      //Now to recover from image density and other parameters
      Int nplanes=1;
      if(im.shape().nelements()==5)
    nplanes=im.shape()[4];
      gwt_p.resize(nplanes, True, False);
      if(im.shape().nelements()==5){
      IPosition blc(Vector<Int>(5,0));
      for (Int fid=0;fid<nplanes;fid++)
        {
          gwt_p[fid]=new TempLattice<Float>(IPosition(2,nx_p, ny_p), 0);
          Array<Float> lala;
          blc[4]=fid;
          im.getSlice(lala, blc, IPosition(5, nx_p, ny_p,1,1,1), True);
          gwt_p[fid]->put( lala.reform(IPosition(2, nx_p, ny_p)));

        }
    }
    else{
      Array<Float> lala;
      im.get(lala, True);
      gwt_p[0]=new TempLattice<Float>(IPosition(2,nx_p, ny_p), 0);
      gwt_p[0]->put( lala.reform(IPosition(2, nx_p, ny_p)));

    }
      const TableRecord& rec=im.miscInfo();
      if(rec.isDefined("d2")){
    d2_p.resize();
    rec.get("d2", d2_p);
    f2_p.resize();
    rec.get("f2", f2_p);
    multiFieldMap_p.clear();
        Int mapsize;
        rec.get("multimapsize", mapsize);
    for(Int k=0; k < mapsize; ++k){
      String key;
      Int val;
      rec.get("key"+String::toString(k), key);
      rec.get("val"+String::toString(k), val);
          //cerr << "key and id " << key << "   "<< val << endl;
      multiFieldMap_p.insert(std::pair<casacore::String, casacore::Int>(key, val));
    }
    

      }
      activeFieldIndex_p=0;
      gwt_p[0]->get(a_gwt_p);
      if(rec.isDefined("dofilter")){
        rec.get("dofilter", doFilter_p);
        rec.get("rbmaj", rbmaj_p);
        rec.get("rbmin", rbmin_p);
        rec.get("cospa", cospa_p);
        rec.get("sinpa", sinpa_p);
      }

      
  }

  
  VisImagingWeight::~VisImagingWeight(){
    /*for (uInt fid=0; fid < gwt_p.nelements(); ++fid){
          gwt_p[fid].resize();
      }*/
    
  }

    Vector<Int> VisImagingWeight::shapeOfdensityGrid(){
      Vector<Int> retval(3, 0);
      retval(2)=gwt_p.nelements();
      if(retval(2) > 0){
    retval[0]=gwt_p[0]->shape()(0);
    retval[1]=gwt_p[0]->shape()(1);

      }
      
      return retval;
    }
    void VisImagingWeight::toImageInterface(casacore::ImageInterface<casacore::Float>& im){

      if( wgtType_p != "uniform")
    throw(AipsError("cannot save weight density for non-Briggs' weighting schemes"));
      
      IPosition where=    IPosition(im.shape().nelements(),0);
      Int lastAx=where.nelements()-1;
      for (uInt fid=0;fid<gwt_p.nelements(); ++fid){
    activeFieldIndex_p=fid;
    gwt_p[fid]->get(a_gwt_p);
    where[lastAx]=fid;
    im.putSlice(a_gwt_p, where);

      }
      Record rec(im.miscInfo());
      rec.define("d2", d2_p);
      rec.define("f2", f2_p);
      rec.define("numfield", Int(multiFieldMap_p.size()));
      uInt keycount=0;
      for (auto iter = multiFieldMap_p.begin( ); iter != multiFieldMap_p.end( ); ++iter, ++keycount){
    rec.define("key"+String::toString(keycount), iter->first);
    rec.define("val"+String::toString(keycount), iter->second);
      }
      rec.define("multimapsize",keycount);
      if(doFilter_p){
        rec.define("dofilter", doFilter_p);
        rec.define("rbmaj", rbmaj_p);
        rec.define("rbmin", rbmin_p);
        rec.define("cospa", cospa_p);
        rec.define("sinpa", sinpa_p);
      }

      
      im.setMiscInfo(rec);

    }
  void VisImagingWeight::setFilter(const String& type, const Quantity& bmaj,
                   const Quantity& bmin, const Quantity& bpa)
  {

     LogIO os(LogOrigin("VisImagingWeight", "setFilter()", WHERE));

    if (type=="gaussian") {
      
      Bool lambdafilt=false;
      
      if( bmaj.getUnit().contains("lambda"))
    lambdafilt=true;
      if(lambdafilt){
    os << "Filtering for Gaussian of shape: "
       << bmaj.get("klambda").getValue() << " by "
       << bmin.get("klambda").getValue() << " (klambda) at p.a. "
       << bpa.get("deg").getValue() << " (degrees)" << LogIO::POST;
    rbmaj_p=log(2.0)/square(bmaj.get("lambda").getValue());
    rbmin_p=log(2.0)/square(bmin.get("lambda").getValue());
      }
      else{
    os << "Filtering for Gaussian of shape: "
       << bmaj.get("arcsec").getValue() << " by "
       << bmin.get("arcsec").getValue() << " (arcsec) at p.a. "
       << bpa.get("deg").getValue() << " (degrees)" << LogIO::POST;
    
    // Convert to values that we can use
    Double fact = 4.0*log(2.0);
    rbmaj_p = fact*square(bmaj.get("rad").getValue());
    rbmin_p = fact*square(bmin.get("rad").getValue());
      }
      Double rbpa  = MVAngle(bpa).get("rad").getValue();
      cospa_p = sin(rbpa);
      sinpa_p = cos(rbpa);
      doFilter_p=true;

    }
    else {
      os << "Unknown filtering " << type << LogIO::EXCEPTION;
    }
  


  
  }


  Bool VisImagingWeight::doFilter() const{

    return doFilter_p;
  }


  void VisImagingWeight::filter(Matrix<Float>& imWeight, const Matrix<Bool>& flag,
                const Matrix<Double>& uvw,
                const Vector<Double>& frequency, const Matrix<Float>& weight) const{

    // Expecting weight[nchan,nrow], where nchan=1 or nChan (data)
    // If nchan=1 (i.e., WEIGHT_SPECTRUM unavailable), then the
    //  following will be handle the channel degeneracy correctly, using %.


    Int nRow=imWeight.shape()(1);
    Int nChan=imWeight.shape()(0);
    Int nChanWt=weight.shape()(0);
    for (Int row=0; row<nRow; row++) {
      for (Int chn=0; chn<nChan; chn++) {
    Double invLambdaC=frequency(chn)/C::c;
    Double u = uvw(0,row);
    Double v = uvw(1,row);
    if(!flag(chn,row) && (weight(chn%nChanWt,row)>0.0) ) {
      Double ru = invLambdaC*(  cospa_p * u + sinpa_p * v);
      Double rv = invLambdaC*(- sinpa_p * u + cospa_p * v);
      Double filter = exp(-rbmaj_p*square(ru) - rbmin_p*square(rv));
      imWeight(chn,row)*=filter;
    }
    else {
      imWeight(chn,row)=0.0;
    }
      }
    }


  }


    void VisImagingWeight::weightUniform(Matrix<Float>& imWeight, const Matrix<Bool>& flag, const Matrix<Double>& uvw,
                                         const Vector<Double>& frequency,
                                         const Matrix<Float>& weight, const Int msId, const Int fieldId) const{


      //cout << " WEIG " << nx_p << "  " << ny_p << "   " << gwt_p[0].shape() << endl;
      //cout << "f2 " << f2_p[0] << " d2 " << d2_p[0] << " uscale " << uscale_p << " vscal " << vscale_p << " origs " << uorigin_p << "  " << vorigin_p << endl;
      String mapid=String::toString(msId)+String("_")+String::toString(fieldId);
      //cout << "min max gwt " << min(gwt_p[0]) << "    " << max(gwt_p[0]) << " mapid " << mapid <<endl;

      if( multiFieldMap_p.find(mapid) == multiFieldMap_p.end( ) )
    throw(AipsError("Imaging weight calculation is requested for a data that was not selected"));
      
      auto mapvp = multiFieldMap_p.find(mapid);
      Int fid = mapvp != multiFieldMap_p.end( ) ? mapvp->second : -1;
      //Int ndrop=0;
      if(activeFieldIndex_p != fid){
    a_gwt_p=gwt_p[fid]->get();
    activeFieldIndex_p=fid;
      }
      Double sumwt=0.0;
      Int nRow=imWeight.shape()(1);
      Int nChannel=imWeight.shape()(0);
      Int nChanWt=weight.shape()(0);
      Float u, v;
      for (Int row=0; row<nRow; row++) {
    for (Int chn=0; chn<nChannel; chn++) {
      if (!flag(chn,row)) {
        Float f=frequency(chn)/C::c;
        u=uvw(0, row)*f;
        v=uvw(1, row)*f;
        Int ucell=Int(uscale_p*u+uorigin_p);
        Int vcell=Int(vscale_p*v+vorigin_p);
        imWeight(chn,row)=weight(chn%nChanWt,row);
        if((ucell>0)&&(ucell<nx_p)&&(vcell>0)&&(vcell<ny_p) &&a_gwt_p(ucell,vcell)>0.0) {
        imWeight(chn,row)/=a_gwt_p(ucell,vcell)*f2_p[fid]+d2_p[fid];
        sumwt+=imWeight(chn,row);
          
        }
        else {
          imWeight(chn,row)=0.0;
          //ndrop++;
        }
      }
      else{
        imWeight(chn,row)=0.0;
      }
    }
      }
      
    }

  /*  unused version?
void VisImagingWeight::weightNatural(Matrix<Float>& imagingWeight, const Matrix<Bool>& flag,
                                         const Matrix<Float>& weight) const{

      

        Int nRow=imagingWeight.shape()(1);
        Vector<Float> wgtRow(nRow);
    
        for (Int row=0; row<nRow; row++) {
      wgtRow(row)=max(weight.column(row));
        }
    weightNatural(imagingWeight, flag, wgtRow);

    }
  */
    void VisImagingWeight::weightNatural(Matrix<Float>& imagingWeight, const Matrix<Bool>& flag,
                                         const Matrix<Float>& weight) const{

        Double sumwt=0.0;
    //cerr << "shape of weight " << weight.shape() << " max " << max (weight.column(0) ) << " max " << min(weight.column(0)) << " " << weight.column(0).shape() << endl;
        Int nRow=imagingWeight.shape()(1);
        Int nChan=imagingWeight.shape()(0);
        Int nChanWt=weight.shape()(0);
        for (Int row=0; row<nRow; row++) {
            for (Int chn=0; chn<nChan; chn++) {
                if( !flag(chn,row) ) {
             imagingWeight(chn,row)=weight(chn%nChanWt,row);
                    sumwt+=imagingWeight(chn,row);
                }
                else {
                    imagingWeight(chn,row)=0.0;
                }
            }
        }


    }


    void VisImagingWeight::weightRadial(Matrix<Float>& imagingWeight,
                                        const Matrix<Bool>& flag,
                                        const Matrix<Double>& uvw,
                                        const Vector<Double>& frequency,
                                        const Matrix<Float>& weight) const{

        Double sumwt=0.0;
        Int nRow=imagingWeight.shape()(1);
        Int nChan=imagingWeight.shape()(0);
    Int nChanWt=weight.shape()(0);

        for (Int row=0; row<nRow; row++) {
            for (Int chn=0; chn< nChan; chn++) {
                Float f=frequency(chn)/C::c;
                if( !flag(chn,row) ) {
                    imagingWeight(chn,row)=
              f*sqrt(square(uvw(0, row))+square(uvw(1, row)))
                    *weight(chn%nChanWt,row);
                    sumwt+=imagingWeight(chn,row);
                }
                else {
                    imagingWeight(chn,row)=0.0;
                }
            }
        }


    }

    VisImagingWeight& VisImagingWeight::operator=(const VisImagingWeight& other){
        if(this != &other){
      //            gwt_p=other.gwt_p;

        gwt_p.resize(other.gwt_p.nelements(), true, false);
        for (uInt k=0; k < gwt_p.nelements(); ++k){
          //gwt_p[k].resize();
          gwt_p[k]=other.gwt_p[k];
        }


            wgtType_p=other.wgtType_p;
            uscale_p=other.uscale_p;
            vscale_p=other.vscale_p;
        f2_p.resize();
        d2_p.resize();
            f2_p=other.f2_p;
            d2_p=other.d2_p;
            uorigin_p=other.uorigin_p;
            vorigin_p=other.vorigin_p;
            nx_p=other.nx_p;
            ny_p=other.ny_p;
        doFilter_p=other.doFilter_p;
        cospa_p=other.cospa_p;
        sinpa_p=other.sinpa_p;
        rbmaj_p=other.rbmaj_p;
        rbmin_p=other.rbmin_p;
        robust_p=other.robust_p;
        rmode_p=other.rmode_p;
        multiFieldMap_p=other.multiFieldMap_p;
        }
        return *this;
    }

    String VisImagingWeight::getType() const{

        return wgtType_p;

    }
  Bool VisImagingWeight::getWeightDensity (Block<Matrix<Float> >& density){
    if(wgtType_p != "uniform"){
      density.resize(0, true, false);
      return false;
    }
    density.resize(gwt_p.nelements(), true, false);
    for (uInt k=0; k < gwt_p.nelements(); ++k){
      density[k].resize(gwt_p[k]->shape());
      density[k]=gwt_p[k]->get();
    }
    return true;
  }
  void VisImagingWeight::setWeightDensity(const Block<Matrix<Float> >& density){
    if(wgtType_p=="uniform"){
      gwt_p.resize(density.nelements(), true, false);
      for (uInt k=0; k < gwt_p.nelements(); ++k){
    //gwt_p[k].resize();
    gwt_p[k]=new TempLattice<Float>(IPosition(2, density[k].shape()[0],  density[k].shape()[1]),0);
    gwt_p[k]->put(density[k]);
      }
      //break any old reference
      f2_p.resize();
      d2_p.resize();
      f2_p.resize(gwt_p.nelements());
      d2_p.resize(gwt_p.nelements());
      f2_p.set(1.0);
      d2_p.set(0.0);
       //Float f2, d2;
      for(uInt fid=0; fid < gwt_p.nelements(); ++fid){
    activeFieldIndex_p=fid;
    a_gwt_p=gwt_p[fid]->get();
    if (rmode_p=="norm") {
      Double sumlocwt = 0.;
      for(Int vgrid=0;vgrid<a_gwt_p.shape()(1);vgrid++) {
          for(Int ugrid=0;ugrid<a_gwt_p.shape()(0);ugrid++) {
        if(a_gwt_p(ugrid, vgrid)>0.0) sumlocwt+=square(a_gwt_p(ugrid,vgrid));
          }
      }
      Double sumwt_fid=sum(a_gwt_p);
      f2_p[fid] = square(5.0*pow(10.0,Double(-robust_p))) / (sumlocwt / sumwt_fid);
      d2_p[fid] = 1.0;
    }
    else if (rmode_p=="abs") {
      f2_p[fid] = square(robust_p);
      d2_p[fid] = 2.0 * square(noise_p.get("Jy").getValue());
      
    }
    else {
      f2_p[fid] = 1.0;
      d2_p[fid] = 0.0;
    }
      }
      
    }
    
    
  }
  void VisImagingWeight::cube2Matrix(const Cube<Bool>& fcube, Matrix<Bool>& fMat)
  {
      fMat.resize(fcube.shape()[1], fcube.shape()[2]);
      Bool deleteIt1;
      Bool deleteIt2;
      const Bool * pcube = fcube.getStorage (deleteIt1);
      Bool * pflags = fMat.getStorage (deleteIt2);
      for (uInt row = 0; row < fcube.shape()[2]; row++) {
          for (Int chn = 0; chn < fcube.shape()[1]; chn++) {
              *pflags = *pcube++;
              for (Int pol = 1; pol < fcube.shape()[0]; pol++, pcube++) {
                  *pflags = *pcube ? *pcube : *pflags;
              }
              pflags++;
          }
      }
      fcube.freeStorage (pcube, deleteIt1);
      fMat.putStorage (pflags, deleteIt2);
  }


  void VisImagingWeight::unPolChanWeight(Matrix<Float>& chanRowWt, const Cube<Float>& corrChanRowWt)  {

    //cout << "VIW::uPCW" << endl;

    IPosition sh3=corrChanRowWt.shape();
    IPosition sh2=sh3.getLast(2);
    //cerr  << "sh3" <<  sh3 << " sh2 " << sh2 << endl;
    chanRowWt.resize(sh2);
    //cout << "assign..." << endl;
    chanRowWt=corrChanRowWt(Slice(0,1,1),Slice(),Slice()).reform(sh2);
    const Int& nPol=sh3[0];
    if (nPol>1) {
      chanRowWt += corrChanRowWt(Slice(nPol-1,1,1),Slice(),Slice()).reform(sh2);
      chanRowWt /= 2.0f;
    }

    //    cout << "VIW::uPCW" << endl;

  }


}//# NAMESPACE CASA - END


