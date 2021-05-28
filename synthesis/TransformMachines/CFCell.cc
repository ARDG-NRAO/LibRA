// -*- C++ -*-
//# CFCell.cc: Implementation of the CFCell class
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
#include <synthesis/TransformMachines/CFCell.h>
#include <synthesis/TransformMachines/Utils.h>
#include <casacore/casa/Utilities/BinarySearch.h>
#include <images/Images/PagedImage.h>
using namespace casacore;
namespace casa{

  
  CountedPtr<CFCell> CFCell::clone()
  {
    CountedPtr<CFCell> clone=new CFCell();
    clone->setParams(*this);
    clone->storage_p = new Array<TT>(*storage_p);
    return clone;
  }

  void CFCell::setParams(const CFCell& other)
  {
    shape_p=other.storage_p->shape();
    shape_p = other.shape_p;
    coordSys_p=other.coordSys_p;
    sampling_p=other.sampling_p;
    xSupport_p=other.xSupport_p;
    ySupport_p=other.ySupport_p;
    wValue_p = other.wValue_p;
    freqValue_p = other.freqValue_p;
    freqIncr_p = other.freqIncr_p;
    muellerElement_p = other.muellerElement_p;
    pa_p = other.pa_p;
    cfShape_p=other.storage_p->shape().asVector();
    cfShape_p.assign(other.cfShape_p);
    conjFreq_p = other.conjFreq_p;
    conjPoln_p = other.conjPoln_p;
  }

  void CFCell::show(const char *Mesg,ostream &os)
  {
    LogIO log_l(LogOrigin("CFCell","show[R&D]"));
      //       << "CoordSys: " << coordSys_p << endl
    if (Mesg) os << Mesg;
    os << "Sampling: "           << sampling_p  << endl
       << "xSupport, ySupport: " << xSupport_p  << " " << ySupport_p << endl
       << "wValues: "            << wValue_p    << endl
       << "wIncr: "            << wIncr_p    << endl
       << "FreqValues: "         << freqValue_p << endl
       << "ConjFreq: "           << conjFreq_p  << endl
       << "ConjPoln: "           << conjPoln_p  << endl
       << "MuellerElements: "    << muellerElement_p << endl
       << "Data shape: "         << storage_p->shape() << " " << cfShape_p << endl
       << "Parallactic Angle(d): "  << pa_p.getValue("deg")
       << endl;
    IPosition dummy;
    Vector<String> csList;
    // os << "CoordSys: ";
    // csList = coordSys_p.list(log_l,MDoppler::RADIO,dummy,dummy);
    // os << csList << endl;
  }

  void CFCell::makePersistent(const char *dir, const char* cfName)
  {
    LogIO log_l(LogOrigin("CFCell","makePersistent[R&D]"));
    String name(dir);
    if (cfShape_p.nelements() == 0)
      {
    	//log_l << "Skipping making " << name << "/" << cfName << " persistent." << LogIO::WARN << LogIO::POST;
    	return;
      }

    // if (fileName_p != "" )name = "test.cf/"+fileName_p;
    String tt=fileName_p;

    if (fileName_p == "") fileName_p = String(cfName);
    name = String(dir) + "/" + fileName_p;
    // log_l << "Making " << name << " persistent. Was " << tt << LogIO::WARN << LogIO::POST;
    //    storeArrayAsImage(name, coordSys_p, *storage_p);

    //    PagedImage<Complex> thisCF(storage_p->shape(),coordSys_p, name);

    IPosition tmpShape;
    tmpShape=shape_p;
    // Zero storage buffer is a hint that this may be an "empty CFs"
    // being made persistent.  So make a 2x2 pixel image (since I
    // don't know how to make a 0x0 pixel image to just save the CS
    // and miscInfo information to the disk).
    if ((storage_p->shape()).product()==0) tmpShape[0]=tmpShape[1]=2;

    PagedImage<Complex> thisCF(tmpShape,coordSys_p, name);
    //cerr << "thisCF.shape() = " << thisCF.shape() << " " << tmpShape << " " << storage_p->shape() << endl;

    //    cerr << "Ref pixel = " << coordSys_p.referencePixel() << endl;
    //    show(NULL,cerr);
    //cerr << storage_p->shape() << endl;

    if ((storage_p->shape()).nelements()>0) thisCF.put(*storage_p);

    Record miscinfo;
    miscinfo.define("Xsupport", xSupport_p);
    miscinfo.define("Ysupport", ySupport_p);
    miscinfo.define("Sampling", sampling_p);
    miscinfo.define("ParallacticAngle",pa_p.getValue("deg"));
    miscinfo.define("MuellerElement", muellerElement_p);
    miscinfo.define("WValue", wValue_p);
    miscinfo.define("WIncr", wIncr_p);
    miscinfo.define("Name", fileName_p);
    miscinfo.define("ConjFreq", conjFreq_p);
    miscinfo.define("ConjPoln", conjPoln_p);
    miscinfo.define("TelescopeName", telescopeName_p);
    miscinfo.define("BandName", bandName_p);
    miscinfo.define("Diameter", diameter_p);
    miscinfo.define("OpCode",isRotationallySymmetric_p);
    thisCF.setMiscInfo(miscinfo);

    //show("thisCell: ", cout);
  }

  void CFCell::clear()
  {
    getStorage()->resize();
    cfShape_p.resize(0);
  }

} // end casa namespace



