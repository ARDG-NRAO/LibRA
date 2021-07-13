//# VisCalGlobals.cc: Implementation of VisCalGlobals
//# Copyright (C) 1996,1997,2000,2001,2002,2003
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

#include <synthesis/MeasurementComponents/VisCalGlobals.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <synthesis/MeasurementComponents/DJones.h>
#include <synthesis/MeasurementComponents/XJones.h>
#include <synthesis/MeasurementComponents/GSpline.h>
#include <synthesis/MeasurementComponents/BPoly.h>
#include <synthesis/MeasurementComponents/EJones.h>
#include <synthesis/MeasurementComponents/EPJones.h>
#include <synthesis/MeasurementComponents/FJones.h>
#include <synthesis/MeasurementComponents/FringeJones.h>
#include <synthesis/MeasurementComponents/AccorJones.h>
#include <synthesis/MeasurementComponents/KJones.h>
#include <synthesis/MeasurementComponents/LJJones.h>
#include <synthesis/MeasurementComponents/AMueller.h>
#include <synthesis/MeasurementComponents/TsysGainCal.h>
#include <synthesis/MeasurementComponents/EVLASwPow.h>
#include <synthesis/MeasurementComponents/SingleDishSkyCal.h>
#include <synthesis/MeasurementComponents/SDDoubleCircleGainCal.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


// Create a specialized VisCal from VisSet
VisCal* createVisCal(const String& type, VisSet& vs) {

  String uptype=type;
  uptype.upcase();

  if (uptype=="P" || uptype=="P JONES") 
    return new PJones(vs);

  else
    // Try request as a solvable type
    return createSolvableVisCal(type,vs);
};

// Create a specialized VisCal from msname, nAnt, nSpw
VisCal* createVisCal(const String& type, String msname, Int MSnAnt, Int MSnSpw) {

  String uptype=type;
  uptype.upcase();

  if (uptype=="P" || uptype=="P JONES") 
    return new PJones(msname,MSnAnt,MSnSpw);

  else
    // Try request as a solvable type
    return createSolvableVisCal(type,msname,MSnAnt,MSnSpw);
};

// Create a specialized VisCal from MSMetaInfoForCal
VisCal* createVisCal(const String& type, const MSMetaInfoForCal& msmc) {

  String uptype=type;
  uptype.upcase();

  if (uptype=="P" || uptype=="P JONES") 
    return new PJones(msmc);

  else
    // Try request as a solvable type
    return createSolvableVisCal(type,msmc);
};


// Create a specialized VisCal from nAnt
VisCal* createVisCal(const String& type, const Int& nAnt) {

  // TBD: handle P w/out an MS?
  //  if (type=="P") 
  //    return new PJones(nAnt);
  //  else
    // Try request as a solvable type
    return createSolvableVisCal(type,nAnt);
};


