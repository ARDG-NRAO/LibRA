#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/lattices/Lattices/PagedArray.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/LEL/LatticeExprNode.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <sstream>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <synthesis/MeasurementComponents/WriteMSAlgorithm.h>
#include <casacore/casa/OS/Time.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  extern Applicator applicator;
WriteMSAlgorithm::WriteMSAlgorithm() : myName("Test Parallel Write")
{
  
};
WriteMSAlgorithm::~WriteMSAlgorithm(){

};

void WriteMSAlgorithm::get(){

  applicator.get(msName);
  Vector<Int> tempIds;
  applicator.get(tempIds);
  rowids.resize(tempIds.shape());
  convertArray(rowids, tempIds);
  applicator.get(somenumber);
};

void WriteMSAlgorithm::put(){
  ///Looks like there is a bug in Serial transport where the
  //Applicator::done() signal seems to be lost
  //Manually force sending it here.
  if(applicator.isSerial()){
    doneSig_p=Applicator::DONE;
    applicator.put(doneSig_p);
  }
  //applicator.put(status+10);
  status=10012;
  cerr << "in writems put " << status << endl;
  applicator.put(status);

};

String& WriteMSAlgorithm::name(){

return myName;
};

void WriteMSAlgorithm::task(){

status = 0; 
Time starttime;

cout <<" In task() " << endl;  

try{
  //open whole ms
    MeasurementSet ms(msName, TableLock(TableLock::UserNoReadLocking), Table::Update);
    ///select on rowids
    cerr << "rowids " << rowids << endl;
    MeasurementSet mssel=ms(RowNumbers(rowids));
    mssel.lock();
    vi::VisibilityIterator2 vi2(mssel, vi::SortColumns(), true);
    
    vi2.originChunks();
    vi2.origin();

    vi::VisBuffer2* vb = vi2.getVisBuffer();




    for (vi2.originChunks();vi2.moreChunks();vi2.nextChunk()) {
      for (vi2.origin();vi2.more();vi2.next()) {
        Cube<Complex> mod(vb->nCorrelations(), vb->nChannels(), vb->nRows(), Complex(somenumber));
        vb->setVisCubeModel(mod);
	vi2.writeVisModel(vb->visCubeModel());
      }

    }

    mssel.unlock();

    status=1;


}catch (AipsError x) {
    cout <<  "Exceptionally yours: " << x.getMesg() << endl;
 } 

cout << "seconds elapsed since start: " << starttime.age () << endl;
};





} //# NAMESPACE CASA - END

