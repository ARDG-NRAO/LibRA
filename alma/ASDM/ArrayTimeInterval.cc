#include <alma/ASDM/ArrayTimeInterval.h>

using namespace std;

namespace asdm {
  
  bool ArrayTimeInterval::readStartTimeDurationInBin_ = false;
  bool ArrayTimeInterval::readStartTimeDurationInXML_ = false;

#ifndef WITHOUT_ACS
  /**
   * Create a ArrayTimeInterval from an IDLArrayTimeInterval object.
   * @param t The IDL ArrayTimeInterval object.
   */
  ArrayTimeInterval::ArrayTimeInterval (asdmIDLTypes::IDLArrayTimeInterval t) {
    start = ArrayTime(t.start);
    duration = Interval(t.duration); 
  }
  
  // inline to IDL conversion.
  const asdmIDLTypes::IDLArrayTimeInterval ArrayTimeInterval::toIDLArrayTimeInterval()const {
    asdmIDLTypes::IDLArrayTimeInterval x;
    x.start = start.get();
    x.duration = duration.get();
    return x;
  }
#endif

  void ArrayTimeInterval::toBin(EndianOSStream& eoss) const {
    int64_t start = getStart().get();
    int64_t midpoint = start + getDuration().get() / 2;
    eoss.writeLongLong(midpoint);
    eoss.writeLongLong(duration.get());
  }

  void ArrayTimeInterval::toBin(const vector<ArrayTimeInterval>& arrayTimeInterval,  EndianOSStream& eoss) {
    eoss.writeInt((int) arrayTimeInterval.size());
    for (unsigned int i = 0; i < arrayTimeInterval.size(); i++) {
      arrayTimeInterval.at(i).toBin(eoss);
    }
  }

  void ArrayTimeInterval::toBin(const vector<vector<ArrayTimeInterval> >& arrayTimeInterval,  EndianOSStream& eoss) {
    eoss.writeInt((int) arrayTimeInterval.size());
    eoss.writeInt((int) arrayTimeInterval.at(0).size());
    for (unsigned int i = 0; i < arrayTimeInterval.size(); i++)
      for (unsigned int j = 0; j < arrayTimeInterval.at(0).size(); j++) {
	arrayTimeInterval.at(i).at(j).toBin(eoss);
      }
  }

  void ArrayTimeInterval::toBin(const vector< vector<vector<ArrayTimeInterval> > >& arrayTimeInterval,  EndianOSStream& eoss) {
    eoss.writeInt((int) arrayTimeInterval.size());
    eoss.writeInt((int) arrayTimeInterval.at(0).size());
    eoss.writeInt((int) arrayTimeInterval.at(0).at(0).size());	
    for (unsigned int i = 0; i < arrayTimeInterval.size(); i++)
      for (unsigned int j = 0; j < arrayTimeInterval.at(0).size(); j++)
	for (unsigned int k = 0; k < arrayTimeInterval.at(0).at(0).size(); j++) {
	  arrayTimeInterval.at(i).at(j).at(k).toBin(eoss);
	}
  }

  void ArrayTimeInterval::readStartTimeDurationInBin(bool b) {
    readStartTimeDurationInBin_ = b;
  }

  bool ArrayTimeInterval::readStartTimeDurationInBin() {
    return readStartTimeDurationInBin_;
  }

  void ArrayTimeInterval::readStartTimeDurationInXML(bool b) {
    readStartTimeDurationInXML_ = b;
  }

  bool ArrayTimeInterval::readStartTimeDurationInXML() {
    return readStartTimeDurationInXML_;
  }


  ArrayTimeInterval ArrayTimeInterval::fromBin(EndianIStream & eis) {
    int64_t start, duration;
    if (readStartTimeDurationInBin_) {
      start = eis.readLongLong();
      duration = eis.readLongLong();
    }
    else {
      int64_t  midpoint = eis.readLongLong();
      duration = eis.readLongLong();
      start = midpoint - duration / 2;
    }      
    return ArrayTimeInterval(start, duration);
  }

  vector<ArrayTimeInterval> ArrayTimeInterval::from1DBin(EndianIStream & eis) {
    int dim1 = eis.readInt();
    vector<ArrayTimeInterval> result;
    for (int i = 0; i < dim1; i++) {
      result.push_back(fromBin(eis));
    }
    return result;	
  }

  vector<vector<ArrayTimeInterval > > ArrayTimeInterval::from2DBin(EndianIStream & eis) {
    int dim1 = eis.readInt();
    int dim2 = eis.readInt();
    vector< vector<ArrayTimeInterval> >result;
    vector <ArrayTimeInterval> aux;
    for (int i = 0; i < dim1; i++) {
      aux.clear();
      for (int j = 0; j < dim2; j++) {
	aux.push_back(fromBin(eis));
      }
      result.push_back(aux);
    }
    return result;	
  }

  vector<vector<vector<ArrayTimeInterval > > > ArrayTimeInterval::from3DBin(EndianIStream & eis) {
    int dim1 = eis.readInt();
    int dim2 = eis.readInt();
    int dim3 = eis.readInt();
    vector<vector< vector<ArrayTimeInterval> > >result;
    vector < vector<ArrayTimeInterval> >aux1;
    vector <ArrayTimeInterval> aux2;
    for (int i = 0; i < dim1; i++) {
      aux1.clear();
      for (int j = 0; j < dim2; j++) {
	aux2.clear();
	for (int k = 0; k < dim3; k++) {
	  aux2.push_back(fromBin(eis));
	}
	aux1.push_back(aux2);
      }
      result.push_back(aux1);
    }
    return result;	
  }

  string ArrayTimeInterval::toString() const {
    ostringstream oss;
    oss << "start=" << start.toFITS() << ",duration=" << duration / 1.e9 << "s." ;
    return oss.str();
  }
} // end name space
