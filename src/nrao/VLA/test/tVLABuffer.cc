//# tVLABuffer.cc: 
//# Copyright (C) 1999,2000,2001
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

#include <casacore/casa/aips.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/iostream.h>

#include <casacore/casa/Inputs.h>
#include <casacore/casa/OS/Path.h>
//#include <nrao/VLA/VLABuffer.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/OS/SymLink.h>
#include <nrao/VLA/VLADiskInput.h>
#include <nrao/VLA/VLATapeInput.h>

#include <casacore/casa/Arrays/Vector.h>

#include <casacore/casa/namespace.h>

int main(int argc, char* argv[]) {
  try {
    /*   VLABuffer buffer; 
    {
      Input inputs(1);
      inputs.Create ("input", "/dev/tape",
		     "Input archive ie., a file/device name");
      inputs.Create ("files", "0",
		     "tape files to read, 0 -> next file");
      inputs.Create ("project", "",
		     "project name, blank -> all projects");
      inputs.ReadArguments (argc, argv);
      
      Path fileName(inputs.GetString("input"));
      String projectName(inputs.GetString("project"));
      projectName.upcase();
      AlwaysAssert(fileName.isValid(), AipsError);
      File file(fileName);
      AlwaysAssert(file.exists(), AipsError);
      if (file.isSymLink()) {
	SymLink link(file);
	fileName = link.followSymLink();
	AlwaysAssert(fileName.isValid(), AipsError);
	file = File(fileName);
	AlwaysAssert(file.exists(), AipsError);
	DebugAssert(file.isSymLink() == false, AipsError);
      }      
      if (file.isRegular()) {
 	buffer = VLABuffer(new VLADiskInput(fileName), projectName);
      } else if (file.isCharacterSpecial()) {
	Block<uInt> files;
	{
	  Block<Int> f = inputs.GetIntArray("files");
	  files.resize(f.nelements());
	  for (uInt i = 0; i < f.nelements(); i++) {
	    AlwaysAssert(f[i] >= 0, AipsError);
	    files[i] = f[i];
	  }
	}
	buffer = VLABuffer(new VLATapeInput(fileName, files), projectName);
      }
    }

    */
    /*
    uInt i = 1;
    uInt nAnt = 0;
    uInt nSpw = 0;
    uInt nSrc = 0;
    while (buffer.isValid()) {
      const uInt nRows = buffer.main().rows();
      cout << "Record " << i 
	   << " contains " << nRows << " rows of data" 
	   << " in " << buffer.main().nDataShapes() << " shape(s)" 
 	   << endl;
//       for (uInt r = 0; r < nRows; r++) {
// 	cout << "Row " << r << " has shape " 
// 	     << buffer.main().data(r).shape() << endl;
//       }
      const uInt newSpw = buffer.spectralWindow().rows();
      if (nSpw != newSpw) {
	cout << newSpw - nSpw << " new entries in the spectral window buffer"
	     << endl;
	for (uInt s = nSpw; s < newSpw; s++) {
	  cout << "Row " << s 
	       << ": nPol: " << buffer.spectralWindow().nCorrelations(s)
	       << " nChan: " << buffer.spectralWindow().nChannels(s)
	       << " Freq: " << buffer.spectralWindow().refFreq(s)
	       << " IF: " << buffer.spectralWindow().IFConversion(s)
	       << endl;
	}
	nSpw = newSpw;
      }
    */
    /*
      const uInt newAnt = buffer.antenna().rows();
      if (nAnt != newAnt) {
	cout << newAnt - nAnt << " new entries in the antenna buffer" << endl;
	for (uInt a = nAnt; a < newAnt; a++) {
	  cout << "Row " << a << ": Antenna: " << buffer.antenna().name(a)
	       << " Pad: " << buffer.antenna().station(a);
	  const uInt id = buffer.antenna().arrayId(a);
	  cout << " Sub-array: " << buffer.array().name(id)
	       << endl;
	}
	nAnt = newAnt;
      }
      const uInt newSrc = buffer.field().rows();
      if (nSrc != newSrc) {
	cout << newSrc - nSrc << " new entries in the field buffer" << endl;
	for (uInt a = nSrc; a < newSrc; a++) {
	  cout << "Row " << a 
	       << ": Field position: " << buffer.field().refDir(a);
	  Int sourceId = buffer.field().sourceId(a);
	  cout << " Source name: " << buffer.source().name(sourceId)
	       << endl;
       
	}
	nSrc = newSrc;
      }
//       const uInt nAnt = 27;
//       const uInt nChan = numChannels();
//       cout << "There are " << nAnt << " antennas and " 
// 	   << nChan << " channels in CDA " << i << endl;
// 	msc.antenna1().put(row, buffer.main().antenna1(row));
// 	msc.antenna1().putColumn(buffer.main().antenna1());
// 	msc.antenna1().putColumnRange(slicer, buffer.main().antenna1());
// 	msc.antenna1().putColumnCells(RefRow(start,end),
// 				      buffer.main().antenna1());
//       } else {
//  	cout << "Record " << i << " is invalid" << endl;
      i++; buffer++;
    }
    */
  }
  catch (AipsError x) {
    cerr << x.getMesg() << endl;
    cout << "FAIL" << endl;
    return 1;
  } 
  cout << "OK" << endl;
}

// Local Variables: 
// compile-command: "gmake OPTLIB=1 tVLABuffer"
// End: 
