//# FFT2D.h 
//# Copyright (C) 2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  General Public
//# License for more details.
//#
//# You should have received a copy of the GNU  General Public License
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
//# $kgolap$
#ifndef SYNTHESIS_FFT2D_H
#define SYNTHESIS_FFT2D_H
#include <casacore/scimath/Mathematics/FFTW.h>
#include <casacore/scimath/Mathematics/FFTPack.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <fftw3.h>
namespace casa{
 class FFT2D
 {
   ///This is a not a full generic   fft class...use casacore::FFTServer or casacore::LatticeFFT for that
   //This is optimized with minimal memcopies for 2D FFTs 
   //Assumes 2D x, y array to be even numbers (e.g (100, 200)...will not work for (101, 200))
   //IMPORTANT: do not use the same FFT2D object for different sizes of x,y. Create
   //another one if necessary as the plan or butterfly in fftpack, for x,y shape is kept
   //and reused so may give wrong result if the FFT2D is used for x',y' shape !!
   //
   typedef unsigned long long ooLong;
 public:
   FFT2D(casacore::Bool useFFTW=true);
   ~FFT2D();
   FFT2D& operator=(const FFT2D& other);
   //out has to be a pointer to an array [(x/2+1), y] shape 
   void r2cFFT(casacore::Complex*& out, casacore::Float*& in, casacore::Long x, casacore::Long y);
   ///Real to complex FFT2D ..each plane of out will contain FFT in X/2+1, Y...
   //the remainder of of the X part is untouched of out lattice is larger than 
   //X/2+1 on the x-axis   
   void r2cFFT(casacore::Lattice<casacore::Complex>& out, casacore::Lattice<casacore::Float>& in);
   ///In place 2D FFT; out has to be of shape [x,y] (origin is at the x/2,y/2)
   void c2cFFT(casacore::Complex*& out, casacore::Long x, casacore::Long y, casacore::Bool toFreq=true);
   void c2cFFT(casacore::DComplex*& out, casacore::Long x, casacore::Long y, casacore::Bool toFreq=true);
   //This will return the 2D FFT of each x-y planes back into the lattice.
   void c2cFFT(casacore::Lattice<casacore::Complex>& inout, casacore::Bool toFreq=true);
   void c2cFFT(casacore::Lattice<casacore::DComplex>& inout, casacore::Bool toFreq=true);
   //Same as above EXCEPT 
   //will do the FFT in DComplex but input and output are Complex
   void c2cFFTInDouble(casacore::Lattice<casacore::Complex>& inout, casacore::Bool toFreq=true);
   //The toFreq=false in FFTShift does the normalization of 1/N_sample expected of ifft
   void fftShift(casacore::Complex*& scr,  casacore::Long x, casacore::Long y, casacore::Bool toFreq=false);
   void fftShift(casacore::DComplex*& scr,  casacore::Long x, casacore::Long y, casacore::Bool toFreq=false);
   void fftShift(casacore::Float*& scr,  casacore::Long x, casacore::Long y);
   void doFFT(casacore::Complex*& out, casacore::Long x, casacore::Long y, casacore::Bool toFreq);
   void doFFT(casacore::DComplex*& out, casacore::Long x, casacore::Long y, casacore::Bool toFreq);
   void doFFT(casacore::Complex*& out, casacore::Float *& in, casacore::Long x, casacore::Long y);
   //C style element conversion avoiding overhead of iterators etc...
   static void complexConvert(casacore::DComplex*& srcD, casacore::Complex*& scr,  const ooLong len, const casacore::Bool down=false);
 private:
   //casacore::FFTW stuff
   fftwf_plan planC2C_forw_p;
   fftwf_plan planC2C_back_p;
   fftwf_plan planR2C_p;
   fftw_plan planC2CD_forw_p;
   fftw_plan planC2CD_back_p;
   casacore::Bool useFFTW_p;
   //casacore::FFTPack stuff
   std::vector<casacore::Float> wsave_p;
   casacore::Int lsav_p;
   //casacore::FFTW fft1_p;
   casacore::Long nx_p, ny_p;
 };
};// end of namespace casa
#endif