// Create a specialized SolvableVisCal from VisSet
SolvableVisCal* createSolvableVisCal(const String& type, VisSet& vs) {

  String uptype=type;
  uptype.upcase();

  if      (uptype=="B" || uptype=="B JONES") 
    return new BJones(vs);

  else if (uptype=="BPOLY") 
    return new BJonesPoly(vs);

  else if (uptype=="G" || uptype=="G JONES") 
    return new GJones(vs);

  else if (uptype=="GSPLINE") 
    return new GJonesSpline(vs);
  
  else if (uptype=="TF" || uptype=="TF JONES" || uptype=="TF NOISE") 
    return new TfJones(vs);

  else if (uptype=="T" || uptype=="T JONES" || uptype=="T NOISE") 
    return new TJones(vs);

  else if (uptype.before('+')=="DLIN" ||
	   uptype.before('+')=="D" || 
	   uptype=="D JONES") 
    return new DlinJones(vs);

  else if (uptype.before('+')=="DFLIN" || 
	   uptype.before('+')=="DF" || 
	   uptype=="DF JONES") 
    return new DflinJones(vs);

  else if (uptype.before('+')=="DLLS" || 
	   uptype=="DLLS JONES")
    return new DllsJones(vs);

  else if (uptype.before('+')=="DFLLS" || 
	   uptype=="DFLLS JONES")
    return new DfllsJones(vs);

  else if (uptype.before('+')=="DGEN" ||
	   uptype.before('+')=="DGENERAL" || 
	   uptype=="DGEN JONES") 
    return new DJones(vs);

  else if (uptype.before('+')=="DFGEN" || 
	   uptype.before('+')=="DFGENERAL" || 
	   uptype=="DFGEN JONES") 
    return new DfJones(vs);

  else if (uptype=="J" || uptype=="J JONES") 
    return new JJones(vs);

  else if (uptype=="JF" || uptype=="JF JONES")
    return new JfJones(vs);

  else if (uptype == "EP" || uptype == "EP JONES")
    return new EPJones(vs);

  else if (uptype == "LJ" || uptype == "LJ JONES")
    return new LJJones(vs);

  else if (uptype=="M" || uptype=="M MUELLER")
    return new MMueller(vs);

  else if (uptype=="A" || uptype=="A MUELLER")
    return new AMueller(vs);

  else if (uptype=="N" || uptype=="A NOISE")
    return new ANoise(vs);

  else if (uptype=="MF" || uptype=="MF MUELLER")
    return new MfMueller(vs);
     
  else if (uptype=="X" || uptype=="X MUELLER")
    return new XMueller(vs);

  else if (uptype=="XJ" || uptype=="X JONES")
    return new XJones(vs);

  else if (uptype=="XF" || uptype=="XF JONES")
    return new XfJones(vs);

  else if (uptype=="K" || uptype=="K JONES")
    return new KJones(vs);

  else if (uptype=="KCROSS" || uptype=="KCROSS JONES")
    return new KcrossJones(vs);

  else if (uptype=="GLINXPH" || uptype=="GLINXPH JONES" ||
	   uptype=="XY+QU")
    return new GlinXphJones(vs);

  else if (uptype=="GLINXPHF" || uptype=="GLINXPHF JONES" ||
	   uptype=="XYF+QU")
    return new GlinXphfJones(vs);

  else if (uptype=="KMBD" || uptype=="KMBD JONES")
    return new KMBDJones(vs);

  else if (uptype.contains("KANTPOS") || uptype.contains("KANTPOS JONES"))
    return new KAntPosJones(vs);

  else if (uptype.contains("TSYS"))
    return new StandardTsys(vs);

  else if (uptype.contains("EVLASWP"))
    return new EVLASwPow(vs);

  else if (uptype=="TFOPAC")  // Not yet solvable (even though an SVJ)
    return new TfOpac(vs);

  else if (uptype=="TOPAC")  // Not yet solvable (even though an SVJ)
    return new TOpac(vs);

  else if (uptype.contains("GAINCURVE"))  // Not yet solvable (even though an SVJ)
    return new EGainCurve(vs);
  
  else if (uptype.contains("POWERCURVE"))  // Not yet solvable (even though an SVJ)
    return new EPowerCurve(vs);
  
  else if (uptype.contains("EVLAGAIN"))
    throw(AipsError("Please regenerate EVLA Sw Pow table using gencal."));

  else if (uptype.contains("TEC") || uptype.contains("F JONES") )  // Ionosphere
    return new FJones(vs);

  else if (uptype.contains("FRINGE"))   // Fringe-fitting
    return new FringeJones(vs);

  else if (uptype.contains("ACCOR"))
    return new AccorJones(vs);

  else if (uptype.contains("SDSKY_PS"))
    return new SingleDishPositionSwitchCal(vs);

  else if (uptype.contains("SDSKY_RASTER"))
    return new SingleDishRasterCal(vs);

  else if (uptype.contains("SDSKY_OTF"))
    return new SingleDishOtfCal(vs);
  
  else if (uptype.contains("SDGAIN_OTFD"))
    return new SDDoubleCircleGainCal(vs);

  else {
    throw(AipsError("Unknown calibration type: '"+type+"'"));
  }
};


