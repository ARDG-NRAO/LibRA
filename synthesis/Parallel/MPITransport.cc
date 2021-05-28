//# MPITransport.cc: class which define an MPI parallel transport layer
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

#ifdef HAVE_MPI

#include <casa/Containers/Record.h>
#include <casa/IO/AipsIO.h>
#include <casa/IO/MemoryIO.h>

#include <synthesis/Parallel/MPITransport.h>
#include <synthesis/Parallel/MPIError.h>
#include <synthesis/Parallel/Algorithm.h>

#include <mpi.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

MPITransport::MPITransport() : PTransport()
{
// Default constructor
//
  // Set default tag and source/destination
  setAnyTag();
  connectAnySource();
};

MPITransport::MPITransport(Int argc, Char *argv[]) : PTransport()
{
   if (debug_p) {
       cerr << "constructing MPITransport" << std::endl;
   }
// Construct from argc, argv
//
   int flag=0;
   MPI_Initialized(&flag);
   //cerr << "FLAG " << flag << endl;
   if((flag && isController()) || MPI_Init(&argc, &argv) == MPI_SUCCESS){
     //if(MPI_Init(&argc, &argv) == MPI_SUCCESS){
      MPI_Comm_rank(MPI_COMM_WORLD, &myCpu);
      MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
      // Set default tag and source/destination
      setAnyTag();
      connectAnySource();
   } else {
     throw MPIError("MPI Init failed!");
   }
}

MPITransport::~MPITransport(){
  if(!isFinalized())
    MPI_Finalize();
}

Bool MPITransport::isFinalized()
{
    int flag;
    MPI_Finalized(&flag);
    return Bool(flag);
}

Int MPITransport::anyTag() 
{
// Return the value which indicates an unset tag
//
   return MPI_ANY_TAG;
};

Int MPITransport::anySource()
{
// Return the value which indicates an unset source
//
   return MPI_ANY_SOURCE;
};

// produce a contiguous vector of per-dimension sizes from a shape/Iposition
const std::vector<uInt> makeContiguousSizes(uInt ndim, const IPosition &ipos) {
    std::vector<uInt> sizes(ndim);
    for (uInt idx=0; idx<ndim; ++idx) {
        sizes[idx] = ipos[idx];
    }
    return sizes;
}

