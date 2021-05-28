//# BeamCalc.cc: Implementation for BeamCalc
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

//#include <stdio.h>
//#include <complex.h>
#include <casa/math.h>
#include <math.h>
//#include <stdlib.h>
//#include <string.h>
#include <images/Images/TempImage.h>
#include <synthesis/MeasurementEquations/AntennaResponses.h>
#include <tables/Tables/TableProxy.h>
#include <casa/Exceptions.h>
#include <casa/Containers/ValueHolder.h>
#include <casa/Arrays/Array.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <casa/OS/Timer.h>
#include <casa/System/AppState.h>
#include <casa/OS/Directory.h>
#include <casatools/Config/State.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 4))
#define GCC44x 1
#else
#define GCC44x 0
#endif


using namespace std;
using namespace casacore;
namespace casa{

  const Double BeamCalc::METER_INCH  = 39.37008;
  const Double BeamCalc::INCH_METER  = (1.0/BeamCalc::METER_INCH);
  const Double BeamCalc::NS_METER    =  0.299792458;     // Exact 
  const Double BeamCalc::METER_NS    = (1.0/BeamCalc::NS_METER);
  const Double BeamCalc::DEG_RAD     = M_PI/180.0;
  const Double BeamCalc::RAD_DEG     = 180.0/M_PI;

  BeamCalc* BeamCalc::instance_p = 0;

  BeamCalc::BeamCalc():
    obsName_p(""),
    antType_p(""),
    obsTime_p(),
    BeamCalc_NumBandCodes_p(0),
    BeamCalcGeometries_p(0),
    bandMinFreq_p(0),
    bandMaxFreq_p(0),
    antRespPath_p(""){
  }

  BeamCalc* BeamCalc::Instance(){
    if(instance_p==0){
      instance_p = new BeamCalc();
    }
    return instance_p;
  }

  // initialise the beam calculation parameters 
  void BeamCalc::setBeamCalcGeometries(const String& obsName,
				       const String& antType,
				       const MEpoch& obsTime,
				       const String& otherAntRayPath){
    
    Unit uS("s");
    Bool verbose = false;


    if(obsName==obsName_p 
       && antType==antType_p 
       && obsTime.get(uS).getValue()==obsTime_p.get(uS).getValue()
       && otherAntRayPath.empty()
       ){
      return; // nothing to do (assuming the databases haven't changed)
    }

    cout << "Processing request for geometries from observatory " << obsName << ", antenna type " << antType << endl;

    LogIO os;
    os << LogOrigin("BeamCalc", "setBeamCalcGeometries()");

    if(obsName!=""){
      obsName_p = obsName;
    }
    if(antType!=""){
      antType_p = antType;
    }
    obsTime_p = obsTime;
    
    
    BeamCalcGeometries_p.resize(0);
    
    AntennaResponses aR;
    String antRespPath;
    String antRayPath = otherAntRayPath;

    Bool useInternal = false;

    os <<  LogIO::NORMAL << "Initialisation of geometries for observatory " << obsName_p 
       << ", antenna type " << antType_p << LogIO::POST;

    if(otherAntRayPath.empty()){
      if(!MeasTable::AntennaResponsesPath(antRespPath, obsName_p)) {
	useInternal = true;
      }
      else{
	if(!aR.init(antRespPath)){
	  // init failed
	  String mesg="Initialisation of antenna response parameters for observatory "
	    +obsName_p+" failed using path "+antRespPath;
	  SynthesisError err(mesg);
	  throw(err);
	}
	uInt respImageChannel;
	MFrequency respImageNomFreq;
	AntennaResponses::FuncTypes respImageFType;
	MVAngle respImageRotOffset;
    
	if(!aR.getImageName(antRayPath,
			    respImageChannel,
			    respImageNomFreq,
			    respImageFType,
			    respImageRotOffset,
			    //
			    obsName_p,
			    obsTime_p,
			    MFrequency(Quantity(0.,Unit("Hz")), MFrequency::TOPO), // any frequency
			    AntennaResponses::INTERNAL,
			    antType_p
			    )
	   ){ // no matching response found
	  os <<  LogIO::NORMAL << "No matching antenna response found for observatory "
	     << obsName_p  << LogIO::POST;
	  useInternal = true;
	}
      }

      if(useInternal){

        Bool found = False;
        String fullFileName;
        const std::list<std::string> &data_path = AppStateSource::fetch( ).dataPath( );
        const std::string distrodata_path =casatools::get_state().distroDataPath( );
        //cerr<<"distrodata_path="<<distrodata_path<<endl; 
        //cerr<<"DATA PATH==="<< *data_path <<endl;
        // The data path search need to be rewritten to adopt the recommanded setting via python
        // file for CASA6. 
        // For now, only the first path that actually exist will be set to the data path (TT 2018.12.10)
        if (data_path.size() > 0 ) {
          for ( std::list<std::string>::const_iterator it=data_path.begin(); ! found && it != data_path.end(); ++it ) {
            Path lpath = Path(*it);
            //os<<"Here to datapath="<<lpath<<LogIO::POST;
            //Path lpath = Path(data_path);
            String slpath = lpath.absoluteName();
            String subdirname;
            if(obsName_p=="VLA" || obsName_p=="EVLA") {
                subdirname="/nrao/VLA";
            }
            else if(obsName_p=="ALMA"){
                subdirname="/alma/response";
            }
            //Directory ddir(slpath+subdirname);
            try {
               Directory ddir(slpath+subdirname);
               ddir.exists();
               found = True;
               fullFileName=slpath;
               break;
            }
            catch (...)  {
            }
           
            //if  (ddir.exists()) {
            //  cerr<<" ddir exists:"<<slpath<<subdirname<<endl;
            //  found = True;
            //  fullFileName=slpath;
            //  break;
            //}
          }
          if (!found && distrodata_path!="") {
             fullFileName = distrodata_path;
             found = True;
          }
        } 
        else if(!found) {
          const char *sep=" ";
          char *aipsPath = strtok(getenv("CASAPATH"),sep);
          if (aipsPath == NULL)
            throw(SynthesisError("CASAPATH not found."));
          fullFileName=aipsPath;
          fullFileName+="/data";
        }


	if(obsName_p=="VLA" && antType_p=="STANDARD"){
	  os <<  LogIO::NORMAL << "Will use default geometries for VLA STANDARD." << LogIO::POST;
	  BeamCalc_NumBandCodes_p = VLABeamCalc_NumBandCodes;
	  BeamCalcGeometries_p.resize(BeamCalc_NumBandCodes_p);
	  bandMinFreq_p.resize(BeamCalc_NumBandCodes_p);
	  bandMaxFreq_p.resize(BeamCalc_NumBandCodes_p);
	  for(uInt i=0; i<BeamCalc_NumBandCodes_p; i++){
	    copyBeamCalcGeometry(&BeamCalcGeometries_p[i], &VLABeamCalcGeometryDefaults[i]);
	    bandMinFreq_p[i] = VLABandMinFreqDefaults[i]; 
	    bandMaxFreq_p[i] = VLABandMaxFreqDefaults[i]; 
	  }
	  //antRespPath_p = fullFileName + "/data/nrao/VLA";
	  antRespPath_p = fullFileName + "/nrao/VLA";
	}
	else if(obsName_p=="EVLA" && antType_p=="STANDARD"){
	  os <<  LogIO::NORMAL << "Will use default geometries for EVLA STANDARD." << LogIO::POST;
	  BeamCalc_NumBandCodes_p = EVLABeamCalc_NumBandCodes;
	  BeamCalcGeometries_p.resize(BeamCalc_NumBandCodes_p);
	  bandMinFreq_p.resize(BeamCalc_NumBandCodes_p);
	  bandMaxFreq_p.resize(BeamCalc_NumBandCodes_p);
	  for(uInt i=0; i<BeamCalc_NumBandCodes_p; i++){
	    copyBeamCalcGeometry(&BeamCalcGeometries_p[i], &EVLABeamCalcGeometryDefaults[i]);
	    bandMinFreq_p[i] = EVLABandMinFreqDefaults[i]; 
	    bandMaxFreq_p[i] = EVLABandMaxFreqDefaults[i]; 
	  }
	  //antRespPath_p = fullFileName + "/data/nrao/VLA";
	  antRespPath_p = fullFileName + "/nrao/VLA";
	}
	else if(obsName_p=="ALMA" && (antType_p=="DA" || antType_p=="DV" || antType_p=="PM")){
	  os <<  LogIO::NORMAL << "Will use default geometries for ALMA DA, DV, and PM." << LogIO::POST;
	  BeamCalc_NumBandCodes_p = ALMABeamCalc_NumBandCodes;
	  BeamCalcGeometries_p.resize(BeamCalc_NumBandCodes_p);
	  bandMinFreq_p.resize(BeamCalc_NumBandCodes_p);
	  bandMaxFreq_p.resize(BeamCalc_NumBandCodes_p);
	  for(uInt i=0; i<BeamCalc_NumBandCodes_p; i++){
	    copyBeamCalcGeometry(&BeamCalcGeometries_p[i], &ALMABeamCalcGeometryDefaults[i]);
	    if(antType_p=="DA"){
	      BeamCalcGeometries_p[i].legwidth *= -1.; // change from + to x shape
	    } 
	    bandMinFreq_p[i] = ALMABandMinFreqDefaults[i]; 
	    bandMaxFreq_p[i] = ALMABandMaxFreqDefaults[i]; 
	  }
	  //antRespPath_p = fullFileName + "/data/alma/responses";
	  antRespPath_p = fullFileName + "/alma/responses";
	}
	else{
	  String mesg="We don't have any antenna ray tracing parameters available for observatory "
	    +obsName_p+", antenna type "+antType_p;
	  SynthesisError err(mesg);
	  throw(err);
	}
	return;  
      } // end if(useInternal)
    }
    
    
    os <<  LogIO::NORMAL << "from file " << antRayPath << endl;
    
    try {
      // read temp table from ASCII file
      TableProxy antParTab = TableProxy(antRayPath, String(""), String("tempRayTraceTab.tab"), 
					false, IPosition(), // autoheader, autoshape 
					String(" "), // separator 
					String("#"), // comment marker
					0,-1, // first and last line 
					Vector<String>(), Vector<String>());

      antParTab.deleteTable(true); // table will be deleted when it goes out of scope
      
      // read the table
      uInt nRows = antParTab.nrows();
      BeamCalc_NumBandCodes_p = nRows;

      BeamCalcGeometries_p.resize(BeamCalc_NumBandCodes_p);
      bandMinFreq_p.resize(BeamCalc_NumBandCodes_p);
      bandMaxFreq_p.resize(BeamCalc_NumBandCodes_p);

      for(uInt i=0; i<BeamCalc_NumBandCodes_p; i++){
 	sprintf(BeamCalcGeometries_p[i].name, antParTab.getCell("NAME", i).asString().c_str());
	bandMinFreq_p[i] = antParTab.getCell("MINFREQ", i).asDouble() * 1E9; // expect GHz 
	bandMaxFreq_p[i] = antParTab.getCell("MAXFREQ", i).asDouble() * 1E9;
	BeamCalcGeometries_p[i].sub_h = antParTab.getCell("SUB_H", i).asDouble();
	Array<Double> ta1;
	ta1.assign(antParTab.getCell("FEEDPOS", i).asArrayDouble());
	for(uInt j=0; j<3;j++){
	  BeamCalcGeometries_p[i].feedpos[j] = ta1(IPosition(1,j));
	}
	BeamCalcGeometries_p[i].subangle = antParTab.getCell("SUBANGLE", i).asDouble();
	BeamCalcGeometries_p[i].legwidth = antParTab.getCell("LEGWIDTH", i).asDouble();
	BeamCalcGeometries_p[i].legfoot = antParTab.getCell("LEGFOOT", i).asDouble();
	BeamCalcGeometries_p[i].legapex = antParTab.getCell("LEGAPEX", i).asDouble();
	BeamCalcGeometries_p[i].Rhole = antParTab.getCell("RHOLE", i).asDouble();
	BeamCalcGeometries_p[i].Rant = antParTab.getCell("RANT", i).asDouble();
	BeamCalcGeometries_p[i].reffreq = antParTab.getCell("REFFREQ", i).asDouble(); // stay in GHz 
	Array<Double> ta2;
	ta2.assign(antParTab.getCell("TAPERPOLY", i).asArrayDouble());
	for(uInt j=0; j<5;j++){
	  BeamCalcGeometries_p[i].taperpoly[j] = ta2(IPosition(1,j));
	}
	BeamCalcGeometries_p[i].ntaperpoly = antParTab.getCell("NTAPERPOLY", i).asInt();
	BeamCalcGeometries_p[i].astigm_0 = antParTab.getCell("ASTIGM_0", i).asDouble();
	BeamCalcGeometries_p[i].astigm_45 = antParTab.getCell("ASTIGM_45", i).asDouble();
	if(verbose){
	  cout << "i name bandMinFreq_p bandMaxFreq_p sub_h feedpos feedpos feedpos subangle legwidth legfoot legapex"
	       << " Rhole Rant reffreq taperpoly taperpoly taperpoly taperpoly taperpoly ntaperpoly astigm0 astigm45" << endl; 
	  cout << i << " " << BeamCalcGeometries_p[i].name << " " << bandMinFreq_p[i] << " " << bandMaxFreq_p[i] 
	       << " " << BeamCalcGeometries_p[i].sub_h 
	       << " " << BeamCalcGeometries_p[i].feedpos[0] << " " << BeamCalcGeometries_p[i].feedpos[1] 
	       << " " << BeamCalcGeometries_p[i].feedpos[2] 
	       << " " << BeamCalcGeometries_p[i].subangle << " " << BeamCalcGeometries_p[i].legwidth 
	       << " " << BeamCalcGeometries_p[i].legfoot << " " << BeamCalcGeometries_p[i].legapex 
	       << " " << BeamCalcGeometries_p[i].Rhole << " " << BeamCalcGeometries_p[i].Rant << " " << BeamCalcGeometries_p[i].reffreq 
	       << " " << BeamCalcGeometries_p[i].taperpoly[0] << " " << BeamCalcGeometries_p[i].taperpoly[1] 
	       << " " << BeamCalcGeometries_p[i].taperpoly[2] << " " << BeamCalcGeometries_p[i].taperpoly[3] 
	       << " " << BeamCalcGeometries_p[i].taperpoly[4] << " " << BeamCalcGeometries_p[i].ntaperpoly 
	       << " " << BeamCalcGeometries_p[i].astigm_0 << " " << BeamCalcGeometries_p[i].astigm_45 << endl; 
	}
      }

    } catch (AipsError x) {
      String mesg="Initialisation of antenna ray tracing parameters for observatory "+obsName_p
	+" failed using path "+antRayPath+"\n with message "+x.getMesg();
      BeamCalcGeometries_p.resize(0);
      SynthesisError err(mesg);
      throw(err);
    }

    if(antRespPath.empty()){ // use containing directory of the antRayPath
      antRespPath_p = Path(antRayPath).dirName();
    }
    else{
      antRespPath_p = Path(antRespPath).dirName();
    }

    os <<  LogIO::NORMAL << "... successful." << LogIO::POST;

    return;

  }

