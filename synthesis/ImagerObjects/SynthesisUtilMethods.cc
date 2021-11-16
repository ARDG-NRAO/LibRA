//# SynthesisUtilMethods.cc: 
//# Copyright (C) 2013-2018
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/sstream.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>

#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <casa/OS/DirectoryIterator.h>
#include <casa/OS/File.h>
#include <casa/OS/Path.h>

#include <casa/OS/HostInfo.h>

#include <images/Images/TempImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <imageanalysis/Utilities/SpectralImageUtil.h>
#include <measures/Measures/MeasTable.h>
#include <measures/Measures/MRadialVelocity.h>
#include <ms/MSSel/MSSelection.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MSDopplerUtil.h>
#include <tables/Tables/Table.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <synthesis/TransformMachines2/Utils.h>

#include <mstransform/MSTransform/MSTransformRegridder.h>
#include <msvis/MSVis/MSUtil.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>
#include <msvis/MSVis/VisBufferUtil.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits>
#include <tuple>
#include <sys/time.h>
#include<sys/resource.h>

#include <synthesis/ImagerObjects/SIImageStore.h>
#include <synthesis/ImagerObjects/SIImageStoreMultiTerm.h>

// supporting code for getAllocatedMemoryInBytes()
#if defined(AIPS_DARWIN) || defined(AIPS_CRAY_PGI) || __cplusplus <= 199711L
#include <casa/OS/Memory.h>
#else
#include <malloc.h>
#include <string>
#include <stdio.h>
#endif