Int MPITransport::put(const Array<Float> &af){
   uInt ndim(af.ndim());
   setDestAndTag(sendTo, myOp);
   IPosition ipos = af.shape();
      // Send the number of dimensions
   MPI_Send((void *)&ndim, 1, MPI_UNSIGNED, sendTo, myOp, MPI_COMM_WORLD);
      // Send the shape vector
   // Don't feel tempted to send directly (void *)ipos.storage(). That doesn't give you
   // a contiguous sequence of size integers. You can get for example integers:
   // dim1, 0, dim2, 0, uninit, ...
   // etc.
   // Dangerous: MPI_Send((void *)ipos.storage(), ndim,  MPI_INT, sendTo, myOp,
   const auto &sizes = makeContiguousSizes(ndim, ipos);
   MPI_Send((void *)sizes.data(), ndim,  MPI_INT, sendTo, myOp,
            MPI_COMM_WORLD);
      // Send the data
   Bool deleteit;
   MPI_Send((void *)af.getStorage(deleteit), af.nelements(),  MPI_FLOAT,
             sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}

Int MPITransport::put(const Array<Double> &af){
   uInt ndim(af.ndim());
   const IPosition ipos = af.shape();
   setDestAndTag(sendTo, myOp);
   MPI_Send((void *)&ndim, 1, MPI_UNSIGNED, sendTo, myOp, MPI_COMM_WORLD);
      // Send the shape vector
   const auto &sizes = makeContiguousSizes(ndim, ipos);
   MPI_Send((const void *)sizes.data(), ndim,  MPI_INT, sendTo, myOp,
            MPI_COMM_WORLD);
      // Send the data
   Bool deleteit;
   MPI_Send((void *)af.getStorage(deleteit), af.nelements(),  MPI_DOUBLE,
             sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}

Int MPITransport::put(const Array<Int> &af){
   uInt ndim(af.ndim());
   IPosition ipos = af.shape();
   setDestAndTag(sendTo, myOp);
   MPI_Send((void *)&ndim, 1, MPI_UNSIGNED, sendTo, myOp, MPI_COMM_WORLD);
      // Send the shape vector
   const auto &sizes = makeContiguousSizes(ndim, ipos);
   MPI_Send((void *)sizes.data(), ndim,  MPI_INT, sendTo, myOp,
            MPI_COMM_WORLD);
      // Send the data
   Bool deleteit;
   MPI_Send((void *)af.getStorage(deleteit), af.nelements(),  MPI_INT,
             sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}

Int MPITransport::put(const Array<Complex> &af){
   uInt ndim(af.ndim());
   setDestAndTag(sendTo, myOp);
   IPosition ipos = af.shape();
      // Send the number of dimensions
   MPI_Send((void *)&ndim, 1, MPI_UNSIGNED, sendTo, myOp, MPI_COMM_WORLD);
      // Send the shape vector
   const auto &sizes = makeContiguousSizes(ndim, ipos);
   MPI_Send((const void *)sizes.data(), ndim,  MPI_INT, sendTo, myOp,
            MPI_COMM_WORLD);
      // Send the data
   Bool deleteit;
   MPI_Send((void *)af.getStorage(deleteit), 2*af.nelements(),  MPI_FLOAT,
             sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}

Int MPITransport::put(const Array<DComplex> &af){
   uInt ndim(af.ndim());
   setDestAndTag(sendTo, myOp);
   IPosition ipos = af.shape();
      // Send the number of dimensions
   MPI_Send((void *)&ndim, 1, MPI_UNSIGNED, sendTo, myOp, MPI_COMM_WORLD);
      // Send the shape vector
   const auto &sizes = makeContiguousSizes(ndim, ipos);
   MPI_Send((void *)sizes.data(), ndim,  MPI_INT, sendTo, myOp,
            MPI_COMM_WORLD);
      // Send the data
   Bool deleteit;
   MPI_Send((void *)af.getStorage(deleteit), 2*af.nelements(),  MPI_DOUBLE,
             sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}

Int MPITransport::put(const Float &f){
   setDestAndTag(sendTo, myOp);
   MPI_Send((void *)&f, 1, MPI_FLOAT, sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}
Int MPITransport::put(const Complex &f){
   setDestAndTag(sendTo, myOp);
   MPI_Send((void *)&f, 2, MPI_FLOAT, sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}
Int MPITransport::put(const DComplex &f){
   setDestAndTag(sendTo, myOp);
   MPI_Send((void *)&f, 2, MPI_DOUBLE, sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}

Int MPITransport::put(const Double &d){
   setDestAndTag(sendTo, myOp);
   MPI_Send((void *)&d, 1, MPI_DOUBLE, sendTo, myOp, MPI_COMM_WORLD);
   return(0);
}
Int MPITransport::put(const Int &i){
   setDestAndTag(sendTo, myOp);
   // warning: sstat set but not used!
   Int sstat = MPI_Send((void *)&i, 1, MPI_INT, sendTo, myOp, MPI_COMM_WORLD);
   (void) sstat; // warning: unused sstat
   return(0);
}
Int MPITransport::put(const Bool &b){
   setDestAndTag(sendTo, myOp);
   Int i(b);
   // warning: sstat set but not used!
   Int sstat = MPI_Send((void *)&i, 1, MPI_INT, sendTo, myOp, MPI_COMM_WORLD);
   (void) sstat; // warning: unused sstat
   return(0);
}

Int MPITransport::put(const Record &r){
   setDestAndTag(sendTo, myOp);
   MemoryIO buffer;
   AipsIO rBuf(&buffer);
   rBuf.putstart("MPIRecord",1);
   rBuf << r;
   rBuf.putend();
    uInt bytes2send=rBuf.getpos();
   // warning: sstat set but not used!
   Int sstat = MPI_Send((void *)&bytes2send, 1, MPI_UNSIGNED, sendTo, myOp, MPI_COMM_WORLD);
   sstat = MPI_Send((void *)buffer.getBuffer(), bytes2send, MPI_UNSIGNED_CHAR, sendTo, myOp,
                    MPI_COMM_WORLD);
   (void) sstat; // warning: unused sstat
   return(0);
}

Int MPITransport::put(const String &s){
   uInt ndim(s.length());
   setDestAndTag(sendTo, myOp);
      // Send the length of the string
   MPI_Send((void *)&ndim, 1, MPI_UNSIGNED, sendTo, myOp, MPI_COMM_WORLD);
      // Send the characters
   MPI_Send((void *)s.chars(), ndim,  MPI_CHAR, sendTo, myOp,
            MPI_COMM_WORLD);
   return(0);
}

Int MPITransport::get(Array<Float> &af){
      // Get the number of dimensions
   setSourceAndTag(getFrom, myOp);
   MPI_Status status;
   uInt ndim;
   MPI_Recv(&ndim, 1, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the shape vector
   aTag = myOp = status.MPI_TAG;
   aWorker = getFrom = status.MPI_SOURCE;
   std::vector<uInt> ashape(ndim);
   MPI_Recv(ashape.data(), ndim, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the data
   Int nelements(1);
   {
   for(uInt i=0;i<ndim;i++){
      nelements *= ashape[i];
   }
   }
   Float *data = new Float[nelements];
   MPI_Recv(data, nelements, MPI_FLOAT, getFrom, myOp, MPI_COMM_WORLD,
            &status);
   IPosition ipos(ndim, ndim);
   for(uInt i=0;i<ndim;i++)
      ipos(i) = ashape[i];
   af.takeStorage(ipos, data, TAKE_OVER);

   return(status.MPI_SOURCE);
}

Int MPITransport::get(Array<Double> &af){
      // Get the number of dimensions
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   uInt ndim;
   MPI_Recv(&ndim, 1, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the shape vector
   aTag = myOp = status.MPI_TAG;
   aWorker = getFrom = status.MPI_SOURCE;
   std::vector<uInt> ashape(ndim);
   MPI_Recv(ashape.data(), ndim, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the data
   Int nelements(1);
   {
   for(uInt i=0;i<ndim;i++){
      nelements *= ashape[i];
   }
   }
   Double *data = new Double[nelements];
   MPI_Recv(data, nelements, MPI_DOUBLE, getFrom, myOp, MPI_COMM_WORLD,
            &status);
   IPosition ipos(ndim, ndim);
   for(uInt i=0;i<ndim;i++)
      ipos(i) = ashape[i];
   af.takeStorage(ipos, data, TAKE_OVER);

   return(status.MPI_SOURCE);
}

Int MPITransport::get(Array<Complex> &af){
      // Get the number of dimensions
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   uInt ndim;
   MPI_Recv(&ndim, 1, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the shape vector
   aTag = myOp = status.MPI_TAG;
   aWorker = getFrom = status.MPI_SOURCE;
   std::vector<uInt> ashape(ndim);
   MPI_Recv(ashape.data(), ndim, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the data
   Int nelements(1);
   {
   for(uInt i=0;i<ndim;i++){
      nelements *= ashape[i];
   }
   }
   Complex *data = new Complex[nelements];
   MPI_Recv(data, 2*nelements, MPI_FLOAT, getFrom, myOp, MPI_COMM_WORLD,
            &status);
   IPosition ipos(ndim, ndim);
   for(uInt i=0;i<ndim;i++)
      ipos(i) = ashape[i];
   af.takeStorage(ipos, data, TAKE_OVER);

   return(status.MPI_SOURCE);
}

Int MPITransport::get(Array<DComplex> &af){
      // Get the number of dimensions
   MPI_Status status;
   uInt ndim;
   setSourceAndTag(getFrom, myOp);
   MPI_Recv(&ndim, 1, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the shape vector
   aTag = myOp = status.MPI_TAG;
   aWorker = getFrom = status.MPI_SOURCE;
   std::vector<uInt> ashape(ndim);
   MPI_Recv(ashape.data(), ndim, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the data
   Int nelements(1);
   {
   for(uInt i=0;i<ndim;i++){
      nelements *= ashape[i];
   }
   }
   DComplex *data = new DComplex[nelements];
   MPI_Recv(data, 2*nelements, MPI_DOUBLE, getFrom, myOp, MPI_COMM_WORLD,
            &status);
   IPosition ipos(ndim, ndim);
   for(uInt i=0;i<ndim;i++)
      ipos(i) = ashape[i];
   af.takeStorage(ipos, data, TAKE_OVER);

   return(status.MPI_SOURCE);
}

Int MPITransport::get(Array<Int> &af){
      // Get the number of dimensions
   MPI_Status status;
   uInt ndim;
   setSourceAndTag(getFrom, myOp);
   MPI_Recv(&ndim, 1, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the shape vector
   aTag = myOp = status.MPI_TAG;
   aWorker = getFrom = status.MPI_SOURCE;
   std::vector<uInt> ashape(ndim);
   MPI_Recv(ashape.data(), ndim, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
      // Get the data
   Int nelements(1);
   {
   for(uInt i=0;i<ndim;i++){
      nelements *= ashape[i];
   }
   }
   Int *data = new Int[nelements];
   MPI_Recv(data, nelements, MPI_INT, getFrom, myOp, MPI_COMM_WORLD,
            &status);
   IPosition ipos(ndim, ndim);
   for(uInt i=0;i<ndim;i++)
      ipos(i) = ashape[i];
   af.takeStorage(ipos, data, TAKE_OVER);

   return(status.MPI_SOURCE);
}

Int MPITransport::get(Float &f){
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   MPI_Recv(&f, 1, MPI_FLOAT, getFrom, myOp, MPI_COMM_WORLD, &status);
   return(status.MPI_SOURCE);
}

Int MPITransport::get(Double &d){
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   MPI_Recv(&d, 1, MPI_DOUBLE, getFrom, myOp, MPI_COMM_WORLD, &status);
   return(status.MPI_SOURCE);
}

Int MPITransport::get(Complex &f){
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   MPI_Recv(&f, 2, MPI_FLOAT, getFrom, myOp, MPI_COMM_WORLD, &status);
   return(status.MPI_SOURCE);
}

Int MPITransport::get(DComplex &d){
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   MPI_Recv(&d, 2, MPI_DOUBLE, getFrom, myOp, MPI_COMM_WORLD, &status);
   return(status.MPI_SOURCE);
}

Int MPITransport::get(Int &i){
   Int r_status(1);
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   r_status = MPI_Recv(&i, 1, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
   (void) r_status; // warning: unused r_status
   return(status.MPI_SOURCE);
}

Int MPITransport::get(Bool &b){
   Int r_status(1);
   MPI_Status status;
   setSourceAndTag(getFrom, myOp);
   Int i;
   r_status = MPI_Recv(&i, 1, MPI_INT, getFrom, myOp, MPI_COMM_WORLD, &status);
   (void) r_status; // warning: unused r_status
   if(i == 0)
      b = false;
   else
      b = true;
   return(status.MPI_SOURCE);
}

Int MPITransport::get(Record &r){
   MPI_Status status;
   (void) status; // warning: unused status
   setSourceAndTag(getFrom, myOp);
   // Get the size of the record in bytes
   uInt bytesSent;
   MPI_Recv(&bytesSent, 1, MPI_UNSIGNED, getFrom, myOp, MPI_COMM_WORLD, &status);
   // Now fill the buffer full of bytes from the record
   uChar buffer[bytesSent];
   MPI_Recv(buffer, bytesSent, MPI_UNSIGNED_CHAR, getFrom, myOp, MPI_COMM_WORLD, &status);
   MemoryIO nBuf(&buffer, bytesSent);
   AipsIO rBuf(&nBuf);
   uInt version = rBuf.getstart("MPIRecord");
   (void)version; // warning: unused version
   rBuf >> r;
   rBuf.getend();
   return(0);
}

Int MPITransport::get(String &s){
   MPI_Status status;
   (void) status; // warning: unused status
   setSourceAndTag(getFrom, myOp);
      // Get the length of the string
   uInt i;
   MPI_Recv(&i, 1, MPI_UNSIGNED, getFrom, myOp, MPI_COMM_WORLD, &status);
   // Send the characters
   Char *theChars = new Char[i+1];
   MPI_Recv(theChars, i, MPI_CHAR, getFrom, myOp, MPI_COMM_WORLD, &status);

   *(theChars+i) = 0;
   s = theChars;
   delete [] theChars;
   return(status.MPI_SOURCE);
}

void MPITransport::setSourceAndTag(Int &source, Int &tag)
{
// Set source and tag for MPI_Recv commands
//
  // Message tag
  tag = aTag;

  // Source
  if (isController()) {
    source = aWorker;
  } else {
    source = controllerRank();
  };
  return;
};

void MPITransport::setDestAndTag(Int &dest, Int &tag)
{
// Set destination and tag for MPI_Send commands
//
  // Message tag
  if (aTag == anyTag()) {
    throw(AipsError("No tag set for MPI send command"));
  } else {
    tag = aTag;
  };

  // Destination
  if (isController()) {
    if (aWorker != anySource()) {
      dest = aWorker;
    } else {
      throw(AipsError("Invalid destination for MPI send command"));
    };
  } else {
    dest = controllerRank();
  };
  return;
};


} //# NAMESPACE CASA - END

#endif

