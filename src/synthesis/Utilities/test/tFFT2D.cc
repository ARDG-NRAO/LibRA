//# tFFT2D.cc:  this tests FFT2D
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
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/ImageConcat.h>
#include <casacore/casa/namespace.h>
#include <casacore/scimath/Mathematics/FFTPack.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>

#include <synthesis/Utilities/FFT2D.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace casa;
int main(int argc, char **argv)
{
 
  try{ 
    

 Matrix<Double> xform(2,2);
   xform = 0.0;
   xform.diagonal() = 1.0;
   DirectionCoordinate dc(MDirection::J2000, Projection::SIN, Quantity(20.0,"deg"), Quantity(20.0, "deg"),
                          Quantity(0.5, "arcsec"), Quantity(0.5,"arcsec"),
                          xform, 50.0, 50.0, 999.0, 
                          999.0);
   Vector<Int> whichStokes(1, Stokes::I);
   StokesCoordinate stc(whichStokes);
   SpectralCoordinate spc(MFrequency::LSRK, 1.5e9, 1e6, 0.0 , 1.420405752E9);
   CoordinateSystem cs;
   cs.addCoordinate(dc); cs.addCoordinate(stc); cs.addCoordinate(spc);
   Int x=10000; Int y=12000;
   PagedImage<Complex> im(IPosition(4,x,y,1,1), cs, "gulu0.image");
   im.set(0.0);
   im.putAt(Complex(3.0, 1.5),IPosition(4,x/100*20,y/100*80,0,0));
   im.putAt (Complex(4.0, 2.0),IPosition(4,x/100*60,y/100*70,0,0));
   im.putAt(Complex(1.0, 0.5),IPosition(4,x/100*10,y/100*10,0,0));
   im.putAt(Complex(2.0, 1.0),IPosition(4,x/100*75,y/100*25,0,0));
   PagedImage<Complex> im3(IPosition(4,x,y,1,1), cs, "gulu_lat.image");
   im3.copyData(im);
   Double wtime0=0.0;
   Double wtime1=0.0;
   Double wtime2=0.0;
   
   Int numthreads=-1;
   if(argc >1 ){
     numthreads=atoi(argv[1]);
#ifdef _OPENMP
     omp_set_num_threads(numthreads);
#endif
   }
   FFT2D ftw(true);
   {
     Array<Complex> arr0;
     Array<Complex> arr1;
     //////Lets do FFT via FFTPack
     {
       FFT2D ft(false);
       im.get(arr0, true);
#ifdef _OPENMP
       wtime0=omp_get_wtime();
#endif
       Bool del;
       Complex *scr= arr0.getStorage(del);
       //ft.fftShift(scr, Long(x), Long(y), true);
       ft.doFFT(scr, Long(x), Long(y), true);
#ifdef _OPENMP
       cerr << "FFTPack forward " << x << " by " << y << " complex takes " << omp_get_wtime()-wtime0 << endl;
       arr0.putStorage(scr, del);
       arr0=Complex(x*y)*arr0;
       scr= arr0.getStorage(del);
       wtime0=omp_get_wtime();
#endif
       
       ft.doFFT(scr, Long(x), Long(y), false);
#ifdef _OPENMP
       cerr << "FFTPack backward " << x << " by " << y << " complex takes " << omp_get_wtime()-wtime0 << endl;
#endif
       arr0.putStorage(scr, del);
     }
     //////Lets do FFT via FFTW
     
     {
       
       im.get(arr1, true);
#ifdef _OPENMP
       wtime0=omp_get_wtime();
#endif

       Bool del;
       Complex *scr= arr1.getStorage(del);
       ftw.doFFT(scr, Long(x), Long(y), true);

#ifdef _OPENMP
       cerr << "FFTW forward " << x << " by " << y << " complex takes " << omp_get_wtime()-wtime0 << endl;
       wtime0=omp_get_wtime();
#endif

       ftw.doFFT(scr, Long(x), Long(y), false);

#ifdef _OPENMP
       cerr << "FFTW backward " << x << " by " << y << " complex takes " << omp_get_wtime()-wtime0 << endl;
#endif


       arr1.putStorage(scr, del);
     }
   
     //////Lets do FFT via FFTW 1 thread
     {
       
      
       im.get(arr1, true);

#ifdef _OPENMP
       numthreads=omp_get_max_threads();
       omp_set_num_threads(1);
       wtime0=omp_get_wtime();
#endif

       FFT2D ft(true);
       Bool del;
       Complex *scr= arr1.getStorage(del);
       ft.doFFT(scr, Long(x), Long(y), true);

#ifdef _OPENMP
       cerr << "FFTW 1-thread forward " << x << " by " << y << " complex takes " << omp_get_wtime()-wtime0 << endl;
       omp_set_num_threads(numthreads);
       wtime0=omp_get_wtime();
#endif

       ft.doFFT(scr, Long(x), Long(y), false);

#ifdef _OPENMP
       cerr << "FFTW 1-thread backward " << x << " by " << y << " complex takes " << omp_get_wtime()-wtime0 << endl;
       arr1.putStorage(scr, del);
#endif
     }
     //Bool del;
     //Complex *scr0=arr0.getStorage(del);
     //Complex *scr1=arr1.getStorage(del);
     cerr << std::setprecision(9) <<  "max bet FFTW/FFTPack " << max(arr0) << "   " << max(arr1) << endl;
   }
#ifdef _OPENMP
  wtime0=omp_get_wtime();
#endif
  Array<Complex> arr;
   Bool isRef=im.get(arr, true);
#ifdef _OPENMP
   wtime1=omp_get_wtime();
#endif
   Bool del;
   Complex *scr= arr.getStorage(del);
   cerr << "isRef " << isRef << " storage is copy " << del << endl;
   //FFT2D smp(true);
   ftw.c2cFFT(scr, Long(x), Long(y));
#ifdef _OPENMP
   wtime2=omp_get_wtime();
#endif
    if(!isRef)
     im.put(arr.reform(IPosition(4, x, y,1,1)));
    Double totTimeFFT2D=0.0;
#ifdef _OPENMP
      totTimeFFT2D= -wtime0  +  omp_get_wtime();
    cerr << "getting array " << wtime1-wtime0 << " fft " << wtime2-wtime1 << " put " << omp_get_wtime()-wtime2 << endl;
   wtime0=omp_get_wtime();
#endif
   ftw.c2cFFT(scr, Long(x), Long(y), false);
#ifdef _OPENMP  
   wtime1=omp_get_wtime();
#endif
   PagedImage<Complex> im2(IPosition(4,x,y,1,1), cs, "gulu_out.image");
   im2.put(arr.reform(IPosition(4, x, y,1,1)));
#ifdef _OPENMP
   cerr << "putting array "   << omp_get_wtime()-wtime1 << " fft " << wtime1 -wtime0 << endl;
#endif
   {
    
#ifdef _OPENMP
     wtime1=omp_get_wtime();
#endif
     LatticeFFT::cfft2d(im3, true);
   }
#ifdef _OPENMP 
   cerr << "LatticeFFT::cfft2d " << omp_get_wtime()-wtime1 << " as compared to  disk array based FFT2D " << totTimeFFT2D <<  endl;
#endif
   Array<Complex> arr2;
   im.get(arr, true);
   im3.get(arr2, true);
   cerr << "max diff lattFFT and FFT2D " << max(arr-arr2) << endl;
   

  }catch( AipsError e ){
    cout << "Exception ocurred." << endl;
    cout << e.getMesg() << endl;
  }
  cout << "OK"<< endl;
  return 0;
};
