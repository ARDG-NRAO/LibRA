//# tVisModelData.cc: Tests the Synthesis model data serving
//# Copyright (C) 2011
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

#include <casacore/casa/Arrays/ArrayMath.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/Flux.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/ms/MSSel/MSSelection.h>

#include <synthesis/TransformMachines2/VisModelData.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/GridFT.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Utilities/Regex.h>

using namespace std;
using namespace casa;
using namespace casacore;

int
main(int argc, char **argv){


  if (argc<2) {
    cout <<"Usage: tVisModelData ms-table-name "<<endl;
    exit(1);
  }
  try{
    
    MeasurementSet myms(argv[1],Table::Update);

    String spwsel=String("*");
    MSSelection selector;
    //selector.setFieldExpr(MSSelection::indexExprStr(field));
    selector.setSpwExpr(spwsel);
    TableExprNode exprNode=selector.toTableExprNode(&myms);
    cerr << "is expr null " << exprNode.isNull() << endl;
    MeasurementSet mssel(myms(exprNode));
    

    Block<Int> sort(0); // create empty block with sortColumns
    //VisibilityIterator vi(mssel,sort);
    vi::VisibilityIterator2 vi(mssel,vi::SortColumns(),false);
    vi::VisBuffer2 *vb=vi.getVisBuffer();

    
   
    // tm.mark();
    vi.originChunks();
    Float meanval=0;
    for (vi.originChunks();vi.moreChunks(); vi.nextChunk()){
      for (vi.origin(); vi.more(); vi.next()){
	
	//meanval+=mean(vb.visCube());
	//vm.getModelVis(vb);
	cerr << "field " << vb->fieldId()(0) << "  spw " << vb->spectralWindows()(0) << " stddev " << stddev(vb->visCubeModel()) << "   mean " << mean(vb->visCubeModel()) <<" max " <<  max(amplitude(vb->visCubeModel())) << "  min " << min(vb->visCubeModel()) << endl;
	//	cerr << vb->visCubeModel().xyPlane(40) << endl;
	//vb.visCube();
      }
    }
    //cerr << "meanval of data "<< meanval << data
    //tm.show("Time to read data");
    //VisModelData::clearModel(myms);


    } catch (AipsError x) {
    cout << "Caught exception " << endl;
    cout << x.getMesg() << endl;
    return(1);
  }














  cout << "Done" << endl;
  exit(0);

}
