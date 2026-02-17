#ifndef SYNTHESIS_OBJFUNCALGLIB_WAVELETS_H
#define SYNTHESIS_OBJFUNCALGLIB_WAVELETS_H

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>

#include <casacore/scimath/Mathematics/FFTServer.h>
#include <casacore/scimath/Functionals/Gaussian2D.h>

#include "lbfgs/optimization.h"

#ifndef isnan
#define isnan(x) std::isnan(x)
#endif

namespace casa { //# NAMESPACE CASA - BEGIN

class ParamAlglibObjWavelet
{
private:
  int nX;
  int nY;
  unsigned int AspLen;
  casacore::Matrix<casacore::Float> itsMatDirty;
  std::vector<casacore::IPosition> center;
  casacore::Matrix<casacore::Float> newResidual;
  casacore::Matrix<casacore::Float> AspConvPsf;
  casacore::Matrix<casacore::Float> dAspConvPsf;
  casacore::Vector<casacore::Float> itsWaveletScales;
  casacore::Vector<casacore::Float> itsWaveletAmps;

public:
  ParamAlglibObjWavelet(const casacore::Matrix<casacore::Float>& dirty,
    const std::vector<casacore::IPosition>& positionOptimum, const casacore::Vector<casacore::Float>& waveletScales, const casacore::Vector<casacore::Float>& waveletAmps):
    itsMatDirty(dirty), itsWaveletScales(waveletScales), itsWaveletAmps(waveletAmps),
    center(positionOptimum)
  {
    nX = itsMatDirty.shape()(0);
    nY = itsMatDirty.shape()(1);
    AspLen = center.size();
    newResidual.resize(nX, nY);
    AspConvPsf.resize(nX, nY);
    dAspConvPsf.resize(nX, nY);
  }

  ~ParamAlglibObjWavelet() = default;

  casacore::Matrix<casacore::Float>  getterDirty() { return itsMatDirty; }
  std::vector<casacore::IPosition> getterCenter() {return center;}
  unsigned int getterAspLen() { return AspLen; }
  int getterNX() { return nX; }
  int getterNY() { return nY; }
  casacore::Matrix<casacore::Float>  getterRes() { return newResidual; }
  void setterRes(const casacore::Matrix<casacore::Float>& res) { newResidual = res; }
  casacore::Matrix<casacore::Float>  getterAspConvPsf() { return AspConvPsf; }
  void setterAspConvPsf(const casacore::Matrix<casacore::Float>& m) { AspConvPsf = m; }
  casacore::Matrix<casacore::Float>  getterDAspConvPsf() { return dAspConvPsf; }
  casacore::Vector<casacore::Float> getterScales() { return itsWaveletScales; }
  casacore::Vector<casacore::Float> getterAmps() { return itsWaveletAmps; }
};

void objfunc_alglib_wavelet(const alglib::real_1d_array &x, double &func, alglib::real_1d_array &grad, void *ptr) 
{
    // retrieve params for GSL bfgs optimization
    casa::ParamAlglibObjWavelet *MyP = (casa::ParamAlglibObjWavelet *) ptr; //re-cast back to ParamAlglibObj to retrieve images

    casacore::Matrix<casacore::Float> itsMatDirty(MyP->getterDirty());
    std::vector<casacore::IPosition> center = MyP->getterCenter();
    const unsigned int AspLen = MyP->getterAspLen();
    const int nX = MyP->getterNX();
    const int nY = MyP->getterNY();
    casacore::Matrix<casacore::Float> newResidual(MyP->getterRes());
    casacore::Matrix<casacore::Float> AspConvPsf(MyP->getterAspConvPsf());
    casacore::Matrix<casacore::Float> dAspConvPsf(MyP->getterDAspConvPsf());

    casacore::Vector<casacore::Float> itsWaveletScales(MyP->getterScales());
    casacore::Vector<casacore::Float> itsWaveletAmps(MyP->getterAmps());

    int nscales;
    itsWaveletScales.shape(nscales);

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

      AspConvPsf = 0.0;
      dAspConvPsf = 0.0;

      for (int l = 0; l < nscales; l++){
	      float temp_scale = sqrt(scale*scale+itsWaveletScales(l)*itsWaveletScales(l));
	      float toadd;

	      const double sigma5 = 5 * temp_scale / 2;
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

		  toadd = itsWaveletAmps(l) * 1.0/(2*M_PI*pow(temp_scale,2))*exp(-(pow(i-center[k][0],2) + pow(j-center[k][1],2))*0.5/pow(temp_scale,2));
		  //toadd = itsWaveletAmps(l) * 1.0/(sqrt(2*M_PI)*temp_scale)*exp(-(pow(i-center[k][0],2) + pow(j-center[k][1],2))*0.5/pow(temp_scale,2));
		  AspConvPsf(i,j) += toadd;
		  dAspConvPsf(i,j)+= toadd * ((pow(i-center[k][0],2) + pow(j-center[k][1],2)) / pow(temp_scale,2) - 2) * fabs(scale) / pow(temp_scale, 2);
		  //dAspConvPsf(i,j)+= toadd * ((pow(i-center[k][0],2) + pow(j-center[k][1],2)) / pow(temp_scale,2) - 1) * fabs(scale) / pow(temp_scale, 2);
		  }
	        }
	      }

    } // end get AspenConvPsf

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