// Create a specialized VisCal from msname, nAnt, nSpw
SolvableVisCal* createSolvableVisCal(const String& type, String msname, Int MSnAnt, Int MSnSpw) {
  String uptype=type;
  uptype.upcase();

  if      (uptype=="B" || uptype=="B JONES") 
    return new BJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="BPOLY") 
    throw(AipsError(uptype+" not yet supported via BJonesPoly(msname,MSnAnt,MSnSpw)"));
  //    return new BJonesPoly(msname,MSnAnt,MSnSpw);

  else if (uptype=="G" || uptype=="G JONES") 
    return new GJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="GSPLINE") 
    throw(AipsError(uptype+" not yet supported via GJonesSpline(msname,MSnAnt,MSnSpw)"));
  //return new GJonesSpline(msname,MSnAnt,MSnSpw);
  
  else if (uptype=="TF" || uptype=="TF JONES" || uptype=="TF NOISE") 
    return new TfJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="T" || uptype=="T JONES" || uptype=="T NOISE") 
    return new TJones(msname,MSnAnt,MSnSpw);

  else if (uptype.before('+')=="DLIN" ||
	   uptype.before('+')=="D" || 
	   uptype=="D JONES") 
    return new DlinJones(msname,MSnAnt,MSnSpw);

  else if (uptype.before('+')=="DFLIN" || 
	   uptype.before('+')=="DF" || 
	   uptype=="DF JONES") 
    return new DflinJones(msname,MSnAnt,MSnSpw);

  else if (uptype.before('+')=="DLLS" || 
	   uptype=="DLLS JONES")
    return new DllsJones(msname,MSnAnt,MSnSpw);

  else if (uptype.before('+')=="DFLLS" || 
	   uptype=="DFLLS JONES")
    return new DfllsJones(msname,MSnAnt,MSnSpw);

  else if (uptype.before('+')=="DGEN" ||
	   uptype.before('+')=="DGENERAL" || 
	   uptype=="DGEN JONES") 
    return new DJones(msname,MSnAnt,MSnSpw);

  else if (uptype.before('+')=="DFGEN" || 
	   uptype.before('+')=="DFGENERAL" || 
	   uptype=="DFGEN JONES") 
    return new DfJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="J" || uptype=="J JONES") 
    return new JJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="JF" || uptype=="JF JONES")
    return new JfJones(msname,MSnAnt,MSnSpw);

  else if (uptype == "EP" || uptype == "EP JONES")
    throw(AipsError(uptype+" not yet supported via EPJones(msname,MSnAnt,MSnSpw)"));
  //    return new EPJones(msname,MSnAnt,MSnSpw);

  else if (uptype == "LJ" || uptype == "LJ JONES")
    throw(AipsError(uptype+" not yet supported via LJJones(msname,MSnAnt,MSnSpw)"));
  //    return new LJJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="M" || uptype=="M MUELLER")
    return new MMueller(msname,MSnAnt,MSnSpw);

  else if (uptype=="A" || uptype=="A MUELLER")
    return new AMueller(msname,MSnAnt,MSnSpw);

  else if (uptype=="N" || uptype=="A NOISE")
    return new ANoise(msname,MSnAnt,MSnSpw);

  else if (uptype=="MF" || uptype=="MF MUELLER")
    return new MfMueller(msname,MSnAnt,MSnSpw);
     
  else if (uptype=="X" || uptype=="X MUELLER")
    return new XMueller(msname,MSnAnt,MSnSpw);

  else if (uptype=="XJ" || uptype=="X JONES")
    return new XJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="XF" || uptype=="XF JONES")
    return new XfJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="K" || uptype=="K JONES")
    return new KJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="KCROSS" || uptype=="KCROSS JONES")
    return new KcrossJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="GLINXPH" || uptype=="GLINXPH JONES" ||
	   uptype=="XY+QU")
    return new GlinXphJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="GLINXPHF" || uptype=="GLINXPHF JONES" ||
	   uptype=="XYF+QU")
    return new GlinXphfJones(msname,MSnAnt,MSnSpw);

  else if (uptype=="KMBD" || uptype=="KMBD JONES")
    return new KMBDJones(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("KANTPOS") || uptype.contains("KANTPOS JONES"))
    return new KAntPosJones(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("TSYS"))
    return new StandardTsys(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("EVLASWP"))
    return new EVLASwPow(msname,MSnAnt,MSnSpw);

  else if (uptype=="TFOPAC")  // Not yet solvable (even though an SVJ)
    return new TfOpac(msname,MSnAnt,MSnSpw);

  else if (uptype=="TOPAC")  // Not yet solvable (even though an SVJ)
    return new TOpac(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("GAINCURVE"))  // Not yet solvable (even though an SVJ)
    return new EGainCurve(msname,MSnAnt,MSnSpw);
  
  else if (uptype.contains("POWERCURVE"))  // Not yet solvable (even though an SVJ)
    return new EPowerCurve(msname,MSnAnt,MSnSpw);
  
  else if (uptype.contains("EVLAGAIN"))
    throw(AipsError("Please regenerate EVLA Sw Pow table using gencal."));

  else if (uptype.contains("TEC") || uptype.contains("F JONES") )  // Ionosphere
    return new FJones(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("FRINGE"))  // Fringe-fitting
    return new FringeJones(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("ACCOR"))
    return new AccorJones(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("SDSKY_PS"))
    throw(AipsError(uptype+" not yet supported via SingleDishPositionSwitchCal(msname,MSnAnt,MSnSpw)"));
  //    return new SingleDishPositionSwitchCal(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("SDSKY_RASTER"))
    throw(AipsError(uptype+" not yet supported via SingleDishRasterCal(msname,MSnAnt,MSnSpw)"));
  //    return new SingleDishRasterCal(msname,MSnAnt,MSnSpw);

  else if (uptype.contains("SDSKY_OTF"))
    throw(AipsError(uptype+" not yet supported via SingleDishOtfCal(msname,MSnAnt,MSnSpw)"));
  //    return new SingleDishOtfCal(msname,MSnAnt,MSnSpw);
  
  else {
    throw(AipsError("Unknown calibration type: '"+type+"'"));
  }
};


