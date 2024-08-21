#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <string>
#include <iostream>
#include <asp.h>


using namespace std;
namespace py = pybind11;


/**
 * @file asp2py.cc
 * @brief pybind11-based hummbee python plugin
 */
/**
 * @brief This module provides a pybind11-based hummbee python plugin.
 * 
 * The module provides a task-level interface for the asp API.
 * It registers an exception handler for AipsError exceptions (from C++) to PyAipsError (in Python).
 * 
 * @param m The pybind11 module object.
 */

template <typename T>
void Asp2py(
    py::array_t<T>& model, 
    py::array_t<T>& psf, 
    py::array_t<T>& residual,
    py::array_t<T>& mask,
    size_t size_x, size_t size_y, 
    float psfwidth,
    float largestscale, float fusedthreshold,
    int nterms,
    float gain, 
    float threshold, float nsigmathreshold,
    float nsigma,
    int cycleniter, float cyclefactor,
    std::string specmode,
    int nSubChans = 1, int chanid = 0)
{
    LogIO os( LogOrigin("Asp","Asp", WHERE) );
    AspMatrixCleaner itsCleaner;

    // Convert py::array_t<T> to raw pointer and shape
    py::buffer_info model_buf = model.request();
    py::buffer_info psf_buf = psf.request();
    py::buffer_info residual_buf = residual.request();
    py::buffer_info mask_buf = mask.request();

    T* model_ptr = static_cast<T*>(model_buf.ptr);
    T* psf_ptr = static_cast<T*>(psf_buf.ptr);
    T* residual_ptr = static_cast<T*>(residual_buf.ptr);
    T* mask_ptr = static_cast<T*>(mask_buf.ptr);

    // Matrix dimensions
    size_t nx = size_x;
    size_t ny = size_y;

    
  Matrix<T> psfMat(size_x, size_y, 0);
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * nx;  
      psfMat(i,j) = psf_ptr[index];
    }
  }
  itsCleaner.setPsf(psfMat);
  

  itsCleaner.setPsfWidth(psfwidth);

  //if user does not provide the largest scale, we calculate it internally.
  itsCleaner.setUserLargestScale(largestscale);

  itsCleaner.setInitScaleXfrs(psfwidth);

  itsCleaner.stopPointMode(-1);
  itsCleaner.ignoreCenterBox(true); // Clean full image
  

  if (fusedthreshold < 0)
  {
    os << LogIO::WARN << "Acceptable fusedthreshld values are >= 0. Changing fusedthreshold from " << fusedthreshold << " to 0." << LogIO::POST;
    fusedthreshold = 0.0;
  }

  itsCleaner.setFusedThreshold(fusedthreshold);
  

  Matrix<T> maskMat(size_x, size_y, 0);
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * nx;
      //maskMat(i,j) = mask[i][j];
      maskMat(i,j) = mask_ptr[index];
    }
  }

  itsCleaner.setInitScaleMasks(maskMat);  //Array<Float> itsMatMask; 
  itsCleaner.setaspcontrol(0, 0, 0, Quantity(0.0, "%"));// Needs to come before the rest


  Matrix<T> dirtyMat(size_x, size_y, 0);
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * nx;
      //dirtyMat(i,j) = residual[i][j];
      dirtyMat(i,j) = residual_ptr[index];
    }
  }
  itsCleaner.setDirty(dirtyMat);

  // InitScaleXfrs and InitScaleMasks should already be set
  vector<Float> itsScaleSizes;
  itsScaleSizes.clear();
  itsScaleSizes = itsCleaner.getActiveSetAspen();
  itsScaleSizes.push_back(0.0); // put 0 scale
  itsCleaner.defineAspScales(itsScaleSizes);

  //for (int i = 0; i < itsScaleSizes.size(); i++)
  //  cout << "itsScale[" << i << "] = " << itsScaleSizes[i] << endl;


  // takeOneStep
  Quantity thresh(threshold, "Jy");
  itsCleaner.setaspcontrol(cycleniter, gain, thresh, Quantity(0.0, "%"));
   

  Matrix<T> modelMat(size_x, size_y, 0);
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * nx;
      //modelMat(i,j) = model[i][j];
      modelMat(i,j) = model_ptr[index];
    }
  }
  

  // get initial peak residual
  T masksum = sum(maskMat);
  bool validMask = ( masksum > 0 );

  T startpeakresidual;
  Matrix<T> maskedDirty(size_x, size_y, 0);

  if (validMask)
  {
    for (int j = 0; j < size_y; j++)
    {
      for (int i = 0; i < size_x; i++)
      {
        if(maskMat(i,j) > 0.99)
          maskedDirty(i,j) = abs(dirtyMat(i,j));
        else
          maskedDirty(i,j) = 0;
      }
    }
    startpeakresidual = max(maskedDirty); 
  }
  else
  {
    startpeakresidual = max(abs(dirtyMat));
  }

  //cout << "startpeakresidual " << startpeakresidual << ", masksum " << masksum << endl;

  // retval
  //  1 = converged
  //  0 = not converged but behaving normally
  // -1 = not converged and stopped on cleaning consecutive smallest scale
  // -2 = not converged and either large scale hit negative or diverging
  // -3 = clean is diverging rather than converging
  itsCleaner.startingIteration( 0 );
  int  retval = itsCleaner.aspclean( modelMat );
  int iterdone = itsCleaner.numberIterations();

  if( retval==-1 ) {os << LogIO::WARN << "AspClean minor cycle stopped on cleaning consecutive smallest scale" << LogIO::POST; }
  if( retval==-2 ) {os << LogIO::WARN << "AspClean minor cycle stopped at large scale negative or diverging" << LogIO::POST;}
  if( retval==-3 ) {os << LogIO::WARN << "AspClean minor cycle stopped because it is diverging" << LogIO::POST; }

  // update residual - this is critical
  dirtyMat = itsCleaner.getterResidual();

  // send back py array model
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * nx;
      //residual[i][j] = dirtyMat(i,j);
      residual_ptr[index] = dirtyMat(i,j);
    }
  }


  float peakresidual = itsCleaner.getterPeakResidual(); 
  float modelflux = sum( modelMat );

 
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * nx;
      //model[i][j] = modelMat(i,j);
      model_ptr[index] = modelMat(i,j);
    }
  }

  os << LogIO::NORMAL1  << "Asp: After one step, residual=" << peakresidual << " model=" << modelflux << " iters=" << iterdone << LogIO::POST;

  int stopCode = 0;

  // Reached cycleniter
  if (iterdone >= cycleniter ) 
    stopCode=1;
  
  // Reached cyclethreshold
  if (threshold >= nsigmathreshold) 
  {
    if(fabs(peakresidual) <= threshold) 
      stopCode=2;
  }
  // Reached n-sigma threshold
  else 
  {
    if(fabs(peakresidual) <= nsigmathreshold && !(iterdone <= 0)) 
    { 
      if (nsigma!=0.0) 
        stopCode=6; 
    }
  }

  // Zero iterations done
  if(iterdone == 0) 
    stopCode=3;

  // Diverged
  if(iterdone > 0 &&
      (fabs(startpeakresidual) > 0.0 ) &&
      (fabs(peakresidual) - fabs(startpeakresidual) )/ fabs(startpeakresidual) > 0.1 )
    {stopCode=4;}


  if(stopCode==0 && iterdone != cycleniter)
  {
    os << LogIO::NORMAL1 << "Exited minor cycle without satisfying stopping criteria " << LogIO::POST;
    stopCode=5;
  }

  os << "[";// << imagestore->getName();
  if(nSubChans>1) os << "C" << chanid ;
  //if(nSubPols>1) os << ":P" << polid ;
  //Int iterend = loopcontrols.getIterDone();
  os << "]"
     <<" iters=0->" << iterdone << " [" << iterdone << "]"
     << ", model=0->" << modelflux
     << ", peakres=" << startpeakresidual << "->" << peakresidual ;

  switch (stopCode)
    {
    case 0:
      os << ", Skipped this plane. Zero mask.";
      break;
    case 1:
      os << ", Reached cycleniter.";
      break;
    case 2:
      os << ", Reached cyclethreshold.";
      break;
    case 3:
      os << ", Zero iterations performed.";
      break;
    case 4:
      os << ", Possible divergence. Peak residual increased by 10% from minimum.";
      break;
    case 5:
      os << ", Exited Asp minor cycle without reaching any stopping criterion.";
      break;
    case 6:
      os << ", Reached n-sigma threshold.";
    default:
      break;
    }

     os << LogIO::POST;
}



