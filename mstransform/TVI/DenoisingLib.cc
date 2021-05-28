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

#include <mstransform/TVI/DenoisingLib.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace denoising { //# NAMESPACE DENOISING - BEGIN

// -----------------------------------------------------------------------
// Wrap CASA Vector with a gsl_vector structure
// -----------------------------------------------------------------------
void GslVectorWrap(Vector<Double> casa_vector, gsl_vector &wrapper)
{
	wrapper.size = casa_vector.size();
	wrapper.stride = casa_vector.steps()(0);
	wrapper.data = casa_vector.data();
	wrapper.owner = false;

	return;
}

// -----------------------------------------------------------------------
//
// Wrap CASA Matrix with a gsl_matrix structure
//
// GSL Matrices are stored in row-major order, meaning that
// each row of elements forms a contiguous block in memory.
// This is the standard “C-language ordering” of two-dimensional arrays.
//
// CASA Matrices are however stored in column-major order
// so the elements of each column forms a contiguous block in memory.
//
// Therefore we have to swap rows-cols in order to match.
//
// Note that FORTRAN stores arrays in column-major order.
// -----------------------------------------------------------------------
void GslMatrixWrap(Matrix<Double> &casa_matrix, gsl_matrix &wrapper, size_t ncols)
{
    ThrowIf (not casa_matrix.contiguousStorage(),
             "Cannot map a non-contiguous CASA matrix to GSL matrix");

	wrapper.size1 = casa_matrix.ncolumn(); // Number of rows
	wrapper.size2 = ncols > 0? ncols : casa_matrix.nrow(); // Number of columns
	wrapper.tda = casa_matrix.nrow();
	wrapper.block = NULL;
	wrapper.data = casa_matrix.data();
	wrapper.owner = false;

	return;
}

