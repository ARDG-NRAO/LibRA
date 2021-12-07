//# DenoisingLib.h: This file contains the interface definition of the MSTransformManager class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef DenoisingLib_H_
#define DenoisingLib_H_

// casacore containers
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>

// logger
#include <casacore/casa/Logging/LogIO.h>

// GSL includes
#include <gsl/gsl_multifit.h>

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

namespace denoising { //# NAMESPACE DENOISING - BEGIN

void GslVectorWrap(Vector<Double> casa_vector, gsl_vector &wrapper);
void GslMatrixWrap(Matrix<Double> &casa_matrix, gsl_matrix &wrapper, size_t ncols=0);

//////////////////////////////////////////////////////////////////////////
// GslLinearModelBase class
//////////////////////////////////////////////////////////////////////////

template<class T> class GslLinearModelBase
{

public:

	GslLinearModelBase(size_t ndata, size_t ncomponents)
	{
		ndata_p = ndata;
		ncomponents_p = ncomponents;
		model_p.resize(ncomponents_p,ndata_p, false);
	}

	size_t ndata() {return ndata_p;}
	size_t ncomponents() {return ncomponents_p;}
	Matrix<T>& getModelMatrix(){return model_p;}
	Vector<T> getModelAt(size_t pos){return model_p.column(pos);}

protected:

	Matrix<T> model_p;
	size_t ndata_p;
	size_t ncomponents_p;
};

//////////////////////////////////////////////////////////////////////////
// GslPolynomialModel class
//////////////////////////////////////////////////////////////////////////

template<class T> class GslPolynomialModel : public GslLinearModelBase<T>
{
	using GslLinearModelBase<T>::model_p;
	using GslLinearModelBase<T>::ndata_p;
	using GslLinearModelBase<T>::ncomponents_p;

public:

	GslPolynomialModel(size_t ndata, size_t order) :
		GslLinearModelBase<T>(ndata,order+1)
	{
		// Initialize model
		model_p.row(0) = 1.0;

		// Populate linear components
		if (order > 0)
		{
			Vector<T> linearComponent;
			linearComponent.reference(model_p.row(1));
			indgen(linearComponent,-1.0,2.0/(ndata_p-1));
		}

		// Populate higher orders
		for (size_t model_idx=2;model_idx<ncomponents_p;model_idx++)
		{
			for (size_t data_idx=0; data_idx<ndata_p; data_idx++)
			{
				model_p(model_idx,data_idx) = pow(model_p(1,data_idx),model_idx);
			}
		}
	}

	GslPolynomialModel(const Vector<T> &data_x, size_t order) :
		GslLinearModelBase<T>(data_x.size(),order+1)
	{
		// Calculate scale
		T min,max,mid,scale;
		minMax(min,max,data_x);
		mid = 0.5*(min+max);
		scale = 1.0 / (min-mid);

		// Populate linear components
		model_p.row(0) = 1.0;
		if (order > 0) model_p.row(1) = scale*(data_x-mid);

		// Populate higher orders
		for (size_t model_idx=2;model_idx<ncomponents_p;model_idx++)
		{
			for (size_t data_idx=0; data_idx<ndata_p; data_idx++)
			{
				model_p(model_idx,data_idx) = pow(model_p(1,data_idx),model_idx);
			}
		}
	}

	Vector<Float>& getLinearComponentFloat()
	{
		if (linear_component_float_p.size() != ndata_p) initLinearComponentFloat();
		return linear_component_float_p;
	}

private:

	void initLinearComponentFloat()
	{
		linear_component_float_p.resize(ndata_p);
		for (size_t data_idx=0; data_idx<ndata_p; data_idx++)
		{
			linear_component_float_p(data_idx) = model_p(1,data_idx);
		}
	}

	Vector<Float> linear_component_float_p; // Float-compatibility

};

//////////////////////////////////////////////////////////////////////////
// GslMultifitLinearBase class
//////////////////////////////////////////////////////////////////////////

class GslMultifitLinearBase
{

public:

	GslMultifitLinearBase();
	GslMultifitLinearBase(GslLinearModelBase<Double> &model);

	~GslMultifitLinearBase();

	void resetModel(GslLinearModelBase<Double> &model);

	void resetNComponents(size_t ncomponents);

	void setDebug(Bool debug) {debug_p = debug;};

        std::pair<std::vector<Complex>, Complex> calcFitCoeff(Vector<Complex> &data);
	template<class T> std::pair<std::vector<T>, double> calcFitCoeff(Vector<T> &data)
	{
		// Set data
		setData(data);

		// Call fit method to calculate coefficients
		double chisq = calcFitCoeffCore(data_p.column(0),gsl_coeff_real_p);

		// Convert GSL vector into vector
                std::vector<T> coeffCASA(ncomponents_p);
		for (size_t coeff_idx=0;coeff_idx<ncomponents_p;coeff_idx++)
		{
			coeffCASA[coeff_idx] = gsl_vector_get(gsl_coeff_real_p, coeff_idx);
		}

		return std::make_pair(coeffCASA, chisq);
	}