// Create a specialized VisCal from MSMetaInfoForCal
SolvableVisCal* createSolvableVisCal(const String& type, const MSMetaInfoForCal& msmc) {
  String uptype=type;
  uptype.upcase();

  if      (uptype=="B" || uptype=="B JONES") 
    return new BJones(msmc);

  else if (uptype=="BPOLY") 
    //throw(AipsError(uptype+" not yet supported via BJonesPoly(msmc)"));
    return new BJonesPoly(msmc);

  else if (uptype=="G" || uptype=="G JONES") 
    return new GJones(msmc);

  else if (uptype=="GSPLINE") 
    //throw(AipsError(uptype+" not yet supported via GJonesSpline(msmc)"))
    return new GJonesSpline(msmc);
  
  else if (uptype=="TF" || uptype=="TF JONES" || uptype=="TF NOISE") 
    return new TfJones(msmc);

  else if (uptype=="T" || uptype=="T JONES" || uptype=="T NOISE") 
    return new TJones(msmc);

  else if (uptype.before('+')=="DLIN" ||
	   uptype.before('+')=="D" || 
	   uptype=="D JONES") 
    return new DlinJones(msmc);

  else if (uptype.before('+')=="DFLIN" || 
	   uptype.before('+')=="DF" || 
	   uptype=="DF JONES") 
    return new DflinJones(msmc);

  else if (uptype.before('+')=="DLLS" || 
	   uptype=="DLLS JONES")
    return new DllsJones(msmc);

  else if (uptype.before('+')=="DFLLS" || 
	   uptype=="DFLLS JONES")
    return new DfllsJones(msmc);

  else if (uptype.before('+')=="DGEN" ||
	   uptype.before('+')=="DGENERAL" || 
	   uptype=="DGEN JONES") 
    return new DJones(msmc);

  else if (uptype.before('+')=="DFGEN" || 
	   uptype.before('+')=="DFGENERAL" || 
	   uptype=="DFGEN JONES") 
    return new DfJones(msmc);

  else if (uptype=="J" || uptype=="J JONES") 
    return new JJones(msmc);

  else if (uptype=="JF" || uptype=="JF JONES")
    return new JfJones(msmc);

  else if (uptype == "EP" || uptype == "EP JONES")
    throw(AipsError(uptype+" not yet supported via EPJones(msmc)"));
  //    return new EPJones(msmc);

  else if (uptype == "LJ" || uptype == "LJ JONES")
    throw(AipsError(uptype+" not yet supported via LJJones(msmc)"));
  //    return new LJJones(msmc);

  else if (uptype=="M" || uptype=="M MUELLER")
    return new MMueller(msmc);

  else if (uptype=="A" || uptype=="A MUELLER")
    return new AMueller(msmc);

  else if (uptype=="N" || uptype=="A NOISE")
    return new ANoise(msmc);

  else if (uptype=="MF" || uptype=="MF MUELLER")
    return new MfMueller(msmc);
     
  else if (uptype=="X" || uptype=="X MUELLER")
    return new XMueller(msmc);

  else if (uptype=="XJ" || uptype=="X JONES")
    return new XJones(msmc);

  else if (uptype=="XF" || uptype=="XF JONES")
    return new XfJones(msmc);

  else if (uptype=="K" || uptype=="K JONES")
    return new KJones(msmc);

  else if (uptype=="KCROSS" || uptype=="KCROSS JONES")
    return new KcrossJones(msmc);

  else if (uptype=="GLINXPH" || uptype=="GLINXPH JONES" ||
	   uptype=="XY+QU")
    return new GlinXphJones(msmc);

  else if (uptype=="GLINXPHF" || uptype=="GLINXPHF JONES" ||
	   uptype=="XYF+QU")
    return new GlinXphfJones(msmc);

  else if (uptype=="XPARANG" || uptype=="XPARANG JONES" ||
	   uptype=="XPARANG+QU")
    return new XparangJones(msmc);

  else if (uptype=="XFPARANG" || uptype=="XFPARANG JONES" ||
	   uptype=="XFPARANG+QU")
    return new XfparangJones(msmc);

  else if (uptype=="POSANG" || uptype=="POSANG JONES")
    return new PosAngJones(msmc);

  else if (uptype=="KMBD" || uptype=="KMBD JONES")
    return new KMBDJones(msmc);

  else if (uptype.contains("KANTPOS") || uptype.contains("KANTPOS JONES"))
    return new KAntPosJones(msmc);

  else if (uptype.contains("TSYS"))
    return new StandardTsys(msmc);

  else if (uptype.contains("EVLASWP"))
    return new EVLASwPow(msmc);

  else if (uptype=="TFOPAC")  // Not yet solvable (even though an SVJ)
    return new TfOpac(msmc);

  else if (uptype=="TOPAC")  // Not yet solvable (even though an SVJ)
    return new TOpac(msmc);

  else if (uptype.contains("GAINCURVE"))  // Not yet solvable (even though an SVJ)
    return new EGainCurve(msmc);
  
  else if (uptype.contains("POWERCURVE"))  // Not yet solvable (even though an SVJ)
    return new EPowerCurve(msmc);
  
  else if (uptype.contains("EVLAGAIN"))
    throw(AipsError("Please regenerate EVLA Sw Pow table using gencal."));

  else if (uptype.contains("TEC") || uptype.contains("F JONES") )  // Ionosphere
    return new FJones(msmc);

  else if (uptype.contains("FRINGE"))  // Fringe-fitting
    return new FringeJones(msmc);

  else if (uptype.contains("ACCOR"))
    return new AccorJones(msmc);

  else if (uptype.contains("SDSKY_PS"))
    return new SingleDishPositionSwitchCal(msmc);

  else if (uptype.contains("SDSKY_RASTER"))
    return new SingleDishRasterCal(msmc);

  else if (uptype.contains("SDSKY_OTF"))
    throw(AipsError(uptype+" not yet supported via SingleDishOtfCal(msmc)"));
  //return new SingleDishOtfCal(msmc);
  
  else if (uptype.contains("SDGAIN_OTFD"))
    return new SDDoubleCircleGainCal(msmc);
  
  else {
    throw(AipsError("Unknown calibration type: '"+type+"'"));
  }
};

