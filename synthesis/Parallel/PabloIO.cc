//# PabloIO.cc:  this defines ClassName, which ...
//# Copyright (C) 1999,2000,2001,2002,2003
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

#ifdef PABLO_IO

//# Includes
#include <synthesis/Parallel/PabloIO.h>
#include <casacore/casa/OS/EnvVar.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Utilities/Regex.h>
#include "IOTrace.h"
#include "PabloTrace.h"
#include <sstream>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// Extern C programs from the Pablo library.

extern "C" Int setTraceFileName(char *);
extern "C" Int endTracing(void);

void PabloIO::init(Int argc, Char **argv, Int tracenode){
  // We set the name of the trace file here.  Typically, you want the
  // file to be written to a local disk, and if possible to a file to.
  // Also, make sure the location you select has a lot of free space.
  // For multiprocessor applications, set 'tracenode' to the
  // appropriate node for each processor (for example, using the MPI
  // rank) - here we just set it to 0.
  //
  // OK there are two environment variables that drive us.  First
  // PABLOSTATS is a colon(:) seperated list (need those leading and
  // trailing :'s) which tell us what processes to trace.  i.e.
  // PABLOSTATS = ':imager:quanta:calibrater:' or PABLOSTATS = 'all'
  // to get them all Second PABLOSTATSDIR is the directory where the
  // stats files should go the default is /var/tmp be careful...
  //
  Path myname(argv[0]);
  if(EnvironmentVariable::isDefined(String("PABLOSTATS"))){
     String pablostats(EnvironmentVariable::get(String("PABLOSTATS")));
     Regex lookfor(String("\\:")+myname.baseName()+String("\\:"));
     if(pablostats.contains(lookfor) || pablostats == String("all")){
        String pablostatsdir("/var/tmp");
        if(EnvironmentVariable::isDefined(String("PABLOSTATSDIR"))){
           pablostatsdir = EnvironmentVariable::get("PABLOSTATSDIR");
        }

        // Set the tracenode file name, typically tracenode is the cpu.
	// Unfortunately, setTraceFileName() wants a Char*, so do a cast.
	ostringstream oss;
	oss << pablostatsdir << "/" << myname.baseName() << tracenode
	    << ".PabloIO";
	setTraceFileName( const_cast<char*>(oss.str().c_str()) );

        initIOTrace();
        traceEvent(1,"Starting instrumentation",24);
     }
  }
}

void PabloIO::terminate(){
    traceEvent(1,"Ending instrumentation",24);
    endIOTrace();
    endTracing();
}


} //# NAMESPACE CASA - END

#endif

