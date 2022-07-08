// -*- C++ -*-
//# PointingOffsets.cc: Implementation of the PointingOffsets class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//

#include <msvis/MSVis/VisibilityIterator2.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <synthesis/TransformMachines2/PointingOffsets.h>
#include <synthesis/TransformMachines/SynthesisError.h>
// #include <casa/Logging/LogIO.h>
// #include <casa/Logging/LogSink.h>
// #include <casa/Logging/LogOrigin.h>

using namespace casacore;
namespace casa{
  using namespace vi;
  using namespace refim;
  //
  //----------------------------------------------------------------------
  //
  PointingOffsets& PointingOffsets::operator=(const PointingOffsets& other)
  {
    if(this!=&other) 
      {
	imageDC_p = other.imageDC_p;
	imageObsInfo_p = other.imageObsInfo_p;
	cachedPointingOffsets_p = other.cachedPointingOffsets_p;
	cachedAntGridPointingOffsets_p = other.cachedAntGridPointingOffsets_p;
      }
    return *this;
  }
  //
  //----------------------------------------------------------------------
  //
  Vector<Vector<double> >PointingOffsets::findMosaicPointingOffset(const ImageInterface<Complex>& image,
								   const VisBuffer2& vb, const Bool& doPointing)
  {
    storeImageParams(image,vb);
    //where in the image in pixels is this pointing
    pixFieldGrad_p.resize(2);
    int antId,numRow;
    antId = 0;
    numRow = 0;
    Vector<Vector<double> >pixFieldGrad_l;
    pixFieldGrad_l.resize(1);
    MDirection dir = vbUtils_p.getPointingDir(vb,antId,numRow,dc_p.directionType(),doPointing);
    thePix_p = toPix(vb, dir, dir);

    pixFieldGrad_p = gradPerPixel(thePix_p);
    pixFieldGrad_l(0)=(pixFieldGrad_p);

    return pixFieldGrad_l;
  };
  //
  //----------------------------------------------------------------------
  //
  Vector<Vector<double> > PointingOffsets::findAntennaPointingOffset(const ImageInterface<Complex>& image,
								     const vi::VisBuffer2& vb, const Bool& doPointing)
  {
    Vector<Vector<double> >antOffsets;
    storeImageParams(image,vb);

    if (epJ_p.null())
      {
	int numRow_p = vb.nRows();
	antOffsets.resize(numRow_p); // The array is resized to fit for a given vb
	if (PO_DEBUG_P==1)
	  cerr << "-------------------------------------------------------------------------" << endl;
	for (int irow=0; irow<numRow_p;irow++)
	  {
	    MDirection antDir1 =vbUtils_p.getPointingDir(vb, vb.antenna1()[irow], irow, dc_p.directionType(), doPointing); 
	    MDirection antDir2 =vbUtils_p.getPointingDir(vb, vb.antenna2()[irow], irow, dc_p.directionType(), doPointing); 
	    
	    //	    MVDirection vbdir=vb.direction1()(0).getValue();	
	    casacore::Vector<double> thePixDir1_l, thePixDir2_l;
	    thePixDir1_l = toPix(vb, antDir1, antDir2);
	    thePixDir2_l = toPix(vb, antDir2, antDir1);
	    thePix_p = (thePixDir1_l + thePixDir2_l)/2.;
	    antOffsets(irow) = gradPerPixel(thePix_p);
	    if (PO_DEBUG_P==1)
	      {
		cerr << irow << " " 
		     << vb.antenna1()[irow] << " " << vb.antenna2()[irow] << " " 
		     << antDir1 << " " << antDir2 << " "
		     << vb.direction1()(irow) << " " << vb.direction2()(irow) << " "
		     << toPix(vb, antDir1, antDir2) << " " << toPix(vb, antDir2,antDir1)
		     << endl;
		
	      }
	    //cerr << irow << " " << antOffsets[irow]<< endl;
	  }
	if (PO_DEBUG_P==1)
	  cerr << "=========================================================================" << endl;
      }
    else
      {
	throw(SynthesisFTMachineError("PointingOffsets::findAntennaPointingOffset(): Antenna pointing CalTable support not yet implemented"));
      }
    return antOffsets;
  }
  //
  //----------------------------------------------------------------------
  //
  void PointingOffsets::fetchPointingOffset(const ImageInterface<Complex>& image,
					   const VisBuffer2& vb, const Bool doPointing)
  {
    setDoPointing(doPointing);
    if (!doPointing) 
      { 
	cachedPointingOffsets_p.assign(findMosaicPointingOffset(image,vb,doPointing));
      }
    else 
      {
	cachedPointingOffsets_p.assign(findAntennaPointingOffset(image,vb,doPointing));
      }
  }
  //
  //----------------------------------------------------------------------
  //
  Vector<double> PointingOffsets::gradPerPixel(const Vector<double>& p)
  {
    Vector<double> gPP(2);
    gPP(0) = p(0) - double(nx_p/2);
    gPP(1) = p(1) - double(ny_p/2);

    gPP(0) = -gPP(0)*2.0*C::pi/double(nx_p)/double(convOversampling_p);
    gPP(1) = -gPP(1)*2.0*C::pi/double(ny_p)/double(convOversampling_p);

    return gPP;
  }
  //
  //----------------------------------------------------------------------
  //
  Vector<double>& PointingOffsets::toPix(const VisBuffer2& vb, 
					 const MDirection& dir1, 
					 const MDirection& dir2) 
  {
    thePix_p.resize(2);
    if(dc_p.directionType() !=  MDirection::castType(dir1.getRef().getType()))
      {
      //pointToPix_p.setModel(theDir);
      
      MEpoch timenow(Quantity(vb.time()(0), timeUnit_p), timeMType_p);
      pointFrame_p.resetEpoch(timenow);
      //////////////////////////
      //pointToPix holds pointFrame_p by reference...
      //thus good to go for conversion
      direction1_p=pointToPix_p(dir1);
      direction2_p=pointToPix_p(dir2);
      dc_p.toPixel(thePix_p, direction1_p);
      //cerr<<" thePix_P from one: "<<thePix_p<< " " << dir1.getRef().getType()<<endl;

    }
    else
      {
      direction1_p=dir1;
      direction2_p=dir2;
      dc_p.toPixel(thePix_p, dir1);
      //cerr<<" thePix_P from two: "<<thePix_p<< " " << dir1.getRef().getType()<<endl;
      }
    // Return the internal variable, just to make code more readable
    // at the place of the call.
    return thePix_p;
  };

