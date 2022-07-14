//# BeamSkyJones.cc: Implementation for BeamSkyJones
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MSObsColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <casacore/tables/Tables.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/measures/Measures/MeasConvert.h>

#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/ComponentShape.h>

#include <synthesis/TransformMachines2/BeamSkyJones.h>
#include <synthesis/TransformMachines/PBMath.h>

#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Regions/ImageRegion.h>

#include <casacore/casa/Utilities/Assert.h>


using namespace casacore;
namespace casa{


namespace refim{
// temporary, for debugging
/*#include <casa/Quanta/MVAngle.h>
void printDirection(std::ostream &os,const MDirection &dir) throw (AipsError) {
  double lngbuf=dir.getValue().getLong("deg").getValue();
  if (lngbuf<0) lngbuf+=360.;
  os<<(dir.getRefString()!="GALACTIC"?MVAngle::Format(MVAngle::TIME):
  MVAngle::Format(MVAngle::ANGLE))<<MVAngle(Quantity(lngbuf,"deg"))<<" "
    <<MVAngle(dir.getValue().getLat("deg"))<<
    " ("<<dir.getRefString()<<")";
}
*/
//

using namespace casa;
using namespace casa::refim;

BeamSkyJones::BeamSkyJones( 
			   const Quantity &parallacticAngleIncrement,
			   BeamSquint::SquintType doSquint,
			   const Quantity &skyPositionThreshold) :
     doSquint_p(doSquint),
     parallacticAngleIncrement_p(parallacticAngleIncrement.getValue("rad")),
     skyPositionThreshold_p(skyPositionThreshold.getValue("rad")),
     lastUpdateVisBuffer_p( ), lastUpdateRow_p(-1),
     lastUpdateIndex1_p(-1), lastUpdateIndex2_p(-1), hasBeenApplied(false), vbutil_p(nullptr)
     
{  
  reset();
  setThreshold(0.01); // use this in apply to determine level of cutoff
};

void BeamSkyJones::reset()
{
  lastFieldId_p=-1;
  lastArrayId_p=-1;
  lastMSId_p=0;
  lastTime_p=-1.0;
  telescope_p="";
}

BeamSkyJones::~BeamSkyJones()
{
};

void BeamSkyJones::showState(LogIO& os)
{
  os << "Field ID    = " << lastFieldId_p+1 << LogIO::POST;
  os << "Telescope   = " << telescope_p << LogIO::POST;
  for (uInt i=0;i<lastParallacticAngles_p.nelements();++i) {
       os << "ParAngle[d] ("<<i<<" model) = " <<
            lastParallacticAngles_p[i]/C::pi*180.<< LogIO::POST;
       os << "Pointing direction ("<<i<<" model) = "<<
             lastDirections_p[i].getAngle().getValue("deg")(0) <<
    ", " <<  lastDirections_p[i].getAngle().getValue("deg")(1) << LogIO::POST;
  }
  os << "delta PA[d] = " << Quantity(parallacticAngleIncrement_p,"rad").getValue("deg") << LogIO::POST;
  os << "skyPositionThreshold[d] = " << Quantity(skyPositionThreshold_p,"rad").getValue("deg") << LogIO::POST;
  os << "SquintType  = " << (Int)(doSquint_p) << LogIO::POST;
};

String BeamSkyJones::telescope(){
  return telescope_p;
}
// update the indices cache. This method could be made protected in the
// future (need access functions for lastUpdateIndex?_p to benefit from it)
// Cache will be valid for a given VisBuffer and row
void BeamSkyJones::updatePBMathIndices(const vi::VisBuffer2 &vb, Int row) const
{
  // for debug
  // cout<<endl<<"BeamSkyJones::updatePBMathIndices row="<<row<<endl<<endl;
  //

  // we will not check msId, arrayId and fieldID as they are supposed to be
  // checked before this method is called. However, if this method is to
  // be made protected, a change may be required here to avoid nasty
  // surprises in the future.

 
  ///////////////AS visbuffer2 copy does not seem to work and there is no way to know the state of the previous vb...will update it anyways.
  /*if (!lastUpdateVisBuffer_p.null() && vb.rowIds()(0)==lastUpdateVisBuffer_p->rowIds()(0) && row==lastUpdateRow_p) return;
  lastUpdateVisBuffer_p=vi::VisBuffer2::factory(vi::VbRekeyable);
  // lastUpdateVisBuffer_p->setShape(vb.getShape()(0), vb.getShape()(1), vb.getShape()(2));
  //lastUpdateVisBuffer_p->copy(vb,true);
  
  vi::VisBufferComponents2 comps = vi::VisBufferComponents2::these(vi::VisBufferComponent2::DataDescriptionIds, vi::VisBufferComponent2::FieldId, vi::VisBufferComponent2::RowIds);
  lastUpdateVisBuffer_p->copyComponents(vb, comps, true, true);
  */
  lastUpdateRow_p=row;

  // Getting the reference on antennae/feed IDs is a
  // fast operation as caching is implemented inside VisBuffer.
  DebugAssert(row<(Int)vb.antenna1().nelements(),AipsError);
  Int ant1=vb.antenna1()[row];
  Int ant2=vb.antenna2()[row];
  Int feed1=vb.feed1()[row];
  Int feed2=vb.feed2()[row];  

  // telescope_p should be valid at this stage because it is updated
  // after each ArrayID change. Care must be taken if the method is to be
  // made protected.
  lastUpdateIndex1_p=indexTelescope(telescope_p,ant1,feed1);
  lastUpdateIndex2_p=indexTelescope(telescope_p,ant2,feed2);
}

Bool BeamSkyJones::changed(const vi::VisBuffer2& vb, Int row)
{
  // for debug
  // cout<<endl<<"BeamSkyJones::changed row="<<row<<" lastUpdateRow_p="<<
  //         lastUpdateRow_p<<endl<<endl;  
  //
  
  if (row < 0) row = 0;
  
  if(vb.msId() != lastMSId_p || vb.arrayId()(0)!=lastArrayId_p ||
     vb.fieldId()(0)!=lastFieldId_p)  {
        lastUpdateVisBuffer_p=NULL; // invalidate index cache
        return true;
  }

  MDirection::Types pointingdirType=MDirection::castType(lastDirections_p[lastUpdateIndex1_p].getRef().getType());
  //if in a local frame and time change => pointing  most probably changed
  if((pointingdirType >= MDirection::HADEC) && (pointingdirType <= MDirection::AZELSWGEO)){
    if(lastTime_p != vb.time()(0))
      return True;
  }
  
  //if (lastUpdateIndex1_p<0 || lastUpdateIndex2_p<0) return true;
  
  updatePBMathIndices(vb,row); // lastUpdateIndex?_p are now valid

  //Unnecessary ...i believe and causes issues with PSF making
  //if (!hasBeenApplied) return true; // we shouldn't have such a flag in
                 // a well designed code

  if (!lastParallacticAngles_p.nelements() && myPBMaths_p.nelements())
       return true; // it's a first call of this method and setPBMath has
                    // definitely been called before

 

  // Obtaining a reference on parallactic angles is a fast operation as
  // caching is implemented inside VisBuffer.
  Float feed1_pa=vb.feedPa1()[row];
  Float feed2_pa=vb.feedPa2()[row];

  // it may be good to check here whether an indexed beam model
  // depend on parallactic angle before returning true
  // An additional interface function may be required for PBMath classes

  if (lastUpdateIndex1_p!=-1)
      if (abs(feed1_pa-lastParallacticAngles_p[lastUpdateIndex1_p]) >
              parallacticAngleIncrement_p) return true;

  if (lastUpdateIndex2_p!=-1)
      if (abs(feed2_pa-lastParallacticAngles_p[lastUpdateIndex2_p]) >
              parallacticAngleIncrement_p) return true;

  /*
   These direction test are not used right now  and are terrible calculations to do on 
   billions of rows of data
   If it is needed a faster algorithm for changed direction is needed
////	      
  if (lastUpdateIndex1_p!=-1)
      if (!directionsCloseEnough(lastDirections_p[lastUpdateIndex1_p],
                     vb.direction1()[row])) return true;

  if (lastUpdateIndex2_p!=-1)
      if (!directionsCloseEnough(lastDirections_p[lastUpdateIndex2_p],
                     vb.direction2()[row])) return true;    
  */

  return false;
};

// return true if two directions are close enough to consider the
// operator unchanged, false otherwise
Bool BeamSkyJones::directionsCloseEnough(const MDirection &dir1,
                           const MDirection &dir2) const throw(AipsError)
{
  Double sep; 
  if (dir1.getRef()!=dir2.getRef())
      sep=dir1.getValue().separation(MDirection::Convert(dir2.getRef(),
              dir1.getRef())(dir2).getValue());
  else sep=dir1.getValue().separation(dir2.getValue());
  return (fabs(sep)<skyPositionThreshold_p);
}

// Does this BeamSkyJones change during this buffer, starting from
// row1?  If row2 <0, row2 = nRow()-1
Bool BeamSkyJones::changedBuffer(const vi::VisBuffer2& vb, Int row1, Int& row2)
{
  Int irow = row1;
  if (irow < 0) irow = 0;
  Int jrow = row2;
  if (jrow < 0) jrow = vb.nRows()-1;
  DebugAssert(jrow<vb.nRows(),AipsError);

  // It is not important now to have a separate function for a "block"
  // operation. Because an appropriate caching is implemented inside
  // both VisBuffer and this class, changed(vb,row) can be called in the
  // loop without a perfomance penalty. We preserve this function to keep
  // the interface intact.

  for (Int ii=irow+1;ii<=jrow;++ii)
       if (changed(vb,ii)) {
           row2 = ii-1;
	   return true;
       }
  return false;
};

// as it is stated in BeamSkyJones.h this method may not be useful
// anymore. Implementing it just in case it is used somewhere.
// Because an appropriate caching is implemented in both VisBuffer and
// BeamSkyJones, this method can use BeamSkyJones::changed in a loop
Bool BeamSkyJones::change(const vi::VisBuffer2& vb)
{
  for (rownr_t i=0;i<vb.nRows();++i)
       if (changed(vb,i)) return true;
  return false;
};

void BeamSkyJones::update(const vi::VisBuffer2& vb, Int row)
{
  // for debug
  //cout<<endl<<"BeamSkyJones::update nrow="<<vb.nRow()<<" row="<<row<<" feed1="<<vb.feed1()(0)<<" feed2="<<vb.feed2()(0)<<endl<<endl;
  //
  
  if (row<0) row=0;
  
  lastFieldId_p=vb.fieldId()(0);
  lastArrayId_p=vb.arrayId()(0);
  lastMSId_p=vb.msId();
  lastTime_p=vb.time()(0);
  if(!vbutil_p){
    vbutil_p=new VisBufferUtil(vb);
  }
  // The pointing direction depends on feed, antenna, pointing errors, etc  
  //MDirection pointingDirection1 = vb.direction1()(row);
  //MDirection pointingDirection2 = vb.direction2()(row);
  MDirection pointingDirection1 = vbutil_p->getPointingDir(vb, vb.antenna1()(row), row);
  MDirection pointingDirection2 = vbutil_p->getPointingDir(vb, vb.antenna2()(row), row);
  //cerr << "DIR1 " << pointingDirection1.toString() << "   " <<  pointingDirection2.toString() << endl; 
  // Look up correct telescope
  const MSObservationColumns& msoc=vb.subtableColumns().observation();
  telescope_p = msoc.telescopeName()(vb.arrayId()(0));

  updatePBMathIndices(vb,row); // lastUpdateIndex?_p are now valid

  if (!lastParallacticAngles_p.nelements() && myPBMaths_p.nelements()) {
       lastParallacticAngles_p.resize(myPBMaths_p.nelements());
       lastParallacticAngles_p.set(1000.); // to force recalculation
                           // it will recalculate directions as well
  }
  if (!lastDirections_p.nelements() && myPBMaths_p.nelements()) 
       lastDirections_p.resize(myPBMaths_p.nelements());
       
  if (lastUpdateIndex1_p == lastUpdateIndex2_p &&
      !directionsCloseEnough(pointingDirection1,pointingDirection2)) {
        // the case is inhomogeneous: pointing directions are slightly
	// different at different antennae
    //This check is an overkill for standard arrays...need to find a better one

    /*	LogIO os;
	os << LogIO::WARN << LogOrigin("BeamSkyJones","update")
	   << "The pointing directions differ for different stations."
	   << LogIO::POST << LogIO::WARN << LogOrigin("BeamSkyJones","update")
	   << "This case is not handled correctly. Continuing anyway."<<LogIO::POST;

    */
	// we could, in principle, clone a PBMath object for one of the
	// antennae and rebuild lastDirections_p.
	// For now, the value for the second antenna will be used
  }
  
  if (lastUpdateIndex1_p!=-1)
      lastDirections_p[lastUpdateIndex1_p]=pointingDirection1;
  ////CAS-6688 using antenna1 only for now
  // if (lastUpdateIndex2_p!=-1)
  //    lastDirections_p[lastUpdateIndex2_p]=pointingDirection2;
  
  // Obtaining a reference on parallactic angles is a fast operation as
  // caching is implemented inside VisBuffer.
  Float feed1_pa=vb.feedPa1()[row];
  Float feed2_pa=vb.feedPa2()[row];

  if (lastUpdateIndex1_p == lastUpdateIndex2_p &&
      abs(abs(feed1_pa-feed2_pa)-parallacticAngleIncrement_p)> 1e-5 ) {
      // the array is not compact: position angles are significantly
      // different at different antennae
      LogIO os;
      //Commenting out this message...more pest than  useful to have it at this low level
      //    os << LogIO::WARN << LogOrigin("BeamSkyJones","update")
      //       << "The array is not compact, position angles differ for different stations."
      //     << LogIO::POST << LogIO::WARN << LogOrigin("BeamSkyJones","update")
      //      << "Primary beams are not correctly handled if they are asymmetric. Continuing anyway."<<LogIO::POST;
    // we could, in principle, clone a PBMath object for one of the
    // antennae and rebuild lastParallacticAngles_p.
    // For now, the value for the second antenna will be used
  }
  if (lastUpdateIndex1_p!=-1)
      lastParallacticAngles_p[lastUpdateIndex1_p]=feed1_pa;
  if (lastUpdateIndex2_p!=-1)
      lastParallacticAngles_p[lastUpdateIndex2_p]=feed2_pa;
};

void BeamSkyJones::assure (const vi::VisBuffer2& vb, Int row)
{
  if(changed(vb, row)) update(vb, row);  
};


ImageInterface<Complex>& 
BeamSkyJones::apply(const ImageInterface<Complex>& in,
		    ImageInterface<Complex>& out,
		    const vi::VisBuffer2& vb, Int row,
		    Bool forward)
{
  if(changed(vb, row)) update(vb, row);
  hasBeenApplied=true;
  // now lastUpdateIndex?_p are valid
  
  if (lastUpdateIndex1_p!=lastUpdateIndex2_p) 
      throw(AipsError("BeamSkyJones::apply(Image...) - can only treat homogeneous PB case"));
  else {
    // for debug
    // cout<<endl<<"BeamSkyJones::apply(Image...) index="<<lastUpdateIndex1_p<<" feed="<<vb.feed1()(0)<<" direction=";
    // printDirection(cout,lastDirections_p[lastUpdateIndex1_p]); cout<<endl<<endl;
    //
    CoordinateSystem cs=in.coordinates();
    Int coordIndex=cs.findCoordinate(Coordinate::DIRECTION);
    MDirection::Types dirType=cs.directionCoordinate(coordIndex).directionType();
    PBMath myPBMath;    
    if (getPBMath(lastUpdateIndex1_p, myPBMath)) 
      return myPBMath.applyPB(in, out, convertDir(vb, lastDirections_p[lastUpdateIndex1_p], dirType), 
	      Quantity(lastParallacticAngles_p[lastUpdateIndex1_p],"rad"),
              doSquint_p, false, threshold(), forward);
    else 
      throw(AipsError("BeamSkyJones::apply(Image...)!!! - PBMath not found"));
  }
}; 


ImageInterface<Float>& 
BeamSkyJones::apply(const ImageInterface<Float>& in,
			  ImageInterface<Float>& out,
			  const vi::VisBuffer2& vb, Int row){
  if(changed(vb, row)) update(vb, row);
  hasBeenApplied=true;
  // now lastUpdateIndex?_p are valid
  
  if (lastUpdateIndex1_p!=lastUpdateIndex2_p) 
    throw(AipsError("BeamSkyJones::apply(Image...) - can only treat homogeneous PB case"));
  else {
    PBMath myPBMath; 
    CoordinateSystem cs=in.coordinates();
    Int coordIndex=cs.findCoordinate(Coordinate::DIRECTION);
    MDirection::Types dirType=cs.directionCoordinate(coordIndex).directionType();
    if (getPBMath(lastUpdateIndex1_p, myPBMath)) 
      return myPBMath.applyPB(in, out,convertDir(vb, lastDirections_p[lastUpdateIndex1_p], dirType), 
			      Quantity(lastParallacticAngles_p[lastUpdateIndex1_p],"rad"),
			      doSquint_p, threshold());
    else 
      throw(AipsError("BeamSkyJones::apply(Image...)!!! - PBMath not found"));
  }

}
ImageInterface<Float>& 
BeamSkyJones::applySquare(const ImageInterface<Float>& in,
			  ImageInterface<Float>& out,
			  const vi::VisBuffer2& vb, Int row)
{
  if(changed(vb, row)) update(vb, row);
  hasBeenApplied=true;
  // now lastUpdateIndex?_p are valid
  
  if (lastUpdateIndex1_p!=lastUpdateIndex2_p)   
    throw(AipsError("BeamSkyJones::applySquare(Image...) - can only treat homogeneous PB case"));
  else {
    // for debug
    //cout<<endl<<"BeamSkyJones::applySquare(Image...) index="<<lastUpdateIndex1_p<<" feed="<<vb.feed1()(0)<<" direction=";
     //printDirection(cout,lastDirections_p[lastUpdateIndex1_p]); cout<<endl<<endl;
    //
    PBMath myPBMath;
    CoordinateSystem cs=in.coordinates();
    Int coordIndex=cs.findCoordinate(Coordinate::DIRECTION);
    MDirection::Types dirType=cs.directionCoordinate(coordIndex).directionType();
    if (getPBMath(lastUpdateIndex1_p, myPBMath)) 
      return myPBMath.applyPB2(in, out, convertDir(vb, lastDirections_p[lastUpdateIndex1_p], dirType), 
           lastParallacticAngles_p[lastUpdateIndex1_p], doSquint_p, threshold()*threshold());
    else 
      throw(AipsError("BeamSkyJones::applySquare(Image...) - PBMath not found"));    
  }
}; 


SkyComponent& 
BeamSkyJones::apply(SkyComponent& in,
		    SkyComponent& out,
		    const vi::VisBuffer2& vb, Int row,
		    Bool forward)
{
  if(changed(vb, row)) update(vb, row);
  hasBeenApplied=true;
  // now lastUpdateIndex?_p are valid
  
  if (lastUpdateIndex1_p!=lastUpdateIndex2_p)
    throw(AipsError("BeamSkyJones::apply(SkyComp...) - can only treat homogeneous PB case"));
  else { 
    // for debug
    // cout<<endl<<"BeamSkyJones::apply(SkyComp...) index="<<lastUpdateIndex1_p<<" feed="<<vb.feed1()(0)<<" direction=";
    // printDirection(cout,lastDirections_p[lastUpdateIndex1_p]); cout<<endl<<endl;
    //
    PBMath myPBMath;
    MDirection compdir=in.shape().refDirection();
    MDirection::Types dirType=MDirection::castType(compdir.getRef().getType());
    if (getPBMath(lastUpdateIndex1_p, myPBMath)) 
      return myPBMath.applyPB(in, out,convertDir(vb, lastDirections_p[lastUpdateIndex1_p], dirType) , 
			      Quantity(vb.getFrequency(0,0), "Hz"),
			      lastParallacticAngles_p[lastUpdateIndex1_p],
			      doSquint_p, false, threshold(), forward);
      else 
      throw(AipsError("BeamSkyJones::apply(SkyComponent,...) - PBMath not found"));    
  }
}; 


SkyComponent& 
BeamSkyJones::applySquare(SkyComponent& in,
		    SkyComponent& out,
		    const vi::VisBuffer2& vb, Int row)
{
  if(changed(vb, row)) update(vb, row);
  hasBeenApplied=true;
  // now lastUpdateIndex?_p are valid
  
  if (lastUpdateIndex1_p!=lastUpdateIndex2_p)   
    throw(AipsError("BeamSkyJones::applySquare(SkyComponent,...) - can only treat homogeneous PB case"));
  else { 
    PBMath myPBMath;
    MDirection compdir=in.shape().refDirection();
    MDirection::Types dirType=MDirection::castType(compdir.getRef().getType());
    if (getPBMath(lastUpdateIndex1_p, myPBMath))
      return myPBMath.applyPB2(in, out, convertDir(vb, lastDirections_p[lastUpdateIndex1_p], dirType), 
			       Quantity(vb.getFrequency(0,0), "Hz"),
			       lastParallacticAngles_p[lastUpdateIndex1_p],
			       doSquint_p);
    else 
      throw(AipsError("BeamSkyJones::applySquare(SkyComponent,...) - PBMath not found"));
  }
}; 

// Apply gradient
ImageInterface<Complex>&
BeamSkyJones::applyGradient(ImageInterface<Complex>& result,
			  const vi::VisBuffer2&,
			  Int)
{  
  return result;
};


void BeamSkyJones::initializeGradients()
{
};

void BeamSkyJones::finalizeGradients()
{
};


SkyComponent&
BeamSkyJones::applyGradient(SkyComponent& result, const vi::VisBuffer2&,
			  Int) 
{  
  return result;
};

void BeamSkyJones::addGradients(const vi::VisBuffer2&, Int,
			      const Float, 
			      const Float,
			      const Matrix<Complex>&, 
			      const Matrix<Float>&) 
{};

// Solve
/*Bool BeamSkyJones::solve (SkyEquation& se)
{
  // Keep compiler quiet
  if(&se) {};
  return false;
};
*/
// return index of compareTelescope, compareAntenna and compareFeed in
// myTelescopes_p; -1 if not found
// if compareAntenna or compareTelescope is -1, this means that the
// PBMath class is the same for all antennae/feeds. An exception will
// be raised, if separate PBMath objects have been assigned by setPBMath
// for different feeds/antennae but -1 is used for query.
//
// It would be good to rename this function to indexBeams as this name
// is more appropriate. 
//
Int BeamSkyJones::indexTelescope(const String &compareTelescope,
                     const Int &compareAntenna, const Int &compareFeed) const
{
  // for debugging
  //cout<<endl<<"BeamSkyJones::indexTelescope tel="<<compareTelescope<<" ant="<<compareAntenna<<" feed="<<compareFeed<<endl<<endl;
  //cout<<"Currently "<<myTelescopes_p.nelements()<<" models have been set"<<endl;
  //for (uInt i=0; i<myTelescopes_p.nelements(); ++i) 
  //     cout<<i<<" telescope: "<<myTelescopes_p[i]<<" ant:" <<
  //         myAntennaIDs_p[i]<<" feed: "<<myFeedIDs_p[i]<<endl;
  //	     
	     
  DebugAssert(myTelescopes_p.nelements()==myAntennaIDs_p.nelements(),
              AipsError);
  DebugAssert(myTelescopes_p.nelements()==myFeedIDs_p.nelements(),
              AipsError);	      
  for (uInt i=0; i<myTelescopes_p.nelements(); ++i) 
       if (myTelescopes_p[i] == compareTelescope) {
           if (compareAntenna==myAntennaIDs_p[i] &&
	       compareFeed==myFeedIDs_p[i]) return i; // -1 will also work
           if (compareAntenna==myAntennaIDs_p[i]) {
	       if (compareFeed==-1)
	           throw AipsError("BeamSkyJones::indexTelescope: separate beam models"
	              "have been set up for different feeds and a common model is requested");
               if (myFeedIDs_p[i]==-1) return i; // valid for all feeds and a given antenna		      
           }
           if (compareFeed==myFeedIDs_p[i]) {
	       if (compareAntenna==-1)
	           throw AipsError("BeamSkyJones::indexTelescope: separate beam models"
	               "have been set up for different antennae and a common model is requested");
	       if (myAntennaIDs_p[i]==-1) return i; // valid for all antennae and a given feed       
           }
	   if (myFeedIDs_p[i]==-1 && myAntennaIDs_p[i]==-1)
	       return i; // valid for all antennae and feeds
           
       }  
  return -1;
};

// get the PBMath object; returns false if that one doesn't exist,
// true if it does exist and is OK
// antennaID and feedID default to -1 to preserve the old interface
// TODO: change the interface and make antennaID and feedID the
// second and third parameter respectively to have a better looking code

Bool BeamSkyJones::getPBMath(const String &telescope, PBMath &myPBMath,
                 const Int &antennaID, const Int &feedID) const
{
  Int indTel = indexTelescope(telescope,antennaID,feedID);
  if (indTel >= 0) 
    return getPBMath((uInt)indTel, myPBMath);
   else 
    return false;  // PBMath not found for this telescope/antenna/feed combination
  
};

Bool BeamSkyJones::getPBMath(uInt whichAnt, PBMath &myPBMath) const
{
  if (whichAnt <  myPBMaths_p.nelements() && Int(whichAnt)>=0) {
    if (myPBMaths_p[whichAnt].ok()) {
      myPBMath = myPBMaths_p[whichAnt];
      return true;
    } else 
      return false;  // whichAnt's PBMath found but not valid    
  } else 
    return false;  // whichAnt's PBMath not found
  
};

// set the PB based on telescope name, antennaID and feedID
// If antennaID or feedID is -1, the PBMath object is set for
// all antennae or feeds, respectively. These are the default
// values to retain the previous interface.
//
// Note. It would be nice to change the interface and make antennaID
// and feedID the second and the third parameter, respectively.
void BeamSkyJones::setPBMath(const String &telescope, PBMath &myPBMath,
                const Int &antennaID, const Int &feedID)
{
   // for debug
   // cout<<endl<<"BeamSkyJones::setPBMath tel="<<telescope<<" ant="<<antennaID<<" feed="<<feedID<<endl<<endl;
   //
   
   DebugAssert(myTelescopes_p.nelements()==myAntennaIDs_p.nelements(),
               AipsError);
   DebugAssert(myTelescopes_p.nelements()==myFeedIDs_p.nelements(),
               AipsError);	      
   DebugAssert(myTelescopes_p.nelements()==myPBMaths_p.nelements(),
               AipsError);

   Bool doRemove=false;
   if (antennaID==-1 || feedID==-1) 
     // we have to remove PBMaths for individual antennae/feeds, if they     
     // were assigned earlier
     for (uInt i=0; i<myTelescopes_p.nelements(); ++i) {
          if (doRemove) {
	      // we have to step back because the previous element
	      // has been removed
	      --i;
	      doRemove=false;
	      DebugAssert(i<myTelescopes_p.nelements(), AipsError);
	  }
          if (myTelescopes_p[i] == telescope) {	      
	      if (myAntennaIDs_p[i]==-1 && antennaID==-1 &&
	          myFeedIDs_p[i]==-1 && feedID==-1)
		     continue;  // to speed things up
	      if ((myAntennaIDs_p[i]!=-1) && (antennaID==-1))
		{
	          if (myFeedIDs_p[i]!=-1) myAntennaIDs_p[i]=-1;
		      // now it's valid for all antennae and a given feed
		      // and will be replaced later
		  else doRemove=true;
		}
              if ((myFeedIDs_p[i]!=-1) && (feedID==-1))
		{
	          if (myAntennaIDs_p[i]!=-1) myFeedIDs_p[i]=-1;
		      // now it's valid for all feeds at a given antenna
		      // and will be replaced later
                  else doRemove=true;
		}
              if (doRemove) {
	          myTelescopes_p.remove(i,false);
	          myAntennaIDs_p.remove(i,false);
		  myFeedIDs_p.remove(i,false);
		  myPBMaths_p.remove(i,false);
		  if (lastParallacticAngles_p.nelements())
		      lastParallacticAngles_p.remove(i,false);
                  if (lastDirections_p.nelements())
		      lastDirections_p.remove(i,false);
	      }
	  }
     }
  // there should be no exception at this stage because all bad elements
  // should be removed by the code above
  Int ind = indexTelescope(telescope,antennaID,feedID);
  if (ind < 0) {
    ind = myPBMaths_p.nelements();
    myPBMaths_p.resize(ind+1);
    myTelescopes_p.resize(ind+1);
    myTelescopes_p[ind] = telescope;
    myAntennaIDs_p.resize(ind+1);
    myAntennaIDs_p[ind] = antennaID;
    myFeedIDs_p.resize(ind+1);
    myFeedIDs_p[ind] = feedID;
    if (lastParallacticAngles_p.nelements())
        lastParallacticAngles_p.resize(ind+1);
    if (lastDirections_p.nelements())
        lastDirections_p.resize(ind+1);
  }
  myPBMaths_p[ind] = myPBMath;
  if (lastParallacticAngles_p.nelements())
      lastParallacticAngles_p[ind]=1000.; // to force
                                          // recalculation (it is >> 2pi)
};
MDirection BeamSkyJones::convertDir(const vi::VisBuffer2& vb, const MDirection& inDir, const MDirection::Types outType){


  if(MDirection::castType(inDir.getRef().getType())==outType){
    return inDir;
  }
   MPosition pos;
   String tel("");
   MSColumns msc(vb.ms());
   if (vb.subtableColumns().observation().nrow() > 0) {
     tel = vb.subtableColumns().observation().telescopeName()(vb.observationId()(0));
   }
   if (tel.length() == 0 || !tel.contains("VLA") ||
       !MeasTable::Observatory(pos,tel)) {
     // unknown observatory, use first antenna
     Int ant1=vb.antenna1()(0);
     pos=vb.subtableColumns().antenna().positionMeas()(ant1);
   }
   MEpoch::Types timeMType=MEpoch::castType(msc.timeMeas()(0).getRef().getType());
   Unit timeUnit=Unit(msc.timeMeas().measDesc().getUnits()(0).getName());
   MEpoch timenow(Quantity(msc.time()(0), timeUnit), timeMType);
    MeasFrame mFrame(timenow, pos);
    MDirection::Ref elRef(outType, mFrame);
    return MDirection::Convert(inDir, elRef)();
}



Bool BeamSkyJones::isHomogeneous() const
{
  // Hogwash!  our "myPBMath_p/myTelescope_p scheme only deals
  // with homogeneous pointings.  Need to fix this!
  // Wait for MS-II
  return true;
};


ImageRegion*
BeamSkyJones::extent (const ImageInterface<Complex>& im, 
		      const vi::VisBuffer2& vb,
		      const Int row,	
		      const Float fPad,  
		      const Int iChan, 
		      const casa::SkyJones::SizeType sizeType)
{
  if(changed(vb, row))  update(vb, row);
  DebugAssert(lastUpdateIndex1_p>=0,AipsError); // should be checked in changed/update
  
  PBMath myPBMath;
  if (getPBMath(lastUpdateIndex1_p, myPBMath)) {
    return myPBMath.extent(im, lastDirections_p[lastUpdateIndex1_p], row, fPad, iChan, sizeType);
  } else {
    throw(AipsError("BeamSkyJones::extent() - PBMath not found"));
  }   
};

ImageRegion*
BeamSkyJones::extent (const ImageInterface<Float>& im, 
		      const vi::VisBuffer2& vb,
		      const Int row,
		      const Float fPad,  
		      const Int iChan,
		      const casa::SkyJones::SizeType sizeType)
{ 
  if(changed(vb, row)) update(vb, row);
  DebugAssert(lastUpdateIndex1_p>=0,AipsError); // should be checked in changed/update
  
  PBMath myPBMath;
  if (getPBMath(lastUpdateIndex1_p, myPBMath)) {
    return myPBMath.extent(im, lastDirections_p[lastUpdateIndex1_p], row, fPad, iChan, sizeType);
  } else {
    throw(AipsError("BeamSkyJones::extent() - PBMath not found"));
  }   
};

Int BeamSkyJones::support(const vi::VisBuffer2& vb, const casacore::CoordinateSystem& cs){
  PBMath myPBMath;

  if(changed(vb, 0)) update(vb, 0);
  if (getPBMath(lastUpdateIndex1_p, myPBMath)) {
    return myPBMath.support(cs);
  } else {
    throw(AipsError("BeamSkyJones::support() - PBMath not found"));
  }

}

void BeamSkyJones::summary(Int n) 
{
  uInt nPBs = myPBMaths_p.nelements();
  LogIO os(LogOrigin("BeamSkyJones", "summary"));
  os << "Beam Summary: "<< LogIO::POST;
  for (uInt i=0; i< nPBs; ++i) {
    String pbclass;
    myPBMaths_p[i].namePBClass(pbclass);
    os << "Model " << i+1 << " for " << myTelescopes_p[i] <<" ant="<<
          myAntennaIDs_p[i]<<" feed="<<myFeedIDs_p[i]<< " uses PB: "
       << pbclass << LogIO::POST;
    if (n >=0) {
      myPBMaths_p[i].summary(n);
    }
  }
};

} //# end of namespace refim

} //# end of namespace casa