//////////////////////////////////////////////////////////////////////////
// GslMultifitLinearBase class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
GslMultifitLinearBase::GslMultifitLinearBase()
{
	model_p = NULL;
	ndata_p = 0;
	ncomponents_p = 0;
	max_ncomponents_p = 0;

	gsl_coeff_real_p = NULL;
	gsl_coeff_imag_p = NULL;
	gsl_covariance_p = NULL;
	gsl_workspace_p = NULL;

	errno_p = 0;
	chisq_p = 0;

	debug_p = False;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
GslMultifitLinearBase::GslMultifitLinearBase(GslLinearModelBase<Double> &model)
{
	setModel(model);

	errno_p = 0;
	chisq_p = 0;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
GslMultifitLinearBase::~GslMultifitLinearBase()
{
	freeGslResources();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::allocGslResources()
{
	gsl_covariance_p = gsl_matrix_alloc (ncomponents_p, ncomponents_p);
	gsl_workspace_p = gsl_multifit_linear_alloc (ndata_p, ncomponents_p);
	gsl_coeff_real_p = gsl_vector_alloc(ncomponents_p);
	gsl_coeff_imag_p = gsl_vector_alloc(ncomponents_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::freeGslResources()
{
	if (gsl_covariance_p != NULL) gsl_matrix_free (gsl_covariance_p);
	if (gsl_workspace_p != NULL) gsl_multifit_linear_free (gsl_workspace_p);
	if (gsl_coeff_real_p != NULL) gsl_vector_free (gsl_coeff_real_p);
	if (gsl_coeff_imag_p != NULL) gsl_vector_free (gsl_coeff_imag_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::setModel(GslLinearModelBase<Double> &model)
{
	model_p = &model;
	ndata_p = model_p->ndata();
	max_ncomponents_p = model_p->ncomponents();

	ncomponents_p = max_ncomponents_p;
	GslMatrixWrap(model_p->getModelMatrix(),gsl_model_p);

	data_p.resize(ndata_p,1,False);

	allocGslResources();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::resetNComponents(size_t ncomponents)
{
    ThrowIf (ncomponents > max_ncomponents_p,
             "Maximum number of components is " + String::toString(max_ncomponents_p));

	ncomponents_p = ncomponents;
	GslMatrixWrap(model_p->getModelMatrix(),gsl_model_p,ncomponents_p);

	freeGslResources();
	allocGslResources();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::resetModel(GslLinearModelBase<Double> &model)
{
	freeGslResources();
	setModel(model);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::setData(Vector<Double> &data)
{
    ThrowIf (data.size() != ndata_p,"Size of data does not match model");

    data_p.column(0).reference(data);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::setData(Vector<Float> &data)
{
	ThrowIf (data.size() != ndata_p,"Size of data does not match model");

	for (size_t idx=0;idx<ndata_p;idx++)
	{
		data_p(idx,0) = data(idx);
	}
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::setData(Vector<Complex> &data)
{
	ThrowIf (data.size() != ndata_p,"Size of data does not match model");

	if (data_p.ncolumn() != 2) data_p.resize(ndata_p,2,False);

	for (size_t idx=0;idx<ndata_p;idx++)
	{
		data_p(idx,0) = real(data(idx));
		data_p(idx,1) = imag(data(idx));
	}
}

// -----------------------------------------------------------------------
//
// Perform least-squares fits to a general linear model, y = X c where
// y is a vector of n observations, X is an n by p matrix of predictor variables,
// and the elements of the vector c are the p unknown best-fit parameters which
// are to be estimated.
//
// NOTE: gsl_multifit_linear expects that the model matrix is organized as follows
// X = [ 1   , x_1  , x_1^2 , ..., x_1^order;
//       1   , x_2  , x_2^2 , ..., x_2^order;
//       1   , x_3  , x_3^2 , ..., x_3^order;
//       ... , ...  , ...   , ..., ...
//       1   , x_n  , x_n^2 , ..., x_n^order]
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::calcFitCoeffCore(Vector<Double> data, gsl_vector* coeff)
{
	// Wrap data vector in a gsl_vector
	gsl_vector data_gsl;
	GslVectorWrap(data,data_gsl);

	// Perform coeff calculation
	errno_p = gsl_multifit_linear (&gsl_model_p, &data_gsl,
			coeff, gsl_covariance_p, &chisq_p, gsl_workspace_p);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Vector<Complex> GslMultifitLinearBase::calcFitCoeff(Vector<Complex> &data)
{
	// Set data
	setData(data);

	// Call fit method to calculate real/imag coefficients
	calcFitCoeffCore(data_p.column(0),gsl_coeff_real_p);
	calcFitCoeffCore(data_p.column(1),gsl_coeff_imag_p);

	// Get imag coefficients
	Vector<Complex> fitCoeff(ncomponents_p);
	for (size_t coeff_idx=0;coeff_idx<ncomponents_p;coeff_idx++)
	{
		fitCoeff(coeff_idx) = Complex(	gsl_vector_get(gsl_coeff_real_p,coeff_idx),
										gsl_vector_get(gsl_coeff_imag_p,coeff_idx));
	}

	return fitCoeff;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::calcFitModelStd(Vector<Complex> &model,Vector<Complex> &std)
{
	// Get imag coefficients
	gsl_vector xGSL;
	double y_real, y_imag, yerr_real, yerr_imag;
	for (size_t data_idx=0;data_idx<ndata_p;data_idx++)
	{
		Vector<Double> xCASA = model_p->getModelAt(data_idx);
		if (xCASA.size() != ncomponents_p) xCASA.resize(ncomponents_p,True);
		GslVectorWrap(xCASA,xGSL);

		y_real = 0;
		yerr_real = 0;
		errno_p = gsl_multifit_linear_est (&xGSL, gsl_coeff_real_p, gsl_covariance_p, &y_real, &yerr_real);

		y_imag = 0;
		yerr_imag = 0;
		errno_p = gsl_multifit_linear_est (&xGSL, gsl_coeff_imag_p, gsl_covariance_p, &y_imag, &yerr_imag);

		if (model.size() > 0) model(data_idx) = Complex(y_real,y_imag);
		if (std.size() > 0 ) std(data_idx) = Complex(yerr_real,yerr_imag);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitLinearBase::calcFitModel(Vector<Complex> &model)
{
	Complex coeff_i;
	Matrix<Double> &modelMatrix = model_p->getModelMatrix();

	coeff_i = Complex(gsl_vector_get(gsl_coeff_real_p,0),
			gsl_vector_get(gsl_coeff_imag_p,0));
	for (size_t data_idx=0; data_idx<ndata_p; data_idx++)
	{
		model(data_idx) = coeff_i*modelMatrix(0,data_idx);
	}

	for (size_t model_idx=1;model_idx<ncomponents_p;model_idx++)
	{
		coeff_i = Complex(gsl_vector_get(gsl_coeff_real_p,model_idx),
				gsl_vector_get(gsl_coeff_imag_p,model_idx));
		for (size_t data_idx=0; data_idx<ndata_p; data_idx++)
		{
			model(data_idx) += coeff_i*modelMatrix(model_idx,data_idx);
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// GslMultifitWeightedLinear class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
GslMultifitWeightedLinear::GslMultifitWeightedLinear() :
		GslMultifitLinearBase()
{

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
GslMultifitWeightedLinear::GslMultifitWeightedLinear(GslLinearModelBase<Double> &model) :
		GslMultifitLinearBase()
{
	setModel(model);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
GslMultifitWeightedLinear::~GslMultifitWeightedLinear()
{

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitWeightedLinear::setModel(GslLinearModelBase<Double> &model)
{
	GslMultifitLinearBase::setModel(model);
	weights_p.resize(ndata_p,False);
	GslVectorWrap(weights_p,gls_weights_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitWeightedLinear::setWeights(Vector<Float> &weights)
{
	// Dim check
	ThrowIf (weights.size() != ndata_p,"Size of weights does not match model");

	// Fill in
	for (size_t idx=0;idx<ndata_p;idx++)
	{
		weights_p(idx) = weights(idx);
	}
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitWeightedLinear::setFlags(Vector<Bool> &flags, Bool goodIsTrue)
{
	// Dim check
	ThrowIf (flags.size() != ndata_p,"Size of flags does not match model");

	// Fill in
	if (goodIsTrue)
	{
		for (size_t idx=0;idx<ndata_p;idx++)
		{
			weights_p(idx) = flags(idx);
		}
	}
	else
	{
		for (size_t idx=0;idx<ndata_p;idx++)
		{
			weights_p(idx) = !flags(idx);
		}
	}
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void GslMultifitWeightedLinear::setWeightsAndFlags(Vector<Float> &weights, Vector<Bool> &flags, Bool goodIsTrue)
{
	// Dim check
	ThrowIf (weights.size() != ndata_p,"Size of weights does not match model");
	ThrowIf (flags.size() != ndata_p,"Size of flags does not match model");

	// Fill in
	if (goodIsTrue)
	{
		for (size_t idx=0;idx<ndata_p;idx++)
		{
			weights_p(idx) = weights(idx)*flags(idx);
		}
	}
	else
	{
		for (size_t idx=0;idx<ndata_p;idx++)
		{
			weights_p(idx) = weights(idx)*!flags(idx);
		}
	}
}

// -----------------------------------------------------------------------
//
// Perform least-squares fits to a general linear weighted model, y = X c where
// y is a vector of n observations, with weights w, X is an n by p matrix of
// predictor variables, and the elements of the vector c are the p unknown best-fit
// parameters which are to be estimated.
//
// NOTE: gsl_multifit_linear expects that the model matrix is organized as follows
// X = [ 1   , x_1  , x_1^2 , ..., x_1^order;
//       1   , x_2  , x_2^2 , ..., x_2^order;
//       1   , x_3  , x_3^2 , ..., x_3^order;
//       ... , ...  , ...   , ..., ...
//       1   , x_n  , x_n^2 , ..., x_n^order]
//
// NOTE: More than one data series can use the same weights / workspace
//       Therefore input data is a matrix where each row represents a data series
//
// -----------------------------------------------------------------------
void GslMultifitWeightedLinear::calcFitCoeffCore(Vector<Double> data, gsl_vector* coeff)
{
	// Wrap data vector in a gsl_vector
	gsl_vector data_gsl;
	GslVectorWrap(data,data_gsl);

	// Perform coeff calculation
	errno_p = gsl_multifit_wlinear (&gsl_model_p, &gls_weights_p, &data_gsl,
			coeff, gsl_covariance_p, &chisq_p, gsl_workspace_p);

	return;
}

//////////////////////////////////////////////////////////////////////////
// IterativelyReweightedLeastSquares class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IterativelyReweightedLeastSquares::IterativelyReweightedLeastSquares() :
		GslMultifitWeightedLinear()
{
	nIter_p = 1;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IterativelyReweightedLeastSquares::IterativelyReweightedLeastSquares(GslLinearModelBase<Double> &model,size_t nIter) :
		GslMultifitWeightedLinear(model)
{
	nIter_p = nIter;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IterativelyReweightedLeastSquares::~IterativelyReweightedLeastSquares()
{

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void IterativelyReweightedLeastSquares::calcFitCoeffCore(Vector<Double> data, gsl_vector* coeff)
{
	// Wrap data vector in a gsl_vector
	gsl_vector data_gsl;
	GslVectorWrap(data,data_gsl);

	if (nIter_p == 1)
	{
		errno_p = gsl_multifit_wlinear (&gsl_model_p, &gls_weights_p, &data_gsl,
				coeff, gsl_covariance_p, &chisq_p, gsl_workspace_p);
	}
	else
	{
		// Create a vector to store iterative weights and wrap it in a gsl_vector
		Vector<Double> reweights(ndata_p);
		reweights = weights_p; // Deep copy
		gsl_vector reweights_gsl;
		GslVectorWrap(reweights,reweights_gsl);

		// Create vectors to store model
		Vector<Double> model(ndata_p);

		// Iterative process
		for (size_t iter=0; iter<nIter_p; iter++)
		{
			// Calculate coefficients for this iteration
			errno_p = gsl_multifit_wlinear (&gsl_model_p, &reweights_gsl, &data_gsl,
					coeff, gsl_covariance_p, &chisq_p, gsl_workspace_p);

			if (iter<nIter_p-1)
			{
				// Calculate output std
				calcFitModelCore(model,coeff);

				// Update weights
				updateWeights(data,model,reweights);
			}
		}
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void IterativelyReweightedLeastSquares::updateWeights(Vector<Double> &data, Vector<Double> &model,  Vector<Double> &weights)
{
	double currentResidual, maxResidual;

	// Find max residual
	maxResidual = 0;
	for (size_t idx=0; idx<ndata_p; idx++)
	{
		currentResidual = 0;
		if (weights(idx) > 0)
		{
			currentResidual = abs(data(idx)-model(idx));
			if (currentResidual > maxResidual) maxResidual = currentResidual;
		}
		weights(idx) = currentResidual;
	}

	// Normalize
	for (size_t idx=0; idx<ndata_p; idx++)
	{
		if (weights(idx) > 0) weights(idx) = (maxResidual - weights(idx))/maxResidual;
	}

	return;
}

} //# NAMESPACE DENOISING - END

} //# NAMESPACE CASA - END