  Int BeamCalc::getBandID(Double freq, // in Hz 
			  const String& obsName,
			  const String& bandName,
			  const String& antType,
			  const MEpoch& obsTime,
			  const String& otherAntRayPath){

    setBeamCalcGeometries(obsName, antType, obsTime, otherAntRayPath); 

    // Check against bandName if it is a non-NULL string.  Otherwise
    // check against frequency range.  The latter method is only for
    // backward compatibility reasons and for older MSes which did not
    // have correct band names in the SPW sub-table.
    if (bandName != "")
      for(uInt i=0; i<BeamCalcGeometries_p.nelements(); i++)
    	  if(String(BeamCalcGeometries_p[i].bandName)==bandName) return i;

    // If the control flow gets here, the given bandName did not match
    // in SPW names in the MS.  So use the fall-back option of using
    // the reference frequency to get the band ID (this will lead to
    // incorrect ID for the edge SPWs which might overlap in frequecy
    // with the adjecent band).
    for(uInt i=0; i<BeamCalc_NumBandCodes_p; i++){
      if((bandMinFreq_p[i]<=freq)&&(freq<=bandMaxFreq_p[i])){
	return i;
      }
    }
    ostringstream mesg;
    mesg << obsName << "/" << bandName << "/" << antType << "/" << freq << "(Hz) combination not recognized.";
    throw(SynthesisError(mesg.str()));
    
  }
  


  calcAntenna* BeamCalc::newAntenna(Double sub_h, Double feed_x, Double feed_y, Double feed_z,
				    Double ftaper, Double thmax, const char *geomfile)
  {
    calcAntenna *a;
    Int i;
    Double d, r, m, z;
    FILE *in;
    String fullFileName(antRespPath_p);
    fullFileName = fullFileName + String("/") + geomfile;

    in = fopen(fullFileName.c_str(), "r");

    if(!in)
      {
	String msg = "File " + fullFileName 
	  + " not found.\n   Did you forget to install package data repository?\n";
	throw(SynthesisError(msg));
      }
    
    a = (calcAntenna *)malloc(sizeof(calcAntenna));
    
    for(i = 0; i < MAXGEOM; i++)
      {
	if(fscanf(in, "%lf%lf%lf", &r, &z, &m) != 3) break;
	a->z[i] = z;
	a->m[i] = m;
	a->radius = r;
      }
    fclose(in);
    a->ngeom = i;
    a->zedge = z;
    a->deltar = a->radius/(float)(a->ngeom-1.0);
    a->bestparabola = a->zedge/(a->radius*a->radius);
    if(i < 3)
      {
	fprintf(stderr, "geom file not valid\n");
	free(a);
	return 0;
      }
    
    z = sub_h-feed_z;
    
    a->sub_h = sub_h;
    a->feed[0] = feed_x;
    a->feed[1] = feed_y;
    a->feed[2] = feed_z;
    d = std::sqrt((double)(feed_x*feed_x + feed_y*feed_y + z*z));
    if(z > 0.0)
      {
	a->K = sub_h + d;
	a->feeddir[0] = -feed_x/d;
	a->feeddir[1] = -feed_y/d;
	a->feeddir[2] = (sub_h-feed_z)/d;
      }
    else
      {
	a->K = std::sqrt((double(feed_x*feed_x + feed_y*feed_y + feed_z*feed_z)));
	a->feeddir[0] = -feed_x/d;
	a->feeddir[1] = -feed_y/d;
	a->feeddir[2] = (sub_h-feed_z)/d;
      }
    for(i = 0; i < 3; i++) a->pfeeddir[i] = a->feeddir[i];
    a->ftaper = fabs(ftaper);
    a->thmax = thmax;
    a->fa2pi = 2.0*M_PI*std::sqrt((double)ftaper)*0.1874/sin(thmax*M_PI/180.0);
    a->legwidth = 0.0;
    a->legfoot = a->radius/2.0;
    a->legapex = sub_h*1.2;
    a->legthick = 0.0;
    a->hole_radius = 0.0; 
    a->dir[0] = a->dir[1] = 0.0;
    a->dir[2] = 1.0;
    strcpy(a->name, "unnamed");
    a->k[0] = a->k[1] = a->k[2] = 0.0;
    /* default to no polarization state */
    Antennasetfreq(a, 1.0);
    Antennasetdir(a, 0);  /* compute hhat and vhat */
    a->gridsize = 0;
    dishvalue(a, a->legfoot, &a->legfootz, 0);
    
    return a;
  }
  
  void BeamCalc::deleteAntenna(calcAntenna *a)
  {
    if(!a) return;
    
    free(a);
  }
  
  void BeamCalc::Antennasetfreq(calcAntenna *a, Double freq)
  {
    Int i;
    
    a->freq = freq;
    a->lambda = NS_METER/freq;
    for(i = 0; i < 3; i++) a->k[i] = -2.0*M_PI*a->dir[i]/a->lambda;
  }
  
  void BeamCalc::Antennasetdir(calcAntenna *a, const Double *dir)
  {
    Double hmag;
    Int i;
    
    if(dir)
      {
	for(i = 0; i < 3; i++) a->dir[i] = dir[i];
	if(a->dir[0] == 0.0 && a->dir[1] == 0.0)
	  {
	    a->hhat[0] = 1.0;
	    a->hhat[1] = a->hhat[2] = 0.0;
	    a->vhat[1] = 1.0;
	    a->vhat[0] = a->vhat[2] = 0.0;
	  }
	else
	  {
	    a->hhat[0] = a->dir[1];
	    a->hhat[1] = -a->dir[0];
	    a->hhat[2] = 0.0;
	    hmag = sqrt(a->hhat[0]*a->hhat[0]
			+ a->hhat[1]*a->hhat[1]);
	    a->hhat[0] /= hmag;
	    a->hhat[1] /= hmag;
	    
	    a->vhat[0] = a->hhat[1]*a->dir[2] 
	      - a->hhat[2]*a->dir[1];
	    a->vhat[1] = a->hhat[2]*a->dir[0] 
	      - a->hhat[0]*a->dir[2];
	    a->vhat[2] = a->hhat[0]*a->dir[1] 
	      - a->hhat[1]*a->dir[0];
	  }
      }
    for(i = 0; i < 3; i++) a->k[i] = -2.0*M_PI*a->dir[i]/a->lambda;
  }
  
