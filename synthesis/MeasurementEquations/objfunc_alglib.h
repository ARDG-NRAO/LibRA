#ifndef SYNTHESIS_OBJFUNCALGLIB_H
#define SYNTHESIS_OBJFUNCALGLIB_H

#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/IPosition.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>

#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Functionals/Gaussian2D.h>

#include "lbfgs/optimization.h"

#ifndef isnan
#define isnan(x) std::isnan(x)
#endif

namespace casa { //# NAMESPACE CASA - BEGIN

class ParamAlglibObj
{
private:
  int nX;
  int nY;
  unsigned int AspLen;
  casacore::Matrix<casacore::Float> itsMatDirty;
  casacore::Matrix<casacore::Complex> itsPsfFT;
  std::vector<casacore::IPosition> center;
  casacore::Matrix<casacore::Float> newResidual;
  casacore::Matrix<casacore::Float> AspConvPsf;
  casacore::Matrix<casacore::Float> dAspConvPsf;
  casacore::Matrix<casacore::Float> Asp;
  casacore::Matrix<casacore::Float> dAsp;

public:
  casacore::FFTServer<casacore::Float,casacore::Complex> fft;

  ParamAlglibObj(const casacore::Matrix<casacore::Float>& dirty,
    const casacore::Matrix<casacore::Complex>& psf,
    const std::vector<casacore::IPosition>& positionOptimum,
    const casacore::FFTServer<casacore::Float,casacore::Complex>& fftin) :
    itsMatDirty(dirty),
    itsPsfFT(psf),
    center(positionOptimum),
    fft(fftin)
  {
    nX = itsMatDirty.shape()(0);
    nY = itsMatDirty.shape()(1);
    AspLen = center.size();
    newResidual.resize(nX, nY);
    AspConvPsf.resize(nX, nY);
    dAspConvPsf.resize(nX, nY);
    Asp.resize(nX, nY);
    dAsp.resize(nX, nY);
  }

  ~ParamAlglibObj() = default;