PYBIND11_MODULE(asp2py, m) 
{
    // Using the simplest helper function to translate AipsError exceptions (from C++) to
    // PyAipsError (in Python).
    pybind11::register_exception<AipsError>(m, "PyAipsError");

    m.doc() = "pybind11-based asp python plugin"; 

    /* CANNOT do this because the changed images are not returned in this way.
    // Has to use the Asp2py function defined above to have the changed images
    // returned to py array.
    m.def("Asp", &Asp<float>, "A task-level interface for the asp API",
        py::arg("model"), py::arg("psf"), py::arg("residual"),
        py::arg("mask"), 
        py::arg("size_x"), py::arg("size_y"),
        py::arg("psfwidth"), py::arg("largestscale"), py::arg("fusedthreshold"),
        py::arg("nterms"), py::arg("gain"), 
        py::arg("threshold"), py::arg("nsigmathreshold"),
        py::arg("nsigma"), py::arg("cycleniter"), py::arg("cyclefactor"),
        py::arg("specmode"), py::arg("nSubChans") = 1, py::arg("chanid") = 0);*/



    m.def("Asp2py", &Asp2py<float>, "A task-level interface for the asp API taking numpy arrray",
        py::arg("model"), py::arg("psf"), py::arg("residual"),
        py::arg("mask"), 
        py::arg("size_x"), py::arg("size_y"),
        py::arg("psfwidth"), py::arg("largestscale"), py::arg("fusedthreshold"),
        py::arg("nterms"), py::arg("gain"), 
        py::arg("threshold"), py::arg("nsigmathreshold"),
        py::arg("nsigma"), py::arg("cycleniter"), py::arg("cyclefactor"),
        py::arg("specmode"), py::arg("nSubChans") = 1, py::arg("chanid") = 0);

}


