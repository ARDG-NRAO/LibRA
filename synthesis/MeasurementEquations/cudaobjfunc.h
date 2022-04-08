#ifndef SYNTHESIS_CUDALBFGS_H
#define SYNTHESIS_CUDALBFGS_H

#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/IPosition.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>

#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Functionals/Gaussian2D.h>

//#include "CudaLBFGS/lbfgs.h"
#include "lbfgs.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <device_functions.h>

#ifndef isnan
#define isnan(x) std::isnan(x)
#endif

namespace casa { //# NAMESPACE CASA - BEGIN

class cpu_objfunc : public cpu_cost_function
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
  casacore::FFTServer<casacore::Float,casacore::Complex> fft;
  casacore::Matrix<casacore::Float> Asp;
  casacore::Matrix<casacore::Float> dAsp;

public:
	cpu_objfunc(const casacore::Matrix<casacore::Float>& dirty,
    const casacore::Matrix<casacore::Complex>& psf,
    const std::vector<casacore::IPosition>& positionOptimum) : 
    cpu_cost_function(2),
    itsMatDirty(dirty),
    itsPsfFT(psf),
    center(positionOptimum)  
    {
	    nX = itsMatDirty.shape()(0);
	    nY = itsMatDirty.shape()(1);
	    AspLen = center.size();
	    newResidual.resize(nX, nY);
	    AspConvPsf.resize(nX, nY);
	    dAspConvPsf.resize(nX, nY);
	    fft = casacore::FFTServer<casacore::Float,casacore::Complex>(itsMatDirty.shape());
	    Asp.resize(nX, nY);
	    dAsp.resize(nX, nY);
	}

	void cpu_f(const floatdouble *h_x, floatdouble *h_y)
	{
		/*const floatdouble x0 = h_x[0];
		const floatdouble x1 = h_x[1];

		// f = (1-x0)^2 + 100 (x1-x0^2)^2

		const floatdouble a = (1.0 - x0);
		const floatdouble b = (x1 - x0 * x0) ;

		*h_y = (a*a) + 100.0f * (b*b);*/
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
	      amp = h_x[2*k];
	      double scale = h_x[2*k+1];
	      std::cout << "f: amp " << amp << " scale " << scale << std::endl;

		  if (isnan(amp) || scale < 0.4) // GSL scale < 0
		  {
		    //std::cout << "nan? " << amp << " neg scale? " << scale << std::endl;
		    // If scale is small (<0.4), make it 0 scale to utilize Hogbom and save time
		    scale = (scale = fabs(scale)) < 0.4 ? 0 : scale;
		    //std::cout << "reset neg scale to " << scale << std::endl;

		    if (scale <= 0)
		    {
		      *h_y = 0;	
		      return;
		    }
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
		  fft.fft0(AspFT, Asp);

		  casacore::Matrix<casacore::Complex> cWork;
		  cWork = AspFT * itsPsfFT;

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
		  //MyP->setterAsp(Asp);
		  //MyP->setterAspConvPsf(AspConvPsf);
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

		std::cout << "after fx " << fx << std::endl;

	    // update newResidual back to the ParamObj
	    //MyP->setterRes(newResidual);

		*h_y = fx;
	}

	void cpu_gradf(const floatdouble *h_x, floatdouble *h_grad)
	{
		/*const floatdouble x0 = h_x[0];
		const floatdouble x1 = h_x[1];

		// df/dx0 = -2 (1-x0) - 400 (x1-x0^2) x0
		// df/dx1 = 200 (x1 - x0^2)

		h_grad[0] = -2.0f * (1.0f - x0) - 400.0f * x0 * (x1 - x0*x0);
		h_grad[1] = 200.0f * (x1 - x0*x0);*/

		// gradient. 0: amplitude; 1: scale
		// returns the gradient evaluated on x
		for (unsigned int k = 0; k < AspLen; k ++)
		{
		  dAsp = 0.0;
		  double amp = h_x[2*k];
	      double scale = h_x[2*k+1];
	      std::cout << "grad: amp " << amp << " scale " << scale << std::endl;

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

		  casacore::Matrix<casacore::Complex> dAspFT;
		  fft.fft0(dAspFT, dAsp);
		  casacore::Matrix<casacore::Complex> dcWork;
		  dcWork = dAspFT * itsPsfFT;

		  fft.fft0(dAspConvPsf, dcWork, false);
		  fft.flip(dAspConvPsf, false, false); //need this


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

		  h_grad[2*k] = dA;
		  h_grad[2*k+1] = dS;

		} // end of derivatives
	}

	void cpu_f_gradf(const floatdouble *h_x, floatdouble *h_f, floatdouble *h_gradf)
	{
		cpu_f(h_x, h_f);
		cpu_gradf(h_x, h_gradf);
	}
};

/*namespace gpu_rosenbrock_d
{
	__global__ void kernelF(const float *d_x, float *d_y)
	{
		const float &x0 = d_x[0];
		const float &x1 = d_x[1];

		// f = (1-x0)^2 + 100 (x1-x0^2)^2

		const float a = (1.0 - x0);
		const float b = (x1 - x0 * x0) ;

		*d_y = (a*a) + 100.0f * (b*b);
	}

	__global__ void kernelGradf(const float *d_x, float *d_grad)
	{
		const float x0 = d_x[0];
		const float x1 = d_x[1];

		// df/dx0 = -2 (1-x0) - 400 (x1-x0^2) x0
		// df/dx1 = 200 (x1 - x0^2)

		d_grad[0] = -2.0f * (1.0f - x0) - 400.0f * x0 * (x1 - x0*x0);
		d_grad[1] = 200.0f * (x1 - x0*x0);
	}
}

class gpu_rosenbrock : public cost_function
{
public:
	gpu_rosenbrock()
	: cost_function(2) {}

	void f(const float *d_x, float *d_y)
	{
		gpu_rosenbrock_d::kernelF<<<1, 1>>>(d_x, d_y);
	}

	void gradf(const float *d_x, float *d_grad)
	{
		gpu_rosenbrock_d::kernelGradf<<<1, 1>>>(d_x, d_grad);
	}

	void f_gradf(const float *d_x, float *d_f, float *d_grad)
	{
		f(d_x, d_f);
		gradf(d_x, d_grad);
	}
};*/

} // end namespace

#endif

/*int main(int argc, char **argv)
{
	// CPU

	cpu_rosenbrock rb1;
	lbfgs minimizer1(rb1);
	minimizer1.setGradientEpsilon(1e-3f);

	float x[2] = {2.0f, -1.0f};
	lbfgs::status stat = minimizer1.cpu_lbfgs(x);

	cout << "CPU Rosenbrock: " << x[0] << " " << x[1] << endl;
	cout << minimizer1.statusToString(stat).c_str() << endl;

	// GPU

	 gpu_rosenbrock rb2;
	 lbfgs minimizer2(rb2);
	
	 x[0] = -4.0f;
	 x[1] = 2.0f;
	
	 float *d_x;
	 cudaMalloc(&d_x,    2 * sizeof(float));
	 cudaMemcpy(d_x, &x, 2 * sizeof(float), cudaMemcpyHostToDevice);
	
	 minimizer2.minimize(d_x);
	
	 cudaMemcpy(&x, d_x, 2 * sizeof(float), cudaMemcpyDeviceToHost);
	
	 cout << "GPU Rosenbrock: " << x[0] << " " << x[1] << endl;

	return 0;
}*/
