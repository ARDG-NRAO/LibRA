#ifndef SYNTHESIS_CPPSOLVERS_H
#define SYNTHESIS_CPPSOLVERS_H

#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/IPosition.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>

#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Functionals/Gaussian2D.h>

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>

#include <Eigen/Core>
#include <chrono>

namespace casa { //# NAMESPACE CASA - BEGIN

class ParamObj
{
private:
  int nX;
  int nY;
  unsigned int AspLen;
  casacore::Matrix<casacore::Float> itsMatDirty;
  casacore::Matrix<casacore::Complex> itsPsfFT;
  std::vector<casacore::IPosition> center;
  //genie
  //Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> newResidual;
  casacore::Matrix<casacore::Float> newResidual;
  casacore::Matrix<casacore::Float> AspConvPsf;
  casacore::Matrix<casacore::Float> dAspConvPsf;
  casacore::FFTServer<casacore::Float,casacore::Complex> fft;
  casacore::Matrix<casacore::Float> Asp;
  casacore::Matrix<casacore::Float> dAsp;

public:
  ParamObj(const casacore::Matrix<casacore::Float>& dirty,
    const casacore::Matrix<casacore::Complex>& psf,
    const std::vector<casacore::IPosition>& positionOptimum) :
    itsMatDirty(dirty),
    itsPsfFT(psf),
    center(positionOptimum)/*,
    newResidual(dirty)*/
  {
    nX = itsMatDirty.shape()(0);
    nY = itsMatDirty.shape()(1);
    AspLen = center.size();
    //genie
    //newResidual = Eigen::MatrixXf::Zero(nX, nY);
    newResidual.resize(nX, nY);
    AspConvPsf.resize(nX, nY);
    dAspConvPsf.resize(nX, nY);
    fft = casacore::FFTServer<casacore::Float,casacore::Complex>(itsMatDirty.shape());
    Asp.resize(nX, nY);
    dAsp.resize(nX, nY);
  }

  ~ParamObj() = default;

  casacore::Matrix<casacore::Float>  getterDirty() { return itsMatDirty; }
  casacore::Matrix<casacore::Complex> getterPsfFT() { return itsPsfFT; }
  std::vector<casacore::IPosition> getterCenter() {return center;}
  unsigned int getterAspLen() { return AspLen; }
  int getterNX() { return nX; }
  int getterNY() { return nY; }
  // genie
  //Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> getterRes() { return newResidual; }
  //void setterRes(const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>& res) { newResidual = res; }
  casacore::Matrix<casacore::Float>  getterRes() { return newResidual; }
  void setterRes(const casacore::Matrix<casacore::Float>& res) { newResidual = res; }
  casacore::Matrix<casacore::Float>  getterAspConvPsf() { return AspConvPsf; }
  void setterAspConvPsf(const casacore::Matrix<casacore::Float>& m) { AspConvPsf = m; }
  casacore::Matrix<casacore::Float>  getterDAspConvPsf() { return dAspConvPsf; }
  casacore::FFTServer<casacore::Float,casacore::Complex> getterFFTServer() { return fft; }
  casacore::Matrix<casacore::Float>  getterAsp() { return Asp; }
  void setterAsp(const casacore::Matrix<casacore::Float>& m) { Asp = m; }
  casacore::Matrix<casacore::Float>  getterDAsp() { return dAsp; }
};

} // end namespace casa


