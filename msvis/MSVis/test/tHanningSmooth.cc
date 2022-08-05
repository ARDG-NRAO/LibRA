//# tHanningSmooth.cc: Tests Hanning smoothing of spectral channels
//# Copyright (C) 1995,1999,2000,2001
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

#include <casacore/casa/aips.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/iostream.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/iomanip.h>
#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/EnvVar.h>

using namespace std;
using namespace casa;

int main(int argc, char **argv) {

  try {
    std::string input_ms;
    if (argc < 2)
    {
      String casadata = EnvironmentVariable::get("CASADATA");
      if (casadata.empty()) {
          throw(AipsError("CASADATA env variable not defined and no file given in command line"));
      }
      input_ms = casadata + "/unittest/hanningsmooth/ngc5921_ut.ms/";
    }
    else
    {
      input_ms = argv[1];
    }

    MS ms(input_ms, Table::Update);

    Block<int> sort(4);
    sort[2] = MS::FIELD_ID;
    sort[3] = MS::ARRAY_ID;
    sort[1] = MS::DATA_DESC_ID;
    sort[0] = MS::TIME;
    Matrix<Int> allChannels;

    VisSet vs(ms,sort,allChannels, 0.0);
    VisIter& vi(vs.iter());
    VisBuffer vb(vi);

    Int row, chn, pol;

    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {

	Cube<Complex>& vc= vb.correctedVisCube();
	Cube<Bool>& fc= vb.flagCube();

	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
        Int nPol=vi.visibilityShape()(0);
	Cube<Complex> smoothedData(nPol,nChan,nRow);
	//smoothedData.reference(vb.correctedVisCube());
	Cube<Bool> newFlag(nPol,nChan,nRow);
	//newFlag.reference(vb.flagCube());
	for (row=0; row<nRow; row++) {
	  for (pol=0; pol<nPol; pol++) {
	    smoothedData(pol,0,row) = vc(pol,0,row); // not really necessary
	    newFlag(pol,0,row) = true;    // since we must flag first channel
	    for (chn=1; chn<nChan-1; chn++) {
	      smoothedData(pol,chn,row) =
		vc(pol,chn-1,row)*0.25 + vc(pol,chn,row)*0.50 +
		vc(pol,chn+1,row)*0.25;
	      newFlag(pol,chn,row) =
		fc(pol,chn-1,row)||fc(pol,chn,row)||fc(pol,chn+1,row);
	    }
	    smoothedData(pol,nChan-1,row) = vc(pol,nChan-1,row);
	    newFlag(pol,nChan-1,row) = true;  // flag last channel
	  }
	}
	vi.setVisAndFlag(smoothedData,newFlag,VisibilityIterator::Corrected);
      }
    }
  }
  catch (const AipsError &x) {
    cerr << "***Exception:" << endl;
    cerr << x.getMesg() << endl;
    return 1;
  }
  
  return 0;
}