  /* sets feeddir after pathology is considered */
  void BeamCalc::alignfeed(calcAntenna *a, const Pathology *p)
  {
    Int i, j;
    Double f[3], s0[3], s[3], d[3], m=0.0;
    
    for(i = 0; i < 3; i++) f[i] = a->feed[i] + p->feedshift[i];
    for(i = 0; i < 3; i++) s0[i] = -p->subrotpoint[i];
    s0[2] += a->sub_h;
    for(i = 0; i < 3; i++) 
      {
	s[i] = 0.0;
	for(j = 0; j < 3; j++) 
	  s[i] += p->subrot[i][j]*s0[j];
	s[i] += p->subrotpoint[i] + p->subshift[i];
	d[i] = s[i]-f[i];
	m += d[i]*d[i];
      }
    m = sqrt(m);
    for(i = 0; i < 3; i++) a->feeddir[i] = d[i]/m;
  }
  
  void BeamCalc::getfeedbasis(const calcAntenna *a, Double B[3][3])
  {
    Int i;
    Double *dir, *vhat, *hhat;
    
    hhat = B[0];
    vhat = B[1];
    dir = B[2];
    
    for(i = 0; i < 3; i++) dir[i] = a->pfeeddir[i];
    
    if(dir[0] == 0.0 && dir[1] == 0.0)
      {
	vhat[0] = 1.0;
	vhat[1] = vhat[2] = 0.0;
	hhat[1] = 1.0;
	hhat[0] = hhat[2] = 0.0;
      }
    else
      {
	vhat[0] = dir[1];
	vhat[1] = -dir[0];
	vhat[2] = 0.0;
	norm3(vhat);
	
	hhat[0] = vhat[1]*dir[2] - vhat[2]*dir[1];
	hhat[1] = vhat[2]*dir[0] - vhat[0]*dir[2];
	hhat[2] = vhat[0]*dir[1] - vhat[1]*dir[0];
      }
  }
  
  void BeamCalc::Efield(const calcAntenna *a, const Complex *pol, Complex *E)
  {
    Double B[3][3];
    Double *hhat, *vhat;
    
    getfeedbasis(a, B);
    hhat = B[0];
    vhat = B[1];

    for(Int i = 0; i < 3; i++)
      E[i] = Complex(hhat[i],0) * pol[0] + Complex(vhat[i],0) * pol[1];
  }
  
  Int BeamCalc::Antennasetfeedpattern(calcAntenna* /*a*/, 
                                      const char* /*filename*/, 
                                      Double /*scale*/)
  {
#if 0
    Int i, N, Nmax;
    Double x, delta;
    VecArray pat;
    
    a->feedpatterndelta = 0.0;
    if(a->feedpattern) deleteVector(a->feedpattern);
    
    if(filename == 0) return 1;
    
    pat = VecArrayfromfile(filename, 2);
    
    if(!pat) return 0;
    N = VectorSize(pat[0]);
    g_assert(N > 2);
    g_assert(pat[0][0] == 0.0);
    
    delta = pat[0][1];
    g_assert(delta > 0.0);
    for(i = 2; i < N; i++) 
      {
	x = pat[0][i]-pat[0][i-1]-delta;
	g_assert(fabs(x) < delta/10000.0);
      }
    
    /* convert to radians */
    delta *= M_PI/180.0;
    
    /* and scale it */
    if(scale > 0.0) delta *= scale;
    
    /* Do we need to truncate the pattern? */
    Nmax = M_PI/delta;
    if(N > Nmax)
      {
	a->feedpattern = newVector(Nmax);
	for(i = 0; i < Nmax; i++) 
	  a->feedpattern[i] = fabs(pat[1][i]);
	deleteVector(pat[1]);
      }
    else a->feedpattern = pat[1];
    
    a->feedpatterndelta = delta;
    deleteVector(pat[0]);
    deleteVecArray(pat);
#endif
    return 1;
  }
  
  calcAntenna* BeamCalc::newAntennafromApertureCalcParams(ApertureCalcParams *ap)
  {
    calcAntenna *a;
    Double dir[3] = {0.0, 0.0, 1.0};
    Double sub_h, feed_x, feed_y, feed_z, thmax, ftaper;
    char geomfile[128];//, *feedfile;
    BeamCalcGeometry *geom;
    Int i;
    Double x, freq, df;
    
    //LogIO os;
    if((0<=ap->band) && (ap->band<(Int)BeamCalcGeometries_p.size())){
      geom = &(BeamCalcGeometries_p[ap->band]);
      //os << "Using antenna parameters for " << geom->bandName << " band" << LogIO::POST;
    }
    else{
      SynthesisError err(String("Internal Error: attempt to access beam geometry for non-existing band."));
      throw(err);
    }
    
    sub_h = geom->sub_h;
    feed_x = geom->feedpos[0]; feed_x = -feed_x;
    feed_y = geom->feedpos[1];
    feed_z = geom->feedpos[2];
    //feedfile = 0;
    thmax = geom->subangle;
    
    freq = ap->freq;
    if(freq <= 0.0) freq = geom->reffreq;

    df = freq-geom->reffreq;
    x = 1.0;
    ftaper = 0.0;
    for(i = 0; i < geom->ntaperpoly; i++)
    {	
    	ftaper += geom->taperpoly[i]*x;
	x *= df;
    }
    sprintf(geomfile, "%s.surface", geom->name);
    
    a = newAntenna(sub_h, feed_x, feed_y, feed_z, ftaper, thmax, geomfile);
    if(!a) return 0;
    
    strcpy(a->name, geom->name);
    
    /* feed pattern file is two column text file containing 
     * angle (in degrees) and power (in dBi) 
     */

    // if(feedfile != 0)
    //   {
    // 	Double scale;
    // 	scale = getKeyValueDouble(kv, "feedpatternscale");
    // 	if(!Antennasetfeedpattern(a, feedfile, scale)) 
    // 	  {
    // 	    deleteAntenna(a);
    // 	    fprintf(stderr, "Problem with feed file <%s>\n",
    // 		    feedfile);
    // 	    return 0;
    // 	  }
    //   }

    Antennasetfreq(a, ap->freq);
    
    a->legwidth = geom->legwidth;
    a->legfoot  = geom->legfoot;
    a->legapex  = geom->legapex;
    
    a->hole_radius = geom->Rhole;
    
    a->astigm_0 = geom->astigm_0;
    a->astigm_45 = geom->astigm_45;

    Antennasetdir(a, dir);

    return a;
  }
  
  Int BeamCalc::dishvalue(const calcAntenna *a, Double r, Double *z, Double *m)
  {
    Double ma, mb, mc, zav, A, B, C, D;
    Double x, d, dd;
    Double s = 1.0;
    Int n;
    
    if(r == 0)
      {
	*z = a->z[0];
	*m = 0.0;
	return 1;
      }
    
    if(r < 0) 
      {
	s = -1.0;
	r = -r;
      }
    d = a->deltar;
    dd = d*d;
    
    n = (Int)floor(r/d + 0.5);	/* the middle point */
    if(n > a->ngeom-2) n = a->ngeom-2;
    
    x = r - n*d;
    
    if(n == 0)
      {
	mc = a->m[1];
	ma = -mc;
	mb = 0.0;
	zav = 2.0*a->z[1] + a->z[0];
      }
    else
      {
	ma = a->m[n-1];
	mb = a->m[n];
	mc = a->m[n+1];
	zav = a->z[n-1] + a->z[n] + a->z[n+1];
      }
    
    A = mb;
    B = 0.5*(mc - ma)/d;
    C = 0.5*(mc - 2.0*mb + ma)/dd;
    
    D = (zav - B*dd)/3.0;
    
    if(m) *m = s*(A + B*x + C*x*x);
    if(z) *z = s*(D + A*x + B*x*x/2.0 + C*x*x*x/3.0);
    
    return 1;
  }

  Int BeamCalc::astigdishvalue(const calcAntenna *a, Double x, Double y, Double *z, Double *m)
  {
    Double ma, mb, mc, zav, A, B, C, D;
    Double r, rr, theta, xp, d, dd, z5, z6, astigm, dastigm;
    Double s = 1.0;
    Int n;
    
    rr = x*x + y*y;
    r = sqrt(rr);

    if(r==0. || (a->astigm_0==0. && a->astigm_45==0.))
      {
	return dishvalue(a, r, z, m);
      }

    // the Zernike polynomials Z5 and Z6
    Double sin2th, cos2th, rho, rho2;

    theta = atan2(y,x);
    sin2th = sin(2.*theta);
    cos2th = cos(2.*theta);
    rho = r / a->radius;
    rho2 = rho*rho;

    z5 = sqrt(6.) * rho2 * sin2th;
    z6 = sqrt(6.) * rho2 * cos2th;

    astigm = 1. + a->astigm_45 * z5 + a->astigm_0 * z6;
    dastigm = 2.* rho2/r * sqrt(6.)*(a->astigm_45*sin2th + a->astigm_0*cos2th);

    d = a->deltar;
    dd = d*d;
    
    n = (Int)floor(r/d + 0.5);	/* the middle point */
    if(n > a->ngeom-2) n = a->ngeom-2;
    
    xp = r - n*d;
    
    if(n == 0)
      {
	mc = a->m[1];
	ma = -mc;
	mb = 0.0;
	zav = 2.0*a->z[1] + a->z[0];
      }
    else
      {
	ma = a->m[n-1];
	mb = a->m[n];
	mc = a->m[n+1];
	zav = a->z[n-1] + a->z[n] + a->z[n+1];
      }

    A = mb;
    B = 0.5*(mc - ma)/d;
    C = 0.5*(mc - 2.0*mb + ma)/dd;
    
    D = (zav - B*dd)/3.0;
    
    
    Double zn = s*(D + A*xp + B*xp*xp/2.0 + C*xp*xp*xp/3.0);
    if(z) *z = zn * astigm;
    if(m) *m = s*(A + B*xp + C*xp*xp) * astigm + dastigm * zn;
    
    return 1;
  }
  