  //
  //----------------------------------------------------------------------
  //

  vector<vector<double> > PointingOffsets::fetchAntOffsetToPix(const VisBuffer2& vb, const Bool doPointing)
  {
    // Int numRow_p = vb.nRows();
    vector<vector<double> > pix_l;

    vector<int> ant1, ant2;
    ant1 = (vb.antenna1()).tovector();
    ant2 = (vb.antenna2()).tovector();
    ant1.insert(ant1.end(),ant2.begin(),ant2.end());
    sort(ant1.begin(),ant1.end());
    auto itr = unique(ant1.begin(),ant1.end());
    ant1.resize(distance(ant1.begin(),itr));

    // cerr <<"ant1.size()" << ant1.size() << endl;
    pix_l.resize(2);
    pix_l[0].resize(ant1.size(),0);
    pix_l[1].resize(ant1.size(),0);

    MVDirection vbdir=vb.direction1()(0).getValue();
    for (unsigned int nant=0; nant< ant1.size();nant++)
      {

	MDirection antDir1 =vbUtils_p.getPointingDir(vb, ant1[nant], 0, dc_p.directionType(), doPointing); 
	// MDirection antDir2 =vbUtils_p.getPointingDir(vb, vb.antenna2()[irow], irow, dc_p.directionType(), doPointing);        
	Vector<double> tmp = toPix(vb, antDir1, vbdir);
	pix_l[0][nant]=tmp[0];
	pix_l[1][nant]=tmp[1];
	// cerr<< "Ant : "<< ant1[nant]<< " Offsets : "<< pix_l[0][nant] << " " << pix_l[1][nant]<<endl;
	// tmp = toPix(vb, antDir2, vbdir);
	// pix_l[2][irow]=tmp[0];
	// pix_l[3][irow]=tmp[1];
      }
    return pix_l;
  }


  //
  //----------------------------------------------------------------------
  //
  void PointingOffsets::storeImageParams(const ImageInterface<Complex>& iimage,
					 const VisBuffer2& vb) 
  {
    //image signature changed...rather simplistic for now
    if((iimage.shape().product() != nx_p*ny_p*nchan_p*npol_p) || nchan_p < 1)
      {
	csys_p=iimage.coordinates();
	int coordIndex=csys_p.findCoordinate(Coordinate::DIRECTION);
	AlwaysAssert(coordIndex>=0, AipsError);
	directionIndex_p=coordIndex;
	dc_p=csys_p.directionCoordinate(directionIndex_p);
	ObsInfo imInfo=csys_p.obsInfo();
	String tel= imInfo.telescope();
	MPosition pos;
	ROMSColumns mscol(vb.ms());
	if (vb.subtableColumns().observation().nrow() > 0) 
	  {
	    tel = vb.subtableColumns().observation().telescopeName()
	      (mscol.observationId()(0));
	  }
	if (tel.length() == 0 || !tel.contains("VLA") ||  
	    !MeasTable::Observatory(pos,tel)) 
	  {
	    // unknown observatory, use first antenna
	    int ant1 = vb.antenna1()(0);
	    pos=vb.subtableColumns().antenna().positionMeas()(ant1);
	  }
	//cout << "TELESCOPE " << tel << endl;
	//Store this to build epochs via the time access of visbuffer later

	timeMType_p=MEpoch::castType(mscol.timeMeas()(0).getRef().getType());
	timeUnit_p=Unit(mscol.timeMeas().measDesc().getUnits()(0).getName());
	// timeUnit_p=Unit("s");
	//cout << "UNIT " << timeUnit_p.getValue() << " name " << timeUnit_p.getName()  << endl;
	pointFrame_p=MeasFrame(imInfo.obsDate(), pos);
	MDirection::Ref elRef(dc_p.directionType(), pointFrame_p);
	//For now we set the conversion from this direction 
	pointToPix_p=MDirection::Convert( MDirection(), elRef);
	nx_p=iimage.shape()(coordIndex);
	ny_p=iimage.shape()(coordIndex+1);
	coordIndex=csys_p.findCoordinate(Coordinate::SPECTRAL);
	int pixAxis=csys_p.pixelAxes(coordIndex)[0];
	nchan_p=iimage.shape()(pixAxis);
	coordIndex=csys_p.findCoordinate(Coordinate::STOKES);
	pixAxis=csys_p.pixelAxes(coordIndex)[0];
	npol_p=iimage.shape()(pixAxis);
      }
  }
  };