using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  casacore::String SynthesisUtilMethods::g_hostname;
  casacore::String SynthesisUtilMethods::g_startTimestamp;
  const casacore::String SynthesisUtilMethods::g_enableOptMemProfile =
      "synthesis.imager.memprofile.enable";

  SynthesisUtilMethods::SynthesisUtilMethods()
  {
    
  }
  
  SynthesisUtilMethods::~SynthesisUtilMethods() 
  {
  }
  
  Int SynthesisUtilMethods::validate(const VisBuffer& vb)
  {
    Int N=vb.nRow(),M=-1;
    for(Int i=0;i<N;i++)
      {
	if ((!vb.flagRow()(i)) && (vb.antenna1()(i) != vb.antenna2()(i)))
	  {M++;break;}
      }
    return M;
  }

  Int SynthesisUtilMethods::validate(const vi::VisBuffer2& vb)
  {
    Int N=vb.nRows(),M=-1;
    for(Int i=0;i<N;i++)
      {
	if ((!vb.flagRow()(i)) && (vb.antenna1()(i) != vb.antenna2()(i)))
	  {M++;break;}
      }
    return M;
  }
  // Get the next largest even composite of 2,3,5.
  // This is to ensure a 'good' image size for FFTW.
  // Translated from gcwrap/scripts/cleanhelper.py : getOptimumSize
  Int SynthesisUtilMethods::getOptimumSize(const Int npix)
  {
    Int n=npix;

    if( n%2 !=0 ){ n+= 1; }

    Vector<uInt> fac = primeFactors(n, false);
    Int val, newlarge;
    for( uInt k=0; k< fac.nelements(); k++ )
      {
	if( fac[k]>5 )
	  {
	    val = fac[k];
	    while( max( primeFactors(val) ) > 5 ){ val+=1;}
	    fac[k] = val;
	  }
      }
    newlarge=product(fac);
    for( Int k=n; k<newlarge; k+=2 )
      {
	if( max( primeFactors(k) ) < 6 ) {return k;}
      }
    return newlarge;
  }

  // Return the prime factors of the given number
  Vector<uInt> SynthesisUtilMethods::primeFactors(uInt n, Bool /*douniq*/)
  {
    Vector<uInt> factors;
    
    Int lastresult = n;
    Int sqlast = int(sqrt(n))+1;
   
    if(n==1){ factors.resize(1);factors[0]=1;return factors;}
    Int c=2;
    while(1)
      {
	if( lastresult == 1 || c > sqlast ) { break; }
	sqlast = (Int)(sqrt(lastresult))+1;
	while(1)
	  {
	    if( c>sqlast){ c=lastresult; break; }
	    if( lastresult % c == 0 ) { break; }
	    c += 1;
	  }
	factors.resize( factors.nelements()+1, true );
	factors[ factors.nelements()-1 ] = c;
	lastresult /= c;
      }
    if( factors.nelements()==0 ) { factors.resize(1);factors[0]=n; }

    //if( douniq ) { factors = unique(factors); }

    /*    
	  /// The Sort::unique isn't working as called below. Results appear to be the
	  /// same as with the cleanhelper python code, so leaving as is for not. CAS-7889
    if( douniq )
      {
	cout << "Test unique fn on : " << factors << endl;
	Sort srt;
	Vector<uInt> unvec=factors; uInt nrec;
	srt.unique(unvec,nrec);
	cout << " Unique : " << unvec << " nr : " << nrec << endl;
      }
    */

    return factors;
  }


  Bool SynthesisUtilMethods::fitPsfBeam(const String& imagename, const Int nterms, const Float psfcutoff)
  {
    LogIO os(LogOrigin("SynthesisUtilMethods", "fitPsfBeam"));

    if (psfcutoff >=1.0 || psfcutoff<=0.0)
      {
	os << "psfcutoff must be >0 and <1" << LogIO::WARN;
	return false;
      }

    std::shared_ptr<SIImageStore> imstore;
    if( nterms>1 )
      { imstore = std::shared_ptr<SIImageStore>(new SIImageStoreMultiTerm( imagename, nterms, true, true ));   }
    else
      { imstore = std::shared_ptr<SIImageStore>(new SIImageStore( imagename, true, true ));   }
  

    os << "Fitting PSF beam for Imagestore : " << imstore->getName() << LogIO::POST;

    imstore->makeImageBeamSet(psfcutoff, true);

    imstore->printBeamSet();

    imstore->releaseLocks();
    
    return true;
  }





  /***make a record of synthesisimager::weight parameters***/
  Record SynthesisUtilMethods::fillWeightRecord(const String& type, const String& rmode,
			       const Quantity& noise, const Double robust,
			       const Quantity& fieldofview,
				 const Int npixels, const Bool multiField, const Bool useCubeBriggs,
			       const String& filtertype, const Quantity& filterbmaj,
                                                const Quantity& filterbmin, const Quantity& filterbpa, const Double& fracBW){

    Record outRec;
    outRec.define("type", type);
    outRec.define("rmode", rmode);
    Record quantRec;
    QuantumHolder(noise).toRecord(quantRec);
    outRec.defineRecord("noise", quantRec);
    outRec.define("robust", robust);
    QuantumHolder(fieldofview).toRecord(quantRec);
    outRec.defineRecord("fieldofview", quantRec);
    outRec.define("npixels", npixels);
    outRec.define("multifield", multiField);
    outRec.define("usecubebriggs", useCubeBriggs);
    outRec.define("filtertype", filtertype);
    QuantumHolder(filterbmaj).toRecord(quantRec);
    outRec.defineRecord("filterbmaj", quantRec);
    QuantumHolder(filterbmin).toRecord(quantRec);
    outRec.defineRecord("filterbmin", quantRec);
    QuantumHolder(filterbpa).toRecord(quantRec);
    outRec.defineRecord("filterbpa", quantRec);
    outRec.define("fracBW", fracBW);

    return outRec;
  }
  void SynthesisUtilMethods::getFromWeightRecord(String& type, String& rmode,
			       Quantity& noise, Double& robust,
			       Quantity& fieldofview,
				Int& npixels, Bool& multiField, Bool& useCubeBriggs,
			       String& filtertype, Quantity& filterbmaj,
                                                 Quantity& filterbmin, Quantity& filterbpa, Double& fracBW, const Record& inRec){
    QuantumHolder qh;
    String err;
    if(!inRec.isDefined("type"))
      throw(AipsError("Record is not filled with SynthesisUtilMethods::fillWeightRecord"));
    inRec.get("type", type);
    inRec.get("rmode", rmode);
    if(!qh.fromRecord(err, inRec.asRecord("noise")))
      throw(AipsError("Error in reading noise param"));
    noise=qh.asQuantity();
    inRec.get("robust", robust);
    if(!qh.fromRecord(err, inRec.asRecord("fieldofview")))
      throw(AipsError("Error in reading fieldofview param"));
    fieldofview=qh.asQuantity();
    inRec.get("npixels", npixels);
    inRec.get("multifield", multiField);
    inRec.get("usecubebriggs", useCubeBriggs);
    inRec.get("filtertype", filtertype);
    if(!qh.fromRecord(err, inRec.asRecord("filterbmaj")))
      throw(AipsError("Error in reading filterbmaj param"));
    filterbmaj=qh.asQuantity();
    if(!qh.fromRecord(err, inRec.asRecord("filterbmin")))
      throw(AipsError("Error in reading filterbmin param"));
    filterbmin=qh.asQuantity();
    if(!qh.fromRecord(err, inRec.asRecord("filterbpa")))
      throw(AipsError("Error in reading filterbpa param"));
    filterbpa=qh.asQuantity();
    inRec.get("fracBW", fracBW);



  }
  
  
  /**
   * Get values from lines of a /proc/self/status file. For example:
   * 'VmRSS:     600 kB'
   * @param str line from status file
   * @return integer value (memory amount, etc.)
   */
  Int SynthesisUtilMethods::parseProcStatusLine(const std::string &str) {
    istringstream is(str);
    std::string token;
    is >> token;
    is >> token;
    Int value = stoi(token);
    return value;
  }

  /**
   * Produces a name for a 'memprofile' output file. For example:
   * casa.synthesis.imager.memprofile.23514.pc22555.hq.eso.org.20171209_120446.txt
   * (where 23514 is the PID passed as input parameter).
   *
   * @param pid PID of the process running the imager
   *
   * @return a longish 'memprofile' filename including PID, machine, timestamp, etc.
   **/
  String SynthesisUtilMethods::makeResourceFilename(int pid)
  {
    if (g_hostname.empty() or g_startTimestamp.empty()) {
      // TODO: not using HOST_NAME_MAX because of issues with __APPLE__
      // somehow tests tAWPFTM, tSynthesisImager, and tSynthesisImagerVi2 fail.
      const int strMax = 255;
      char hostname[strMax];
      gethostname(hostname, strMax);
      g_hostname = hostname;

      auto time = std::time(nullptr);
      auto gmt = std::gmtime(&time);
      const char* format = "%Y%m%d_%H%M%S";
      char timestr[strMax];
      std::strftime(timestr, strMax, format, gmt);
      g_startTimestamp = timestr;
    }

    return String("casa.synthesis.imager.memprofile." + String::toString(pid) +
		  "." + g_hostname + "." + g_startTimestamp + ".txt");
  }

    Bool SynthesisUtilMethods::adviseChanSel(Double& freqStart, Double& freqEnd, 
		       const Double& freqStep,  const MFrequency::Types& freqframe,
		       Vector<Int>& spw, Vector<Int>& start,
					     Vector<Int>& nchan, const String& ms, const String& ephemtab, const Int field_id, const Bool getFreqRange, const String spwselection){

  LogIO os(LogOrigin("SynthesisUtilMethods", "adviseChanSel"));
  if(ms==String("")){
    throw(AipsError("Need a valid MS"));
  }
  spw.resize();
  start.resize();
  nchan.resize();
  try {
    if(!getFreqRange){
      Vector<Int> bnchan;
      Vector<Int>  bstart;
      Vector<Int>  bspw;
      Double fS, fE;
      fS=freqStart;
      fE=freqEnd;
      if(freqEnd < freqStart){
	fS=freqEnd;
	fE=freqStart;
      }
    
      
      {
	
	MeasurementSet elms(String(ms), TableLock(TableLock::AutoNoReadLocking), Table::Old);
	if(ephemtab != "" && freqframe == MFrequency::REST ){
	   MSUtil::getSpwInSourceFreqRange(bspw, bstart, bnchan, elms, fS, fE, fabs(freqStep), ephemtab, field_id);
	}
	else
	  MSUtil::getSpwInFreqRange(bspw, bstart, bnchan, elms, fS, fE, fabs(freqStep), freqframe, field_id);
	elms.relinquishAutoLocks(true);

      }
      spw=Vector<Int> (bspw);
      start=Vector<Int> (bstart);
      nchan=Vector<Int> (bnchan);
    }
    else{
    
      {
	MeasurementSet elms(ms, TableLock(TableLock::AutoNoReadLocking), Table::Old);
	MSSelection thisSelection;
	String spsel=spwselection;
	if(spsel=="")spsel="*";
	thisSelection.setSpwExpr(spsel);
	TableExprNode exprNode=thisSelection.toTableExprNode(&elms);
	Matrix<Int> chanlist=thisSelection.getChanList();
	if(chanlist.ncolumn() <3){
	  freqStart=-1.0;
	  freqEnd=-1.0;
	  return false;
	}
	Vector<Int> elspw=chanlist.column(0);
	Vector<Int> elstart=chanlist.column(1);
	Vector<Int> elnchan=Vector<Int> (chanlist.column(2)-elstart)+1;
	if(ephemtab != "" ){
	  const MSColumns mscol(ms);
	  MEpoch ep=mscol.timeMeas()(0);
	  Quantity sysvel;
	  String ephemTable("");
	  MDirection::Types mtype=MDirection::APP;
	  MDirection mdir(mtype);
	  if(Table::isReadable(ephemtab)){
	    ephemTable=ephemtab;
	  }
	  else if(ephemtab=="TRACKFIELD"){
	   ephemTable=(mscol.field()).ephemPath(field_id); 
	  }
	  else if(MDirection::getType(mtype, ephemtab)){
	    mdir=MDirection(mtype);
	  }
	  
	  MSUtil::getFreqRangeAndRefFreqShift(freqStart, freqEnd, sysvel, ep, elspw, elstart, elnchan, elms, ephemTable , mdir, True);

	}
	else
	  MSUtil::getFreqRangeInSpw(freqStart, freqEnd, elspw, elstart, elnchan, elms, freqframe, field_id);
      }

    }



        
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
    return false;
  } 
  catch (...){
    os << LogIO::SEVERE << "Unknown  exception handled" 
       << LogIO::POST;
    return false;
    
  }
  
  return true;
  
  }

  void SynthesisUtilMethods::getResource(String label, String fname)
  {
     // TODO: not tested on anything else than LINUX (MACOS for the future)
#if !defined(AIPS_LINUX)
      return;
#endif

     Bool isOn = false;
     AipsrcValue<Bool>::find(isOn, g_enableOptMemProfile);
     if (!isOn)
         return;

     // TODO: reorganize, use struct or something to hold and pass info over. ifdef lnx
     LogIO casalog( LogOrigin("SynthesisUtilMethods", "getResource", WHERE) );

     // To hold memory stats, in MB
     int vmRSS = -1, vmHWM = -1, vmSize = -1, vmPeak = -1, vmSwap = -1;
     pid_t pid = -1;
     int fdSize = -1;

     // TODO: this won't probably work on anything but linux
     ifstream procFile("/proc/self/status");
     if (procFile.is_open()) {
       std::string line;
       while (not procFile.eof()) {
	 getline(procFile, line);
	 const std::string startVmRSS = "VmRSS:";
	 const std::string startVmWHM = "VmHWM:";
	 const std::string startVmSize = "VmSize:";
	 const std::string startVmPeak = "VmPeak:";
	 const std::string startVmSwap = "VmSwap:";
	 const std::string startFDSize = "FDSize:";
         const double KB_TO_MB = 1024.0;
	 if (startVmRSS == line.substr(0, startVmRSS.size())) {
	   vmRSS = parseProcStatusLine(line.c_str()) / KB_TO_MB;
	 } else if (startVmWHM == line.substr(0, startVmWHM.size())) {
	   vmHWM = parseProcStatusLine(line.c_str()) / KB_TO_MB;
	 } else if (startVmSize == line.substr(0, startVmSize.size())) {
	   vmSize = parseProcStatusLine(line.c_str()) / KB_TO_MB;
         } else if (startVmPeak == line.substr(0, startVmPeak.size())) {
	   vmPeak = parseProcStatusLine(line.c_str()) / KB_TO_MB;
	 } else if (startVmSwap == line.substr(0, startVmSwap.size())) {
	   vmSwap = parseProcStatusLine(line.c_str()) / KB_TO_MB;
	 } else if (startFDSize == line.substr(0, startFDSize.size())) {
	   fdSize = parseProcStatusLine(line.c_str());
	 }
       }
       procFile.close();
     }

     pid = getpid();

     struct rusage usage;
     struct timeval now;
     getrusage(RUSAGE_SELF, &usage);
     now = usage.ru_utime;

     // TODO: check if this works as expected when /proc/self/status is not there
     // Not clear at all if VmHWM and .ru_maxrss measure the same thing
     // Some alternative is needed for the other fields as well: VmSize, VMHWM, FDSize.
     if (vmHWM < 0) {
       vmHWM = usage.ru_maxrss;
     }

     ostringstream oss;
     oss << "PID: " << pid ;
     oss << " MemRSS (VmRSS): " << vmRSS << " MB.";
     oss << " VmWHM: " << vmHWM << " MB.";
     oss << " VirtMem (VmSize): " << vmSize << " MB.";
     oss << " VmPeak: " << vmPeak << " MB.";
     oss << " VmSwap: " << vmSwap << " MB.";
     oss << " ProcTime: " << now.tv_sec << '.' << now.tv_usec;
     oss << " FDSize: " << fdSize;
     oss <<  " [" << label << "] ";
     casalog << oss.str() << LogIO::NORMAL3 <<  LogIO::POST;

     // Write this to a file too...
     try {
       if (fname.empty()) {
         fname = makeResourceFilename(pid);
       }
       ofstream ofile(fname, ios::app);
       if (ofile.is_open()) {
         if (0 == ofile.tellp()) {
             casalog << g_enableOptMemProfile << " is enabled, initializing output file for "
                 "imager profiling information (memory and run time): " << fname <<
                 LogIO::NORMAL <<  LogIO::POST;
             ostringstream header;
             header << "# PID, MemRSS_(VmRSS)_MB, VmWHM_MB, VirtMem_(VmSize)_MB, VmPeak_MB, "
                 "VmSwap_MB, ProcTime_sec, FDSize, label_checkpoint";
             ofile << header.str() << '\n';
         }
         ostringstream line;
         line << pid << ',' << vmRSS << ',' << vmHWM << ',' << vmSize << ','
              << vmPeak << ','<< vmSwap << ',' << now.tv_sec << '.' << now.tv_usec << ','
              << fdSize << ',' << '[' << label << ']';
         ofile << line.str() << '\n';
         ofile.close();
       }
     } catch(std::runtime_error &exc) {
         casalog << "Could not write imager memory+runtime information into output file: "
                 << fname << LogIO::WARN <<  LogIO::POST;
     }
  }


  // Data partitioning rules for CONTINUUM imaging
  //
  //  ALL members of the selection parameters in selpars are strings
  //  ONLY.  This methods reads the selection parameters from selpars
  //  and returns a partitioned Record with npart data selection
  //  entries.
  //
  //  The algorithm used to do the partitioning along the TIME axis is
  //  as follows:
  //    
  //    for each MS in selpars
  //      - get the selection parameters
  //      - generate a selected MS
  //      - get number of rows in the selected MS
  //      - divide the rows in nparts
  //      - for each part
  //          - get compute rowBeg and rowEnd
  //          - modify rowEnd such that rowEnd points to the end of
  //            full integration data.  This is done as follows:
  //               tRef = TIME(rowEnd);
  //               reduce rowEnd till TIME(rowEnd) != tRef
  //          - Construct a T0~T1 string
  //          - Fill it in the timeSelPerPart[msID][PartNo] array
  //
  Record SynthesisUtilMethods::continuumDataPartition(Record &selpars, const Int npart)
  {
    LogIO os( LogOrigin("SynthesisUtilMethods","continuumDataPartition",WHERE) );

    Record onepart, allparts;
    Vector<Vector<String> > timeSelPerPart;
    timeSelPerPart.resize(selpars.nfields());

    // Duplicate the entire input record npart times, with a specific partition id.
    // Modify each sub-record according to partition id.
    for (uInt msID=0;msID<selpars.nfields();msID++)
      {
	Record thisMS= selpars.subRecord(RecordFieldId("ms"+String::toString(msID)));
	String msName = thisMS.asString("msname");
	timeSelPerPart[msID].resize(npart,true);
	//
	// Make a selected MS and extract the time-column information
	//
	MeasurementSet ms(msName,TableLock(TableLock::AutoNoReadLocking), Table::Old),
	  selectedMS(ms);
	MSInterface msI(ms);	MSSelection msSelObj; 
	msSelObj.reset(msI,MSSelection::PARSE_NOW,
		       thisMS.asString("timestr"),
		       thisMS.asString("antenna"),
		       thisMS.asString("field"),
		       thisMS.asString("spw"),
		       thisMS.asString("uvdist"),
		       thisMS.asString("taql"),
		       "",//thisMS.asString("poln"),
		       thisMS.asString("scan"),
		       "",//thisMS.asString("array"),
		       thisMS.asString("state"),
		       thisMS.asString("obs")//observation
		       );
	msSelObj.getSelectedMS(selectedMS);

	//--------------------------------------------------------------------
	// Use the selectedMS to generate time selection strings per part
	//
	//	Double Tint;
	MSMainColumns mainCols(selectedMS);
	Vector<rownr_t> rowNumbers = selectedMS.rowNumbers();
	Int nRows=selectedMS.nrow(), 
	  dRows=nRows/npart;
	Int rowBegID=0, rowEndID=nRows-1;
	Int rowBeg=rowNumbers[rowBegID], rowEnd=rowNumbers[rowEndID];
	//cerr << "NRows, dRows, npart = " << nRows << " " << dRows << " " << npart << " " << rowBeg << " " << rowEnd << endl;

	rowEndID = rowBegID + dRows;
	

	MVTime mvInt=mainCols.intervalQuant()(0);
	//Time intT(mvInt.getTime());
	//	Tint = intT.modifiedJulianDay();

	Int partNo=0;
	// The +1 in rowBeg and rowEnd below is because it appears
	// that TaQL by default counts from 1, not 0.
	while(rowEndID < nRows)
	  {
	    //	    rowBeg=rowNumbers[rowBegID]; rowEnd = rowNumbers[rowEndID];
	    rowBeg=rowBegID+1; rowEnd = rowEndID+1;
	    stringstream taql;
	    taql << "ROWNUMBER() >= " << rowBeg << " && ROWNUMBER() <= " << rowEnd;
	    timeSelPerPart[msID][partNo] = taql.str();

	    if (partNo == npart - 1) break;
	    partNo++;
	    rowBegID = rowEndID+1;
	    rowEndID = min(rowBegID + dRows, nRows-1);
	    if (rowEndID == nRows-1) break;
	  }

	//rowBeg=rowNumbers[rowBegID]; rowEnd = rowNumbers[nRows-1];
	stringstream taql;
	rowBeg=rowBegID+1; rowEnd = nRows-1+1;
	taql << "ROWNUMBER() >= " << rowBeg << " && ROWNUMBER() <= " << rowEnd;
	timeSelPerPart[msID][partNo] = taql.str();
	os << endl << "Rows = " << rowBeg << " " << rowEnd << " "
	   << "[P][M]: " << msID << ":" << partNo << " " << timeSelPerPart[msID][partNo]
	   << LogIO::POST;	    
      }
    //
    // The time selection strings for all parts of the current
    // msID are in timeSelPerPart.  
    //--------------------------------------------------------------------
    //
    // Now reverse the order of parts and ME loops. Create a Record
    // per part, get the MS for thisPart.  Put the associated time
    // selection string in it.  Add the thisMS to thisPart Record.
    // Finally add thisPart Record to the allparts Record.
    //
    for(Int iPart=0; iPart<npart; iPart++)
      {
	Record thisPart;
	thisPart.assign(selpars);
	for (uInt msID=0; msID<selpars.nfields(); msID++)	  
	  {
	    Record thisMS = thisPart.subRecord(RecordFieldId("ms"+String::toString(msID)));

	    thisMS.define("taql",timeSelPerPart[msID][iPart]);
	    thisPart.defineRecord(RecordFieldId("ms"+String::toString(msID)) , thisMS);
	  }
	allparts.defineRecord(RecordFieldId(String::toString(iPart)), thisPart);
      }
    //    cerr << allparts << endl;
    return allparts;

    // for( Int part=0; part < npart; part++)
    //   {

    // 	onepart.assign(selpars);


    // 	//-------------------------------------------------
    // 	// WARNING : This is special-case code for two specific datasets
    // 	for ( uInt msid=0; msid<selpars.nfields(); msid++)
    // 	  {
    // 	    Record onems = onepart.subRecord( RecordFieldId("ms"+String::toString(msid)) );
    // 	    String msname = onems.asString("msname");
    // 	    String spw = onems.asString("spw");
    // 	    if(msname.matches("DataTest/twopoints_twochan.ms"))
    // 	      {
    // 		onems.define("spw", spw+":"+String::toString(part));
    // 	      }
    // 	    if(msname.matches("DataTest/point_twospws.ms"))
    // 	      {
    // 		onems.define("spw", spw+":"+ (((Bool)part)?("10~19"):("0~9"))  );
    // 	      }
    // 	    if(msname.matches("DataTest/reg_mawproject.ms"))
    // 	      {
    // 		onems.define("scan", (((Bool)part)?("1~17"):("18~35"))  );
    // 	      }
    // 	    onepart.defineRecord( RecordFieldId("ms"+String::toString(msid)) , onems );
    // 	  }// end ms loop
    // 	//-------------------------------------------------

    // 	allparts.defineRecord( RecordFieldId(String::toString(part)), onepart );

    //   }// end partition loop

    // return allparts;
  }


  // Data partitioning rules for CUBE imaging
  Record SynthesisUtilMethods::cubeDataPartition(const Record &selpars, const Int npart,
		  const Double freqBeg, const Double freqEnd, const MFrequency::Types eltype)
  {
    LogIO os( LogOrigin("SynthesisUtilMethods","cubeDataPartition",WHERE) );
    // Temporary special-case code. Please replace with actual rules.
    Vector<Double> fstart(npart);
    Vector<Double> fend(npart);
    Double step=(freqEnd-freqBeg)/Double(npart);
    fstart(0)=freqBeg;
    fend(0)=freqBeg+step;
    for (Int k=1; k < npart; ++k){
    	fstart(k)=fstart(k-1)+step;
    	fend(k)=fend(k-1)+step;
    }
    return cubeDataPartition( selpars, fstart, fend, eltype );

  }


  Record SynthesisUtilMethods::cubeDataImagePartition(const Record & selpars, const CoordinateSystem&
				    incsys, const Int npart, const Int nchannel, 
				    Vector<CoordinateSystem>& outCsys,
						 Vector<Int>& outnChan){

    LogIO os( LogOrigin("SynthesisUtilMethods","cubeDataImagePartition",WHERE) );
    outnChan.resize(npart);
    outCsys.resize(npart);
    Int nomnchan=nchannel/npart;
    outnChan.set(nomnchan);
    nomnchan=nchannel%npart;
    for (Int k=0; k < nomnchan; ++k)
      outnChan[k]+=1;
    Vector<Int> shp(0);
    //shp(0)=20; shp(1)=20; shp(2)=1; shp(3)=outnChan[0];
    Vector<Float> shift(4, 0.0);
    Vector<Float> fac(4, 1.0);
    Vector<Double> freqEnd(npart);
    Vector<Double> freqStart(npart);
    Float chanshift=0.0;
    for (Int k =0; k <npart; ++k){
      shift(3)=chanshift;
      //cerr << k << " shift " << shift << endl;
      outCsys[k]=incsys.subImage(shift, fac, shp);
      freqStart[k]=SpectralImageUtil::worldFreq(outCsys[k], 0.0);
      freqEnd[k]=SpectralImageUtil::worldFreq(outCsys[k], Double(outnChan[k]-1));
      if(freqStart[k] > freqEnd[k]){
	Double tmp=freqEnd[k];
	freqEnd[k]=freqStart[k];
	freqStart[k]=tmp;
      }
      chanshift+=Float(outnChan[k]);      
    }
     MFrequency::Types eltype=incsys.spectralCoordinate(incsys.findCoordinate(Coordinate::SPECTRAL)).frequencySystem(true);

     //os << "freqStart="<<freqStart<<" freqend="<<freqEnd<< "eltype="<<eltype<<LogIO::POST;
     Record rec=cubeDataPartition(selpars, freqStart, freqEnd, eltype);
     for (Int k=0; k < npart ; ++k){
       outCsys[k].save(rec.asrwRecord(String::toString(k)), "coordsys");
       rec.asrwRecord(String::toString(k)).define("nchan", outnChan[k]);
     }
     return rec;
  }

  Record SynthesisUtilMethods::cubeDataPartition(const Record &selpars, const Vector<Double>& freqBeg, const Vector<Double>&freqEnd, const MFrequency::Types eltype){
    LogIO os( LogOrigin("SynthesisUtilMethods","cubeDataPartition",WHERE) );
    Record retRec;
    Int npart=freqBeg.shape()(0);
    for (Int k=0; k < npart; ++k){
      Int nms=selpars.nfields();
      Record partRec;
      for(Int j=0; j < nms; ++j){
    	  if(selpars.isDefined(String("ms"+String::toString(j)))){
    		  Record msRec=selpars.asRecord(String("ms"+String::toString(j)));
    		  if(!msRec.isDefined("msname"))
    			  throw(AipsError("No msname key in ms record"));
    		  String msname=msRec.asString("msname");
    		  String userspw=msRec.isDefined("spw")? msRec.asString("spw") : "*";
    		  String userfield=msRec.isDefined("field") ? msRec.asString("field") : "*";
                  String userstate=msRec.isDefined("state") ? msRec.asString("state") : "*";
                  
    		  MeasurementSet elms(msname);
    		  Record laSelection=elms.msseltoindex(userspw, userfield);
                  if (userfield=="")  userfield="*";
                  MSSelection mssel;
                  mssel.setSpwExpr(userspw);
                  mssel.setFieldExpr(userfield);
                  mssel.setStateExpr(userstate);
                  TableExprNode exprNode = mssel.toTableExprNode(&elms);
    		  Matrix<Int> spwsel=mssel.getChanList();
    		  Vector<Int> fieldsel=mssel.getFieldList();
                  // case for scan intent specified 
                  if (userstate!="*") {
                    MeasurementSet elselms((elms)(exprNode), &elms);
                    MSColumns tmpmsc(elselms);
                    Vector<Int> fldidv=tmpmsc.fieldId().getColumn();
                    if (fldidv.nelements()==0)
                      throw(AipsError("No field ids were selected, please check input parameters"));
                    std::set<Int> ufldids(fldidv.begin(),fldidv.end());
                    std::vector<Int> tmpv(ufldids.begin(), ufldids.end());
                    fieldsel.resize(tmpv.size());
                    uInt count=0;
                    for (std::vector<int>::const_iterator it=tmpv.begin();it != tmpv.end(); it++)
                    {
                      fieldsel(count) = *it;
                      count++;
                    }
                  }
    		  //Matrix<Int> spwsel=laSelection.asArrayInt("channel");
    		  //Vector<Int> fieldsel=laSelection.asArrayInt("field");
    		  Vector<Int> freqSpw;
    		  Vector<Int> freqStart;
    		  Vector<Int> freqNchan;
		  String newspw;
		  try{
		    MSUtil::getSpwInFreqRange(freqSpw, freqStart, freqNchan, elms, freqBeg(k), freqEnd(k),0.0, eltype, fieldsel[0]);
		    newspw=mergeSpwSel(freqSpw, freqStart, freqNchan, spwsel);
		    //cerr << "try " << freqSpw <<  "  " << freqStart << "  " << freqNchan << endl;
		  }
		  catch(...){
		    //cerr << "In catch " << endl;
		    newspw="";
		  }
		  //String newspw=mergeSpwSel(freqSpw, freqStart, freqNchan, spwsel);
		  if(newspw=="") newspw="-1";
    		  msRec.define("spw", newspw);
    		  partRec.defineRecord(String("ms"+String::toString(j)),msRec);
    	  }

      }
      retRec.defineRecord(String::toString(k), partRec);
    }




    return retRec;
  }


 String  SynthesisUtilMethods::mergeSpwSel(const Vector<Int>& fspw, const Vector<Int>& fstart, const Vector<Int>& fnchan, const Matrix<Int>& spwsel)
  {
	 String retval="";
	 Int cstart, cend;
  	  for(Int k=0; k < fspw.shape()(0); ++k){
  		  cstart=fstart(k);
  		  cend=fstart(k)+fnchan(k)-1;
  		  for (Int j=0; j < spwsel.shape()(0); ++j){
  			//need to put this here as multiple rows can have the same spw
  			cstart=fstart(k);
  			cend=fstart(k)+fnchan(k)-1;
  			if(spwsel(j,0)==fspw[k]){
			  if(cstart < spwsel(j,1)) cstart=spwsel(j,1);
			  if(cend > spwsel(j,2)) cend= spwsel(j,2);
  				if(!retval.empty()) retval=retval+(",");
  				retval=retval+String::toString(fspw[k])+":"+String::toString(cstart)+"~"+String::toString(cend);
  			}
  		  }
  	  }



  	  return retval;
  }

  // Image cube partitioning rules for CUBE imaging
  Record SynthesisUtilMethods::cubeImagePartition(Record &impars, Int npart)
  {
    LogIO os( LogOrigin("SynthesisUtilMethods","cubeImagePartition",WHERE) );

    Record onepart, allparts;

    // Duplicate the entire input record npart times, with a specific partition id.
    // Modify each sub-record according to partition id.
    for( Int part=0; part < npart; part++)
      {

	onepart.assign(impars);

	//-------------------------------------------------
	// WARNING : This is special-case code for two specific datasets
	for ( uInt imfld=0; imfld<impars.nfields(); imfld++)
	  {
	    Record onefld = onepart.subRecord( RecordFieldId(String::toString(imfld)) );
	    Int nchan = onefld.asInt("nchan");
	    //String freqstart = onems.asString("freqstart");

	    onefld.define("nchan", nchan/npart);
	    onefld.define("freqstart", (((Bool)part)?("1.2GHz"):("1.0GHz"))  );

	    String imname = onefld.asString("imagename");
	    onefld.define("imagename", imname+".n"+String::toString(part));

	    onepart.defineRecord( RecordFieldId( String::toString(imfld) ), onefld );
	  }// end ms loop
	//-------------------------------------------------

	allparts.defineRecord( RecordFieldId(String::toString(part)), onepart );

      }// end partition loop

    return allparts;
    

  }

  String SynthesisUtilMethods::asComprehensibleDirectionString(MDirection const &direction)
  {
    MVAngle mvRa=direction.getAngle().getValue()(0);
    MVAngle mvDec=direction.getAngle().getValue()(1);
    ostringstream oos;
    oos << "     ";
    Int widthRA=20;
    Int widthDec=20;
    oos.setf(ios::left, ios::adjustfield);
    oos.width(widthRA);  oos << mvRa(0.0).string(MVAngle::TIME,8);
    oos.width(widthDec); oos << mvDec.string(MVAngle::DIG2,8);
    oos << "     "
        << MDirection::showType(direction.getRefPtr()->getType());
    return String(oos);
  }

  uLong SynthesisUtilMethods::getAllocatedMemoryInBytes()
  {
#if defined(AIPS_DARWIN) || defined(AIPS_CRAY_PGI) || __cplusplus <= 199711L
    // If on mac, or if compiling with < c++11, then use the old function.
    return (uLong) allocatedMemoryInBytes();
#else
    // NOTE: Don't trust mallinfo if you've ever allocated more than 2GB at once. (see mallinfo bugs section)
    //       Prefer malloc_info() over mallinfo().

    // Get the human-readable xml info dump.
    char *buf;
    size_t bufsize;
    FILE *fp = open_memstream(&buf, &bufsize);
    if (fp == NULL)
        return 0;
    malloc_info(0, fp);
    fclose(fp);

    // Extract the "mmap" info out. For example, there will be this line at the end of the xml when ~2GB is allocated:
    // <total type="mmap" count="14" size="2147409920"/>
    // I (BGB) believe that "count" is the number of mmap block structs that are holding onto the memory, and "size" is the total of those blocks.
    // The returned value for this example would be 2147409920.
    std::string strbuf(buf);
    size_t totalpos = strbuf.rfind("<total type=\"mmap\" count=\"");
    size_t startpos = strbuf.find("size=\"", totalpos); startpos += 6; // strlen("size=\"") == 6
    size_t endpos   = strbuf.find("\"", startpos);
    if (totalpos == std::string::npos || startpos == std::string::npos || endpos == std::string::npos)
        return 0;
    std::string sizestr = strbuf.substr(startpos, endpos-startpos);
    return (uLong) std::stoul(sizestr);
#endif
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////    Parameter Containers     ///////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Read String from Record
  String SynthesisParams::readVal(const Record &rec, String id, String& val) const
  {
    if( rec.isDefined( id ) )
      {
	String inval("");
	if( rec.dataType( id )==TpString ) 
	  { rec.get( id , inval );  // Read into temp string
	    //	    val = inval;
	    //	    return String("");
	    // Set value only if it is not a null string. Otherwise, leave it unchanged as it will
	    // retain the default value that was set before this function was called.
	    if(inval.length()>0){val=inval;}
	    return String(""); 
	  }
	else { return String(id + " must be a string\n"); }
      }
    else { return String("");}
  }

  // Read Integer from Record
  String SynthesisParams::readVal(const Record &rec, String id, Int& val) const
  {
    if( rec.isDefined( id ) )
      {
	if( rec.dataType( id )==TpInt ) { rec.get( id , val ); return String(""); }
	else  { return String(id + " must be an integer\n"); }
      }
    else { return String(""); }
  }

  // Read Float from Record
  String SynthesisParams::readVal(const Record &rec, String id, Float& val) const
  {
    if( rec.isDefined( id ) )
      {
      if ( rec.dataType( id )==TpFloat || rec.dataType( id )==TpDouble )  
	{ rec.get( id , val ); return String(""); }
      else { return String(id + " must be a float\n"); }
      }
    else { return String(""); }
  }

  // Read Bool from Record
  String SynthesisParams::readVal(const Record &rec, String id, Bool& val) const
  {
    if( rec.isDefined( id ) )
      {
	if( rec.dataType( id )==TpBool ) { rec.get( id , val ); return String(""); }
	else { return String(id + " must be a bool\n"); }
      }
    else{ return String(""); }
  }

  // Read Vector<Int> from Record
  String SynthesisParams::readVal(const Record &rec, String id, Vector<Int>& val) const
  {
    if( rec.isDefined( id ) )
      {
	if( rec.dataType( id )==TpArrayInt || rec.dataType( id )==TpArrayUInt ) 
	  { rec.get( id , val ); return String(""); }
	else if ( rec.dataType( id ) == TpArrayBool ) // An empty python vector [] comes in as this.
	  {
	    Vector<Bool> vec; rec.get( id, vec );
	    if( vec.nelements()==0 ){val.resize(0); return String("");}
	    else{ return String(id + " must be a vector of strings.\n"); }
	  }
	else { return String(id + " must be a vector of integers\n"); }
      }
    else{ return String(""); }
  }

  // Read Vector<Float> from Record
  String SynthesisParams::readVal(const Record &rec, String id, Vector<Float>& val) const
  {
    if( rec.isDefined( id ) )
      {
	if( rec.dataType( id )==TpArrayFloat )
	  { 
	    rec.get( id , val ); return String(""); 
	    /*
	    Array<Float> vec; rec.get(id, vec );
	    cout << " vec : " << vec << endl;
	    if( vec.shape().nelements()==1 )
	      {
		val.resize( vec.shape()[0] );
		for(uInt i=0;i<val.nelements();i++){val[i]=(Float)vec(IPosition(1,i));}
		return String("");
	      }
	    else { return String(id + " must be a 1D vector of floats"); }
	    */
	  }
	else if ( rec.dataType( id ) ==TpArrayDouble ) 
	  {
	    Vector<Double> vec; rec.get( id, vec );
	    val.resize(vec.nelements());
	    for(uInt i=0;i<val.nelements();i++){val[i]=(Float)vec[i];}
	    return String("");
	  }
	else if ( rec.dataType( id ) ==TpArrayInt ) 
	  {
	    Vector<Int> vec; rec.get( id, vec );
	    val.resize(vec.nelements());
	    for(uInt i=0;i<val.nelements();i++){val[i]=(Float)vec[i];}
	    return String("");
	  }
	else if ( rec.dataType( id ) == TpArrayBool ) // An empty python vector [] comes in as this.
	  {
	    Vector<Bool> vec; rec.get( id, vec );
	    if( vec.shape().product()==0 ){val.resize(0); return String("");}
	    else{ return String(id + " must be a vector of strings.\n"); }
	    // val.resize(0); return String("");
	  }
	else { return String(id + " must be a vector of floats\n"); }
      }
    else{ return String(""); }
  }

  // Read Vector<String> from Record
  String SynthesisParams::readVal(const Record &rec, String id, Vector<String>& val) const
  {
    if( rec.isDefined( id ) )
      {
	if( rec.dataType( id )==TpArrayString || rec.dataType( id )==TpArrayChar ) 
	  { rec.get( id , val ); return String(""); }
	else if ( rec.dataType( id ) == TpArrayBool ) // An empty python vector [] comes in as this.
	  {
	    Vector<Bool> vec; rec.get( id, vec );
	    if( vec.nelements()==0 ){val.resize(0); return String("");}
	    else{ return String(id + " must be a vector of strings.\n"); }
	  }
	else { return String(id + " must be a vector of strings.\n"); 
	}
      }
    else{ return String(""); }
  }

  // Convert String to Quantity
  String SynthesisParams::stringToQuantity(String instr, Quantity& qa) const
  {
    //QuantumHolder qh;
    //String error;
    //    if( qh.fromString( error, instr ) ) { qa = qh.asQuantity(); return String(""); }
    //else { return String("Error in converting " + instr + " to a Quantity : " + error + " \n"); }
    if ( casacore::Quantity::read( qa, instr ) ) { return String(""); }
    else  { return String("Error in converting " + instr + " to a Quantity \n"); }
  }

  // Convert String to MDirection
  // UR : TODO :    If J2000 not specified, make it still work.
  String SynthesisParams::stringToMDirection(String instr, MDirection& md) const
  {
    try
      {
	String tmpRF, tmpRA, tmpDEC;
	std::istringstream iss(instr);
	iss >> tmpRF >> tmpRA >> tmpDEC;
	if( tmpDEC.length() == 0 )// J2000 may not be specified.
	  {
	    tmpDEC = tmpRA;
	    tmpRA = tmpRF;
	    tmpRF = String("J2000");
	  }
	casacore::Quantity tmpQRA;
	casacore::Quantity tmpQDEC;
	casacore::Quantity::read(tmpQRA, tmpRA);
	casacore::Quantity::read(tmpQDEC, tmpDEC);

	if(tmpQDEC.getFullUnit()==Unit("deg") && tmpDEC.contains(":")){
	  LogIO os( LogOrigin("SynthesisParams","stringToMDirection",WHERE) );
	  os << LogIO::WARN 
	     << "You provided the Declination/Latitude value \""<< tmpDEC
	     << "\" which is understood to be in units of hours.\n"
	     << "If you meant degrees, please replace \":\" by \".\"."
	     << LogIO::POST;
	}

	MDirection::Types theRF;
	Bool status = MDirection::getType(theRF, tmpRF);
	if (!status) {
	  throw AipsError();
	}
	md = MDirection (tmpQRA, tmpQDEC, theRF);
	return String("");
      }
    catch(AipsError &x)
      {
	return String("Error in converting '" + instr + "' to MDirection. Need format : 'J2000 19:59:28.500 +40.44.01.50'\n");
      }
  }

  // Read Quantity from a Record string
  String SynthesisParams::readVal(const Record &rec, String id, Quantity& val) const
  {
    if( rec.isDefined( id ) )
      {
	if( rec.dataType( id )==TpString ) 
	  { String valstr;  rec.get( id , valstr ); return stringToQuantity(valstr, val); }
	else { return String(id + " must be a string in the format : '1.5GHz' or '0.2arcsec'...'\n"); }
      }
    else{ return String(""); }
  }

  // Read MDirection from a Record string
  String SynthesisParams::readVal(const Record &rec, String id, MDirection& val) const
  {
    if( rec.isDefined( id ) )
      {
	if( rec.dataType( id )==TpString ) 
	  { String valstr;  rec.get( id , valstr ); return stringToMDirection(valstr, val); }
	else { return String(id + " must be a string in the format : 'J2000 19:59:28.500 +40.44.01.50'\n"); }
      }
    else{ return String(""); }
  }

  // Convert MDirection to String
  String SynthesisParams::MDirectionToString(MDirection val) const
  {
    MVDirection mvpc( val.getAngle() );
    MVAngle ra = mvpc.get()(0);
    MVAngle dec = mvpc.get()(1);
    // Beware of precision here......( for VLBA / ALMA ). 14 gives 8 digits after the decimal for arcsec.
    return  String(val.getRefString() + " " + ra.string(MVAngle::TIME,14) + " " +  dec.string(MVAngle::ANGLE,14));
  }

  // Convert Quantity to String
  String SynthesisParams::QuantityToString(Quantity val) const
  {
    std::ostringstream ss;
    //use digits10 to ensure the conersions involve use full decimal digits guranteed to be 
    //correct plus extra digits to deal with least significant digits (or replace with
    // max_digits10 when it is available)
    ss.precision(std::numeric_limits<double>::digits10+2);
    ss << val;
    return ss.str();
    // NOTE - 2017.10.04: It was found (CAS-10773) that we cannot use to_string for this as
    // the decimal place is fixed to 6 digits. 
    //TT: change to C++11 to_string which handles double value to string conversion 
    //return String(std::to_string( val.getValue(val.getUnit()) )) + val.getUnit() ;
  }
  
  // Convert Record contains Quantity or Measure quantities to String
  String SynthesisParams::recordQMToString(const Record &rec) const
  { 
     Double val;
     String unit;
     if ( rec.isDefined("m0") ) 
       {
         Record subrec = rec.subRecord("m0");
         subrec.get("value",val); 
         subrec.get("unit",unit);
       }
     else if (rec.isDefined("value") )
       {
         rec.get("value",val);
         rec.get("unit",unit);
       }
     return String::toString(val) + unit;
  } 


  /////////////////////// Selection Parameters

  SynthesisParamsSelect::SynthesisParamsSelect():SynthesisParams()
  {
    setDefaults();
  }

  SynthesisParamsSelect::~SynthesisParamsSelect()
  {
  }

  SynthesisParamsSelect::SynthesisParamsSelect(const SynthesisParamsSelect& other){
	  operator=(other);
  }

  SynthesisParamsSelect& SynthesisParamsSelect::operator=(const SynthesisParamsSelect& other){
	  if(this!=&other) {
		  msname=other.msname;
		      spw=other.spw;
		      freqbeg=other.freqbeg;
		      freqend=other.freqend;
		      freqframe=other.freqframe;
		      field=other.field;
		      antenna=other.antenna;
		      timestr=other.timestr;
		      scan=other.scan;
		      obs=other.obs;
		      state=other.state;
		      uvdist=other.uvdist;
		      taql=other.taql;
		      usescratch=other.usescratch;
		      readonly=other.readonly;
		      incrmodel=other.incrmodel;
		      datacolumn=other.datacolumn;

	  }
	  return *this;
  }

  void SynthesisParamsSelect::fromRecord(const Record &inrec)
  {
    setDefaults();

    String err("");

    try
      {
	
	err += readVal( inrec, String("msname"), msname );

	err += readVal( inrec, String("readonly"), readonly );
	err += readVal( inrec, String("usescratch"), usescratch );

	// override with entries from savemodel.
	String savemodel("");
	err += readVal( inrec, String("savemodel"), savemodel );
	if( savemodel=="none" ){usescratch=false; readonly=true;}
	else if( savemodel=="virtual" ){usescratch=false; readonly=false;}
	else if ( savemodel=="modelcolumn" ){ usescratch=true; readonly=false; }

	err += readVal( inrec, String("incrmodel"), incrmodel );

	err += readVal( inrec, String("spw"), spw );

	/// -------------------------------------------------------------------------------------
	// Why are these params here ? Repeats of defineimage.
	err += readVal( inrec, String("freqbeg"), freqbeg);
	err += readVal( inrec, String("freqend"), freqend);

	String freqframestr( MFrequency::showType(freqframe) );
	err += readVal( inrec, String("outframe"), freqframestr);
	if( ! MFrequency::getType(freqframe, freqframestr) )
	  { err += "Invalid Frequency Frame " + freqframestr ; }
	/// -------------------------------------------------------------------------------------

	err += readVal( inrec, String("field"),field);
	err += readVal( inrec, String("antenna"),antenna);
	err += readVal( inrec, String("timestr"),timestr);
	err += readVal( inrec, String("scan"),scan);
	err += readVal( inrec, String("obs"),obs);
	err += readVal( inrec, String("state"),state);
	err += readVal( inrec, String("uvdist"),uvdist);
	err += readVal( inrec, String("taql"),taql);

	err += readVal( inrec, String("datacolumn"),datacolumn);

	err += verify();

      }
    catch(AipsError &x)
      {
	err = err + x.getMesg() + "\n";
      }
      
      if( err.length()>0 ) throw(AipsError("Invalid Selection Parameter set : " + err));
      
  }

  String SynthesisParamsSelect::verify() const
  {
    String err;
    // Does the MS exist on disk.
    Directory thems( msname );
    if( thems.exists() )
      {
	// Is it readable ? 
	if( ! thems.isReadable() )
	  { err += "MS " + msname + " is not readable.\n"; }
	// Depending on 'readonly', is the MS writable ? 
	if( readonly==false && ! thems.isWritable() ) 
	  { err += "MS " + msname + " is not writable.\n"; }
      }
    else 
      { err += "MS does not exist : " + msname + "\n"; }
    
    return err;
  }
  
  void SynthesisParamsSelect::setDefaults()
  {
    msname="";
    spw="";
    freqbeg="";
    freqend="";
    MFrequency::getType(freqframe,"LSRK");
    field="";
    antenna="";
    timestr="";
    scan="";
    obs="";
    state="";
    uvdist="";
    taql="";
    usescratch=false;
    readonly=true;
    incrmodel=false;
    datacolumn="corrected";
  }

  Record SynthesisParamsSelect::toRecord()const
  {
    Record selpar;
    selpar.define("msname",msname);
    selpar.define("spw",spw);
    selpar.define("freqbeg",freqbeg);
    selpar.define("freqend",freqend);
    selpar.define("freqframe", MFrequency::showType(freqframe)); // Convert MFrequency::Types to String
    //looks like fromRecord looks for outframe !
    selpar.define("outframe", MFrequency::showType(freqframe)); 
    selpar.define("field",field);
    selpar.define("antenna",antenna);
    selpar.define("timestr",timestr);
    selpar.define("scan",scan);
    selpar.define("obs",obs);
    selpar.define("state",state);
    selpar.define("uvdist",uvdist);
    selpar.define("taql",taql);
    selpar.define("usescratch",usescratch);
    selpar.define("readonly",readonly);
    selpar.define("incrmodel",incrmodel);
    selpar.define("datacolumn",datacolumn);

    return selpar;
  }


  /////////////////////// Image Parameters

  SynthesisParamsImage::SynthesisParamsImage():SynthesisParams()
  {
    setDefaults();
  }

  SynthesisParamsImage::~SynthesisParamsImage()
  {
  }

  SynthesisParamsImage& SynthesisParamsImage::operator=(const SynthesisParamsImage& other){
    if(this != &other){
      imageName=other.imageName;
      stokes=other.stokes;
      startModel.resize(); startModel=other.startModel;
      imsize.resize(); imsize=other.imsize;
      cellsize.resize(); cellsize=other.cellsize;
      projection=other.projection;
      useNCP=other.useNCP;
      phaseCenter=other.phaseCenter;
      phaseCenterFieldId=other.phaseCenterFieldId;
      obslocation=other.obslocation;
      pseudoi=other.pseudoi;
      nchan=other.nchan;
      nTaylorTerms=other.nTaylorTerms;
      chanStart=other.chanStart;
      chanStep=other.chanStep;
      freqStart=other.freqStart;
      freqStep=other.freqStep;
      refFreq=other.refFreq;
      velStart=other.velStart;
      velStep=other.velStep;
      freqFrame=other.freqFrame;
      mFreqStart=other.mFreqStart;
      mFreqStep=other.mFreqStep;
      mVelStart=other.mVelStart;
      mVelStep=other.mVelStep;
      restFreq.resize(); restFreq=other.restFreq;
      start=other.start;
      step=other.step;
      frame=other.frame;
      veltype=other.veltype;
      mode=other.mode;
      reffreq=other.reffreq;
      sysvel=other.sysvel;
      sysvelframe=other.sysvelframe;
      sysvelvalue=other.sysvelvalue;
      qmframe=other.qmframe;
      mveltype=other.mveltype;
      tststr=other.tststr;
      startRecord=other.startRecord;
      stepRecord=other.stepRecord;
      reffreqRecord=other.reffreqRecord;
      sysvelRecord=other.sysvelRecord;
      restfreqRecord=other.restfreqRecord;
      csysRecord=other.csysRecord;
      csys=other.csys;
      imshape.resize(); imshape=other.imshape;
      freqFrameValid=other.freqFrameValid;
      overwrite=other.overwrite;
      deconvolver=other.deconvolver;
      distance=other.distance;
      trackDir=other.trackDir;
      trackSource=other.trackSource;
      movingSource=other.movingSource;



    }

    return *this;

  }

  void SynthesisParamsImage::fromRecord(const Record &inrec)
  {
    setDefaults();
    String err("");

    try
      {

	err += readVal( inrec, String("imagename"), imageName);

	//// imsize
	if( inrec.isDefined("imsize") ) 
	  {
	    DataType tp = inrec.dataType("imsize");
	    
	    if( tp == TpInt ) // A single integer for both dimensions.
	      {
		Int npix; inrec.get("imsize", npix);
		imsize.resize(2);
		imsize.set( npix );
	      }
	    else if( tp == TpArrayInt ) // An integer array : [ nx ] or [ nx, ny ]
	      {
		Vector<Int> ims;
		inrec.get("imsize", ims);
		if( ims.nelements()==1 ) // [ nx ]
		  {imsize.set(ims[0]); }
		else if( ims.nelements()==2 ) // [ nx, ny ]
		  { imsize[0]=ims[0]; imsize[1]=ims[1]; }
		else // Wrong array length
		  {err += "imsize must be either a single integer, or a vector of two integers\n";  }
	      }
	    else // Wrong data type
	      { err += "imsize must be either a single integer, or a vector of two integers\n";   }

	  }//imsize
	
	//// cellsize
	if( inrec.isDefined("cell") ) 
	  {
	    try
	      {
		DataType tp = inrec.dataType("cell");
		if( tp == TpInt ||  
		    tp == TpFloat || 
		    tp == TpDouble )
		  {
		    Double cell = inrec.asDouble("cell");
		    cellsize.set( Quantity( cell , "arcsec" ) );
		  }
		else if ( tp == TpArrayInt ||  
			  tp == TpArrayFloat || 
			  tp == TpArrayDouble )
		  {
		    Vector<Double> cells;
		    inrec.get("cell", cells);
		    if(cells.nelements()==1) // [ cellx ]
		      {cellsize.set( Quantity( cells[0], "arcsec" ) ); }
		    else if( cells.nelements()==2 ) // [ cellx, celly ]
		      { cellsize[0]=Quantity(cells[0],"arcsec"); cellsize[1]=Quantity(cells[1],"arcsec"); }
		    else // Wrong array length
		      {err += "cellsize must be a single integer/string, or a vector of two integers/strings\n";  }
		  }
		else if( tp == TpString )
		  {
		    String cell;
		    inrec.get("cell",cell);
		    Quantity qcell;
		    err += stringToQuantity( cell, qcell );
		    cellsize.set( qcell );
		  }
		else if( tp == TpArrayString )
		  {
		    Array<String> cells;
		    inrec.get("cell", cells);
		    Vector<String> vcells(cells);
		    if(cells.nelements()==1) // [ cellx ]
		      { 
			Quantity qcell; 
			err+= stringToQuantity( vcells[0], qcell ); cellsize.set( qcell ); 
		      }
		    else if( cells.nelements()==2 ) // [ cellx, celly ]
		      { 
			err+= stringToQuantity( vcells[0], cellsize[0] );
			err+= stringToQuantity( vcells[1], cellsize[1] );
		      }
		    else // Wrong array length
		      {err += "cellsize must be a single integer/string, or a vector of two integers/strings\n";  }
		  }
		else // Wrong data type
		  { err += "cellsize must be a single integer/string, or a vector of two integers/strings\n";   }
		
	      } 
	    catch(AipsError &x)
	      {
		err += "Error reading cellsize : " + x.getMesg();
	      }
	  }// cellsize

	//// stokes
	err += readVal( inrec, String("stokes"), stokes);
	if(stokes.matches("pseudoI"))
	  {
	    stokes="I";
	    pseudoi=true;
	  }
	else {pseudoi=false;}

	/// PseudoI

	////nchan
	err += readVal( inrec, String("nchan"), nchan);

	/// phaseCenter (as a string) . // Add INT support later.
	//err += readVal( inrec, String("phasecenter"), phaseCenter );
	if( inrec.isDefined("phasecenter") )
	  {
	    String pcent("");
	    if( inrec.dataType("phasecenter") == TpString )
	      {
		inrec.get("phasecenter",pcent);
		if( pcent.length() > 0 ) // if it's zero length, it means 'figure out from first field in MS'.
		  {
		    err += readVal( inrec, String("phasecenter"), phaseCenter );
		    phaseCenterFieldId=-1;
		    //// Phase Center Field ID.... if explicitly specified, and not via phasecenter.
		    //   Need this, to deal with a null phase center being translated to a string to go back out.
		    err += readVal( inrec, String("phasecenterfieldid"), phaseCenterFieldId);
		  }
		//else {  phaseCenterFieldId=0; } // Take the first field of the MS.
		else {  phaseCenterFieldId=-2; } // deal with this later in buildCoordinateSystem to assign the first selected field 
	      }
	    if (inrec.dataType("phasecenter")==TpInt 
		|| inrec.dataType("phasecenter")==TpFloat 
		|| inrec.dataType("phasecenter")==TpDouble )
	      {
		// This will override the previous setting to 0 if the phaseCenter string is zero length.
		err += readVal( inrec, String("phasecenter"), phaseCenterFieldId );
	      }

	    if( ( inrec.dataType("phasecenter") != TpString && inrec.dataType("phasecenter")!=TpInt
		  && inrec.dataType("phasecenter")!=TpFloat && inrec.dataType("phasecenter")!=TpDouble ) )
	      //		|| ( phaseCenterFieldId==-1 ) )
	      {
		err += String("Cannot set phasecenter. Please specify a string or int\n");
	      }
	  }

	
	//// Projection
	if( inrec.isDefined("projection") )
	  {
	    if( inrec.dataType("projection") == TpString )
	      {
		String pstr;
		inrec.get("projection",pstr);

		try
		  {
		    if( pstr.matches("NCP") )
		      {
			pstr ="SIN";
			useNCP=true;
		      }
		    projection=Projection::type( pstr );
		  }
		catch(AipsError &x)
		  {
		    err += String("Invalid projection code : " + pstr );
		  }
	      }
	    else { err += "projection must be a string\n"; } 
	  }//projection

	// Frequency frame stuff. 
	err += readVal( inrec, String("specmode"), mode);
	// Alias for 'mfs' is 'cont'
	if(mode=="cont") mode="mfs";

        err += readVal( inrec, String("outframe"), frame);
        qmframe="";
        // mveltype is only set when start/step is given in mdoppler
        mveltype=""; 
        //start 
        String startType("");
        String widthType("");
        if( inrec.isDefined("start") ) 
          {
            if( inrec.dataType("start") == TpInt ) 
              {
	        err += readVal( inrec, String("start"), chanStart);
	        start = String::toString(chanStart);
                startType="chan";
              }
            else if( inrec.dataType("start") == TpString ) 
              {
	        err += readVal( inrec, String("start"), start);
                if( start.contains("Hz") ) 
                  {
                    stringToQuantity(start,freqStart);
                    startType="freq";
                  }
                else if( start.contains("m/s") )
                  {
                    stringToQuantity(start,velStart); 
                    startType="vel";
                  } 
              }
            else if ( inrec.dataType("start") == TpRecord ) 
              {
                //record can be freq in Quantity or MFreaquency or vel in Quantity or
                //MRadialVelocity or Doppler (by me.todoppler())
                // ** doppler => converted to radialvel with frame 
                startRecord = inrec.subRecord("start");
                if(startRecord.isDefined("m0") )
                  { 
                    //must be a measure
                    String mtype;
                    startRecord.get("type", mtype);
                    if( mtype=="frequency")
                      { 
                        //mfrequency
                        startRecord.get(String("refer"), qmframe);
                        if ( frame!="" && frame!=qmframe)
                          {
                            // should emit warning to the logger
                            cerr<<"The frame in start:"<<qmframe<<" Override frame="<<frame<<endl;
                          }
                        start = recordQMToString(startRecord);
                        stringToQuantity(start,freqStart);
                        startType="freq";
                      }
                    else if( mtype=="radialvelocity")
                      {
                        //mradialvelocity
                        startRecord.get(String("refer"), qmframe);
                        if ( frame!="" && frame!=qmframe)
                          {
                            // should emit warning to the logger
                            cerr<<"The frame in start:"<<qmframe<<" Override frame="<<frame<<endl;
                          }
                        start = recordQMToString(startRecord);
                        stringToQuantity(start,velStart);
                        startType="vel";
                      }
                    else if( mtype=="doppler") 
                      {
                        //use veltype in mdoppler
                        //start = MDopToVelString(startRecord);
                        start = recordQMToString(startRecord);
                        stringToQuantity(start,velStart);
                        startRecord.get(String("refer"), mveltype);
                        mveltype.downcase();
                        startType="vel";
                      }
                  }
                else 
                  {
                    start = recordQMToString(startRecord);
                    if ( start.contains("Hz") ) 
                      { 
                         stringToQuantity(start,freqStart);
                         startType="freq";
                      }
                    else if ( start.contains("m/s") ) 
                      { 
                         stringToQuantity(start,velStart);
                         startType="vel";
                      }
                    else { err+= String("Unrecognized Quantity unit for start, must contain m/s or Hz\n"); }
                  }
              }
            else { err += String("start must be an integer, a string, or frequency/velocity in Quantity/Measure\n");}
           }

        //step
        if( inrec.isDefined("width") ) 
          {
            if( inrec.dataType("width") == TpInt )
              {           
	        err += readVal( inrec, String("width"), chanStep);
                step = String::toString(chanStep);
                widthType="chan";
              }
            else if( inrec.dataType("width") == TpString ) 
              {
	        err += readVal( inrec, String("width"), step);
                if( step.contains("Hz") ) 
                  {
                    stringToQuantity(step,freqStep);
                    widthType="freq";
                  }
                else if( step.contains("m/s") )
                  {
                    stringToQuantity(step,velStep); 
                    widthType="vel";
                  } 
              }
            else if ( inrec.dataType("width") == TpRecord ) 
              {
                //record can be freq in Quantity or MFreaquency or vel in Quantity or
                //MRadialVelocity or Doppler (by me.todoppler())
                // ** doppler => converted to radialvel with frame 
                stepRecord = inrec.subRecord("width");
                if(stepRecord.isDefined("m0") )
                  { 
                    //must be a measure
                    String mtype;
                    stepRecord.get("type", mtype);
                    if( mtype=="frequency")
                      { 
                        //mfrequency
                        stepRecord.get(String("refer"), qmframe);
                        if ( frame!="" && frame!=qmframe)
                          {
                            // should emit warning to the logger
                            cerr<<"The frame in step:"<<qmframe<<" Override frame="<<frame<<endl;
                          }
                        step = recordQMToString(stepRecord);
                        stringToQuantity(step, freqStep);
                        widthType="freq";
                      }
                    else if( mtype=="radialvelocity")
                      {
                        //mradialvelocity
                        stepRecord.get(String("refer"), qmframe);
                        if ( frame!="" && frame!=qmframe)
                          {
                            // should emit warning to the logger
                            cerr<<"The frame in step:"<<qmframe<<" Override frame="<<frame<<endl;
                          }
                        step = recordQMToString(stepRecord);
                        stringToQuantity(step,velStep);
                        widthType="vel";
                      }
                    else if( mtype=="doppler") 
                      {
                        //step = MDopToVelString(stepRecord);
                        step = recordQMToString(stepRecord);
                        stringToQuantity(step,velStep);
                        startRecord.get(String("refer"), mveltype);
                        mveltype.downcase();
                        widthType="vel";
                      }
                  }
                else 
                  {
                    step = recordQMToString(stepRecord);
                    if ( step.contains("Hz") ) 
                      { 
                        stringToQuantity(step,freqStep);
                        widthType="freq";
                      }
                    else if ( step.contains("m/s") ) 
                      { 
                        stringToQuantity(step,velStep);
                        widthType="vel";
                      }
                    else { err+= String("Unrecognized Quantity unit for step, must contain m/s or Hz\n"); }
                  }
              }
            else { err += String("step must be an integer, a string, or frequency/velocity in Quantity/Measure\n");}
          }

        //check for start, width unit consistentcy
        if (startType!=widthType && startType!="" &&  widthType!="") 
           err += String("Cannot mix start and width with different unit types (e.g. km/s vs. Hz)\n");
 
        //reffreq (String, Quantity, or Measure)
	if( inrec.isDefined("reffreq") )
          {
            if( inrec.dataType("reffreq")==TpString ) 
              {
	        err += readVal( inrec, String("reffreq"), refFreq); 
              }
            else if( inrec.dataType("reffreq")==TpRecord) 
              {
                String reffreqstr;
                reffreqRecord = inrec.subRecord("reffreq"); 
                if(reffreqRecord.isDefined("m0") )
                  { 
                    String mtype;
                    reffreqRecord.get("type", mtype);
                    if( mtype=="frequency")
                      {
                        reffreqstr = recordQMToString(reffreqRecord);
                        stringToQuantity(reffreqstr,refFreq);
                      }
                    else{ err+= String("Unrecognized Measure for reffreq, must be a frequency measure\n");}
                  }
                else  
                  {
                    reffreqstr = recordQMToString(reffreqRecord);
                    if( reffreqstr.contains("Hz") ) { stringToQuantity(reffreqstr,refFreq);}
                    else { err+= String("Unrecognized Quantity unit for reffreq, must contain Hz\n");}
                  }
              }
            else { err += String("reffreq must be a string, or frequency in Quantity/Measure\n");}
          }
   
	err += readVal( inrec, String("veltype"), veltype); 
        veltype = mveltype!=""? mveltype:veltype;
        // sysvel (String, Quantity)
        if( inrec.isDefined("sysvel") )
          {
            if( inrec.dataType("sysvel")==TpString )
              {
	        err += readVal( inrec, String("sysvel"), sysvel); 
              }
            else if( inrec.dataType("sysvel")==TpRecord )
              {
                sysvelRecord = inrec.subRecord("sysvel"); 
                sysvel = recordQMToString(sysvelRecord);
                if( sysvel=="" || !sysvel.contains("m/s") )
                  { err+= String("Unrecognized Quantity unit for sysvel, must contain m/s\n");}
              }
            else
              { err += String("sysvel must be a string, or velocity in Quantity\n");}
          }
	err += readVal( inrec, String("sysvelframe"), sysvelframe); 

        // rest frequencies (record or vector of Strings)
        if( inrec.isDefined("restfreq") )
          {
	    Vector<String> rfreqs(0);
            Record restfreqSubRecord;
            if( inrec.dataType("restfreq")==TpRecord )
              {
                restfreqRecord = inrec.subRecord("restfreq");
                // assume multiple restfreqs are index as '0','1'..
                if( restfreqRecord.isDefined("0") )
                  {
                    rfreqs.resize( restfreqRecord.nfields() );
                    for( uInt fr=0; fr<restfreqRecord.nfields(); fr++)
                      {
                        restfreqSubRecord = restfreqRecord.subRecord(String::toString(fr));
                        rfreqs[fr] = recordQMToString(restfreqSubRecord);
                      }
                  }
              }
            else if( inrec.dataType("restfreq")==TpArrayString ) 
              {
	        //Vector<String> rfreqs(0);
	        err += readVal( inrec, String("restfreq"), rfreqs );
                // case no restfreq is given: set to
              }
	    else if( inrec.dataType("restfreq")==TpString ) 
              {
	        rfreqs.resize(1);
	        err += readVal( inrec, String("restfreq"), rfreqs[0] );
                // case no restfreq is given: set to
              }
	    restFreq.resize( rfreqs.nelements() );
	    for( uInt fr=0; fr<rfreqs.nelements(); fr++)
	      {
		err += stringToQuantity( rfreqs[fr], restFreq[fr] );
	      }
	  } // if def restfreq

        // optional - coordsys, imshape
        // if exist use them. May need a consistency check with the rest of impars?
        if( inrec.isDefined("csys") )
          { 
	    //            cout<<"HAS CSYS KEY - got from input record"<<endl;
            if( inrec.dataType("csys")==TpRecord )
              {
                //csysRecord = inrec.subRecord("csys");
                csysRecord.defineRecord("coordsys",inrec.subRecord("csys"));
              }
            if( inrec.isDefined("imshape") ) 
              {
                if ( inrec.dataType("imshape") == TpArrayInt )
                  {
                    err += readVal( inrec, String("imshape"), imshape ); 
                  }
              }
           }
                
	//String freqframestr( MFrequency::showType(freqFrame) );
	//err += readVal( inrec, String("outframe"), freqframestr);
	//if( ! MFrequency::getType(freqFrame, freqframestr) )
	//  { err += "Invalid Frequency Frame " + freqframestr ; }

        String freqframestr = (frame!="" && qmframe!="")? qmframe:frame;
	if( frame!="" && ! MFrequency::getType(freqFrame, freqframestr) )
	  { err += "Invalid Frequency Frame " + freqframestr ; }
	err += readVal( inrec, String("restart"), overwrite );

	err += readVal(inrec, String("freqframevalid"), freqFrameValid);
	// startmodel parsing copied in SynthesisParamDeconv. Clean this up !!! 
        if( inrec.isDefined("startmodel") ) 
          {
            if( inrec.dataType("startmodel")==TpString )
	      {
		String onemodel;
		err += readVal( inrec, String("startmodel"), onemodel );
		if( onemodel.length()>0 )
		  {
		    startModel.resize(1);
		    startModel[0] = onemodel;
		  }
		else {startModel.resize();}
	      }
	    else if( inrec.dataType("startmodel")==TpArrayString || 
		     inrec.dataType("startmodel")==TpArrayBool)
	      {
		err += readVal( inrec, String("startmodel"), startModel );
	      }
	    else {
	      err += String("startmodel must be either a string(singleterm) or a list of strings(multiterm)\n");
	      }
	  }

	err += readVal( inrec, String("nterms"), nTaylorTerms );
	err += readVal( inrec, String("deconvolver"), deconvolver );

	// Force nchan=1 for anything other than cube modes...
	if(mode=="mfs") nchan=1;
        //read obslocation
        if(inrec.isDefined("obslocation_rec")){
          String errorobs;
          const Record obsrec=inrec.asRecord("obslocation_rec");
          MeasureHolder mh;
          if(!mh.fromRecord(errorobs, obsrec)){
            err+=errorobs;
          }
          obslocation=mh.asMPosition();

        }
       
        

	err += verify();
	
      }
    catch(AipsError &x)
      {
	err = err + x.getMesg() + "\n";
      }
      
      if( err.length()>0 ) throw(AipsError("Invalid Image Parameter set : " + err));
     
  }

  String SynthesisParamsImage::MDopToVelString(Record &rec)
  {
    if( rec.isDefined("type") ) 
      {
        String measType;
        String unit;
        Double val = 0;
        rec.get("type", measType);
        if(measType=="doppler")
          {
            rec.get(String("refer"), mveltype);
            Record dopRecord = rec.subRecord("m0");
            String dopstr = recordQMToString(dopRecord);
            //cerr<<"dopstr="<<dopstr<<endl;
            MRadialVelocity::Types mvType;
            //use input frame
            qmframe = frame!=""? frame: "LSRK";
            MRadialVelocity::getType(mvType, qmframe);
            MDoppler::Types mdType;
            MDoppler::getType(mdType, mveltype);
            MDoppler dop(Quantity(val,unit), mdType);
            MRadialVelocity mRadVel(MRadialVelocity::fromDoppler(dop, mvType));
            Double velval = mRadVel.get("m/s").getValue();
            return start = String::toString(velval) + String("m/s");
          }
        else
          { return String("");}
      }
      else { return String("");}
  }

  String SynthesisParamsImage::verify() const
  {
    String err;

    if( imageName=="" ) {err += "Please supply an image name\n";}

    if( imsize.nelements() != 2 ){ err += "imsize must be a vector of 2 Ints\n"; }
    if( cellsize.nelements() != 2 ) { err += "cellsize must be a vector of 2 Quantities\n"; }

    //// default is nt=2 but deconvolver != mtmfs by default.
    //    if( nchan>1 and nTaylorTerms>1 )
    //  {err += "Cannot have more than one channel with ntaylorterms>1\n";}

    if( (mode=="mfs") && nchan>1 )
      { err += "specmode=mfs cannot have nchan="+String::toString(nchan)+" (must be 1)\n";}

    if( ! stokes.matches("I") && ! stokes.matches("Q") && 
	! stokes.matches("U") && ! stokes.matches("V") && 
	! stokes.matches("RR") && ! stokes.matches("LL") && 
	! stokes.matches("XX") && ! stokes.matches("YY") && 
	! stokes.matches("IV") && ! stokes.matches("IQ") && 
	! stokes.matches("RRLL") && ! stokes.matches("XXYY") &&
	! stokes.matches("QU") && ! stokes.matches("UV") && 
	! stokes.matches("IQU") && ! stokes.matches("IUV") && 
	! stokes.matches("IQUV") ) 
      { err += "Stokes " + stokes + " is an unsupported option \n";}

    ///    err += verifySpectralSetup();  

    // Allow only one starting model. No additions to be done.
    if( startModel.nelements()>0 )
      {
	if( deconvolver!="mtmfs" ) {

	  if( startModel.nelements()!=1 ){err += String("Only one startmodel image is allowed.\n");}
	  else
	    {
	      File fp( imageName+String(".model") );
	      if( fp.exists() ) err += "Model " + imageName+".model exists, but a starting model of " + startModel[0] + " is also being requested. Please either reset startmodel='' to use what already exists, or delete " + imageName + ".model so that it uses the new model specified in startmodel.";
	    }
	  }
	else {// mtmfs
	  File fp( imageName+String(".model.tt0") ); 
	  if( fp.exists() ) 
	    {err += "Model " + imageName+".model.tt* exists, but a starting model of ";
	      for (uInt i=0;i<startModel.nelements();i++){ err += startModel[i] + ","; }
	      err +=" is also being requested. Please either reset startmodel='' to use what already exists, or delete " + imageName + ".model.tt* so that it uses the new model specified in startmodel";
	    }
	}

	// Check that startmodel exists on disk !
	for(uInt ss=0;ss<startModel.nelements();ss++)
	  {
	    File fp( startModel[ss] );
	    if( ! fp.exists() ) {err += "Startmodel " + startModel[ss] + " cannot be found on disk.";}
	  }

      }

    
    /// Check imsize for efficiency.
    Int imxnew = SynthesisUtilMethods::getOptimumSize( imsize[0] );
    Int imynew = SynthesisUtilMethods::getOptimumSize( imsize[1] );

    if( imxnew != imsize[0]  || imynew != imsize[1] )
      {
	LogIO os( LogOrigin("SynthesisParamsImage","buildCoordinateSystem",WHERE) );
	if( imxnew != imsize[0] ) {os << LogIO::WARN << "imsize with "+String::toString(imsize[0])+" pixels is not an efficient imagesize. Try "+String::toString(imxnew)+" instead." << LogIO::POST;}
	if( imsize[0] != imsize[1] && imynew != imsize[1] ) {os << LogIO::WARN << "imsize with "+String::toString(imsize[1])+" pixels is not an efficient imagesize. Try "+String::toString(imynew)+" instead." << LogIO::POST;}
	//err += "blah";
      }
    
	return err;
  }// verify()

  /*  
  // Convert all user options to  LSRK freqStart, freqStep, 
  // Could have (optional) log messages coming out of this function, to tell the user what the
  // final frequency setup is ?

  String SynthesisParamsImage::verifySpectralSetup()
  {
  }
  */

  void SynthesisParamsImage::setDefaults()
  {
    // Image definition parameters
    imageName = String("");
    imsize.resize(2); imsize.set(100);
    cellsize.resize(2); cellsize.set( Quantity(1.0,"arcsec") );
    stokes="I";
    phaseCenter=MDirection();
    phaseCenterFieldId=-1;
    projection=Projection::SIN;
    useNCP=false;
    startModel=Vector<String>(0);
    freqFrameValid=True;
    overwrite=false;
    // PseudoI
    pseudoi=false;

    // Spectral coordinates
    nchan=1;
    mode="mfs";
    start="";
    step="";
    chanStart=0;
    chanStep=1;
    //freqStart=Quantity(0,"Hz");
    //freqStep=Quantity(0,"Hz");
    //velStart=Quantity(0,"m/s");
    //velStep=Quantity(0,"m/s");
    freqStart=Quantity(0,"");
    freqStep=Quantity(0,"");
    velStart=Quantity(0,"");
    velStep=Quantity(0,"");
    veltype=String("radio");
    restFreq.resize(0);
    refFreq = Quantity(0,"Hz");
    frame = "";
    freqFrame=MFrequency::LSRK;
    sysvel="";
    sysvelframe="";
    sysvelvalue=Quantity(0.0,"m/s");
    nTaylorTerms=1;
    deconvolver="hogbom";
    ///csysRecord=Record();
    //

    
  }

  Record SynthesisParamsImage::toRecord() const
  {
    Record impar;
    impar.define("imagename", imageName);
    impar.define("imsize", imsize);
    Vector<String> cells(2);
    cells[0] = QuantityToString( cellsize[0] );
    cells[1] = QuantityToString( cellsize[1] );
    impar.define("cell", cells );
    if(pseudoi==true){impar.define("stokes","pseudoI");}
    else{impar.define("stokes", stokes);}
    impar.define("nchan", nchan);
    impar.define("nterms", nTaylorTerms);
    impar.define("deconvolver",deconvolver);
    impar.define("phasecenter", MDirectionToString( phaseCenter ) );
    impar.define("phasecenterfieldid",phaseCenterFieldId);
    impar.define("projection", (useNCP? "NCP" : projection.name()) );

    impar.define("specmode", mode );
    // start and step can be one of these types
    if( start!="" )
      { 
        if( !start.contains("Hz") && !start.contains("m/s") && 
           String::toInt(start) == chanStart )
          {
            impar.define("start",chanStart); 
          }
        else if( startRecord.nfields() > 0 )
          {
            impar.defineRecord("start", startRecord ); 
          }
        else 
          {
            impar.define("start",start);
        }
      }
    else { 
        impar.define("start", start ); 
      }
    if( step!="" )
      {
        if( !step.contains("Hz") && !step.contains("m/s") && 
           String::toInt(step) == chanStep )
          {
            impar.define("width", chanStep);
          }
        else if( stepRecord.nfields() > 0 )
          { 
            impar.defineRecord("width",stepRecord);
          }
        else
          {
            impar.define("width",step);
          }
      }
    else 
      { 
        impar.define("width", step);
      }
    //impar.define("chanstart", chanStart );
    //impar.define("chanstep", chanStep );
    //impar.define("freqstart", QuantityToString( freqStart ));
    //impar.define("freqstep", QuantityToString( freqStep ) );
    //impar.define("velstart", QuantityToString( velStart ));
    //impar.define("velstep", QuantityToString( velStep ) );
    impar.define("veltype", veltype);
    if (restfreqRecord.nfields() != 0 ) 
      {
        impar.defineRecord("restfreq", restfreqRecord);
      }
    else
      {
        Vector<String> rfs( restFreq.nelements() );
        for(uInt rf=0; rf<restFreq.nelements(); rf++){rfs[rf] = QuantityToString(restFreq[rf]);}
        impar.define("restfreq", rfs);
      }
    //impar.define("reffreq", QuantityToString(refFreq));
    //reffreq
    if( reffreqRecord.nfields() != 0 )  
      { impar.defineRecord("reffreq",reffreqRecord); }
    else
      { impar.define("reffreq",reffreq); }
    //impar.define("reffreq", reffreq );
    //impar.define("outframe", MFrequency::showType(freqFrame) );
    impar.define("outframe", frame );
    //sysvel
    if( sysvelRecord.nfields() != 0 )
      { impar.defineRecord("sysvel",sysvelRecord); } 
    else
      { impar.define("sysvel", sysvel );}
    impar.define("sysvelframe", sysvelframe );

    impar.define("restart",overwrite );
    impar.define("freqframevalid", freqFrameValid);
    impar.define("startmodel", startModel );

    if( csysRecord.isDefined("coordsys") )
      {
	//        cout <<" HAS CSYS INFO.... writing to output record"<<endl;
        impar.defineRecord("csys", csysRecord.subRecord("coordsys"));
        impar.define("imshape", imshape);
      } 
    //    else cout << " NO CSYS INFO to write to output record " << endl;
    ///Now save obslocation
    Record tmprec;
    String err;
    MeasureHolder mh(obslocation);
    if(mh.toRecord(err, tmprec)){
      impar.defineRecord("obslocation_rec", tmprec);
    }
    else{
      throw(AipsError("failed to save obslocation to record"));
    }
    return impar;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////     Build a coordinate system.  ////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////   To be used from SynthesisImager, to construct the images it needs
  ////   To also be connected to a 'makeimage' method of the synthesisimager tool.
  ////       ( need to supply MS only to add  'ObsInfo' to the csys )
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  

  CoordinateSystem SynthesisParamsImage::buildCoordinateSystem(vi::VisibilityIterator2& vi2, const std::map<Int, std::map<Int, Vector<Int> > >& chansel, Block<const MeasurementSet *> mss) 

  {
    //vi2.getImpl()->spectralWindows( spwids );
    //The above is not right
    //////////// ///Kludge to find all spw selected
    //std::vector<Int> pushspw;
    vi::VisBuffer2* vb=vi2.getVisBuffer();
    vi2.originChunks();
    vi2.origin();
    /// This version uses the new vi2/vb2
    // get the first ms for multiple MSes
    //MeasurementSet msobj=vi2.ms();
    Int fld=vb->fieldId()(0);

	//handling first ms only
        Double gfreqmax=-1.0;
	Double gdatafend=-1.0;
	Double gdatafstart=1e14;
	Double gfreqmin=1e14;
	Vector<Int> spwids0;
	Int j=0;
        Int minfmsid=0;
	//for cube mode ,for a list of MSs, check ms to send to buildCoordSysCore contains start freq/vel
	Double imStartFreq=getCubeImageStartFreq();
        std::vector<Int> sourceMsWithStartFreq;

	
	for (auto forMS0=chansel.begin(); forMS0 !=chansel.end(); ++forMS0, ++j){
    //auto forMS0=chansel.find(0);
	  map<Int, Vector<Int> > spwsels=forMS0->second;
	  Int nspws=spwsels.size();
	  Vector<Int> spwids(nspws);
	  Vector<Int> nChannels(nspws);
	  Vector<Int> firstChannels(nspws);
	  //Vector<Int> channelIncrement(nspws);
	  
	  Int k=0;
	  for (auto it=spwsels.begin(); it != spwsels.end(); ++it, ++k){
	    spwids[k]=it->first;
	    nChannels[k]=(it->second)[0];
	    firstChannels[k]=(it->second)[1];
	  }
	  if(j==0)
	    spwids0=spwids;
	  // std::tie (spwids, nChannels, firstChannels, channelIncrement)=(static_cast<vi::VisibilityIteratorImpl2 * >(vi2.getImpl()))->getChannelInformation(false);
	  
	  //cerr << "SPWIDS "<< spwids <<  "  nchan " << nChannels << " firstchan " << firstChannels << endl;
	  
	  //////////////////This returns junk for multiple ms CAS-9994..so kludged up along with spw kludge
	  //Vector<Int> flds;
	  //vi2.getImpl()->fieldIds( flds );
	  //AlwaysAssert( flds.nelements()>0 , AipsError );
	  //fld = flds[0];
	  Double freqmin=0, freqmax=0;
	  freqFrameValid=(freqFrame != MFrequency::REST || mode=="cubesource");
	  
	  //MFrequency::Types dataFrame=(MFrequency::Types)vi2.subtableColumns().spectralWindow().measFreqRef()(spwids[0]);
	  MFrequency::Types dataFrame=(MFrequency::Types)MSColumns(*mss[j]).spectralWindow().measFreqRef()(spwids[0]);
	  
	  Double datafstart, datafend;
	  //VisBufferUtil::getFreqRange(datafstart, datafend, vi2, dataFrame );
	  //cerr << std::setprecision(12) << "before " << datafstart << "   " << datafend << endl;
	  Bool status=MSUtil::getFreqRangeInSpw( datafstart, datafend, spwids, firstChannels, nChannels,*mss[j], dataFrame,  True);
	  //cerr << "after " << datafstart << "   " << datafend << endl;
	  if((datafstart > datafend) || !status)
	    throw(AipsError("spw selection failed")); 
	  //cerr << "datafstart " << datafstart << " end " << datafend << endl;
	  
	  if (mode=="cubedata") {
	    freqmin = datafstart;
	    freqmax = datafend;
	  }
	  else if(mode == "cubesource"){
	    if(!trackSource){
	      throw(AipsError("Cannot be in cubesource without tracking a moving source"));
	    }
	    String ephemtab(movingSource);
	    if(movingSource=="TRACKFIELD"){
	      Int fieldID=MSColumns(*mss[j]).fieldId()(0);
	      ephemtab=Path(MSColumns(*mss[j]).field().ephemPath(fieldID)).absoluteName();
	    }
	    MEpoch refep=MSColumns(*mss[j]).timeMeas()(0);
	    Quantity refsysvel;
	    MSUtil::getFreqRangeAndRefFreqShift(freqmin,freqmax,refsysvel, refep, spwids,firstChannels, nChannels, *mss[j], ephemtab, trackDir, true);
	    if(j==0)
	      sysvelvalue=refsysvel;
	    /*Double freqMinTopo, freqMaxTopo;
	    MSUtil::getFreqRangeInSpw( freqMinTopo, freqMaxTopo, spwids, firstChannels,
				       nChannels,*mss[j], freqFrameValid? MFrequency::TOPO:MFrequency::REST , True);
	    cerr << std::setprecision(10) << (freqmin-freqMinTopo) << "       "  << (freqmax-freqMaxTopo) << endl;
	    sysfreqshift=((freqmin-freqMinTopo)+(freqmax-freqMaxTopo))/2.0;
	    */
	  }
	  else {
	    //VisBufferUtil::getFreqRange(freqmin,freqmax, vi2, freqFrameValid? freqFrame:MFrequency::REST );
	    //cerr << "before " << freqmin << "   " << freqmax << endl;
	    MSUtil::getFreqRangeInSpw( freqmin, freqmax, spwids, firstChannels,
				       nChannels,*mss[j], freqFrameValid? freqFrame:MFrequency::REST , True);
	    //cerr << "after " << freqmin << "   " << freqmax << endl;
	  }

	  
          
          
	  if(freqmin < gfreqmin) gfreqmin=freqmin;
	  if(freqmax > gfreqmax) gfreqmax=freqmax;
	  if(datafstart < gdatafstart) gdatafstart=datafstart;
	  if(datafend > gdatafend) gdatafend=datafend;
          // pick ms to use for setting spectral coord for output images 
          // when startfreq is specified find first ms that it fall within the freq range
          // of the ms (with channel selection applied).
          // startfreq is converted to the data frame freq based on Measure ref (for the direction, epech, location)
          // of that ms.
	  if(imStartFreq > 0.0 && imStartFreq >= freqmin && imStartFreq <= freqmax){
            if(mode != "cubesource"){
              minfmsid=j;
              spwids0=spwids;
              vi2.originChunks();
              vi2.origin();
              while(vb->msId() != j && vi2.moreChunks() ){
                vi2.nextChunk();
                vi2.origin();
              }
              fld=vb->fieldId()(0);
             
            }
            else{
              sourceMsWithStartFreq.push_back(j);
            }
	  }
           
        }
        if(sourceMsWithStartFreq.size() > 1){
          auto result = std::find(std::begin(sourceMsWithStartFreq), std::end(sourceMsWithStartFreq), 0);
          if(result == std::end(sourceMsWithStartFreq)){
            throw(AipsError("Reorder the input list of MSs so that MS "+String::toString( sourceMsWithStartFreq[0])+ "is first to match startfreq you provided"));
          }
        }
    MeasurementSet msobj = *mss[minfmsid];
   // return buildCoordinateSystemCore( msobj, spwids0, fld, gfreqmin, gfreqmax, gdatafstart, gdatafend );
    return buildCoordinateSystemCore( msobj, spwids0, fld, gfreqmin, gfreqmax, gdatafstart, gdatafend );
  }
  

  CoordinateSystem SynthesisParamsImage::buildCoordinateSystem(ROVisibilityIterator* rvi ) 
  {
    /// This version uses the old vi/vb
    // get the first ms for multiple MSes
    MeasurementSet msobj=rvi->getMeasurementSet();
    Vector<Int> spwids;
    Vector<Int> nvischan;
    rvi->allSelectedSpectralWindows(spwids,nvischan);
    Int fld = rvi->fieldId();
    Double freqmin=0, freqmax=0;
    Double datafstart, datafend;
    //freqFrameValid=(freqFrame != MFrequency::REST || mode != "cubedata" );
    freqFrameValid=(freqFrame != MFrequency::REST );
    MSColumns msc(msobj);
    MFrequency::Types dataFrame=(MFrequency::Types)msc.spectralWindow().measFreqRef()(spwids[0]);
    rvi->getFreqInSpwRange(datafstart, datafend, dataFrame );
    if (mode=="cubedata") {
       freqmin = datafstart;
       freqmax = datafend;
    }
    else { 
       rvi->getFreqInSpwRange(freqmin,freqmax,freqFrameValid? freqFrame:MFrequency::REST );
    }
    // Following three lines are  kind of redundant but need to get freq range in the data frame to be used
    // to select channel range for default start 
    //cerr<<"freqmin="<<freqmin<<" datafstart="<<datafstart<<" freqmax="<<freqmax<<" datafend="<<datafend<<endl;
    return buildCoordinateSystemCore( msobj, spwids, fld, freqmin, freqmax, datafstart, datafend );
  }

  CoordinateSystem SynthesisParamsImage::buildCoordinateSystemCore(
								   MeasurementSet& msobj, 
								   Vector<Int> spwids, Int fld, 
								   Double freqmin, Double freqmax,
                                                                   Double datafstart, Double datafend )
  {
    LogIO os( LogOrigin("SynthesisParamsImage","buildCoordinateSystem",WHERE) );
  
    CoordinateSystem csys;
    if( csysRecord.nfields()!=0 ) 
      {
        //use cysRecord
        Record subRec1;
	//        cout<<"USE THE EXISTING CSYS +++++++++++++++++"<<endl;
        CoordinateSystem *csysptr = CoordinateSystem::restore(csysRecord,"coordsys");
        //csys = *csysptr; 
        //CoordinateSystem csys(*csysptr); 
        csys = *csysptr;

      }
    else {
      MSColumns msc(msobj);
      String telescop = msc.observation().telescopeName()(0);
      MEpoch obsEpoch = msc.timeMeas()(0);
      MPosition obsPosition;
    if(!(MeasTable::Observatory(obsPosition, telescop)))
      {
        os << LogIO::WARN << "Did not get the position of " << telescop
           << " from data repository" << LogIO::POST;
        os << LogIO::WARN
           << "Please contact CASA to add it to the repository."
           << LogIO::POST;
        os << LogIO::WARN << "Using first antenna position as refence " << LogIO::POST;
	 // unknown observatory, use first antenna
      obsPosition=msc.antenna().positionMeas()(0);
      }
      MDirection phaseCenterToUse = phaseCenter;
      
    if( phaseCenterFieldId != -1 )
      {
	MSFieldColumns msfield(msobj.field());
        if(phaseCenterFieldId == -2) // the case for  phasecenter=''
          {
	    if(trackSource){
	      phaseCenterToUse=getMovingSourceDir(msobj, obsEpoch, obsPosition, MDirection::ICRS);
	    }
	    else{
	      phaseCenterToUse=msfield.phaseDirMeas( fld );
	    }
          }
        else 
          {
	    phaseCenterToUse=msfield.phaseDirMeas( phaseCenterFieldId ); 
          }
      }
    // Setup Phase center if it is specified only by field id.

    /////////////////// Direction Coordinates
    MVDirection mvPhaseCenter(phaseCenterToUse.getAngle());
    // Normalize correctly
    MVAngle ra=mvPhaseCenter.get()(0);
    ra(0.0);

    MVAngle dec=mvPhaseCenter.get()(1);
    Vector<Double> refCoord(2);
    refCoord(0)=ra.get().getValue();    
    refCoord(1)=dec;
    Vector<Double> refPixel(2); 
    refPixel(0) = Double(imsize[0]/2);
    refPixel(1) = Double(imsize[1]/2);

    Vector<Double> deltas(2);
    deltas(0)=-1* cellsize[0].get("rad").getValue();
    deltas(1)=cellsize[1].get("rad").getValue();
    Matrix<Double> xform(2,2);
    xform=0.0;xform.diagonal()=1.0;

    Vector<Double> projparams(2); 
    projparams = 0.0;
    if( useNCP==true ) { projparams[0]=0.0, projparams[1]=1/tan(refCoord(1));   }
    Projection projTo( projection.type(), projparams );

    DirectionCoordinate
      myRaDec(MDirection::Types(phaseCenterToUse.getRefPtr()->getType()),
	      //	      projection,
	      projTo,
	      refCoord(0), refCoord(1),
	      deltas(0), deltas(1),
	      xform,
	      refPixel(0), refPixel(1));


    //defining observatory...needed for position on earth
    // get the first ms for multiple MSes
    
    
    obslocation=obsPosition;
    ObsInfo myobsinfo;
    myobsinfo.setTelescope(telescop);
    myobsinfo.setPointingCenter(mvPhaseCenter);
    myobsinfo.setObsDate(obsEpoch);
    myobsinfo.setObserver(msc.observation().observer()(0));

    /// Attach obsInfo to the CoordinateSystem
    ///csys.setObsInfo(myobsinfo);


    /////////////////// Spectral Coordinate

    //Make sure frame conversion is switched off for REST frame data.
    //Bool freqFrameValid=(freqFrame != MFrequency::REST);

    //freqFrameValid=(freqFrame != MFrequency::REST );
    //UR//freqFrameValid=(freqFrame != MFrequency::REST || mode != "cubedata" );
    //UR - moved freqFrameValid calc to vi/vi2 dependent wrappers.

    if(spwids.nelements()==0)
      {
        Int nspw=msc.spectralWindow().nrow();
        spwids.resize(nspw);
        indgen(spwids); 
      }
    MFrequency::Types dataFrame=(MFrequency::Types)msc.spectralWindow().measFreqRef()(spwids[0]);
    Vector<Double> dataChanFreq, dataChanWidth;
    std::vector<std::vector<Int> > averageWhichChan;
    std::vector<std::vector<Int> > averageWhichSPW;
    std::vector<std::vector<Double> > averageChanFrac;
    
    if(spwids.nelements()==1)
      {
        dataChanFreq=msc.spectralWindow().chanFreq()(spwids[0]);
        dataChanWidth=msc.spectralWindow().chanWidth()(spwids[0]);
      }
    else 
      {
	if(!MSTransformRegridder::combineSpwsCore(os,msobj, spwids,dataChanFreq,dataChanWidth,
											  averageWhichChan,averageWhichSPW,averageChanFrac))
          {
            os << LogIO::SEVERE << "Error combining SpWs" << LogIO::POST;
          }
      }
    Double minDataFreq = min(dataChanFreq);
    if(start=="" && minDataFreq < datafstart  ) {
        // limit data chan freq vector for default start case with channel selection
        Int chanStart, chanEnd;
        Int lochan = 0;
        Int nDataChan = dataChanFreq.nelements();
        Int hichan = nDataChan-1;
        Double diff_fmin, diff_fmax;
        Bool ascending = dataChanFreq[nDataChan-1] - dataChanFreq[0] > 0;
        for(Int ichan = 0; ichan < nDataChan; ichan++) 
          {
            diff_fmin = dataChanFreq[ichan] - datafstart;  
            diff_fmax = datafend - dataChanFreq[ichan];  
            // freqmin and freqmax should corresponds to the channel edges
            if(ascending) 
              {
                
                if( diff_fmin > 0 &&  diff_fmin <= dataChanWidth[ichan]/2. )
                  {
                    lochan = ichan;
                  }
                else if(diff_fmax > 0 && diff_fmax <= dataChanWidth[ichan]/2. )
                  {
                    hichan = ichan;
                  }
              }
            else
              {
                if( diff_fmax > 0 && diff_fmax <= dataChanWidth[ichan]/2. )
                  {
                    hichan = ichan;
                  }
                else if( diff_fmin > 0 && diff_fmin <= dataChanWidth[ichan]/2. )
                  {
                    lochan = ichan;
                  }   
              }
           }
        chanStart = lochan;
        chanEnd = hichan;
        if (lochan > hichan) 
          {
            chanStart=hichan;
            chanEnd=lochan; 
          }
        Vector<Double> tempChanFreq = dataChanFreq(Slice(chanStart,chanEnd-chanStart+1,1)); 
        Vector<Double> tempChanWidth = dataChanWidth(Slice(chanStart,chanEnd-chanStart+1,1)); 
        dataChanFreq.resize(tempChanFreq.nelements());
        dataChanWidth.resize(tempChanWidth.nelements());
        dataChanFreq = tempChanFreq;
        dataChanWidth = tempChanWidth;
      }
    Quantity qrestfreq = restFreq.nelements() >0 ? restFreq[0]: Quantity(0.0, "Hz");
    String cubemode;
    if ( qrestfreq.getValue("Hz")==0 ) 
      {
        MSDopplerUtil msdoppler(msobj);
        Vector<Double> restfreqvec;
        msdoppler.dopplerInfo(restfreqvec, spwids(0), fld);
        qrestfreq = restfreqvec.nelements() >0 ? Quantity(restfreqvec(0),"Hz"): Quantity(0.0, "Hz");
        if ( qrestfreq.getValue("Hz")==0 and mode!="mfs" )
          {
          cubemode = findSpecMode(mode);
          if ( cubemode=="channel" || cubemode=="frequency" )
            {
              //Double provisional_restfreq = msc.spectralWindow().refFrequency()(spwids(0));
              //By PLWG request, changed to center (mean) frequency of the selected spws -2015-06-22(TT) 
              Double provisional_restfreq = (datafend+datafstart)/2.0;
              qrestfreq = Quantity(provisional_restfreq, "Hz");
              os << LogIO::WARN << "No rest frequency info, using the center of the selected spw(s):"
                 << provisional_restfreq <<" Hz. Velocity labelling may not be correct." 
                 << LogIO::POST;
            } 
          else { // must be vel mode
            throw(AipsError("No valid rest frequency is defined in the data, please specify the restfreq parameter") );
            } 
        }
      }
    Double refPix;
    Vector<Double> chanFreq;
    Vector<Double> chanFreqStep;
    String specmode;

    if(mode=="cubesource"){
      MDoppler mdop(sysvelvalue, MDoppler::RELATIVISTIC);
      dataChanFreq=mdop.shiftFrequency(dataChanFreq);
      dataChanWidth=mdop.shiftFrequency(dataChanWidth);
      if (std::isnan(dataChanFreq[0]) || std::isnan(dataChanFreq[dataChanFreq.nelements()-1])) {
	throw(AipsError("The Doppler shift correction of the data channel frequencies resulted in 'NaN' using the radial velocity = "+
              String::toString(sysvelvalue)+". Typically this indicates a problem in the ephemeris data being used.")); 
      }
    }
    
    if (!getImFreq(chanFreq, chanFreqStep, refPix, specmode, obsEpoch, 
		   obsPosition, dataChanFreq, dataChanWidth, dataFrame, qrestfreq, freqmin, freqmax,
		   phaseCenterToUse))
      throw(AipsError("Failed to determine channelization parameters"));

    Bool nonLinearFreq(false);
    String veltype_p=veltype;
    veltype_p.upcase(); 
    if(veltype_p.contains("OPTICAL") || veltype_p.matches("Z") || veltype_p.contains("BETA") ||
         veltype_p.contains("RELATI") || veltype_p.contains("GAMMA")) 
      {
        nonLinearFreq= true;
      }

    SpectralCoordinate mySpectral;
    Double stepf;
    if(!nonLinearFreq) 
    //TODO: velocity mode default start case (use last channels?)
      {
        Double startf=chanFreq[0];
        //Double stepf=chanFreqStep[0];
        if(chanFreq.nelements()==1) 
          {
            stepf=chanFreqStep[0];
          }
        else 
          { 
            stepf=chanFreq[1]-chanFreq[0];
          }
        Double restf=qrestfreq.getValue("Hz");
	//stepf=9e8;
        if ( mode=="mfs" and restf == 0.0 ) restf = restFreq[0].getValue("Hz");
        //cerr<<" startf="<<startf<<" stepf="<<stepf<<" refPix="<<refPix<<" restF="<<restf<<endl;
        // once NOFRAME is implemented do this 
        if(mode=="cubedata") 
          {
      //       mySpectral = SpectralCoordinate(freqFrameValid ? MFrequency::Undefined : MFrequency::REST, 
             mySpectral = SpectralCoordinate(freqFrame == MFrequency::REST? 
                                             MFrequency::REST : MFrequency::Undefined, 
        	                             startf, stepf, refPix, restf);
          }
	else if(mode=="cubesource") 
          {
	    /*stepf=chanFreq.nelements() > 1 ?(freqmax-freqmin)/Double(chanFreq.nelements()-1) : freqmax-freqmin;
	    startf=freqmin+stepf/2.0;
	    */
             mySpectral = SpectralCoordinate(MFrequency::REST, 
        	                             startf, stepf, refPix, restf);
          }
        else 
          {
             mySpectral = SpectralCoordinate(freqFrameValid ? freqFrame : MFrequency::REST, 
		startf, stepf, refPix, restf);
          }
      }
    else 
      { // nonlinear freq coords - use tabular setting
        // once NOFRAME is implemented do this 
        if(mode=="cubedata") 
          {
            //mySpectral = SpectralCoordinate(freqFrameValid ? MFrequency::Undefined : MFrequency::REST,
            mySpectral = SpectralCoordinate(freqFrame == MFrequency::REST ? 
                                            MFrequency::REST : MFrequency::Undefined,
                                            chanFreq, (Double)qrestfreq.getValue("Hz"));
          }
	else if (mode=="cubesource") 
          {
            mySpectral = SpectralCoordinate(MFrequency::REST,
                                            chanFreq, (Double)qrestfreq.getValue("Hz"));
          }
        else 
          {
            mySpectral = SpectralCoordinate(freqFrameValid ? freqFrame : MFrequency::REST,
                 chanFreq, (Double)qrestfreq.getValue("Hz"));
          }
      }
    //cout << "Rest Freq : " << restFreq << endl;

    //for(uInt k=1 ; k < restFreq.nelements(); ++k)
      //mySpectral.setRestFrequency(restFreq[k].getValue("Hz"));
     
    uInt nrestfreq = restFreq.nelements();
    if ( nrestfreq > 1 ) {
      Vector<Double> restfreqval( nrestfreq - 1 );
      for ( uInt k=1 ; k < nrestfreq; ++k ) {
        restfreqval[k-1] = restFreq[k].getValue("Hz");
      }    
      mySpectral.setRestFrequencies(restfreqval, 0, true);
    }

    // no longer needed, done inside SIImageStore
    //if ( freqFrameValid ) {
    // mySpectral.setReferenceConversion(MFrequency::LSRK,obsEpoch,obsPosition,phaseCenterToUse);   
    //}

    //    cout << "RF from coordinate : " << mySpectral.restFrequency() << endl;

    ////////////////// Stokes Coordinate
   
    Vector<Int> whichStokes = decideNPolPlanes(stokes);
    if(whichStokes.nelements()==0)
      throw(AipsError("Stokes selection of " + stokes + " is invalid"));
    StokesCoordinate myStokes(whichStokes);

    //////////////////  Build Full coordinate system. 

    //CoordinateSystem csys;
    csys.addCoordinate(myRaDec);
    csys.addCoordinate(myStokes);
    csys.addCoordinate(mySpectral);
    csys.setObsInfo(myobsinfo);

    //store back csys to impars record
    //cerr<<"save csys to csysRecord..."<<endl;
    if(csysRecord.isDefined("coordsys"))
      csysRecord.removeField("coordsys");
    csys.save(csysRecord,"coordsys");
    //cerr<<"BUILDCOORDSYS:: new csysRecord ="<<csysRecord<<endl;
    // imshape
    imshape.resize(4);
    imshape[0] = imsize[0];
    imshape[1] = imsize[0];
    imshape[2] = whichStokes.nelements();
    imshape[3] = chanFreq.nelements(); 
    //toRecord();
    //////////////// Set Observatory info, if MS is provided.
    // (remove this section after verified...)
    /***
    if( ! msobj.isNull() )
      {
	//defining observatory...needed for position on earth
	MSColumns msc(msobj);
	String telescop = msc.observation().telescopeName()(0);
	MEpoch obsEpoch = msc.timeMeas()(0);
	MPosition obsPosition;
	if(!(MeasTable::Observatory(obsPosition, telescop)))
	  {
	    os << LogIO::WARN << "Did not get the position of " << telescop 
	       << " from data repository" << LogIO::POST;
	    os << LogIO::WARN 
	       << "Please contact CASA to add it to the repository."
	       << LogIO::POST;
	    os << LogIO::WARN << "Frequency conversion will not work " << LogIO::POST;
	  }
	
	ObsInfo myobsinfo;
	myobsinfo.setTelescope(telescop);
	myobsinfo.setPointingCenter(mvPhaseCenter);
	myobsinfo.setObsDate(obsEpoch);
	myobsinfo.setObserver(msc.observation().observer()(0));

	/// Attach obsInfo to the CoordinateSystem
	csys.setObsInfo(myobsinfo);

      }// if MS is provided.
      ***/
    } // end of else when coordsys record is not defined...
 
    //    cout << " ----- ----- ------ ------ CSYS WORLD AXIS UNITS : " << csys.worldAxisUnits() << endl;

   return csys;
  }


  /*
#ifdef USEVIVB2
  CoordinateSystem SynthesisParamsImage::buildCoordinateSystem(vi::VisibilityIterator2* vi2)
#else
  CoordinateSystem SynthesisParamsImage::buildCoordinateSystem(ROVisibilityIterator* rvi )
#endif
  {
    LogIO os( LogOrigin("SynthesisParamsImage","buildCoordinateSystem",WHERE) );
  

    // get the first ms for multiple MSes
#ifdef USEVIVB2
    MeasurementSet msobj=vi2->getMeasurementSet();
#else
    MeasurementSet msobj=rvi->getMeasurementSet();
#endif

    MDirection phaseCenterToUse = phaseCenter;
    if( phaseCenterFieldId != -1 )
      {
	MSFieldColumns msfield(msobj.field());
	phaseCenterToUse=msfield.phaseDirMeas( phaseCenterFieldId ); 
      }
    // Setup Phase center if it is specified only by field id.

    /////////////////// Direction Coordinates
    MVDirection mvPhaseCenter(phaseCenterToUse.getAngle());
    // Normalize correctly
    MVAngle ra=mvPhaseCenter.get()(0);
    ra(0.0);

    MVAngle dec=mvPhaseCenter.get()(1);
    Vector<Double> refCoord(2);
    refCoord(0)=ra.get().getValue();    
    refCoord(1)=dec;
    Vector<Double> refPixel(2); 
    refPixel(0) = Double(imsize[0]/2);
    refPixel(1) = Double(imsize[1]/2);

    Vector<Double> deltas(2);
    deltas(0)=-1* cellsize[0].get("rad").getValue();
    deltas(1)=cellsize[1].get("rad").getValue();
    Matrix<Double> xform(2,2);
    xform=0.0;xform.diagonal()=1.0;

    Vector<Double> projparams(2); 
    projparams = 0.0;
    if( useNCP==true ) { projparams[0]=0.0, projparams[1]=1/tan(refCoord(1));   }
    Projection projTo( projection.type(), projparams );

    DirectionCoordinate
      myRaDec(MDirection::Types(phaseCenterToUse.getRefPtr()->getType()),
	      //	      projection,
	      projTo,
	      refCoord(0), refCoord(1),
	      deltas(0), deltas(1),
	      xform,
	      refPixel(0), refPixel(1));


    //defining observatory...needed for position on earth
    // get the first ms for multiple MSes
    MSColumns msc(msobj);
    String telescop = msc.observation().telescopeName()(0);
    MEpoch obsEpoch = msc.timeMeas()(0);
    MPosition obsPosition;
    if(!(MeasTable::Observatory(obsPosition, telescop)))
      {
        os << LogIO::WARN << "Did not get the position of " << telescop
           << " from data repository" << LogIO::POST;
        os << LogIO::WARN
           << "Please contact CASA to add it to the repository."
           << LogIO::POST;
        os << LogIO::WARN << "Frequency conversion will not work " << LogIO::POST;
      }

    ObsInfo myobsinfo;
    myobsinfo.setTelescope(telescop);
    myobsinfo.setPointingCenter(mvPhaseCenter);
    myobsinfo.setObsDate(obsEpoch);
    myobsinfo.setObserver(msc.observation().observer()(0));

    /// Attach obsInfo to the CoordinateSystem
    ///csys.setObsInfo(myobsinfo);


    /////////////////// Spectral Coordinate

    //Make sure frame conversion is switched off for REST frame data.
    //Bool freqFrameValid=(freqFrame != MFrequency::REST);

    //freqFrameValid=(freqFrame != MFrequency::REST );
    freqFrameValid=(freqFrame != MFrequency::REST || mode != "cubedata" );

    // *** get selected spw ids ***
    Vector<Int> spwids;
#ifdef USEVIVB2
    vi2->spectralWindows( spwids );
#else
    Vector<Int> nvischan;
    rvi->allSelectedSpectralWindows(spwids,nvischan);
#endif
    if(spwids.nelements()==0)
      {
        Int nspw=msc.spectralWindow().nrow();
        spwids.resize(nspw);
        indgen(spwids); 
      }
    MFrequency::Types dataFrame=(MFrequency::Types)msc.spectralWindow().measFreqRef()(spwids[0]);
    Vector<Double> dataChanFreq, dataChanWidth;
    if(spwids.nelements()==1)
      {
        dataChanFreq=msc.spectralWindow().chanFreq()(spwids[0]);
        dataChanWidth=msc.spectralWindow().chanWidth()(spwids[0]);
      }
    else 
      {
        SubMS thems(msobj);
        if(!thems.combineSpws(spwids,true,dataChanFreq,dataChanWidth))
	  //if(!MSTransformRegridder::combineSpws(os,msobj.tableName(),spwids,dataChanFreq,dataChanWidth))
          {
            os << LogIO::SEVERE << "Error combining SpWs" << LogIO::POST;
          }
      }
    
    Quantity qrestfreq = restFreq.nelements() >0 ? restFreq[0]: Quantity(0.0, "Hz");
    if( qrestfreq.getValue("Hz")==0 ) 
      {
#ifdef USEVIVB2
	///// TTCheck
	Vector<Int> flds;
	vi2->fieldIds( flds );
	AlwaysAssert( flds.nelements()>0 , AipsError );
	Int fld = flds[0];
#else
        Int fld = rvi->fieldId();
#endif
        MSDopplerUtil msdoppler(msobj);
        Vector<Double> restfreqvec;
        msdoppler.dopplerInfo(restfreqvec, spwids[0], fld);
        qrestfreq = restfreqvec.nelements() >0 ? Quantity(restfreqvec[0],"Hz"): Quantity(0.0, "Hz");
      }
    Double refPix;
    Vector<Double> chanFreq;
    Vector<Double> chanFreqStep;
    String specmode;

    //for mfs 
    Double freqmin=0, freqmax=0;
#ifdef USEVIVB2
    vi2->getFreqInSpwRange(freqmin,freqmax,freqFrameValid? freqFrame:MFrequency::REST );
#else
    rvi->getFreqInSpwRange(freqmin,freqmax,freqFrameValid? freqFrame:MFrequency::REST );
#endif

    if (!getImFreq(chanFreq, chanFreqStep, refPix, specmode, obsEpoch, 
		   obsPosition, dataChanFreq, dataChanWidth, dataFrame, qrestfreq, freqmin, freqmax,
		   phaseCenterToUse))
      throw(AipsError("Failed to determine channelization parameters"));

    Bool nonLinearFreq(false);
    String veltype_p=veltype;
    veltype_p.upcase(); 
    if(veltype_p.contains("OPTICAL") || veltype_p.matches("Z") || veltype_p.contains("BETA") ||
         veltype_p.contains("RELATI") || veltype_p.contains("GAMMA")) 
      {
        nonLinearFreq= true;
      }

    SpectralCoordinate mySpectral;
    Double stepf;
    if(!nonLinearFreq) 
    //TODO: velocity mode default start case (use last channels?)
      {
        Double startf=chanFreq[0];
        //Double stepf=chanFreqStep[0];
        if(chanFreq.nelements()==1) 
          {
            stepf=chanFreqStep[0];
          }
        else 
          { 
            stepf=chanFreq[1]-chanFreq[0];
          }
        Double restf=qrestfreq.getValue("Hz");
        //cerr<<" startf="<<startf<<" stepf="<<stepf<<" refPix="<<refPix<<" restF="<<restf<<endl;
        // once NOFRAME is implemented do this 
        if(mode=="cubedata") 
          {
      //       mySpectral = SpectralCoordinate(freqFrameValid ? MFrequency::Undefined : MFrequency::REST, 
             mySpectral = SpectralCoordinate(freqFrame == MFrequency::REST? 
                                             MFrequency::REST : MFrequency::Undefined, 
        	                             startf, stepf, refPix, restf);
          }
        else 
          {
             mySpectral = SpectralCoordinate(freqFrameValid ? freqFrame : MFrequency::REST, 
		startf, stepf, refPix, restf);
          }
      }
    else 
      { // nonlinear freq coords - use tabular setting
        // once NOFRAME is implemented do this 
        if(mode=="cubedata") 
          {
            //mySpectral = SpectralCoordinate(freqFrameValid ? MFrequency::Undefined : MFrequency::REST,
            mySpectral = SpectralCoordinate(freqFrame == MFrequency::REST ? 
                                            MFrequency::REST : MFrequency::Undefined,
                                            chanFreq, (Double)qrestfreq.getValue("Hz"));
          }
        else 
          {
            mySpectral = SpectralCoordinate(freqFrameValid ? freqFrame : MFrequency::REST,
                 chanFreq, (Double)qrestfreq.getValue("Hz"));
          }
      }
    //cout << "Rest Freq : " << restFreq << endl;

    for(uInt k=1 ; k < restFreq.nelements(); ++k)
      mySpectral.setRestFrequency(restFreq[k].getValue("Hz"));

    if ( freqFrameValid ) {
      mySpectral.setReferenceConversion(MFrequency::LSRK,obsEpoch,obsPosition,phaseCenterToUse);   
    }

    //    cout << "RF from coordinate : " << mySpectral.restFrequency() << endl;

    ////////////////// Stokes Coordinate
   
    Vector<Int> whichStokes = decideNPolPlanes(stokes);
    if(whichStokes.nelements()==0)
      throw(AipsError("Stokes selection of " + stokes + " is invalid"));
    StokesCoordinate myStokes(whichStokes);

    //////////////////  Build Full coordinate system. 

    CoordinateSystem csys;
    csys.addCoordinate(myRaDec);
    csys.addCoordinate(myStokes);
    csys.addCoordinate(mySpectral);
    csys.setObsInfo(myobsinfo);

    //////////////// Set Observatory info, if MS is provided.
    // (remove this section after verified...)
    return csys;
  }
*/

  MDirection SynthesisParamsImage::getMovingSourceDir(const MeasurementSet& ms, const MEpoch& refEp, const MPosition& obsposition, const MDirection::Types outframe){
    MDirection outdir;
    String ephemtab(movingSource);
    if(movingSource=="TRACKFIELD"){
      Int fieldID=MSColumns(ms).fieldId()(0);
      ephemtab=Path(MSColumns(ms).field().ephemPath(fieldID)).absoluteName();
    }
    casacore::MDirection::Types planetType=MDirection::castType(trackDir.getRef().getType());
    if( (! Table::isReadable(ephemtab)) &&   ( (planetType <= MDirection::N_Types) || (planetType >= MDirection::COMET)))
      throw(AipsError("Does not have a valid ephemeris table or major solar system object defined"));
    MeasFrame mframe(refEp, obsposition);
    MDirection::Ref outref1(MDirection::AZEL, mframe);
    MDirection::Ref outref(outframe, mframe);
    MDirection tmpazel;
    if(planetType >=MDirection::MERCURY && planetType <MDirection::COMET){
      tmpazel=MDirection::Convert(trackDir, outref1)();
    }
    else{
      MeasComet mcomet(Path(ephemtab).absoluteName());
      mframe.set(mcomet);
      tmpazel=MDirection::Convert(MDirection(MDirection::COMET), outref1)();
    }
    outdir=MDirection::Convert(tmpazel, outref)();

    return outdir;
  }
  
  Bool SynthesisParamsImage::getImFreq(Vector<Double>& chanFreq, Vector<Double>& chanFreqStep, 
                                       Double& refPix, String& specmode,
                                       const MEpoch& obsEpoch, const MPosition& obsPosition, 
                                       const Vector<Double>& dataChanFreq, 
                                       const Vector<Double>& dataChanWidth,
                                       const MFrequency::Types& dataFrame, 
                                       const Quantity& qrestfreq, const Double& freqmin, const Double& freqmax,
				       const MDirection& phaseCenter) 
  {

    String inStart, inStep; 
    specmode = findSpecMode(mode);
    String freqframe;
    Bool verbose("true"); // verbose logging messages from calcChanFreqs
    LogIO os( LogOrigin("SynthesisParamsImage","getImFreq",WHERE) );

    refPix=0.0; 
    Bool descendingfreq(false);
    Bool descendingoutfreq(false);

    if( mode.contains("cube") )
      { 
        String restfreq=QuantityToString(qrestfreq);
        // use frame from input start or width in MFreaquency or MRadialVelocity
        freqframe = qmframe!=""? qmframe: MFrequency::showType(freqFrame);
        // emit warning here if qmframe is used 
        //
        inStart = start;
        inStep = step;
        if( specmode=="channel" ) 
          {
            inStart = String::toString(chanStart);
            inStep = String::toString(chanStep); 
            // negative step -> descending channel indices 
            if (inStep.contains(casacore::Regex("^-"))) descendingfreq=true;
            // input frame is the data frame
            //freqframe = MFrequency::showType(dataFrame);
          }
        else if( specmode=="frequency" ) 
          {
            //if ( freqStart.getValue("Hz") == 0 && freqStart.getUnit() != "" ) { // default start
            //start = String::toString( freqmin ) + freqStart.getUnit();
            //}
            //else {
            //start = String::toString( freqStart.getValue(freqStart.getUnit()) )+freqStart.getUnit();  
            //}
            //step = String::toString( freqStep.getValue(freqStep.getUnit()) )+freqStep.getUnit();  
            // negative freq width -> descending freq ordering
            if(inStep.contains(casacore::Regex("^-"))) descendingfreq=true;
          }
        else if( specmode=="velocity" ) 
          {
            // if velStart is empty set start to vel of freqmin or freqmax?
            //if ( velStart.getValue(velStart.getUnit()) == 0 && !(velStart.getUnit().contains("m/s")) ) {
            //  start = "";
            //}
            //else { 
            //  start = String::toString( velStart.getValue(velStart.getUnit()) )+velStart.getUnit();  
            //}
            //step = String::toString( velStep.getValue(velStep.getUnit()) )+velStep.getUnit();  
            // positive velocity width -> descending freq ordering
            if (!inStep.contains(casacore::Regex("^-"))) descendingfreq=true;
          }

      if (inStep=='0') inStep="";

      MRadialVelocity mSysVel; 
      Quantity qVel;
      MRadialVelocity::Types mRef;
      if(mode!="cubesource") 
        {
	  
	  
          if(freqframe=="SOURCE") 
            {
              os << LogIO::SEVERE << "freqframe=\"SOURCE\" is only allowed for mode=\"cubesrc\""
                 << LogIO::EXCEPTION;
              return false; 
            }
        }
      else // only for cubesrc mode: TODO- check for the ephemeris info.
        {
	  freqframe=MFrequency::showType(dataFrame);
          if(sysvel!="") {
            stringToQuantity(sysvel,qVel);
            MRadialVelocity::getType(mRef,sysvelframe);
            mSysVel=MRadialVelocity(qVel,mRef);
          }
          else // and if no ephemeris info, issue a warning... 
            {  mSysVel=MRadialVelocity();}
        }
      // cubedata mode: input start, step are those of the input data frame
      if ( mode=="cubedata" ) 
        {
          freqframe=MFrequency::showType(dataFrame);
          freqFrameValid=false; // no conversion for vb.lsrfrequency()
        }
      //if ( mode=="cubedata" ) freqframe=MFrequency::REST;
      
      // *** NOTE *** 
      // calcChanFreqs alway returns chanFreq in
      // ascending freq order. 
      // for step < 0 calcChanFreqs returns chanFreq that 
      // contains start freq. in its last element of the vector. 
      //
      os << LogIO::DEBUG1<<"mode="<<mode<<" specmode="<<specmode<<" inStart="<<inStart
         <<" inStep="<<inStep<<" restfreq="<<restfreq<<" freqframe="<<freqframe
         <<" dataFrame="<<dataFrame <<" veltype="<<veltype<<" nchan="<<nchan
         << LogIO::POST;
      ostringstream ostr;
      ostr << " phaseCenter='" << phaseCenter;
      os << String(ostr)<<"' ";

      Double dummy; // dummy variable  - weightScale is not used here
      Bool rst=MSTransformRegridder::calcChanFreqs(os,
                           chanFreq, 
                           chanFreqStep,
                           dummy,
                           dataChanFreq,
                           dataChanWidth,
                           phaseCenter,
                           dataFrame,
                           obsEpoch,
                           obsPosition,
                           specmode,
                           nchan,
                           inStart,
                           inStep,
                           restfreq,
                           freqframe,
                           veltype,
                           verbose, 
                           mSysVel
                           );

      if( nchan==-1 ) 
	{ 
	  nchan = chanFreq.nelements(); 
	  os << LogIO::DEBUG1 << "Setting nchan to number of selected channels : " << nchan << LogIO::POST;
	}

      if (!rst) {
        os << LogIO::SEVERE << "calcChanFreqs failed, check input start and width parameters"
           << LogIO::EXCEPTION;
        return false;
      }
      os << LogIO::DEBUG1
         <<"chanFreq 0="<<chanFreq[0]<<" chanFreq last="<<chanFreq[chanFreq.nelements()-1]
         << LogIO::POST;

      if (chanFreq[0]>chanFreq[chanFreq.nelements()-1]) {
        descendingoutfreq = true;
      }

       //if (descendingfreq && !descendingoutfreq) {
      if ((specmode=="channel" && descendingfreq==1) 
          || (specmode!="channel" && (descendingfreq != descendingoutfreq))) { 
        // reverse the freq vector if necessary so the first element can be
        // used to set spectralCoordinates in all the cases.
        //
        // also do for chanFreqStep..
        std::vector<Double> stlchanfreq;
        chanFreq.tovector(stlchanfreq);
        std::reverse(stlchanfreq.begin(),stlchanfreq.end());
        chanFreq=stlchanfreq;
        chanFreqStep=-chanFreqStep;
      }
    }
    else if ( mode=="mfs" ) {
      chanFreq.resize(1);
      chanFreqStep.resize(1);
      //chanFreqStep[0] = freqmax - freqmin;
      Double freqmean = (freqmin + freqmax)/2;
      if (refFreq.getValue("Hz")==0) {
        chanFreq[0] = freqmean;
        refPix = 0.0;
	chanFreqStep[0] = freqmax - freqmin;
      }
      else { 
        chanFreq[0] = refFreq.getValue("Hz"); 
	// Set the new reffreq to be the refPix (CAS-9518)
        refPix  = 0.0; // (refFreq.getValue("Hz") - freqmean)/chanFreqStep[0];
	// A larger bandwidth to compensate for the shifted reffreq (CAS-9518)
	chanFreqStep[0] = freqmax - freqmin + 2*fabs(chanFreq[0] - freqmean);
      }

      if( nchan==-1 ) nchan=1;
      if( qrestfreq.getValue("Hz")==0.0 )  {
         restFreq.resize(1);
         restFreq[0] = Quantity(freqmean,"Hz");
      }
    }
    else {
       // unrecognized mode, error
       os << LogIO::SEVERE << "mode="<<mode<<" is unrecognized."
          << LogIO::EXCEPTION;
       return false;
    }
    return true;

  }//getImFreq
  /////////////////////////
  Double SynthesisParamsImage::getCubeImageStartFreq(){
    Double inStartFreq=-1.0;
    String checkspecmode("");
    if(mode.contains("cube")) {
      checkspecmode = findSpecMode(mode);
    } 
    if(checkspecmode!="") {
      MFrequency::Types mfreqframe = frame!="" ? MFrequency::typeFromString(frame):MFrequency::LSRK;
      if(checkspecmode=="channel") {
	inStartFreq=-1.0;  
      }
      else {
	if(checkspecmode=="frequency") {
	  inStartFreq = freqStart.get("Hz").getValue();  
	}
	else if(checkspecmode=="velocity") {
	  MDoppler::Types DopType;
	  MDoppler::getType(DopType, veltype);
	  MDoppler mdop(velStart,DopType);
	  Quantity qrestfreq = restFreq.nelements() >0 ? restFreq[0]: Quantity(0.0, "Hz");
	  inStartFreq = MFrequency::fromDoppler(mdop, qrestfreq.getValue(Unit("Hz")), mfreqframe).getValue(); 
	}
      }
    }

    return inStartFreq;

  }

  String SynthesisParamsImage::findSpecMode(const String& mode) const
  {
    String specmode;
    specmode="channel";
    if ( mode.contains("cube") ) {
      // if velstart or velstep is defined -> specmode='vel'
      // else if freqstart or freqstep is defined -> specmode='freq'
      // velocity: assume unset if velStart => 0.0 with no unit
      //           assume unset if velStep => 0.0 with/without unit
      if ( !(velStart.getValue()==0.0 && velStart.getUnit()=="" ) ||
           !( velStep.getValue()==0.0)) { 
        specmode="velocity";
      }
      else if ( !(freqStart.getValue()==0.0 && freqStart.getUnit()=="") ||
                !(freqStep.getValue()==0.0)) {
        specmode="frequency";
      }
    }
    return specmode;
  }


  Vector<Int> SynthesisParamsImage::decideNPolPlanes(const String& stokes) const
  {
    Vector<Int> whichStokes(0);
    if(stokes=="I" || stokes=="Q" || stokes=="U" || stokes=="V" || 
       stokes=="RR" ||stokes=="LL" || 
       stokes=="XX" || stokes=="YY" ) {
      whichStokes.resize(1);
      whichStokes(0)=Stokes::type(stokes);
    }
    else if(stokes=="IV" || stokes=="IQ" || 
	    stokes=="RRLL" || stokes=="XXYY" ||
	    stokes=="QU" || stokes=="UV"){
      whichStokes.resize(2);
      
      if(stokes=="IV"){ whichStokes[0]=Stokes::I; whichStokes[1]=Stokes::V;}
      else if(stokes=="IQ"){whichStokes[0]=Stokes::I; whichStokes[1]=Stokes::Q;}
      else if(stokes=="RRLL"){whichStokes[0]=Stokes::RR; whichStokes[1]=Stokes::LL;}
      else if(stokes=="XXYY"){whichStokes[0]=Stokes::XX; whichStokes[1]=Stokes::YY; }
      else if(stokes=="QU"){whichStokes[0]=Stokes::Q; whichStokes[1]=Stokes::U; }
      else if(stokes=="UV"){ whichStokes[0]=Stokes::U; whichStokes[1]=Stokes::V; }
	
    }
  
    else if(stokes=="IQU" || stokes=="IUV") {
      whichStokes.resize(3);
      if(stokes=="IUV")
	{whichStokes[0]=Stokes::I; whichStokes[1]=Stokes::U; whichStokes[2]=Stokes::V;}
      else
	{whichStokes[0]=Stokes::I; whichStokes[1]=Stokes::Q; whichStokes[2]=Stokes::U;}
    }
    else if(stokes=="IQUV"){
      whichStokes.resize(4);
      whichStokes(0)=Stokes::I; whichStokes(1)=Stokes::Q;
      whichStokes(2)=Stokes::U; whichStokes(3)=Stokes::V;
    }
      
    return whichStokes;
  }// decidenpolplanes

  IPosition SynthesisParamsImage::shp() const
  {
    uInt nStokes = ( decideNPolPlanes(stokes) ).nelements();

    if( imsize[0]<=0 || imsize[1]<=0 || nStokes<=0 || nchan<=0 )
      {
	throw(AipsError("Internal Error : Image shape is invalid : [" + String::toString(imsize[0]) + "," + String::toString(imsize[1]) + "," + String::toString(nStokes) + "," + String::toString(nchan) + "]" )); 
      }

    return IPosition( 4, imsize[0], imsize[1], nStokes, nchan );
  }

  Record SynthesisParamsImage::getcsys() const
  {
      return csysRecord;
  }

  Record SynthesisParamsImage::updateParams(const Record& impar)
  {
      Record newimpar( impar );
      if ( impar.isDefined("csys") ) 
       { 
           Vector<Int> newimsize(2);
           newimsize[0] = imshape[0];
           newimsize[1] = imshape[1];
           newimpar.define("imsize", newimsize);
           if ( newimpar.isDefined("direction0") )
             {
               Record dirRec = newimpar.subRecord("direction0");
               Vector<Double> cdelta = dirRec.asArrayDouble("cdelt");
               Vector<String> cells(2);
               cells[0] = String::toString(-1*cdelta[0]) + "rad";
               cells[1] = String::toString(-1*cdelta[1]) + "rad";
               newimpar.define("cell", cells );
             } 
           if ( newimpar.isDefined("stokes1") )
             {
               Record stokesRec = newimpar.subRecord("stokes1");
               Vector<String> stokesvecs = stokesRec.asArrayString("stokes"); 
               String stokesStr;
               for (uInt j = 0; j < stokesvecs.nelements(); j++)
                 {
                     stokesStr+=stokesvecs[j];
                 }
             }
           if ( newimpar.isDefined("nchan") ) 
             {
               newimpar.define("nchan",imshape[2]);
             }
           if ( newimpar.isDefined("spectral2") ) 
             {
               Record specRec = newimpar.subRecord("spectral2");
               if ( specRec.isDefined("restfreqs") ) 
                 {
                   Vector<Double> restfs = specRec.asArrayDouble("restfreqs");
                   Vector<String> restfstrs(restfs.nelements());
                   for(uInt restf=0; restf<restfs.nelements(); restf++){restfstrs[restf] = String::toString(restfs[restf]) + "Hz";}
                   newimpar.define("restfreq",restfstrs);
                 }
               //reffreq?
               //outframe
               //sysvel
               //sysvelframe
             }      
        }
      return newimpar;
  }

 /////////////////////// Grid/FTMachine Parameters

  SynthesisParamsGrid::SynthesisParamsGrid():SynthesisParams()
  {
    setDefaults();
  }

  SynthesisParamsGrid::~SynthesisParamsGrid()
  {
  }


  void SynthesisParamsGrid::fromRecord(const Record &inrec)
  {
    setDefaults();

    String err("");

    try
      {
	err += readVal( inrec, String("imagename"), imageName);

	// FTMachine parameters
	err += readVal( inrec, String("gridder"), gridder );
	err += readVal( inrec, String("padding"), padding );
	err += readVal( inrec, String("useautocorr"), useAutoCorr );
	err += readVal( inrec, String("usedoubleprec"), useDoublePrec );
	err += readVal( inrec, String("wprojplanes"), wprojplanes );
	err += readVal( inrec, String("convfunc"), convFunc );

	err += readVal( inrec, String("vptable"), vpTable );

	//// convert 'gridder' to 'ftmachine' and 'mtype'
	ftmachine="gridft";
	mType="default";
	if(gridder=="ft" || gridder=="gridft" || gridder=="standard" )
	  { ftmachine="gridft"; }
	if( (gridder=="widefield" || gridder=="wproject" || gridder=="wprojectft" ) && (wprojplanes>1 || wprojplanes==-1))
	  { ftmachine="wprojectft";}

	if(gridder=="ftmosaic" || gridder=="mosaicft" || gridder=="mosaic" )
	  { ftmachine="mosaicft"; }
	if(gridder=="imagemosaic") {
	    mType="imagemosaic";
	    if (wprojplanes>1 || wprojplanes==-1){ ftmachine="wprojectft"; }
	  }
	if(gridder=="awproject" || gridder=="awprojectft" || gridder=="awp")
	  {ftmachine="awprojectft";}
	if(gridder=="singledish") {
	  ftmachine="sd";
	}

	String deconvolver;
	err += readVal( inrec, String("deconvolver"), deconvolver );
	if( deconvolver== "mtmfs" ) 
	  { mType="multiterm"; }// Takes precedence over imagemosaic

	// facets	
	err += readVal( inrec, String("facets"), facets);
	// chanchunks
	err += readVal( inrec, String("chanchunks"), chanchunks);

	// Spectral interpolation
	err += readVal( inrec, String("interpolation"), interpolation );// not used in SI yet...
	// Track moving source ?
	err += readVal( inrec, String("distance"), distance );
	err += readVal( inrec, String("tracksource"), trackSource );
	err += readVal( inrec, String("trackdir"), trackDir );

	// The extra params for WB-AWP
	err += readVal( inrec, String("aterm"), aTermOn );
	err += readVal( inrec, String("psterm"), psTermOn );
	err += readVal( inrec, String("mterm"), mTermOn );
 	err += readVal( inrec, String("wbawp"), wbAWP );
	err += readVal( inrec, String("cfcache"), cfCache );
	err += readVal( inrec, String("usepointing"), usePointing );
	err += readVal( inrec, String("pointingoffsetsigdev"), pointingOffsetSigDev );
	err += readVal( inrec, String("dopbcorr"), doPBCorr );
	err += readVal( inrec, String("conjbeams"), conjBeams );
	err += readVal( inrec, String("computepastep"), computePAStep );
	err += readVal( inrec, String("rotatepastep"), rotatePAStep );

	// The extra params for single-dish
	err += readVal( inrec, String("pointingcolumntouse"), pointingDirCol );
	err += readVal( inrec, String("skypolthreshold"), skyPosThreshold );
	err += readVal( inrec, String("convsupport"), convSupport );
	err += readVal( inrec, String("truncate"), truncateSize );
	err += readVal( inrec, String("gwidth"), gwidth );
	err += readVal( inrec, String("jwidth"), jwidth );
	err += readVal( inrec, String("minweight"), minWeight );
	err += readVal( inrec, String("clipminmax"), clipMinMax );

	// Single or MultiTerm mapper : read in 'deconvolver' and set mType here.
	//	err += readVal( inrec, String("mtype"), mType );

	if( ftmachine=="awprojectft" && cfCache=="" )
	  {cfCache=imageName+".cf"; }

	if( ftmachine=="awprojectft" && 
	    usePointing==True && 
	    pointingOffsetSigDev.nelements() != 2 )
	  {
	    // Set the default to a large value so that it behaves like CASA 5.6's usepointing=True.
	    pointingOffsetSigDev.resize(2);
	    pointingOffsetSigDev[0]=600.0;
	    pointingOffsetSigDev[1]=600.0;
	  }

	err += verify();
	
      }
    catch(AipsError &x)
      {
	err = err + x.getMesg() + "\n";
      }
      
      if( err.length()>0 ) throw(AipsError("Invalid Gridding/FTM Parameter set : " + err));
      
  }

  String SynthesisParamsGrid::verify() const
  {
    String err;

    // Check for valid FTMachine type.
    // Valid other params per FTM type, etc... ( check about nterms>1 )

    if( imageName=="" ) {err += "Please supply an image name\n";}

    if( (ftmachine != "gridft") && (ftmachine != "wprojectft") && 
	(ftmachine != "mosaicft") && (ftmachine != "awprojectft") && 
	(ftmachine != "mawprojectft") && (ftmachine != "protoft") &&
	(ftmachine != "sd"))
      { err += "Invalid ftmachine name. Must be one of 'gridft', 'wprojectft', 'mosaicft', 'awprojectft', 'mawpojectft'";   }

    if( ((ftmachine=="mosaicft") && (mType=="imagemosaic"))  || 
	((ftmachine=="awprojectft") && (mType=="imagemosaic")) )
      {  err +=  "Cannot use " + ftmachine + " with " + mType + 
	  " because it is a redundant choice for mosaicing. "
	  "In the future, we may support the combination to signal the use of single-pointing sized image grids during gridding and iFT, "
	  "and only accumulating it on the large mosaic image. For now, please set either mappertype='default' to get mosaic gridding "
	  " or ftmachine='ft' or 'wprojectft' to get image domain mosaics. \n"; }

    if( facets < 1 )
      {err += "Must have at least 1 facet\n"; }
    //if( chanchunks < 1 )
    //  {err += "Must have at least 1 chanchunk\n"; }
    if( (facets>1) && (chanchunks>1) )
      { err += "The combination of facetted imaging with channel chunking is not yet supported. Please choose only one or the other for now. \n";}

    if(ftmachine=="wproject" && (wprojplanes==0 || wprojplanes==1))
      {err += "The wproject gridder must be accompanied with wprojplanes>1 or wprojplanes=-1\n";}

    if((ftmachine=="awprojectft") && (facets>1) )
      {err += "The awprojectft gridder supports A- and W-Projection. "
	  "Instead of using facets>1 to deal with the W-term, please set the number of wprojplanes to a value > 1 "
	  "to trigger the combined AW-Projection algorithm. \n";  } // Also, the way the AWP cfcache is managed, even if all facets share a common one so that they reuse convolution functions, the first facet's gridder writes out the avgPB and all others see that it's there and don't compute their own. As a result, the code will run, but the first facet's weight image will be duplicated for all facets.  If needed, this must be fixed in the way the AWP gridder manages its cfcache. But, since the AWP gridder supports joint A and W projection, facet support may never be needed in the first place... 

    if((ftmachine=="awprojectft") && (wprojplanes==-1) )
      {err +="The awprojectft gridder does not support wprojplanes=-1 for automatic calculation. Please pick a value >1" ;}

    if( (ftmachine=="mosaicft") && (facets>1) )
      { err += "The combination of mosaicft gridding with multiple facets is not supported. "
	  "Please use the awprojectft gridder instead, and set wprojplanes to a value > 1 to trigger AW-Projection. \n"; }

    if( ftmachine=="awprojectft" && usePointing==True && pointingOffsetSigDev.nelements() != 2 )
      {
	err += "The pointingoffsetsigdev parameter must be a two-element vector of doubles in order to be used with usepointing=True and the AWProject gridder. Setting it to the default of \n ";
      }



    // todo: any single-dish specific limitation?

    return err;
  }

  void SynthesisParamsGrid::setDefaults()
  {
    imageName="";
    // FTMachine parameters
    //ftmachine="GridFT";
    ftmachine="gridft";
    gridder=ftmachine;
    padding=1.2;
    useAutoCorr=false;
    useDoublePrec=true; 
    wprojplanes=1; 
    convFunc="SF"; 
    vpTable="";
    
    // facets
    facets=1;

    // chanchunks
    chanchunks=1;

    // Spectral Axis interpolation
    interpolation=String("nearest");

    //mosaic use pointing
    usePointing=false;
    // Moving phase center ?
    distance=Quantity(0,"m");
    trackSource=false;
    trackDir=MDirection(Quantity(0.0, "deg"), Quantity(90.0, "deg"));

    // The extra params for WB-AWP
    aTermOn    = true;
    psTermOn   = true;
    mTermOn    = false;
    wbAWP      = true;
    cfCache  = "";
    usePointing = false;
    pointingOffsetSigDev.resize(0);
    //    pointingOffsetSigDev.set(30.0);
    doPBCorr   = true;
    conjBeams  = true;
    computePAStep=360.0;
    rotatePAStep=5.0;

    // extra params for single-dish
    pointingDirCol = "";
    skyPosThreshold = 0.0;
    convSupport = -1;
    truncateSize = Quantity(-1.0);
    gwidth = Quantity(-1.0);
    jwidth = Quantity(-1.0);
    minWeight = 0.0;
    clipMinMax = False;

    // Mapper type
    mType = String("default");
    
  }

  Record SynthesisParamsGrid::toRecord() const
  {
    Record gridpar;

	gridpar.define("imagename", imageName);
    // FTMachine params
    gridpar.define("padding", padding);
    gridpar.define("useautocorr",useAutoCorr );
    gridpar.define("usedoubleprec", useDoublePrec);
    gridpar.define("wprojplanes", wprojplanes);
    gridpar.define("convfunc", convFunc);
    gridpar.define("vptable", vpTable);

    gridpar.define("facets", facets);
    gridpar.define("chanchunks", chanchunks);
    
    gridpar.define("interpolation",interpolation);

    gridpar.define("distance", QuantityToString(distance));
    gridpar.define("tracksource", trackSource);
    gridpar.define("trackdir", MDirectionToString( trackDir ));

    gridpar.define("aterm",aTermOn );
    gridpar.define("psterm",psTermOn );
    gridpar.define("mterm",mTermOn );
    gridpar.define("wbawp", wbAWP);
    gridpar.define("cfcache", cfCache);
    gridpar.define("usepointing",usePointing );
    gridpar.define("pointingoffsetsigdev", pointingOffsetSigDev);
    gridpar.define("dopbcorr", doPBCorr);
    gridpar.define("conjbeams",conjBeams );
    gridpar.define("computepastep", computePAStep);
    gridpar.define("rotatepastep", rotatePAStep);

    gridpar.define("pointingcolumntouse", pointingDirCol );
    gridpar.define("skyposthreshold", skyPosThreshold );
    gridpar.define("convsupport", convSupport );
    gridpar.define("truncate", QuantityToString(truncateSize) );
    gridpar.define("gwidth", QuantityToString(gwidth) );
    gridpar.define("jwidth", QuantityToString(jwidth) );
    gridpar.define("minweight", minWeight );
    gridpar.define("clipminmax", clipMinMax );

    if( mType=="multiterm") gridpar.define("deconvolver","mtmfs");
    ///    else gridpar.define("deconvolver","singleterm");

    if( mType=="imagemosaic") gridpar.define("gridder","imagemosaic");
    else gridpar.define("gridder", gridder);

    //    gridpar.define("mtype", mType);

    return gridpar;
  }



  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

 /////////////////////// Deconvolver Parameters

  SynthesisParamsDeconv::SynthesisParamsDeconv():SynthesisParams()
  {
    setDefaults();
  }

  SynthesisParamsDeconv::~SynthesisParamsDeconv()
  {
  }


  void SynthesisParamsDeconv::fromRecord(const Record &inrec)
  {
    setDefaults();

    String err("");

    try
      {
	
	err += readVal( inrec, String("imagename"), imageName );
	err += readVal( inrec, String("deconvolver"), algorithm );


	//err += readVal( inrec, String("startmodel"), startModel );
	// startmodel parsing copied from SynthesisParamsImage. Clean this up !!!
        if( inrec.isDefined("startmodel") ) 
          {
            if( inrec.dataType("startmodel")==TpString )
	      {
		String onemodel;
		err += readVal( inrec, String("startmodel"), onemodel );
		if( onemodel.length()>0 )
		  {
		    startModel.resize(1);
		    startModel[0] = onemodel;
		  }
		else {startModel.resize();}
	      }
	    else if( inrec.dataType("startmodel")==TpArrayString || 
		     inrec.dataType("startmodel")==TpArrayBool)
	      {
		err += readVal( inrec, String("startmodel"), startModel );
	      }
	    else {
	      err += String("startmodel must be either a string(singleterm) or a list of strings(multiterm)\n");
	      }
	  }
	//------------------------

	err += readVal( inrec, String("id"), deconvolverId );
	err += readVal( inrec, String("nterms"), nTaylorTerms );

	err += readVal( inrec, String("scales"), scales );
	err += readVal( inrec, String("scalebias"), scalebias );

        err += readVal( inrec, String("usemask"), maskType );
        if( maskType=="auto-thresh" ) 
          {
            autoMaskAlgorithm = "thresh";
          }
        else if( maskType=="auto-thesh2" )
          {
            autoMaskAlgorithm = "thresh2";
          }
        else if( maskType=="auto-onebox" ) 
          {
            autoMaskAlgorithm = "onebox";
          }
        else if( maskType=="user" || maskType=="pb" )
          {
            autoMaskAlgorithm = "";
          }
              

        if( inrec.isDefined("mask") ) 
          {
            if( inrec.dataType("mask")==TpString )
              {
                err+= readVal( inrec, String("mask"), maskString );
              }
            else if( inrec.dataType("mask")==TpArrayString ) 
              {
                err+= readVal( inrec, String("mask"), maskList );
              }
           }
        
        if( inrec.isDefined("pbmask") )
          {
            err += readVal( inrec, String("pbmask"), pbMask ); 
          }
        if( inrec.isDefined("maskthreshold") ) 
          {
            if( inrec.dataType("maskthreshold")==TpString )
              {
                err += readVal( inrec, String("maskthreshold"), maskThreshold );
                //deal with the case a string is a float value without unit
                Quantity testThresholdString;
                Quantity::read(testThresholdString,maskThreshold);
                if( testThresholdString.getUnit()=="" )
                  {
                    if(testThresholdString.getValue()<1.0)
                      {
                          fracOfPeak = testThresholdString.getValue();
                          maskThreshold=String("");
                      }
                  }
              }
            else if( inrec.dataType("maskthreshold")==TpFloat || inrec.dataType("maskthreshold")==TpDouble )
              {

                err += readVal( inrec, String("maskthreshold"), fracOfPeak );
                if( fracOfPeak >=1.0 ) 
                  {
                    // maskthreshold is sigma ( * rms = threshold) 
                    //
                    maskThreshold=String::toString(fracOfPeak);
                    fracOfPeak=0.0;
                  }
              }
            else 
              {
                err += "maskthreshold must be a string, float, or double\n";
              }
           }
        if( inrec.isDefined("maskresolution") ) 
          { 
            if( inrec.dataType("maskresolution")==TpString )
              {
                err += readVal(inrec, String("maskresolution"), maskResolution );
                //deal with the case a string is a float value without unit
                Quantity testResolutionString;
                Quantity::read(testResolutionString,maskResolution);
                if( testResolutionString.getUnit()=="" )
                  {
                      maskResByBeam = testResolutionString.getValue();
                      maskResolution=String("");
                  }
              }
            else if( inrec.dataType("maskresolution")==TpFloat || inrec.dataType("maskresolution")==TpDouble )
              {

                err += readVal( inrec, String("maskresolution"), maskResByBeam );
              }
            else 
              {
                err += "maskresolution must be a string, float, or double\n";
              }
           }
             
       
        if( inrec.isDefined("nmask") ) 
          {
            if( inrec.dataType("nmask")==TpInt )
              {
                err+= readVal(inrec, String("nmask"), nMask );
              }
            else 
              {
                err+= "nmask must be an integer\n";
              }
          }
        if( inrec.isDefined("autoadjust") )
          {
            if( inrec.dataType("autoadjust")==TpBool )
              {
                err+= readVal(inrec, String("autoadjust"), autoAdjust );
              }
            else 
              {
                err+= "autoadjust must be a bool\n";
              }
          }
        //params for the new automasking algorithm
        if( inrec.isDefined("sidelobethreshold"))
          {
            if(inrec.dataType("sidelobethreshold")==TpFloat || inrec.dataType("sidelobethreshold")==TpDouble )
              {
                err+= readVal(inrec, String("sidelobethreshold"), sidelobeThreshold );
              }
            else 
              {
                err+= "sidelobethreshold must be a float or double";
              }
          }
        if( inrec.isDefined("noisethreshold"))
          {
            if(inrec.dataType("noisethreshold")==TpFloat || inrec.dataType("noisethreshold")==TpDouble )
              {
                err+= readVal(inrec, String("noisethreshold"), noiseThreshold );
              }
            else 
              {
                err+= "noisethreshold must be a float or double";
              }
          }
        if( inrec.isDefined("lownoisethreshold"))
          {
            if(inrec.dataType("lownoisethreshold")==TpFloat || inrec.dataType("lownoisethreshold")==TpDouble )
              {
                err+= readVal(inrec, String("lownoisethreshold"), lowNoiseThreshold );
              }
            else 
              {
                err+= "lownoisethreshold must be a float or double";
              }
          }
        if( inrec.isDefined("negativethreshold"))
          {
            if(inrec.dataType("negativethreshold")==TpFloat || inrec.dataType("negativethreshold")==TpDouble )
              {
                err+= readVal(inrec, String("negativethreshold"), negativeThreshold );
              }
            else 
              {
                err+= "negativethreshold must be a float or double";
              }
          }
        if( inrec.isDefined("smoothfactor"))
          {
            if( inrec.dataType("smoothfactor")==TpFloat || inrec.dataType("smoothfactor")==TpDouble )
              {
                err+= readVal(inrec, String("smoothfactor"), smoothFactor );
              }
            else 
              {
                err+= "smoothfactor must be a float or double";
              }
          }
        if( inrec.isDefined("minbeamfrac"))
          {
            if( inrec.dataType("minbeamfrac")==TpFloat || inrec.dataType("minbeamfrac")==TpDouble )
              {
                err+= readVal(inrec, String("minbeamfrac"), minBeamFrac );
              }
            else 
              {
                if (inrec.dataType("minbeamfrac")==TpInt) {
                  cerr<<"minbeamfrac is int"<<endl;
                }
                if (inrec.dataType("minbeamfrac")==TpString) {
                  cerr<<"minbeamfrac is String"<<endl;
                }
                err+= "minbeamfrac must be a float or double";
              }
          }
        if( inrec.isDefined("cutthreshold"))
          {
            if( inrec.dataType("cutthreshold")==TpFloat || inrec.dataType("cutthreshold")==TpDouble )
              {
                err+= readVal(inrec, String("cutthreshold"), cutThreshold );
              }
            else {
                err+= "cutthreshold must be a float or double";
            }
          }
        if( inrec.isDefined("growiterations"))
          {
            if (inrec.dataType("growiterations")==TpInt) {
                err+= readVal(inrec, String("growiterations"), growIterations );
            }
            else {
                err+= "growiterations must be an integer\n";
            }
          } 
        if( inrec.isDefined("dogrowprune"))
          {
            if (inrec.dataType("dogrowprune")==TpBool) {
                err+= readVal(inrec, String("dogrowprune"), doGrowPrune );
            }
            else {
                err+= "dogrowprune must be a bool\n";
            }
          } 
        if( inrec.isDefined("minpercentchange"))
          {
            if (inrec.dataType("minpercentchange")==TpFloat || inrec.dataType("minpercentchange")==TpDouble ) {
                err+= readVal(inrec, String("minpercentchange"), minPercentChange );
            }
            else {
                err+= "minpercentchange must be a float or double";
            }
          }
        if( inrec.isDefined("verbose")) 
          {
            if (inrec.dataType("verbose")==TpBool ) {
               err+= readVal(inrec, String("verbose"), verbose);
            }
            else {
               err+= "verbose must be a bool";
            }
          }
        if( inrec.isDefined("fastnoise"))
          {
            if (inrec.dataType("fastnoise")==TpBool ) {
               err+= readVal(inrec, String("fastnoise"), fastnoise);
            }
            else {
               err+= "fastnoise must be a bool";
            }
          }
        if( inrec.isDefined("nsigma") )
          {
            if(inrec.dataType("nsigma")==TpFloat || inrec.dataType("nsigma")==TpDouble ) {
               err+= readVal(inrec, String("nsigma"), nsigma );
              }
	    else if(inrec.dataType("nsigma")==TpInt)
	      {
		int tnsigma;
		err+= readVal(inrec, String("nsigma"), tnsigma );
		nsigma = float(tnsigma);
	      }
            else {
               err+= "nsigma must be an int, float or double";
            }
          }
        if( inrec.isDefined("restoringbeam") )     
	  {
	    String errinfo("");
	    try {
	      
	      if( inrec.dataType("restoringbeam")==TpString )     
		{
		  err += readVal( inrec, String("restoringbeam"), usebeam);
          // FIXME ! usebeam.length() == 0 is a poorly formed conditional, it
          // probably needs simplification or parenthesis, the compiler is
          // compaining about it
		  if( (! usebeam.matches("common")) && usebeam.length()!=0 )
		    {
		      Quantity bsize;
		      err += readVal( inrec, String("restoringbeam"), bsize );
		      restoringbeam.setMajorMinor( bsize, bsize );
		      usebeam = String("");
		    }
		  errinfo = usebeam;
		}
	      else if( inrec.dataType("restoringbeam")==TpArrayString )
		{
		  Vector<String> bpars;
		  err += readVal( inrec, String("restoringbeam"), bpars );

		  for (uInt i=0;i<bpars.nelements();i++) { errinfo += bpars[i] + " "; }

		  if( bpars.nelements()==1 && bpars[0].length()>0 ) { 
		    if( bpars[0]=="common") { usebeam="common"; }
		    else {
		      Quantity axis; stringToQuantity( bpars[0] , axis);
		      restoringbeam.setMajorMinor( axis, axis ); 
		    }
		  }else if( bpars.nelements()==2 ) { 
		    Quantity majaxis, minaxis;
		    stringToQuantity( bpars[0], majaxis ); stringToQuantity( bpars[1], minaxis );
		    restoringbeam.setMajorMinor( majaxis, minaxis );
		  }else if( bpars.nelements()==3 ) {
		    Quantity majaxis, minaxis, pa;
		    stringToQuantity( bpars[0], majaxis ); stringToQuantity( bpars[1], minaxis ); stringToQuantity( bpars[2], pa );
		    restoringbeam.setMajorMinor( majaxis, minaxis );
		    restoringbeam.setPA( pa );
		  }else {
		    restoringbeam = GaussianBeam();
		    usebeam = String("");
		  }
		}
	    } catch( AipsError &x) {
	      err += "Cannot construct a restoringbeam from supplied parameters " + errinfo + ". Please check that majoraxis >= minoraxis and all entries are strings.";
	      restoringbeam = GaussianBeam();
	      usebeam = String("");
	    }
	    
	  }// if isdefined(restoringbeam)

        if( inrec.isDefined("interactive") ) 
	  {    
	    if( inrec.dataType("interactive")==TpBool )     
	      {err += readVal( inrec, String("interactive"), interactive );}
	    else if ( inrec.dataType("interactive")==TpInt )
	      {Int inter=0; err += readVal( inrec, String("interactive"), inter); interactive=(Bool)inter;}
	  }

	//err += readVal( inrec, String("interactive"), interactive );
	
	err += verify();
	
      }
    catch(AipsError &x)
      {
	err = err + x.getMesg() + "\n";
      }
      
      if( err.length()>0 ) throw(AipsError("Invalid Deconvolver Parameter set : " + err));
      
  }

  String SynthesisParamsDeconv::verify() const
  {
    String err;

    if( imageName=="" ) {err += "Please supply an image name\n";}
    
    // Allow mask inputs in only one way. User specified OR already on disk. Not both
    if( maskString.length()>0 )
      {
	  File fp( imageName+".mask" );
	  if( fp.exists() ) err += "Mask image " + imageName+".mask exists, but a specific input mask of " + maskString + " has also been supplied. Please either reset mask='' to reuse the existing mask, or delete " + imageName + ".mask before restarting";
      }

    if( pbMask >= 1.0)
      {err += "pbmask must be < 1.0 \n"; }
    else if( pbMask < 0.0)
      {err += "pbmask must be a positive value \n"; }

    if(  maskType=="none" ) 
      {
        if( maskString!="" || (maskList.nelements()!=0 && maskList[0]!="") )
          {
           cerr<<"maskString="<<maskString<<endl;
           cerr<<"maskList.nelements()="<<maskList.nelements()<<" maskList[0]="<<maskList[0]<<endl;
           err += "mask is specified but usemask='none'. Please set usemask='user' to use the mask parameter\n";}
      } 
    if ( fracOfPeak >= 1.0) 
      {err += "fracofpeak must be < 1.0 \n"; }
    else if ( fracOfPeak < 0.0) 
      {err += "fracofpeak must be a positive value \n"; }
  
    return err;
  }

  void SynthesisParamsDeconv::setDefaults()
  {
    imageName="";
    algorithm="hogbom";
    startModel=Vector<String>(0);
    deconvolverId=0;
    nTaylorTerms=1;
    scales.resize(1); scales[0]=0.0;
    scalebias=0.6;
    maskType="none";
    maskString="";
    maskList.resize(1); maskList[0]="";
    pbMask=0.0;
    autoMaskAlgorithm="thresh";
    maskThreshold="";
    maskResolution="";
    fracOfPeak=0.0; 
    nMask=0;
    interactive=false;
    autoAdjust=False;
  }

  Record SynthesisParamsDeconv::toRecord() const
  {
    Record decpar;

    decpar.define("imagename", imageName);
    decpar.define("deconvolver", algorithm);
    decpar.define("startmodel",startModel);
    decpar.define("id",deconvolverId);
    decpar.define("nterms",nTaylorTerms);
    decpar.define("scales",scales);
    decpar.define("scalebias",scalebias);
    decpar.define("usemask",maskType);
    if( maskList.nelements()==1 && maskList[0]=="") 
      {
        decpar.define("mask",maskString);
      }
    else {
        decpar.define("mask",maskList);
    }
    decpar.define("pbmask",pbMask);
    if (fracOfPeak > 0.0) 
      {
        decpar.define("maskthreshold",fracOfPeak);
      }
    else 
      {
        decpar.define("maskthreshold",maskThreshold);
      }
    decpar.define("maskresolution",maskResolution);
    decpar.define("nmask",nMask);
    decpar.define("autoadjust",autoAdjust);
    decpar.define("sidelobethreshold",sidelobeThreshold);
    decpar.define("noisethreshold",noiseThreshold);
    decpar.define("lownoisethreshold",lowNoiseThreshold);
    decpar.define("negativethreshold",negativeThreshold);
    decpar.define("smoothfactor",smoothFactor);
    decpar.define("minbeamfrac",minBeamFrac);
    decpar.define("cutthreshold",cutThreshold);
    decpar.define("growiterations",growIterations);
    decpar.define("dogrowprune",doGrowPrune);
    decpar.define("minpercentchange",minPercentChange);
    decpar.define("verbose", verbose);
    decpar.define("fastnoise", fastnoise);
    decpar.define("interactive",interactive);
    decpar.define("nsigma",nsigma);

    return decpar;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////


} //# NAMESPACE CASA - END