  /* Returns position of subreflector piece (x, y, z) and
   * its normal (u, v, w)
   */
  Int BeamCalc::subfromdish(const calcAntenna *a, Double x, Double y, Double *subpoint)
  {
    Double r, z, m, u, v, w;
    Double dx, dy, dz, dl, t;
    Double n[3], sf[3], sd[3];
    Int i;
    
    r = sqrt(x*x + y*y);
    
    if(r == 0)
      {
	subpoint[0] = 0.0;
	subpoint[1] = 0.0;
	subpoint[2] = a->sub_h;
      }
    else
      {
	astigdishvalue(a, x, y, &z, &m);

	/* Compute reflected unit vector direction */
	m = tan(2.0*atan(m));
	w = 1.0/sqrt(1.0+m*m);
	u = -m*(x/r)*w;
	v = -m*(y/r)*w;
	
	dx = a->feed[0]-x;
	dy = a->feed[1]-y;
	dz = a->feed[2]-z;
	dl = a->K + z;
	
	t = 0.5*(dx*dx + dy*dy + dz*dz - dl*dl)
	  / (-dl + u*dx + v*dy + w*dz);
	
	subpoint[0] = x + u*t;
	subpoint[1] = y + v*t;
	subpoint[2] = z + w*t;
      }
    
    for(i = 0; i < 3; i++) sf[i] = a->feed[i] - subpoint[i];
    sd[0] = x - subpoint[0];
    sd[1] = y - subpoint[1];
    sd[2] = z - subpoint[2];
    
    norm3(sf);
    norm3(sd);
    
    for(i = 0; i < 3; i++) n[i] = sd[i]+sf[i];
    
    norm3(n);
    
    for(i = 0; i < 3; i++) subpoint[3+i] = n[i];
    
    return 1;
  }
  
  Int BeamCalc::dishfromsub(const calcAntenna *a, Double x, Double y, Double *dishpoint)
  {

    Double x1, y1, dx, dy, mx, my, r, d;
    const Double eps = 0.001;
    Int iter, niter=500;
    Double sub[5][6];
    
    x1 = x;
    y1 = y;
    
    for(iter = 0; iter < niter; iter++)
      {
	subfromdish(a, x1, y1, sub[0]);
	subfromdish(a, x1-eps, y1, sub[1]);
	subfromdish(a, x1+eps, y1, sub[2]);
	subfromdish(a, x1, y1-eps, sub[3]);
	subfromdish(a, x1, y1+eps, sub[4]);
	mx = 0.5*(sub[2][0]-sub[1][0])/eps;
	my = 0.5*(sub[4][1]-sub[3][1])/eps;
	dx = (x-sub[0][0])/mx;
	dy = (y-sub[0][1])/my;
	if(fabs(dx) > a->radius/7.0) 
	  {
	    if(dx < 0) dx = -a->radius/7.0;
	    else dx = a->radius/7.0;
	  }
	if(fabs(dy) > a->radius/7.0) 
	  {
	    if(dy < 0) dy = -a->radius/7.0;
	    else dy = a->radius/7.0;
	  }
	r = sqrt(x1*x1 + y1*y1);
	if(r >= a->radius)
	  if(x1*dx + y1*dy > 0.0) return 0;
	x1 += 0.5*dx;
	y1 += 0.5*dy;
	if(fabs(dx) < 0.005*eps && fabs(dy) < 0.005*eps) break;
      }
    if(iter == niter) return 0;
    
    r = sqrt(x1*x1 + y1*y1);
    dishpoint[0] = x1;
    dishpoint[1] = y1;
    //	dishpoint[2] = polyvalue(a->shape, r);
    dishpoint[3] = sub[0][0];
    dishpoint[4] = sub[0][1];
    dishpoint[5] = sub[0][2];
    d = sqrt(1.0+mx*mx+my*my);
    dishpoint[6] = mx/d;
    dishpoint[7] = my/d;
    dishpoint[8] = 1.0/d;
    dishpoint[9] = sub[0][3];
    dishpoint[10] = sub[0][4];
    dishpoint[11] = sub[0][5];
    
    if(r > a->radius) return 0;
    else return 1;
  }
  
  void BeamCalc::printAntenna(const calcAntenna *a)
  {
    printf("Antenna: %s  %p\n", a->name, a);
    printf("  freq    = %f GHz  lambda = %f m\n", a->freq, a->lambda);
    printf("  feeddir = %f, %f, %f\n", 
	   a->feeddir[0], a->feeddir[1], a->feeddir[2]); 
    printf("  pfeeddir = %f, %f, %f\n", 
	   a->pfeeddir[0], a->pfeeddir[1], a->pfeeddir[2]); 
  }
  
  Ray * BeamCalc::newRay(const Double *sub)
  {
    Ray *ray;
    Int i;
    
    ray = (Ray *)malloc(sizeof(Ray));
    for(i = 0; i < 6; i++) ray->sub[i] = sub[i];
    
    return ray;
  }
  
  void BeamCalc::deleteRay(Ray *ray)
  {
    if(ray) free(ray);
  }
  
  Pathology* BeamCalc::newPathology()
  {
    Pathology *P;
    Int i, j;
    
    P = (Pathology *)malloc(sizeof(Pathology));
    
    for(i = 0; i < 3; i++) P->subrotpoint[i] = P->subshift[i] = P->feedshift[i] = 0.0;
    for(i = 0; i < 3; i++) for(j = 0; j < 3; j++) 
      P->feedrot[i][j] = P->subrot[i][j] = 0.0;
    for(i = 0; i < 3; i++) P->feedrot[i][i] = P->subrot[i][i] = 1.0;
    
    P->az_offset = 0.0;
    P->el_offset = 0.0;
    P->phase_offset = 0.0;
    P->focus = 0.0;
    
    return P;
  }
  
  Pathology* BeamCalc::newPathologyfromApertureCalcParams(ApertureCalcParams* /*ap*/)
  {
    Pathology *P;
    
    P = newPathology();
    
    return P;
  }
  
  void BeamCalc::deletePathology(Pathology *P)
  {
    if(P) free(P);
  }
  