namespace {

// objective fucntion
double my_f (const gsl_vector *x, void *params)
{
    // retrieve params for GSL bfgs optimization
    casa::ParamObj *MyP = (casa::ParamObj *) params; //re-cast back to ParamObj to retrieve images

	casacore::Matrix<casacore::Float> itsMatDirty(MyP->getterDirty());
    casacore::Matrix<casacore::Complex> itsPsfFT(MyP->getterPsfFT());
    std::vector<casacore::IPosition> center = MyP->getterCenter();
    const unsigned int AspLen = MyP->getterAspLen();
    const int nX = MyP->getterNX();
    const int nY = MyP->getterNY();
    //genie
    //Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> newResidual = MyP->getterRes();
    casacore::Matrix<casacore::Float> newResidual(MyP->getterRes());
    casacore::FFTServer<casacore::Float,casacore::Complex> fft = MyP->getterFFTServer();
    casacore::Matrix<casacore::Float> AspConvPsf(MyP->getterAspConvPsf());
    casacore::Matrix<casacore::Float> Asp(MyP->getterAsp());

	//Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> AmpAspConvPsfSum = Eigen::MatrixXf::Zero(nX, nY);
    double fx = 0.0;
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
      amp = gsl_vector_get(x, 2*k);
      double scale = gsl_vector_get(x, 2*k+1);
      //std::cout << "f: amp " << amp << " scale " << scale << std::endl;

	  if (isnan(amp) || scale < 0.4) // GSL scale < 0
	  {
	    //std::cout << "nan? " << amp << " neg scale? " << scale << std::endl;
	    // If scale is small (<0.4), make it 0 scale to utilize Hogbom and save time
	    scale = (scale = fabs(scale)) < 0.4 ? 0 : scale;
	    //std::cout << "reset neg scale to " << scale << std::endl;

	    if (scale <= 0)
	      return fx;
	  }

	  // generate a gaussian for each Asp in the Aspen set
	  // x[0]: Amplitude0,       x[1]: scale0
	  // x[2]: Amplitude1,       x[3]: scale1
	  // x[2k]: Amplitude(k), x[2k+1]: scale(k+1)
	  //casacore::Matrix<casacore::Float> Asp(nX, nY);
	  Asp = 0.0;

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
	  //std::cout << "minI " << minI << " minJ " << minJ << " minX " << minX << " minY " << minY << std::endl;

	  for (int j = minJ; j <= maxJ; j++)
	  {
	    for (int i = minI; i <= maxI; i++)
	    {
	      const int px = i;
	      const int py = j;

	      Asp(i,j) = (1.0/(sqrt(2*M_PI)*fabs(scale)))*exp(-(pow(i-center[k][0],2) + pow(j-center[k][1],2))*0.5/pow(scale,2));
	    }
	  }

	  casacore::Matrix<casacore::Complex> AspFT;
	  //casacore::FFTServer<casacore::Float,casacore::Complex> fft(itsMatDirty.shape());
	  fft.fft0(AspFT, Asp);

	  casacore::Matrix<casacore::Complex> cWork;
	  cWork = AspFT * itsPsfFT;
	  ////casacore::Matrix<casacore::Float> AspConvPsf(itsMatDirty.shape(), (casacore::Float)0.0);

	  /*
      start = std::chrono::high_resolution_clock::now();
	  casacore::Matrix<casacore::Float> AspConvPsf(itsMatDirty.shape(), AspConvPsfPtr, casacore::TAKE_OVER);
	  stop = std::chrono::high_resolution_clock::now();
      duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start) ;
      std::cout << "BFGS AspConvPsf takeover runtime " << duration.count() << " us" << std::endl;*/

	  fft.fft0(AspConvPsf, cWork, false);
	  fft.flip(AspConvPsf, false, false); //need this

	  /*casacore::Bool ddelc;
	  const casacore::Float *dptrc = AspConvPsf.getStorage(ddelc);
	  float *ddptrc = const_cast<float*>(dptrc);
	  Eigen::MatrixXf MAspConvPsf = Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>>(ddptrc, nX, nY);

	  AmpAspConvPsfSum = AmpAspConvPsfSum + amp * MAspConvPsf; //optimumstrength*PsfConvAspen
	  AspConvPsf.freeStorage(dptrc, ddelc);*/
	  // correct way is to do it here, but since we only have one aspen, it doesn't matter.
	  /*for (int j = minJ; j < maxJ; ++j)
	  {
		for(int i = minI; i < maxI; ++i)
		{
		  AmpAspConvPsfSum(i, j) += amp * AspConvPsf(i, j);
		}
	  }*/

	  // save this for my_df
	  MyP->setterAsp(Asp);
	  MyP->setterAspConvPsf(AspConvPsf);
	} // end get amp * AspenConvPsf

	// Update the residual using the current residual image and the latest Aspen.
	// Sanjay used, Res = OrigDirty - active-set aspen * Psf, in 2004, instead.
	// Both works but the current approach is simpler and performs well too.
	/*casacore::Bool ddel;
	const casacore::Float *dptr = itsMatDirty.getStorage(ddel);
	float *ddptr = const_cast<float*>(dptr);
	Eigen::MatrixXf Mdirty = Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>>(ddptr, nX, nY);
	newResidual = Mdirty - AmpAspConvPsfSum;
	itsMatDirty.freeStorage(dptr, ddel);


	// generate ChiSq
	// returns the objective function value
	for (int j = minY; j < maxY; ++j)
	{
	  for(int i = minX; i < maxX; ++i)
	  {
	    fx = fx + double(pow(newResidual(i, j), 2));
	  }
	}*/