// Create a specialized SolvableVisCal from nAnt
SolvableVisCal* createSolvableVisCal(const String& type, const Int& nAnt) {

  if      (type=="B") 
    return new BJones(nAnt);

  else if (type=="G") 
    return new GJones(nAnt);

  else if (type=="T") 
    return new TJones(nAnt);
  
  else if (type=="D") 
    return new DJones(nAnt);

  else if (type=="Df") 
    return new DfJones(nAnt);

  else if (type=="J") 
    return new JJones(nAnt);
     
  else {
    throw(AipsError("Unknown calibration type: '"+type+"'"));
  }
};


namespace viscal {

// Parameter axis slicer for caltables
//   (experimental)
Slice calParSlice(String caltabname, String what, String pol)
{
  String caltype=calTableType(caltabname);  // From SolvableVisCal.h (for now)
  return calParSliceByType(caltype,what,pol);
}

Slice calParSliceByType(String caltype, String what, String pol)
{
  // Upcase all inputs
  //  TBD: do this with local copies
  caltype.upcase();
  what.upcase();
  pol.upcase();
  Int s(0),n(0),i(1);

  if (caltype=="B TSYS") {
    if (what=="TSYS") {
      s=0;
      i=1;
    }
    else
      throw(AipsError("Unsupported value type: "+what));
  }
  else if (caltype=="TOPAC") {
    if (what=="OPAC") {
      if (pol=="")
        return Slice(0,1,1); // trivial
      else
        throw(AipsError("Can't select with pol='"+pol+"' on unpolarized OPAC table."));
    }
    else
      throw(AipsError("Unsupported value type: "+what));
  }
  else if (caltype.contains("EVLASWP")) {
      // caltype = "G EVLASWPOW" so put before "G" !
    i=2;  // 2 pars-per-pol
    if ((what=="GAINAMP") || (what=="SWPOWER"))
      s=0;
    else if (what=="TSYS")
      s=1;
    else
      throw(AipsError("Unsupported value type: "+what));
  }
  else if ((caltype[0]=='G') ||
            caltype[0]=='B' ||
            caltype[0]=='D') {
    if (what=="AMP" ||
        what=="PHASE" ||
        what=="REAL" ||
        what=="IMAG") {
      s=0;  // nominal start is first pol
      i=1;  // increment is nominally 1 (only good for 1-par-per-pol caltypes
    }
    else
      throw(AipsError("Unsupported value type: "+what));
  } 
  else if ((caltype[0]=='T') || (caltype[0]=='X' && !caltype.contains("MUELLER"))) {
    if (what=="AMP" ||
        what=="PHASE" ||
        what=="REAL" ||
        what=="IMAG") {
      if (pol=="")
         return Slice(0,1,1); // trivial
      else
         throw(AipsError("Can't select with pol='"+pol+"' on unpolarized table."));
    }
    else
      throw(AipsError("Unsupported value type: "+what));
  }
  else if (caltype[0]=='A') {
    if (what=="AMP" ||
        what=="PHASE" ||
        what=="REAL" ||
        what=="IMAG") {
      s=0;  // nominal start is first pol
      i=1;  // increment is nominally 1 (only good for 1-par-per-pol caltypes
    }
    else
      throw(AipsError("Unsupported value type: "+what));
  } 
  else if (caltype[0]=='K') {
    if (caltype=="KANTPOS JONES") {
      // antenna x,y,z corrections not pol
      if (pol=="")
        return Slice(0,3,1);
      else
        throw(AipsError("Can't select with pol='"+pol+"' on antpos table."));
    } else {
      if (what=="DELAY") {
        s=0;
        i=1;
      } else {
        throw(AipsError("Unsupported value type: "+what));
      }
    }
  }
  else if (caltype[0]=='F') {
    if (caltype.contains("FRINGE")) {
      i = 4;
      if (what=="PHASE") {
        s = 0;
      } else if (what=="DELAY") {
        s = 1;
      } else if (what=="RATE") {
        s = 2;
      } else if (what=="DISP") {
        s = 3;
      } else { 
        throw(AipsError("Unsupported value type: "+what));
      }
    } else if (what=="TEC") {
      if (pol=="")
        return Slice(0,1,1); // trivial
      else
        throw(AipsError("Can't select with pol='"+pol+"' on unpolarized TEC table."));
    }
    else
      throw(AipsError("Unsupported value type: "+what));
  }
  else if (caltype.contains("SDSKY") || caltype.contains("SDGAIN")) {
    if ((what=="AMP") || (what=="REAL")) {
      s=0;  // nominal start is first pol
      i=1;  // increment is nominally 1 (only good for 1-par-per-pol caltypes
    } else { 
      throw(AipsError("Unsupported value type: "+what));
    }
  }
  else if (caltype=="EGAINCURVE") {
    if (what=="AMP" ||
        what=="PHASE" ||
        what=="REAL" ||
        what=="IMAG") {
      if (pol=="")
        return Slice(0,8,1);
    }
  }
  else if (caltype=="EPOWERCURVE") {
    if (what=="AMP" ||
        what=="PHASE" ||
        what=="REAL" ||
        what=="IMAG") {
      if (pol=="")
        return Slice(0,16,1);
    }
  }
  else
    throw(AipsError("Unsupported cal type: "+caltype));

  // Apply generalized pol selection
  if (pol=="L" ||
      pol=="Y") {
    s+=i; // jump by incr to 2nd pol
    n=1;
  }
  else if (pol=="R" || pol=="X") {
    n=1;  // for _all_ single-pol selections
  }
  else if (pol=="" || pol=="RL" || pol=="XY") {
    n=2;  // both pols
  }
  else {
    throw(AipsError("Unsupported pol: "+pol));
  }

  return Slice(s,n,i);

}

} //# NAMESPACE VISCAL - END


} //# NAMESPACE CASA - END