  void BeamCalc::normvec(const Double *a, const Double *b, Double *c)
  {
    Int i;
    Double r;
    for(i = 0; i < 3; i++) c[i] = a[i] - b[i];
    r = sqrt(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]);
    for(i = 0; i < 3; i++) c[i] /= r;
  }
  
  Double BeamCalc::dAdOmega(const calcAntenna *a, const Ray *ray1, const Ray *ray2,
			    const Ray *ray3, const Pathology *p)
  {
    Double A, Omega;
    Double n1[3], n2[3], n3[3], f[3], ci, cj, ck;
    Int i;
    
    /* Area in aperture is in a plane z = const */
    A = 0.5*fabs(
		 (ray1->aper[0]-ray2->aper[0])*(ray1->aper[1]-ray3->aper[1]) -
		 (ray1->aper[0]-ray3->aper[0])*(ray1->aper[1]-ray2->aper[1]) );
    
    for(i = 0; i < 3; i++) f[i] = a->feed[i] + p->feedshift[i];
    
    normvec(ray1->sub, f, n1);
    normvec(ray2->sub, f, n2);
    normvec(ray3->sub, f, n3);
    
    for(i = 0; i < 3; i++)
      {
	n1[i] -= n3[i];
	n2[i] -= n3[i];
      }
    
    ci = n1[1]*n2[2] - n1[2]*n2[1];
    cj = n1[2]*n2[0] - n1[0]*n2[2];
    ck = n1[0]*n2[1] - n1[1]*n2[0];
    
    Omega = 0.5*sqrt(ci*ci + cj*cj + ck*ck);
    
    return A/Omega;
  }
  
  Double BeamCalc::dOmega(const calcAntenna *a, const Ray *ray1, const Ray *ray2,
			  const Ray *ray3, const Pathology *p)
  {
    Double Omega;
    Double n1[3], n2[3], n3[3], f[3], ci, cj, ck;
    Int i;
    
    for(i = 0; i < 3; i++) f[i] = a->feed[i] + p->feedshift[i];
    
    normvec(ray1->sub, f, n1);
    normvec(ray2->sub, f, n2);
    normvec(ray3->sub, f, n3);
    
    for(i = 0; i < 3; i++)
      {
	n1[i] -= n3[i];
	n2[i] -= n3[i];
      }
    
    ci = n1[1]*n2[2] - n1[2]*n2[1];
    cj = n1[2]*n2[0] - n1[0]*n2[2];
    ck = n1[0]*n2[1] - n1[1]*n2[0];
    
    Omega = 0.5*sqrt(ci*ci + cj*cj + ck*ck);
    
    return Omega;
  }
  
  Double BeamCalc::Raylen(const Ray *ray)
  {
    Double len, d[3];
    Int i;
    
    /* feed to subreflector */
    for(i = 0; i < 3; i++) d[i] = ray->feed[i] - ray->sub[i];
    len  = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
    
    /* subreflector to dish */
    for(i = 0; i < 3; i++) d[i] = ray->sub[i] - ray->dish[i];
    len += sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
    
    /* dish to aperture */
    for(i = 0; i < 3; i++) d[i] = ray->dish[i] - ray->aper[i];
    len += sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
    
    return len;
  }
  
  void BeamCalc::Pathologize(Double *sub, const Pathology *p)
  {
    Int i;
    Int j;
    Double tmp[6];
    
    for(i = 0; i < 3; i++) sub[i] -= p->subrotpoint[i];
    
    for(i = 0; i < 3; i++) 
      {
	tmp[i] = 0.0;
	tmp[i+3] = 0.0;
	for(j = 0; j < 3; j++) tmp[i] += p->subrot[i][j]*sub[j];
	for(j = 0; j < 3; j++) tmp[i+3] += p->subrot[i][j]*sub[j+3];
      }
    
    for(i = 0; i < 3; i++) 
      sub[i] = tmp[i] + p->subrotpoint[i] + p->subshift[i];
    for(i = 4; i < 6; i++) 
      sub[i] = tmp[i];
  }
  
  void BeamCalc::applyPathology(Pathology *P, calcAntenna *a)
  {
    Double dx[3];
    Int i, j;
    
    if(P->focus != 0.0)
      {
	dx[0] = -a->feed[0];
	dx[1] = -a->feed[1];
	dx[2] = a->sub_h-a->feed[2];
	norm3(dx);
	for(i = 0; i < 3; i++) P->feedshift[i] += P->focus*dx[i];
	
	P->focus = 0.0;
      }
    
    for(i = 0; i < 3; i++) a->pfeeddir[i] = 0.0;
    for(j = 0; j < 3; j++) for(i = 0; i < 3; i++)
      a->pfeeddir[j] += P->feedrot[j][i]*a->feeddir[i];
  }
  
  
  void BeamCalc::intersectdish(const calcAntenna *a, const Double *sub, const Double *unitdir,
			       Double *dish, Int niter)
  {
    Double A, B, C, t, m, r;
    Double x[3], n[3];
    Int i, iter;
    
    /* First intersect with ideal paraboloid */
    A = a->bestparabola*(unitdir[0]*unitdir[0] + unitdir[1]*unitdir[1]);
    B = 2.0*a->bestparabola*(unitdir[0]*sub[0] + unitdir[1]*sub[1])
      -unitdir[2];
    C = a->bestparabola*(sub[0]*sub[0] + sub[1]*sub[1]) - sub[2];
    t = 0.5*(sqrt(B*B-4.0*A*C) - B)/A; /* take greater root */
    
    for(iter = 0; ; iter++)
      {
	/* get position (x) and normal (n) on the real dish */
	for(i = 0; i < 2; i++) x[i] = sub[i] + t*unitdir[i];
	r = sqrt(x[0]*x[0] + x[1]*x[1]);
	astigdishvalue(a, x[0], x[1], &(x[2]), &m);
	n[2] = 1.0/sqrt(1.0+m*m);
	n[0] = -m*(x[0]/r)*n[2];
	n[1] = -m*(x[1]/r)*n[2];
	
	if(iter >= niter) break;
	
	A = B = 0;
	for(i = 0; i < 3; i++)
	  {
	    A += n[i]*(x[i]-sub[i]);	/* n dot (x-sub) */
	    B += n[i]*unitdir[i];		/* n dot unitdir */
	  }
	t = A/B;
      }
    
    for(i = 0; i < 3; i++)
      {
	dish[i] = x[i];
	dish[i+3] = n[i];
      }
  }
  
  void BeamCalc::intersectaperture(const calcAntenna *a, const Double *dish, 
				   const Double *unitdir, Double *aper)
  {
    Double t;
    Int i;
    
    t = (a->zedge-dish[2])/unitdir[2];
    for(i = 0; i < 3; i++) aper[i] = dish[i] + t*unitdir[i];
    
    aper[3] = aper[4] = 0.0;
    aper[5] = 1.0;
  }
  
  /* gain in power */
  Double BeamCalc::feedfunc(const calcAntenna *a, Double theta)
  {
    Double stheta;
    
    stheta = sin(theta);
    return exp(2.0*(-0.083)*a->fa2pi*a->fa2pi*stheta*stheta);
  }
  
  /* gain in power */
  Double BeamCalc::feedgain(const calcAntenna *a, const Ray *ray, const Pathology */*p*/)
  {
    Double costheta = 0.0;
    Double v[3];
    Int i;
    
    for(i = 0; i < 3; i++) v[i] = ray->sub[i] - ray->feed[i];
    norm3(v);
    
    for(i = 0; i < 3; i++) 
      {
	costheta += a->pfeeddir[i]*v[i];
      }

    
    return exp(2.0*(-0.083)*a->fa2pi*a->fa2pi*(1.0-costheta*costheta));
  }
  
  Ray* BeamCalc::trace(const calcAntenna *a, Double x, Double y, const Pathology *p)
  {
    Ray *ray;
    Double idealsub[12];
    Double fu[3], du[3], au[3], ndotf=0.0, ndotd=0.0;
    Int i;
    const Int niter = 7;
    
    subfromdish(a, x, y, idealsub);
    
    ray = newRay(idealsub);
    
    Pathologize(ray->sub, p);
    
    if(ray->sub[5] < -1.0 || ray->sub[5] > -0.0) 
      {
	deleteRay(ray);
	return 0;
      }
    
    for(i = 0; i < 3; i++) ray->feed[i] = a->feed[i] + p->feedshift[i];
    
    /* now determine unit vector pointing to dish */
    
    /* unit toward feed */
    for(i = 0; i < 3; i++) fu[i] = ray->feed[i] - ray->sub[i];
    norm3(fu);
    
    /* unit toward dish */
    for(i = 0; i < 3; i++) ndotf += ray->sub[i+3]*fu[i];
    for(i = 0; i < 3; i++) du[i] = 2.0*ray->sub[i+3]*ndotf - fu[i];
    
    /* dish point */
    intersectdish(a, ray->sub, du, ray->dish, niter);
    
    /* unit toward aperture */
    for(i = 0; i < 3; i++) ndotd += ray->dish[i+3]*du[i];
    for(i = 0; i < 3; i++) au[i] = du[i] - 2.0*ray->dish[i+3]*ndotd;
    
    intersectaperture(a, ray->dish, au, ray->aper);
    
    return ray;
  }
  
  void BeamCalc::tracepol(Complex *E0, const Ray *ray, Complex *E1)
  {
    Complex fac;
    Double v1[3], v2[3], v3[3];
    Double r[3];
    Int i;
    
    for(i = 0; i < 3; i++)
      {
	v1[i] = ray->sub[i]  - ray->feed[i];
	v2[i] = ray->dish[i] - ray->sub[i];
	v3[i] = ray->aper[i] - ray->dish[i];
	E1[i] = E0[i];
      }
    norm3(v1); 
    norm3(v2);
    norm3(v3);
    
    for(i = 0; i < 3; i++) r[i] = v1[i] - v2[i];
    norm3(r); 
    fac = Complex(r[0],0)*E1[0] + Complex(r[1],0)*E1[1] + Complex(r[2],0)*E1[2];
    for(i = 0; i < 3; i++) E1[i] = Complex(r[i],0)*fac*2.0 - E1[i];
    
    for(i = 0; i < 3; i++) r[i] = v2[i] - v3[i];
    norm3(r); 
    fac = Complex(r[0],0)*E1[0] + Complex(r[1],0)*E1[1] + Complex(r[2],0)*E1[2];
    for(i = 0; i < 3; i++) E1[i] = Complex(r[i],0)*fac*2.0 - E1[i];
  }
  
  Int BeamCalc::legplanewaveblock(const calcAntenna *a, Double x, Double y)
  {
    /* outside the leg foot area, the blockage is spherical wave */
    if(x*x + y*y > a->legfoot*a->legfoot) return 0;
    
    if(a->legwidth == 0.0) return 0;
    
    if(strcmp(a->name, "VLBA") == 0) 
      {
	const Double s=1.457937;
	const Double c=1.369094;
	if(fabs(s*x+c*y) < -a->legwidth) return 1;
	if(fabs(s*x-c*y) < -a->legwidth) return 1;
      }
    else if(a->legwidth < 0.0)  /* "x shaped" legs */
      {
	if(fabs(x-y)*M_SQRT2 < -a->legwidth) return 1;
	if(fabs(x+y)*M_SQRT2 < -a->legwidth) return 1;
      }
    else if(a->legwidth > 0.0) /* "+ shaped" legs */
      {
	if(fabs(x)*2.0 < a->legwidth) return 1;
	if(fabs(y)*2.0 < a->legwidth) return 1;
      }
    
    return 0;
  }
  
  Int BeamCalc::legplanewaveblock2(const calcAntenna *a, const Ray *ray)
  {
    Int i, n;
    Double dr2;
    // phi set but not used
    Double theta /*, phi*/;
    Double r0[3], dr[3], l0[3], l1[3], dl[3], D[3]; 
    Double D2, N[3], ll, rr;
    const Double thetaplus[4] = 
      {0, M_PI/2.0, M_PI, 3.0*M_PI/2.0};
    const Double thetacross[4] = 
      {0.25*M_PI, 0.75*M_PI, 1.25*M_PI, 1.75*M_PI};
    const Double thetavlba[4] =
      {0.816817, 2.3247756, 3.9584096, 5.466368};
    const Double *thetalut;
    
    if(a->legwidth == 0.0) return 0;
    
    if(strcmp(a->name, "VLBA") == 0) thetalut = thetavlba;
    else if(a->legwidth < 0.0) thetalut = thetacross;
    else thetalut = thetaplus;
    
    /* inside the leg feet is plane wave blockage */
    dr2 = ray->dish[0]*ray->dish[0] + ray->dish[1]*ray->dish[1];
    if(dr2 >= a->legfoot*a->legfoot) return 0;
    
    for(i = 0; i < 3; i++)
      {
	r0[i] = ray->dish[i];
	dr[i] = ray->aper[i] - r0[i];
      }
    rr = r0[0]*r0[0] + r0[1]*r0[1];
    
    l0[2] = a->legfootz;
    l1[0] = l1[1] = 0.0;
    l1[2] = a->legapex;
    // phi set but not used
    // phi = atan2(r0[1], r0[0]);
    for(n = 0; n < 4; n++)
      {
	theta = thetalut[n];
	l0[0] = a->legfoot*cos(theta);
	l0[1] = a->legfoot*sin(theta);
	ll = l0[0]*l0[0] + l0[1]*l0[1];
	if((l0[0]*r0[0] + l0[1]*r0[1])/sqrt(ll*rr) < 0.7) continue;
	for(i = 0; i < 3; i++) dl[i] = l1[i] - l0[i];
	for(i = 0; i < 3; i++) D[i] = r0[i] - l0[i];
	
	N[0] = dr[1]*dl[2] - dr[2]*dl[1];
	N[1] = dr[2]*dl[0] - dr[0]*dl[2];
	N[2] = dr[0]*dl[1] - dr[1]*dl[0];
	norm3(N);
	
	D2 = D[0]*N[0] + D[1]*N[1] + D[2]*N[2];
	
	if(fabs(D2) <= 0.5*fabs(a->legwidth)) return 1;
      }
    
    return 0;
  }
  
  Int BeamCalc::legsphericalwaveblock(const calcAntenna *a, const Ray *ray)
  {
    Int i, n;
    Double dr2;
    // phi set but not used
    Double theta /*, phi*/;
    Double r0[3], dr[3], l0[3], l1[3], dl[3], D[3]; 
    Double D2, N[3], ll, rr;
    const Double thetaplus[4] = 
      {0, M_PI/2.0, M_PI, 3.0*M_PI/2.0};
    const Double thetacross[4] = 
      {0.25*M_PI, 0.75*M_PI, 1.25*M_PI, 1.75*M_PI};
    const Double thetavlba[4] =
      {0.816817, 2.3247756, 3.9584096, 5.466368};
    const Double *thetalut;
    
    if(a->legwidth == 0.0) return 0;
    
    if(strcmp(a->name, "VLBA") == 0) thetalut = thetavlba;
    else if(a->legwidth < 0.0) thetalut = thetacross;
    else thetalut = thetaplus;
    
    /* inside the leg feet is plane wave blockage */
    dr2 = ray->dish[0]*ray->dish[0] + ray->dish[1]*ray->dish[1];
    if(dr2 < a->legfoot*a->legfoot) return 0;
    
    for(i = 0; i < 3; i++)
      {
	r0[i] = ray->dish[i];
	dr[i] = ray->sub[i] - r0[i];
      }
    rr = r0[0]*r0[0] + r0[1]*r0[1];
    
    l0[2] = a->legfootz;
    l1[0] = l1[1] = 0.0;
    l1[2] = a->legapex;
    // phi set but not used
    // phi = atan2(r0[1], r0[0]);
    for(n = 0; n < 4; n++)
      {
	theta = thetalut[n];
	l0[0] = a->legfoot*cos(theta);
	l0[1] = a->legfoot*sin(theta);
	ll = l0[0]*l0[0] + l0[1]*l0[1];
	if((l0[0]*r0[0] + l0[1]*r0[1])/sqrt(ll*rr) < 0.7) continue;
	for(i = 0; i < 3; i++) dl[i] = l1[i] - l0[i];
	for(i = 0; i < 3; i++) D[i] = r0[i] - l0[i];
	
	N[0] = dr[1]*dl[2] - dr[2]*dl[1];
	N[1] = dr[2]*dl[0] - dr[0]*dl[2];
	N[2] = dr[0]*dl[1] - dr[1]*dl[0];
	norm3(N);
	
	D2 = D[0]*N[0] + D[1]*N[1] + D[2]*N[2];
	
	if(fabs(D2) <= 0.5*fabs(a->legwidth)) return 1;
      }
    
    return 0;
  }
  

  void BeamCalc::copyBeamCalcGeometry(BeamCalcGeometry* to, BeamCalcGeometry* from){
    sprintf(to->name, "%s", from->name);
    sprintf(to->bandName, "%s", from->bandName);
    to->sub_h = from->sub_h;
    for(uInt j=0; j<3;j++){
      to->feedpos[j] = from->feedpos[j];
    }
    to->subangle = from->subangle;
    to->legwidth = from->legwidth;
    to->legfoot = from->legfoot;
    to->legapex = from->legapex;
    to->Rhole = from->Rhole;
    to->Rant = from->Rant;
    to->reffreq = from->reffreq;
    for(uInt j=0; j<5;j++){
      to->taperpoly[j] = from->taperpoly[j];
    }
    to->ntaperpoly = from->ntaperpoly;
    to->astigm_0 = from->astigm_0;
    to->astigm_45 = from->astigm_45;

  }


  /* The meat of the calculation */
  
  
  Int BeamCalc::calculateAperture(ApertureCalcParams *ap)
  {
    // not used
    // Complex fp, Exr, Eyr, Exl, Eyl;
    Complex Er[3], El[3];
    Complex Pr[2], Pl[2]; 
    Complex q[2];
    //Double dx, dy, Rhole, Rant, x0, y0, R2, H2, eps;
    //Complex rr, rl, lr, ll, tmp;
    Double L0, phase;
    Double sp, cp;
    Double B[3][3];
    calcAntenna *a;
    Pathology *p;
    Int nx, ny, os;
    Int i, j;
    //Double pac, pas; /* parallactic angle cosine / sine */
    Complex Iota; Iota=Complex(0,1);

    //UNUSED: Complex E1[3];
    //UNUSED: Double x,y, r2, L, amp, dP, dA, d0, x1, y1, dx1, dy1, dx2, dy2, dO;
    //UNUSED: Ray *ray, *rayx, *rayy;
    //UNUSED: Int iter;
    //UNUSED: Int niter=6;

    a = newAntennafromApertureCalcParams(ap);
    p = newPathologyfromApertureCalcParams(ap);
    
    /* compute central ray pathlength */
    {
      Ray *tmpRay;
      tmpRay = trace(a, 0.0, 0.00001, p);
      L0 = Raylen(tmpRay);
      deleteRay(tmpRay);
    }
    
    //pac = cos(ap->pa+M_PI/2);
    //pas = sin(ap->pa+M_PI/2);

    if(obsName_p=="EVLA" || obsName_p=="VLA"){
      /* compute polarization vectors in circular basis */
      Pr[0] = 1.0/M_SQRT2; Pr[1] =  Iota/M_SQRT2;
      Pl[0] = 1.0/M_SQRT2; Pl[1] = -Iota/M_SQRT2;

      /* compensate for feed orientation */
      getfeedbasis(a, B); 
      phase = atan2(B[0][1], B[0][0]);
      cp = cos(phase);
      sp = sin(phase);
      
      q[0] = Pr[0];
      q[1] = Pr[1];
      Pr[0] =  Complex(cp,0)*q[0] + Complex(sp,0)*q[1];
      Pr[1] = -Complex(sp,0)*q[0] + Complex(cp,0)*q[1];
      q[0] = Pl[0];
      q[1] = Pl[1];
      Pl[0] =  Complex(cp,0)*q[0] + Complex(sp,0)*q[1];
      Pl[1] = -Complex(sp,0)*q[0] + Complex(cp,0)*q[1];
    }
    else{
      /* in linear basis */
      Pr[0] = 1.0; Pr[1] = 0.0;
      Pl[0] = 0.0; Pl[1] = 1.0;
    }
    
    
    
    /* compute 3-vector feed efields for the two polarizations */
    Efield(a, Pr, Er); 
    Efield(a, Pl, El); 
    ap->aperture->set(Complex(0.0));
    
    os = ap->oversamp;
    nx = ap->nx*os;
    ny = ap->ny*os;
    //dx = ap->dx/os;
    //dy = ap->dy/os;
    //x0 = ap->x0 - ap->dx/2.0 + dx/2.0;
    //y0 = ap->y0 - ap->dy/2.0 + dy/2.0;
    //Rant = a->radius;
    //Rhole = a->hole_radius;
    //R2 = Rant*Rant;
    //H2 = Rhole*Rhole;
    
    //eps = dx/4.0;
    
    IPosition pos(4);
    //    shape = ap->aperture->shape();

    
    // cerr << "max threads " << omp_get_max_threads() 
    // 	 << " threads available " << omp_get_num_threads() 
    // 	 << endl;
    //Int Nth=1;
    //#ifdef _OPENMP
    //Nth=max(omp_get_max_threads()-2,1);
    //#endif
    // Timer tim;
    // tim.mark();
    //#pragma omp parallel default(none) firstprivate(Er, El, nx, ny)  private(i,j) shared(ap, a, p, L0) num_threads(Nth)
    {
      //#pragma omp for
    for(j = 0; j < ny; j++)
      {
	for(i = 0; i < nx; i++)
	  {
	    computePixelValues(ap, a, p, L0, Er, El, i,j);
	  }
      }
    }
    // tim.show("BeamCalc:");
    
    deletePathology(p);
    deleteAntenna(a);
    
    return 1;
  }

  void BeamCalc::computePixelValues(const ApertureCalcParams *ap, 
				    const calcAntenna *a, const Pathology *p,
				    const Double &L0,
				    Complex *Er, Complex *El,
				    const Int &i, const Int &j)
  {
    Complex fp, Exr, Eyr, Exl, Eyl;
    //    Complex Er[3], El[3];
    Complex E1[3];
    Double dx, dy, Rant, x0, y0, x, y, r2, R2, H2, eps, Rhole;
    Complex rr, rl, lr, ll, tmp;
    Double L, amp, dP, dA, dO, x1, y1, dx1, dy1, dx2, dy2, phase;
    //Int nx, ny;
    Int os;
    Int niter=6;
    Double pac, pas;
    Double cp,sp; /* parallactic angle cosine / sine */
    Complex Iota; Iota=Complex(0,1);
    IPosition pos(4);pos=0;

    Ray *ray=0, *rayx=0, *rayy=0;
    /* determine parallactic angle rotated coordinates */
    
    os = ap->oversamp;
    //nx = ap->nx*os;
    //ny = ap->ny*os;
    dx = ap->dx/os;
    dy = ap->dy/os;
    x0 = ap->x0 - ap->dx/2.0 + dx/2.0;
    y0 = ap->y0 - ap->dy/2.0 + dy/2.0;
    Rant = a->radius;
    Rhole = a->hole_radius;
    R2 = Rant*Rant;
    H2 = Rhole*Rhole;
    //   for(Int i=0; i < nx; ++i)
     {
       eps = dx/4.0;
       pac = cos(ap->pa+M_PI/2);
       pas = sin(ap->pa+M_PI/2);
      
      x = pac*(x0 + i*dx) - pas*(y0 + j*dy);
      y = pas*(x0 + i*dx) + pac*(y0 + j*dy);
      x = -x;

      if(fabs(x) > Rant) goto nextpoint;
      if(fabs(y) > Rant) goto nextpoint;
      r2 = x*x + y*y;
      if(r2 > R2) goto nextpoint;
      if(r2 < H2) goto nextpoint;
      
      ray = rayx = rayy = 0;
      
      x1 = x;
      y1 = y;
      
      for(Int iter = 0; iter < niter; iter++)
	{
	  ray = trace(a, x1, y1, p);
	  if(!ray) goto nextpoint;
	  x1 += (x - ray->aper[0]);
	  y1 += (y - ray->aper[1]);
	  deleteRay(ray);
	  ray = 0;
	}

      ray = trace(a, x1, y1, p);
      
      /* check for leg blockage */
      if(legplanewaveblock2(a, ray))
	goto nextpoint;
      if(legsphericalwaveblock(a, ray))
	goto nextpoint;
      
      if(y < 0) rayy = trace(a, x1, y1+eps, p);
      else rayy = trace(a, x1, y1-eps, p);
      
      if(x < 0) rayx = trace(a, x1+eps, y1, p);
      else rayx = trace(a, x1-eps, y1, p);
      
      if(ray == 0 || rayx == 0 || rayy == 0)
	goto nextpoint;
      
      /* compute solid angle subtended at the feed */
      dx1 = rayx->aper[0]-ray->aper[0];
      dy1 = rayx->aper[1]-ray->aper[1];
      dx2 = rayy->aper[0]-ray->aper[0];
      dy2 = rayy->aper[1]-ray->aper[1];
      
      dA = 0.5*fabs(dx1*dy2 - dx2*dy1);
      dO = (dOmega(a, rayx, rayy, ray, p)/dA)*dx*dx;
      dP = dO*feedgain(a, ray, p);
      amp = sqrt(dP);
      
      L = Raylen(ray);
      
      phase = 2.0*M_PI*(L-L0)/a->lambda;
      
      /* phase retard the wave */
      cp = cos(phase);
      sp = sin(phase);
      //	    fp = cp + sp*1.0i;
      
      fp = Complex(cp,sp);
      
      
      tracepol(Er, ray, E1);
      Exr = fp*amp*E1[0];
      Eyr = fp*amp*E1[1];
      
      // 	    rr = Exr - 1.0i*Eyr;
      // 	    rl = Exr + 1.0i*Eyr;
      rr = Exr - Iota*Eyr;
      rl = Exr + Iota*Eyr;
      
      tracepol(El, ray, E1);
      Exl = fp*amp*E1[0];
      Eyl = fp*amp*E1[1];
      // 	    lr = Exl - 1.0i*Eyl;
      // 	    ll = Exl + 1.0i*Eyl;
      lr = Exl - Iota*Eyl;
      ll = Exl + Iota*Eyl;
      // 	    pos(0)=(Int)((j/os) - (25.0/dy/os)/2 + shape(0)/2 - 0.5);
      // 	    pos(1)=(Int)((i/os) - (25.0/dx/os)/2 + shape(1)/2 - 0.5);
      // Following 3 lines go with ANT tag in VLACalc.....
      //	    Double antRadius=BeamCalcGeometryDefaults[ap->band].Rant;
      //	    pos(0)=(Int)((j/os) - (antRadius/dy/os) + shape(0)/2 - 0.5);
      //	    pos(1)=(Int)((i/os) - (antRadius/dx/os) + shape(1)/2 - 0.5);
      // Following 2 lines go with the PIX tag in VLACalc...
      pos(0)=(Int)((j/os));
      pos(1)=(Int)((i/os));
      pos(3)=0;

      pos(2)=0;tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+rr,pos);
      pos(2)=1;tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+rl,pos);
      pos(2)=2;tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+lr,pos);
      pos(2)=3;tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+ll,pos);
    nextpoint:
      if(ray)  deleteRay(ray);
      if(rayx) deleteRay(rayx);
      if(rayy) deleteRay(rayy);
    }
  }
  //
  //----------------------------------------------------------------------
  // Compute only the required polarizations.
  //
  Int BeamCalc::calculateAperture(ApertureCalcParams *ap, const Int& whichPoln)
  {
    //Complex fp, Exr, Eyr, Exl, Eyl;
    Complex Er[3], El[3];
    Complex Pr[2], Pl[2]; 
    Complex q[2];
    //Double dx, dy, Rhole, Rant;//x0, y0, R2, H2, eps;
    //Complex rr, rl, lr, ll, tmp;
    Double L0, phase;
    Double sp, cp;
    Double B[3][3];
    calcAntenna *a;
    Pathology *p;
    Int nx, ny, os;
    Int i, j;
    //Double pac, pas; /* parallactic angle cosine / sine */
    Complex Iota; Iota=Complex(0,1);

    //UNUSED: Complex E1[3];
    //UNUSED: Double x,y, r2, L, amp, dP, dA, d0, x1, y1, dx1, dy1, dx2, dy2, dO;
    //UNUSED: Ray *ray, *rayx, *rayy;
    //UNUSED: Int iter;
    //UNUSED: Int niter=6;

    a = newAntennafromApertureCalcParams(ap);
    p = newPathologyfromApertureCalcParams(ap);
    
    /* compute central ray pathlength */
    {
      Ray *tmpRay;
      tmpRay = trace(a, 0.0, 0.00001, p);
      L0 = Raylen(tmpRay);
      deleteRay(tmpRay);
    }
    
    //pac = cos(ap->pa+M_PI/2);
    //pas = sin(ap->pa+M_PI/2);

    if(obsName_p=="EVLA" || obsName_p=="VLA"){
      /* compute polarization vectors in circular basis */
      Pr[0] = 1.0/M_SQRT2; Pr[1] =  Iota/M_SQRT2;
      Pl[0] = 1.0/M_SQRT2; Pl[1] = -Iota/M_SQRT2;

      /* compensate for feed orientation */
      getfeedbasis(a, B); 
      phase = atan2(B[0][1], B[0][0]);
      cp = cos(phase);
      sp = sin(phase);
      
      q[0] = Pr[0];
      q[1] = Pr[1];
      Pr[0] =  Complex(cp,0)*q[0] + Complex(sp,0)*q[1];
      Pr[1] = -Complex(sp,0)*q[0] + Complex(cp,0)*q[1];
      q[0] = Pl[0];
      q[1] = Pl[1];
      Pl[0] =  Complex(cp,0)*q[0] + Complex(sp,0)*q[1];
      Pl[1] = -Complex(sp,0)*q[0] + Complex(cp,0)*q[1];
    }
    else{
      /* in linear basis */
      Pr[0] = 1.0; Pr[1] = 0.0;
      Pl[0] = 0.0; Pl[1] = 1.0;
    }
    
    
    
    /* compute 3-vector feed efields for the two polarizations */
    if ((whichPoln == Stokes::RR) || (whichPoln == Stokes::XX))
      Efield(a, Pr, Er); 
    else if ((whichPoln == Stokes::LL) || (whichPoln == Stokes::YY))
      Efield(a, Pl, El); 
    else
      {Efield(a, Pr, Er); Efield(a, Pl, El);}

    ap->aperture->set(Complex(0.0));
    
    os = ap->oversamp;
    nx = ap->nx*os;
    ny = ap->ny*os;
    // dx = ap->dx/os;
    // dy = ap->dy/os;
    //x0 = ap->x0 - ap->dx/2.0 + dx/2.0;
    //y0 = ap->y0 - ap->dy/2.0 + dy/2.0;
    // Rant = a->radius;
    // Rhole = a->hole_radius;
    //R2 = Rant*Rant;
    //H2 = Rhole*Rhole;
    
    //eps = dx/4.0;
    
    IPosition pos(4);
    //    shape = ap->aperture->shape();

    
    // cerr << "max threads " << omp_get_max_threads() 
    // 	 << " threads available " << omp_get_num_threads() 
    // 	 << endl;
    // Int Nth=1, localWhichPoln=whichPoln;
    Int localWhichPoln=whichPoln;
    //#ifdef _OPENMP
    //Nth=max(omp_get_max_threads()-2,1);
    //#endif
    // Timer tim;
    // tim.mark();
    //#if GCC44x
    //#pragma omp parallel default(none) firstprivate(Er, El, nx, ny, localWhichPoln)  private(i,j) shared(ap, a, p, L0) num_threads(Nth)
    //#else
    //#pragma omp parallel default(none) firstprivate(Er, El, nx, ny, localWhichPoln)  private(i,j) shared(ap, a, p, L0) num_threads(Nth)
    //#endif
    {
      //#pragma omp for
    for(j = 0; j < ny; j++)
      {
	for(i = 0; i < nx; i++)
	  {
	    computePixelValues(ap, a, p, L0, Er, El, i,j,localWhichPoln);
	  }
      }
    }
    // tim.show("BeamCalc:");
    
    deletePathology(p);
    deleteAntenna(a);
    
    return 1;
  }

  void BeamCalc::computePixelValues(const ApertureCalcParams *ap, 
				     const calcAntenna *a, const Pathology *p,
				     const Double &L0,
				     Complex *Er, Complex *El,
				     const Int &i, const Int &j,
				     const Int& whichPoln)
  {
    Complex fp, Exr, Eyr, Exl, Eyl;
    //    Complex Er[3], El[3];
    Complex E1[3];
    Double dx, dy, x0, y0, x, y, r2, Rhole, Rant, R2, H2, eps;
    Complex rr, rl, lr, ll, tmp;
    Double L, amp, dP, dA, dO, x1, y1, dx1, dy1, dx2, dy2, phase;
    //Int nx, ny;
    Int os;
    Int niter=6;
    Double pac, pas, cp,sp; /* parallactic angle cosine / sine */
    Complex Iota; Iota=Complex(0,1);
    IPosition pos(4);pos=0;

    Ray *ray=0, *rayx=0, *rayy=0;
    /* determine parallactic angle rotated coordinates */
    
    os = ap->oversamp;
    //nx = ap->nx*os;
    //ny = ap->ny*os;
    dx = ap->dx/os;
    dy = ap->dy/os;
    x0 = ap->x0 - ap->dx/2.0 + dx/2.0;
    y0 = ap->y0 - ap->dy/2.0 + dy/2.0;
    Rant = a->radius;
    Rhole = a->hole_radius;
    R2 = Rant*Rant;
    H2 = Rhole*Rhole;
    //   for(Int i=0; i < nx; ++i)
     {
      eps = dx/4.0;
      pac = cos(ap->pa+M_PI/2);
      pas = sin(ap->pa+M_PI/2);
      
      x = pac*(x0 + i*dx) - pas*(y0 + j*dy);
      y = pas*(x0 + i*dx) + pac*(y0 + j*dy);
      x = -x;
      
      if(fabs(x) > Rant) goto nextpoint;
      if(fabs(y) > Rant) goto nextpoint;
      r2 = x*x + y*y;
      if(r2 > R2) goto nextpoint;
      if(r2 < H2) goto nextpoint;
      
      ray = rayx = rayy = 0;
      
      x1 = x;
      y1 = y;
      
      for(Int iter = 0; iter < niter; iter++)
	{
	  ray = trace(a, x1, y1, p);
	  if(!ray) goto nextpoint;
	  x1 += (x - ray->aper[0]);
	  y1 += (y - ray->aper[1]);
	  deleteRay(ray);
	  ray = 0;
	}
      
      ray = trace(a, x1, y1, p);
      
      /* check for leg blockage */
      if(legplanewaveblock2(a, ray))
	goto nextpoint;
      if(legsphericalwaveblock(a, ray))
	goto nextpoint;
      
      if(y < 0) rayy = trace(a, x1, y1+eps, p);
      else rayy = trace(a, x1, y1-eps, p);
      
      if(x < 0) rayx = trace(a, x1+eps, y1, p);
      else rayx = trace(a, x1-eps, y1, p);
      
      if(ray == 0 || rayx == 0 || rayy == 0)
	goto nextpoint;
      
      /* compute solid angle subtended at the feed */
      dx1 = rayx->aper[0]-ray->aper[0];
      dy1 = rayx->aper[1]-ray->aper[1];
      dx2 = rayy->aper[0]-ray->aper[0];
      dy2 = rayy->aper[1]-ray->aper[1];
      
      dA = 0.5*fabs(dx1*dy2 - dx2*dy1);
      dO = (dOmega(a, rayx, rayy, ray, p)/dA)*dx*dx;
      dP = dO*feedgain(a, ray, p);
      amp = sqrt(dP);
      
      L = Raylen(ray);
      
      phase = 2.0*M_PI*(L-L0)/a->lambda;
      
      /* phase retard the wave */
      cp = cos(phase);
      sp = sin(phase);
      //	    fp = cp + sp*1.0i;
      
      fp = Complex(cp,sp);
      
      
      tracepol(Er, ray, E1);
      Exr = fp*amp*E1[0];
      Eyr = fp*amp*E1[1];
      // 	    rr = Exr - 1.0i*Eyr;
      // 	    rl = Exr + 1.0i*Eyr;
      rr = Exr - Iota*Eyr;
      rl = Exr + Iota*Eyr;

      tracepol(El, ray, E1);
      Exl = fp*amp*E1[0];
      Eyl = fp*amp*E1[1];
      // 	    lr = Exl - 1.0i*Eyl;
      // 	    ll = Exl + 1.0i*Eyl;
      lr = Exl - Iota*Eyl;
      ll = Exl + Iota*Eyl;
      
      // 	    pos(0)=(Int)((j/os) - (25.0/dy/os)/2 + shape(0)/2 - 0.5);
      // 	    pos(1)=(Int)((i/os) - (25.0/dx/os)/2 + shape(1)/2 - 0.5);
      // Following 3 lines go with ANT tag in VLACalc.....
      //	    Double antRadius=BeamCalcGeometryDefaults[ap->band].Rant;
      //	    pos(0)=(Int)((j/os) - (antRadius/dy/os) + shape(0)/2 - 0.5);
      //	    pos(1)=(Int)((i/os) - (antRadius/dx/os) + shape(1)/2 - 0.5);
      // Following 2 lines go with the PIX tag in VLACalc...
      pos(0)=(Int)((j/os));
      pos(1)=(Int)((i/os));
      pos(2)=0;
      pos(3)=0;
      
      if (whichPoln==Stokes::RR)
	{tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+rr,pos);}
      else if (whichPoln==Stokes::RL)
	{tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+rl,pos);}
      else if (whichPoln==Stokes::LR)
	{tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+lr,pos);}
      else if (whichPoln==Stokes::LL)
	{tmp=ap->aperture->getAt(pos);ap->aperture->putAt(tmp+ll,pos);}
      else {
	SynthesisError err(String("BeamCalc::computePixelValues: Cannot handle Stokes ")+String(whichPoln));
	throw(err);
      } 

    nextpoint:
      if(ray)  deleteRay(ray);
      if(rayx) deleteRay(rayx);
      if(rayy) deleteRay(rayy);
    }
  }

  //
  //----------------------------------------------------------------------
  // Compute only the required polarizations.for linear polarization
  //
  Int BeamCalc::calculateApertureLinPol(ApertureCalcParams *ap, const Int& whichPoln)
  {
    Complex Ex[3], Ey[3];
    Complex Px[2], Py[2]; 
    //Double dx, dy, Rhole, Rant;//x0, y0, R2, H2, eps;
    Double L0;
    calcAntenna *a;
    Pathology *p;
    Int nx, ny, os;
    Int i, j;
    //Double pac, pas; /* parallactic angle cosine / sine */
    //    Complex Iota=Complex(0,1);


    a = newAntennafromApertureCalcParams(ap);
    p = newPathologyfromApertureCalcParams(ap);
    
    /* compute central ray pathlength */
    {
      Ray *tmpRay;
      tmpRay = trace(a, 0.0, 0.00001, p);
      L0 = Raylen(tmpRay);
      deleteRay(tmpRay);
    }
    
    //pac = cos(ap->pa+M_PI/2);
    //pas = sin(ap->pa+M_PI/2);

    /* in linear basis */
    Px[0] = 0.0; Px[1] = 1.0;
    Py[0] = 1.0; Py[1] = 0.0;
    
    IPosition pos(4); pos=0;
    
    /* compute 3-vector feed efields for the two polarizations */
    Efield(a, Py, Ey); 
    Efield(a, Px, Ex);

    if (whichPoln == Stokes::XX){
      pos(2)=0;
    }
    else if (whichPoln == Stokes::YY){
      pos(2)=3;
    }
    else if (whichPoln == Stokes::XY){ 
      pos(2)=1;
    }
    else if (whichPoln == Stokes::YX){ 
      pos(2)=2;
    }
    else {
      SynthesisError err(String("BeamCalc::calculateApertureLinPol: Cannot handle Stokes ")+String(whichPoln));
      throw(err);
    } 

    // set only the affected plane to zero
    for(j = 0; j < ap->nx; j++){
      pos(0)= j;
      for(i = 0; i < ap->ny; i++){
	pos(1)= i;
	ap->aperture->putAt(Complex(0.),pos);
      }
    }
    
    os = ap->oversamp;
    nx = ap->nx*os;
    ny = ap->ny*os;
    // dx = ap->dx/os;
    // dy = ap->dy/os;
    //x0 = ap->x0 - ap->dx/2.0 + dx/2.0;
    //y0 = ap->y0 - ap->dy/2.0 + dy/2.0;
    // Rant = a->radius;
    // Rhole = a->hole_radius;
    //R2 = Rant*Rant;
    //H2 = Rhole*Rhole;
    
    //eps = dx/4.0;
    
    // cerr << "max threads " << omp_get_max_threads() 
    // 	 << " threads available " << omp_get_num_threads() 
    // 	 << endl;
    Int Nth=1, localWhichPoln=whichPoln;
#ifdef _OPENMP
    Nth=max(omp_get_max_threads()-2,1);
#endif
    // Timer tim;
    // tim.mark();
#if GCC44x
#pragma omp parallel default(none) firstprivate(Ex, Ey, nx, ny, localWhichPoln)  private(i,j) shared(ap, a, p, L0) num_threads(Nth)
#else
#pragma omp parallel default(none) firstprivate(Ex, Ey, nx, ny, localWhichPoln)  private(i,j) shared(ap, a, p, L0) num_threads(Nth)
#endif
    {
#pragma omp for
    for(j = 0; j < ny; j++)
      {
	for(i = 0; i < nx; i++)
	  {
	    computePixelValuesLinPol(ap, a, p, L0, Ex, Ey, i,j,localWhichPoln);
	  }
      }
    }
    // tim.show("BeamCalc:");
    
    deletePathology(p);
    deleteAntenna(a);
    
    return 1;
  }

  void BeamCalc::computePixelValuesLinPol(const ApertureCalcParams *ap, 
					  const calcAntenna *a, const Pathology *p,
					  const Double &L0,
					  Complex *Ex, Complex *Ey,
					  const Int &i, const Int &j,
					  const Int& whichPoln)
  {
    Complex fp, Exx, Eyx, Exy, Eyy;

    Complex E1[3];
    Double dx, dy, x0, y0, x, y, r2, Rhole, Rant, R2, H2, eps;
    Complex xx, xy, yx, yy, tmp;
    Double L, amp, dP, dA, dO, x1, y1, dx1, dy1, dx2, dy2, phase;
    //Int nx, ny;
    Int os;
    Int niter=6;
    Double pac, pas, cp,sp; /* parallactic angle cosine / sine */
    Complex Iota; Iota=Complex(0,1);
    IPosition pos(4);pos=0;

    Ray *ray=0, *rayx=0, *rayy=0;
    /* determine parallactic angle rotated coordinates */
    
    os = ap->oversamp;
    //nx = ap->nx*os;
    //ny = ap->ny*os;
    dx = ap->dx/os;
    dy = ap->dy/os;
    x0 = ap->x0 - ap->dx/2.0 + dx/2.0;
    y0 = ap->y0 - ap->dy/2.0 + dy/2.0;
    Rant = a->radius;
    Rhole = a->hole_radius;
    R2 = Rant*Rant;
    H2 = Rhole*Rhole;
    //   for(Int i=0; i < nx; ++i)
     {
      eps = dx/4.0;
      pac = cos(ap->pa+M_PI/2);
      pas = sin(ap->pa+M_PI/2);
      
      x = pac*(x0 + i*dx) - pas*(y0 + j*dy);
      y = pas*(x0 + i*dx) + pac*(y0 + j*dy);
      x = -x;
      
      if(fabs(x) > Rant) goto nextpoint;
      if(fabs(y) > Rant) goto nextpoint;
      r2 = x*x + y*y;
      if(r2 > R2) goto nextpoint;
      if(r2 < H2) goto nextpoint;
      
      ray = rayx = rayy = 0;
      
      x1 = x;
      y1 = y;
      
      for(Int iter = 0; iter < niter; iter++)
	{
	  ray = trace(a, x1, y1, p);
	  if(!ray) goto nextpoint;
	  x1 += (x - ray->aper[0]);
	  y1 += (y - ray->aper[1]);
	  deleteRay(ray);
	  ray = 0;
	}
      
      ray = trace(a, x1, y1, p);
      
      /* check for leg blockage */
      if(legplanewaveblock2(a, ray))
	goto nextpoint;
      if(legsphericalwaveblock(a, ray))
	goto nextpoint;
      
      if(y < 0) rayy = trace(a, x1, y1+eps, p);
      else rayy = trace(a, x1, y1-eps, p);
      
      if(x < 0) rayx = trace(a, x1+eps, y1, p);
      else rayx = trace(a, x1-eps, y1, p);
      
      if(ray == 0 || rayx == 0 || rayy == 0)
	goto nextpoint;
      
      /* compute solid angle subtended at the feed */
      dx1 = rayx->aper[0]-ray->aper[0];
      dy1 = rayx->aper[1]-ray->aper[1];
      dx2 = rayy->aper[0]-ray->aper[0];
      dy2 = rayy->aper[1]-ray->aper[1];
      
      dA = 0.5*fabs(dx1*dy2 - dx2*dy1);
      dO = (dOmega(a, rayx, rayy, ray, p)/dA)*dx*dx;
      dP = dO*feedgain(a, ray, p);
      amp = sqrt(dP);
      
      L = Raylen(ray);
      
      phase = 2.0*M_PI*(L-L0)/a->lambda;
      
      /* phase retard the wave */
      cp = cos(phase);
      sp = sin(phase);
      
      fp = Complex(cp,sp);
      
      
      tracepol(Ex, ray, E1);
      Exx = fp*amp*E1[0];
      Eyx = fp*amp*E1[1];

      tracepol(Ey, ray, E1);
      Exy = fp*amp*E1[0];
      Eyy = fp*amp*E1[1];


      xx = Exx;
      xy = Complex(0.);
      yx = Complex(0.);
      yy = Eyy;

      // 	    pos(0)=(Int)((j/os) - (25.0/dy/os)/2 + shape(0)/2 - 0.5);
      // 	    pos(1)=(Int)((i/os) - (25.0/dx/os)/2 + shape(1)/2 - 0.5);
      // Following 3 lines go with ANT tag in VLACalc.....
      //	    Double antRadius=BeamCalcGeometryDefaults[ap->band].Rant;
      //	    pos(0)=(Int)((j/os) - (antRadius/dy/os) + shape(0)/2 - 0.5);
      //	    pos(1)=(Int)((i/os) - (antRadius/dx/os) + shape(1)/2 - 0.5);
      // Following 2 lines go with the PIX tag in VLACalc...
      pos(0)=(Int)((j/os));
      pos(1)=(Int)((i/os));
      pos(3)=0;

      if (whichPoln==Stokes::XX){
	pos(2)=0;
	tmp=ap->aperture->getAt(pos);
	ap->aperture->putAt(tmp+xx,pos);
      }

      else if (whichPoln==Stokes::XY){
	pos(2)=1;
	tmp=ap->aperture->getAt(pos);
	ap->aperture->putAt(tmp+xy,pos);
      }

      else if (whichPoln==Stokes::YX){
	pos(2)=2;
	tmp=ap->aperture->getAt(pos);
	ap->aperture->putAt(tmp+yx,pos);
      }

      else if (whichPoln==Stokes::YY){
	pos(2)=3;
	tmp=ap->aperture->getAt(pos); 
	ap->aperture->putAt(tmp+yy,pos); 
      }

    nextpoint:
      if(ray)  deleteRay(ray);
      if(rayx) deleteRay(rayx);
      if(rayy) deleteRay(rayy);
    }
  }
};

