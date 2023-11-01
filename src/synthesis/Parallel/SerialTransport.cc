//# ClassFileName.cc:  this defines ClassName, which ...
//# Copyright (C) 1998,1999,2000
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

//# Includes

#include <synthesis/Parallel/SerialTransport.h>
#include <casacore/casa/Containers/Record.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  // OK this is all pretty straight forward.  Just assign the pointers.  
  // On the gets check it's nonzero first.  Probably should throw an 
  // exception.

SerialTransport::SerialTransport(): PTransport(), inQue(0), outQue(0), lastInQue(0)
{
    if (debug_p) {
        cerr << "constructing SerialTransport" << std::endl;
    }
    _data.resize(20);
}

Int SerialTransport::add2Queue(void *item){
   if (debug_p) {
       cerr << ">add2Queue: " << item << ", " << inQue << ", " << outQue << ", " <<
           lastInQue << ", " << _data.nelements() << "\n";
   }

   _data[inQue] = item;
   ++inQue;
   // This was here from ancient times. It looks wrong. Applicator/algorithm for example
   // won't work at all.
   // outQue = 0;  // eh? only strict put... get sequence supported?

   if(inQue >= _data.nelements())
      _data.resize(inQue*2, false, true);
   lastInQue = inQue;


   return(0);
}
void *SerialTransport::getFromQueue(){
   void *ptr2data(nullptr);
   if (debug_p) {
       cerr << ">getFromQueue: " << _data[outQue] << ", " << inQue << ", " <<
           outQue << ", " << lastInQue << "\n";
   }
   if(outQue < lastInQue){
      ptr2data = _data[outQue];
      ++outQue;
   }
   else {
       cerr << "**** inconsistency in SerialTransport::getFromQueue" << std::endl;
   }
   return(ptr2data);
}

Int SerialTransport::put(const Array<Int> &af){
  //cerr << "put array " << Vector<Int>(af) << endl;
   return add2Queue((void *)&af);
}
Int SerialTransport::put(const Array<Float> &af){
   return add2Queue((void *)&af);
}
Int SerialTransport::put(const Array<Double> &af){
   return add2Queue((void *)&af);
}

Int SerialTransport::put(const Array<Complex> &af){
   return add2Queue((void *)&af);
}

Int SerialTransport::put(const Array<DComplex> &af){
   return add2Queue((void *)&af);
}


Int SerialTransport::put(const Float &f){
   return add2Queue((void *)&f);
}
Int SerialTransport::put(const DComplex &f){
   return add2Queue((void *)&f);
}
Int SerialTransport::put(const Complex &f){
   return add2Queue((void *)&f);
}

Int SerialTransport::put(const Double &d){
   return add2Queue((void *)&d);
}
Int SerialTransport::put(const Int &i){
   return add2Queue((void *)&i);
}
Int SerialTransport::put(const String &s){
   return add2Queue((void *)&s);
}
Int SerialTransport::put(const Bool &b){
   return add2Queue((void *)&b);
}
Int SerialTransport::put(const Record &r){
   return add2Queue((void *)&r);
}


Int SerialTransport::get(Array<Float> &af){
   Int r_status(1);
   af = *((Array<Float> *)getFromQueue());
   
   return(r_status);
}

Int SerialTransport::get(Array<Double> &af){
   Int r_status(1);
   af = *((Array<Double> *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(Array<Complex> &af){
   Int r_status(1);
   af = *((Array<Complex> *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(Array<DComplex> &af){
   Int r_status(1);
   af = *((Array<DComplex> *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(Array<Int> &af){
   Int r_status(1);
   af = *((Array<Int> *)getFromQueue());
   //cerr << "get array " << Vector<Int>(af) << endl;
   return(r_status);
}

Int SerialTransport::get(Complex &f){
   Int r_status(1);
   f = *((Complex *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(DComplex &f){
   Int r_status(1);
   f = *((DComplex *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(Float &f){
   Int r_status(1);
   f = *((Float *)getFromQueue());
   return(r_status);
}
Int SerialTransport::get(Double &d){
   Int r_status(1);
   d = *((Double *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(Int &i){
   Int r_status(0);
   i = *((Int *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(String &s){
   Int r_status(1);
   String * val = (String *)getFromQueue();
   if (val)
       s = *val;
   return(r_status);
}

Int SerialTransport::get(Record &r){
   Int r_status(1);
   r = *((Record *)getFromQueue());
   return(r_status);
}

Int SerialTransport::get(Bool &b){
   Int r_status(1);
   b = *((Bool *)getFromQueue());
   return(r_status);
}

} //# NAMESPACE CASA - END

