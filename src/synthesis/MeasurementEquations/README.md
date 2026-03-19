AspClean Extension Guide

This document explains how to extend the AspMatrixCleaner 
(AspClean) base class to implement new algorithms 
(e.g., WaveletAspClean) without modifying the base implementation.

1. Overview

The base AspMatrixCleaner provides:

* Core AspClean algorithm flow
* Scale image generation (Gaussian by default)
* Optimization pipeline (LBFGS)
* Normalization logic

To implement a new variant (e.g., wavelet-based), subclass and override specific functions.

2. What You Need to Override

To implement a custom AspClean variant, override the following functions:

2a. Scale Image Generation
virtual void makeScaleImage(...);
virtual void makeInitScaleImage(...);

Used to define your model (wavelet, etc.)

2b. Normalization 

virtual double computeScaleNormalization(double scaleSize) const;

Defines how scale responses are normalized.

2c. Optimization (LBFGS)
virtual void runLBFGS(
    alglib::minlbfgsstate &state,
    alglib::real_1d_array &x,
    alglib::minlbfgsreport &rep,
    const std::vector<IPosition> &activeSetCenter,
    FFTServer<Float> &fft) const;


3. Minimal Example: WaveletAspMatrixCleaner

Step 1. Define subclass
class WaveletAspMatrixCleaner : public AspMatrixCleaner
{
public:
    // override required methods
};

Step 2. Implement scale model
inline float waveletScaleValue(
    int i, int j,
    double cx, double cy,
    float scale)
{ 
	...
}

Step 3. Override scale image functions
void WaveletAspMatrixCleaner::makeScaleImage(
    Matrix<Float>& iscale,
    Float scaleSize,
    const IPosition& center)
{
    fillScaleImage(
        iscale,
        waveletScaleValue,
        center[0],
        center[1],
        scaleSize);
}

void WaveletAspMatrixCleaner::makeInitScaleImage(
    Matrix<Float>& iscale,
    Float scaleSize,
    Int refi,
    Int refj)
{
    fillScaleImage(
        iscale,
        waveletScaleValue,
        refi,
        refj,
        scaleSize);
}

Step 4. Override normalization
float WaveletAspMatrixCleaner::computeScaleNormalization(double scaleSize) const
{
    ...
}

Step 5. Override LBFGS
void WaveletAspMatrixCleaner::runLBFGS(
    alglib::minlbfgsstate &state,
    alglib::real_1d_array &x,
    alglib::minlbfgsreport &rep,
    const std::vector<IPosition> &activeSetCenter,
    FFTServer<Float,Complex> &fft) const
{
    ParamAlglibObjWavelet optParam(
        *itsDirty,
        activeSetCenter,
        itsWaveletScales,
        itsWaveletAmps);

    ParamAlglibObjWavelet *ptrParam = &optParam;
  
    alglib::minlbfgsoptimize(
        state,
        objfunc_alglib_wavelet,
        NULL,
        (void *) ptrParam);

    minlbfgsresults(state, x, rep);
}