    //genie
    for (int j = minY; j < maxY; ++j)
	{
	  for(int i = minX; i < maxX; ++i)
	  {
	  	newResidual(i, j) = itsMatDirty(i, j) - amp * AspConvPsf(i, j);
	    fx = fx + double(pow(newResidual(i, j), 2));
	  }
	}
    //genie

	//std::cout << "after fx " << fx << std::endl;

    // update newResidual back to the ParamObj
    MyP->setterRes(newResidual);

	return fx;
}

// gradient
void my_df (const gsl_vector *x, void *params, gsl_vector *grad)
{
    casa::ParamObj *MyP = (casa::ParamObj *) params; //re-cast back to ParamObj to retrieve images
    casacore::Matrix<casacore::Float> itsMatDirty(MyP->getterDirty());
    casacore::Matrix<casacore::Complex> itsPsfFT(MyP->getterPsfFT());
    std::vector<casacore::IPosition> center = MyP->getterCenter();
    const unsigned int AspLen = MyP->getterAspLen();
    const int nX = MyP->getterNX();
    const int nY = MyP->getterNY();
    //genie
    //Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> newResidual = MyP->getterRes();
    casacore::Matrix<casacore::Float> newResidual(MyP->getterRes());
    casacore::Matrix<casacore::Float> AspConvPsf(MyP->getterAspConvPsf());
    casacore::Matrix<casacore::Float> dAspConvPsf(MyP->getterDAspConvPsf());
    casacore::FFTServer<casacore::Float,casacore::Complex> fft = MyP->getterFFTServer();
    casacore::Matrix<casacore::Float> Asp(MyP->getterAsp());
    casacore::Matrix<casacore::Float> dAsp(MyP->getterDAsp());

    // gradient. 0: amplitude; 1: scale
	// returns the gradient evaluated on x
	for (unsigned int k = 0; k < AspLen; k ++)
	{
	  //casacore::Matrix<casacore::Float> Asp(nX, nY);
	  //Asp = 0.0;
	  //casacore::Matrix<casacore::Float> dAsp(nX, nY);
	  dAsp = 0.0;
	  double amp = gsl_vector_get(x, 2*k);
      double scale = gsl_vector_get(x, 2*k+1);
      //std::cout << "grad: amp " << amp << " scale " << scale << std::endl;

      if (isnan(amp) || scale < 0.4) // GSL scale < 0
	  {
	    //std::cout << "grad: nan? " << amp << " neg scale? " << scale << std::endl;
	    scale = (scale = fabs(scale)) < 0.4 ? 0.4 : scale; // This cannot be 0 for df to work
	    //std::cout << "reset neg scale to " << scale << std::endl;
	  }

	  const double sigma5 = 5 * scale / 2;
	  const int minI = std::max(0, (int)(center[k][0] - sigma5));
	  const int maxI = std::min(nX-1, (int)(center[k][0] + sigma5));
	  const int minJ = std::max(0, (int)(center[k][1] - sigma5));
	  const int maxJ = std::min(nY-1, (int)(center[k][1] + sigma5));

	  for (int j = minJ; j <= maxJ; j++)
	  {
	    for (int i = minI; i <= maxI; i++)
	    {
	      const int px = i;
	      const int py = j;

	      //Asp(i,j) = (1.0/(sqrt(2*M_PI)*fabs(scale)))*exp(-(pow(i-center[k][0],2) + pow(j-center[k][1],2))*0.5/pow(scale,2));
	      dAsp(i,j)= Asp(i,j) * (((pow(i-center[k][0],2) + pow(j-center[k][1],2)) / pow(scale,2) - 1) / fabs(scale)); // verified by python
	    }
	  }

	  /*casacore::Matrix<casacore::Complex> AspFT;
	  fft.fft0(AspFT, Asp);
	  casacore::Matrix<casacore::Complex> cWork;
	  cWork = AspFT * itsPsfFT;
	  //casacore::Matrix<casacore::Float> AspConvPsf(itsMatDirty.shape(), (casacore::Float)0.0);
	  fft.fft0(AspConvPsf, cWork, false);
	  fft.flip(AspConvPsf, false, false); //need this
      */
	  casacore::Matrix<casacore::Complex> dAspFT;
	  fft.fft0(dAspFT, dAsp);
	  casacore::Matrix<casacore::Complex> dcWork;

	  /*start = std::chrono::high_resolution_clock::now();
      double **array1;
      array1 = static_cast<double **>(calloc(nX * nY, sizeof(double *)));
      stop = std::chrono::high_resolution_clock::now();
      duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
      std::cout << "array " << array1[1] << " BFGS calloc runtime " << duration.count() << " ms" << std::endl;*/

	  dcWork = dAspFT * itsPsfFT;
	  //casacore::Matrix<casacore::Float> dAspConvPsf(itsMatDirty.shape(), (casacore::Float)0.0);
	  fft.fft0(dAspConvPsf, dcWork, false);
	  fft.flip(dAspConvPsf, false, false); //need this

	  //casacore::Matrix<casacore::Float> GradAmp(itsMatDirty.shape(), (casacore::Float)0.0);
	  //casacore::Matrix<casacore::Float> GradScale(itsMatDirty.shape(), (casacore::Float)0.0);

	  // reset grad to 0. This is important to get the correct optimization.
	  double dA = 0.0;
      double dS = 0.0;
	  //std::cout << "before grad " << 2*k << ": " << gsl_vector_get(grad, 2*k) << std::endl;
	  //std::cout << "before grad " << 2*k+1 << ": " << gsl_vector_get(grad, 2*k+1) << std::endl;

	  for (int j = minJ; j <= maxJ; j++)
	  {
	    for (int i = minI; i <= maxI; i++)
	    {
          /*
	      // generate derivatives of amplitude
	      GradAmp(i,j) = (-2) * newResidual(i,j) * AspConvPsf(i,j);
	      // generate derivative of scale
	      GradScale(i,j) = (-2) * amp * newResidual(i,j) * dAspConvPsf(i,j);

	      dA += double(GradAmp(i,j));
	      dS += double(GradScale(i,j));*/

          // derivatives of amplitude
	      dA += double((-2) * newResidual(i,j) * AspConvPsf(i,j));
	      // derivative of scale
	      dS += double((-2) * amp * newResidual(i,j) * dAspConvPsf(i,j));
	    }
	  }

	  gsl_vector_set(grad, 2*k, dA);
	  gsl_vector_set(grad, 2*k+1, dS);

	  // the following scale up doesn't seem necessary since after opt scale is back to initial guess
	  //gsl_vector_set(grad, 2*k, dA * 1e2); //G55 test scale up
	  //gsl_vector_set(grad, 2*k+1, dS * 1e6); //G55 test scale up
	  //std::cout << "after grad " << 2*k << ": " << gsl_vector_get(grad, 2*k) << " amp " << amp << std::endl;
	  //std::cout << "after grad " << 2*k+1 << ": " << gsl_vector_get(grad, 2*k+1) << " scale " << scale << std::endl;
	} // end of derivatives

}