  casacore::Matrix<casacore::Float>  getterDirty() { return itsMatDirty; }
  casacore::Matrix<casacore::Complex> getterPsfFT() { return itsPsfFT; }
  std::vector<casacore::IPosition> getterCenter() {return center;}
  unsigned int getterAspLen() { return AspLen; }
  int getterNX() { return nX; }
  int getterNY() { return nY; }
  casacore::Matrix<casacore::Float>  getterRes() { return newResidual; }
  void setterRes(const casacore::Matrix<casacore::Float>& res) { newResidual = res; }
  casacore::Matrix<casacore::Float>  getterAspConvPsf() { return AspConvPsf; }
  void setterAspConvPsf(const casacore::Matrix<casacore::Float>& m) { AspConvPsf = m; }
  casacore::Matrix<casacore::Float>  getterDAspConvPsf() { return dAspConvPsf; }
  casacore::Matrix<casacore::Float>  getterAsp() { return Asp; }
  void setterAsp(const casacore::Matrix<casacore::Float>& m) { Asp = m; }
  casacore::Matrix<casacore::Float>  getterDAsp() { return dAsp; }
};

void objfunc_alglib(const alglib::real_1d_array &x, double &func, alglib::real_1d_array &grad, void *ptr) 
{
    // retrieve params for GSL bfgs optimization
    casa::ParamAlglibObj *MyP = (casa::ParamAlglibObj *) ptr; //re-cast back to ParamAlglibObj to retrieve images

    casacore::Matrix<casacore::Float> itsMatDirty(MyP->getterDirty());
    casacore::Matrix<casacore::Complex> itsPsfFT(MyP->getterPsfFT());
    std::vector<casacore::IPosition> center = MyP->getterCenter();
    const unsigned int AspLen = MyP->getterAspLen();
    const int nX = MyP->getterNX();
    const int nY = MyP->getterNY();
    casacore::Matrix<casacore::Float> newResidual(MyP->getterRes());
    casacore::Matrix<casacore::Float> AspConvPsf(MyP->getterAspConvPsf());
    casacore::Matrix<casacore::Float> Asp(MyP->getterAsp());
    casacore::Matrix<casacore::Float> dAspConvPsf(MyP->getterDAspConvPsf());
    casacore::Matrix<casacore::Float> dAsp(MyP->getterDAsp());

    func = 0;
    double amp = 1;

    const int refi = nX/2;
    const int refj = nY/2;

    int minX = nX - 1;
    int maxX = 0;
    int minY = nY - 1;
    int maxY = 0;

    // First, get the amp * AspenConvPsf for each Aspen to update the residual
    for (unsigned int k = 0; k < AspLen; k ++)
    {
        amp = x[2*k];
        double scale = x[2*k+1];
        //std::cout << "f: amp " << amp << " scale " << scale << std::endl;

      if (isnan(amp) || scale < 0.4) // GSL scale < 0
      {
        //std::cout << "nan? " << amp << " neg scale? " << scale << std::endl;
        // If scale is small (<0.4), make it 0 scale to utilize Hogbom and save time
        scale = (scale = fabs(scale)) < 0.4 ? 0 : scale;
        //std::cout << "reset neg scale to " << scale << std::endl;

        if (scale <= 0)
          return;
      }

      // generate a gaussian for each Asp in the Aspen set
      // x[0]: Amplitude0,       x[1]: scale0
      // x[2]: Amplitude1,       x[3]: scale1
      // x[2k]: Amplitude(k), x[2k+1]: scale(k+1)
      //casacore::Matrix<casacore::Float> Asp(nX, nY);
      Asp = 0.0;
      dAsp = 0.0;

      const double sigma5 = 5 * scale / 2;
      const int minI = std::max(0, (int)(center[k][0] - sigma5));
      const int maxI = std::min(nX-1, (int)(center[k][0] + sigma5));
      const int minJ = std::max(0, (int)(center[k][1] - sigma5));
      const int maxJ = std::min(nY-1, (int)(center[k][1] + sigma5));

      if (minI < minX)
        minX = minI;
      if (maxI > maxX)
        maxX = maxI;
      if (minJ < minY)
        minY = minJ;
      if (maxJ > maxY)
        maxY = maxJ;

      for (int j = minJ; j <= maxJ; j++)
      {
        for (int i = minI; i <= maxI; i++)
        {
          const int px = i;
          const int py = j;

          Asp(i,j) = (1.0/(sqrt(2*M_PI)*fabs(scale)))*exp(-(pow(i-center[k][0],2) + pow(j-center[k][1],2))*0.5/pow(scale,2));
          dAsp(i,j)= Asp(i,j) * (((pow(i-center[k][0],2) + pow(j-center[k][1],2)) / pow(scale,2) - 1) / fabs(scale)); // verified by python
        }
      }

      casacore::Matrix<casacore::Complex> AspFT;
      MyP->fft.fft0(AspFT, Asp);
      casacore::Matrix<casacore::Complex> cWork;
      cWork = AspFT * itsPsfFT;
      MyP->fft.fft0(AspConvPsf, cWork, false);
      MyP->fft.flip(AspConvPsf, false, false); //need this

      // gradient. 0: amplitude; 1: scale
      // returns the gradient evaluated on x
      casacore::Matrix<casacore::Complex> dAspFT;

      //auto start = std::chrono::high_resolution_clock::now();
      MyP->fft.fft0(dAspFT, dAsp);
      //auto stop = std::chrono::high_resolution_clock::now();
      //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start) ;
      //std::cout << "BFGS fft0 runtime " << duration.count() << " us" << std::endl;

      casacore::Matrix<casacore::Complex> dcWork;
      dcWork = dAspFT * itsPsfFT;
      MyP->fft.fft0(dAspConvPsf, dcWork, false);
      MyP->fft.flip(dAspConvPsf, false, false); //need this
    } // end get amp * AspenConvPsf

    // reset grad to 0. This is important to get the correct optimization.
    double dA = 0.0;
    double dS = 0.0;

    // Update the residual using the current residual image and the latest Aspen.
    // Sanjay used, Res = OrigDirty - active-set aspen * Psf, in 2004, instead.
    // Both works but the current approach is simpler and performs well too.
    for (int j = minY; j < maxY; ++j)
    {
      for(int i = minX; i < maxX; ++i)
      {
        newResidual(i, j) = itsMatDirty(i, j) - amp * AspConvPsf(i, j);
        func = func + double(pow(newResidual(i, j), 2));

        // derivatives of amplitude
        dA += double((-2) * newResidual(i,j) * AspConvPsf(i,j));
        // derivative of scale
        dS += double((-2) * amp * newResidual(i,j) * dAspConvPsf(i,j));
      }
    }
    //std::cout << "after f " << func << std::endl;

    grad[0] = dA;
    grad[1] = dS; 
}



} // end namespace casa

#endif // SYNTHESIS_OBJFUNCALGLIB_H