	void getFitCoeff(Vector<Complex> &coeff);
	template<class T> void getFitCoeff(Vector<T> &coeff)
	{
		coeff.resize(ncomponents_p, false);
		for (size_t model_idx=0;model_idx<ncomponents_p;model_idx++)
		{
			coeff(model_idx) = gsl_vector_get(gsl_coeff_real_p,model_idx);
		}

		return;
	}

	void calcFitModelStd(Vector<Complex> &model,Vector<Complex> &std);
	template<class T> void calcFitModelStd(	Vector<T> &model, Vector<T> &std)
	{
		calcFitModelStdCore(model,std,gsl_coeff_real_p);

		return;
	}

	void calcFitModel(Vector<Complex> &model);
	template<class T> void calcFitModel(Vector<T> &model)
	{
		calcFitModelCore(model,gsl_coeff_real_p);

		return;
	}


protected:

	void allocGslResources();
	void freeGslResources();

	virtual void setModel(GslLinearModelBase<Double> &model);

	void setData(Vector<Float> &data);
	void setData(Vector<Double> &data);
	void setData(Vector<Complex> &data);

	virtual double calcFitCoeffCore(Vector<Double> data, gsl_vector* coeff);

	template<class T> void calcFitModelStdCore(	Vector<T> &model, Vector<T> &std, gsl_vector *coeff)
	{
		// Get imag coefficients
		gsl_vector xGSL;
		double y, yerr;
		for (size_t data_idx=0;data_idx<ndata_p;data_idx++)
		{
			Vector<Double> xCASA = model_p->getModelAt(data_idx);
			if (xCASA.size() != ncomponents_p) xCASA.resize(ncomponents_p,True);
			GslVectorWrap(xCASA,xGSL);

			y = 0;
			yerr = 0;
			errno_p = gsl_multifit_linear_est (&xGSL, coeff, gsl_covariance_p, &y, &yerr);

			if (model.size() > 0) model(data_idx) = y;
			if (std.size() > 0 ) std(data_idx) = yerr;
		}

		return;
	}

	template<class T> void calcFitModelCore(Vector<T> &model, gsl_vector *coeff)
	{
		Double coeff_i;
		Matrix<Double> &modelMatrix = model_p->getModelMatrix();

		coeff_i = gsl_vector_get(coeff,0);
		for (size_t data_idx=0; data_idx<ndata_p; data_idx++)
		{
			model(data_idx) = coeff_i*modelMatrix(0,data_idx);
		}

		for (size_t model_idx=0;model_idx<ncomponents_p;model_idx++)
		{
			coeff_i = gsl_vector_get(coeff,model_idx);
			for (size_t data_idx=0; data_idx<ndata_p; data_idx++)
			{
				model(data_idx) = coeff_i*modelMatrix(model_idx,data_idx);
			}
		}

		return;
	}

	// Model
	size_t ndata_p;
	size_t ncomponents_p;
	size_t max_ncomponents_p;
	gsl_matrix gsl_model_p;
	GslLinearModelBase<Double> *model_p;

	// GSL Resources
	gsl_vector *gsl_coeff_real_p;
	gsl_vector *gsl_coeff_imag_p;
	gsl_matrix *gsl_covariance_p;
	gsl_multifit_linear_workspace *gsl_workspace_p;

	// Data
	Matrix<Double> data_p;

	// Fit result
	int errno_p;
	double chisq_p;

	// Misc
	Bool debug_p;
};

//////////////////////////////////////////////////////////////////////////
// GslMultifitWeightedLinear class
//////////////////////////////////////////////////////////////////////////

class GslMultifitWeightedLinear : public GslMultifitLinearBase
{

public:

	GslMultifitWeightedLinear();
	GslMultifitWeightedLinear(GslLinearModelBase<Double> &model);
	~GslMultifitWeightedLinear();

	void setWeights(Vector<Float> &weights);
	void setFlags(Vector<Bool> &flags,Bool goodIsTrue=True);
	void setWeightsAndFlags(Vector<Float> &weights, Vector<Bool> &flags, Bool goodIsTrue=True);

protected:

	void setModel(GslLinearModelBase<Double> &model);
	virtual double calcFitCoeffCore(Vector<Double> data, gsl_vector* coeff);

	// Weights
	Vector<Double> weights_p;
	gsl_vector gls_weights_p;
};

//////////////////////////////////////////////////////////////////////////
// Iteratively Reweighted Least Squares class
//////////////////////////////////////////////////////////////////////////

class IterativelyReweightedLeastSquares : public GslMultifitWeightedLinear
{

public:

	IterativelyReweightedLeastSquares();
	IterativelyReweightedLeastSquares(GslLinearModelBase<Double> &model,size_t nIter);
	~IterativelyReweightedLeastSquares();

	void setNIter(size_t nIter) {nIter_p = nIter;};

	virtual double calcFitCoeffCore(Vector<Double> data, gsl_vector* coeff);
	virtual void updateWeights(Vector<Double> &data, Vector<Double> &model,  Vector<Double> &weights);

protected:

	size_t nIter_p;

};

} //# NAMESPACE DENOISING - END

} //# NAMESPACE CASA - END


#endif /* DenoisingLib_H_ */