void my_fdf (const gsl_vector *v, void *params, double *f, gsl_vector *df)
{
  //auto start = std::chrono::high_resolution_clock::now();
  *f = my_f(v, params); // has to be double in GSL
  /*auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "BFGS my_f runtime " << duration.count() << " us" << std::endl;*/

  my_df(v, params, df);
}

void debug_print(const gsl_multimin_fdfminimizer *s, const int k)
{
    const gsl_vector   *x          = NULL;
    const gsl_vector   *g          = NULL;

    std::cout << "At iteration k = " << k << std::endl;

    g = gsl_multimin_fdfminimizer_gradient(s);
    std::cout << "g = " << gsl_vector_get(g, 0) << " " << gsl_vector_get(g, 1) << std::endl;

    x = gsl_multimin_fdfminimizer_x(s);
    std::cout << "x = " << gsl_vector_get(x, 0) << " " << gsl_vector_get(x, 1) << std::endl;


    std::cout << "f(x) = " << gsl_multimin_fdfminimizer_minimum(s) << std::endl;
}

int findComponent(int NIter, gsl_multimin_fdfminimizer *s)
{
  int iter = 0;
  int status = 0;

  do
  {
	// Make the move!
	status = gsl_multimin_fdfminimizer_iterate(s);

	/*if (status == GSL_ENOPROG) // 27: not making progress towards solution
		gsl_multimin_fdfminimizer_restart(s);*/
	if (status)
        break;

	status = gsl_multimin_test_gradient(s->gradient, 1E-3);
    // debug_print(s, iter);

	iter++;
  } while(status == GSL_CONTINUE && iter < NIter);

  return status;
}

} // end namespace

#endif