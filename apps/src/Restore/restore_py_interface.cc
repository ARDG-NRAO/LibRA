#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <iostream>
#include <pybind11/numpy.h>
#include <vector>
#include <cmath>

#include <restore.h>

using namespace std;
namespace py = pybind11;


// Restore function that has the updated images reflected in python
template <typename T>
void Restore2py(
    py::array_t<T>& model,
    py::array_t<T>& residual,
    py::array_t<T>& image,
    size_t size_x,
    size_t size_y,
    double refi,
    double refj,
    double inci,
    double incj,
    double majaxis,
    double minaxis,
    double pa,
    bool pbcor = false,
    py::array_t<T>& pb = py::array_t<T>(),
    py::array_t<T>& image_pbcor = py::array_t<T>()) 
{
    LogIO os( LogOrigin("Restore","Restore", WHERE) );
    // Convert py::array_t<T> to raw pointer and shape
    py::buffer_info model_buf = model.request();
    py::buffer_info residual_buf = residual.request();
    py::buffer_info image_buf = image.request();

    T* model_ptr = static_cast<T*>(model_buf.ptr);
    T* residual_ptr = static_cast<T*>(residual_buf.ptr);
    T* image_ptr = static_cast<T*>(image_buf.ptr);


  Matrix<T> dirtyMat(size_x, size_y, 0);
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * size_x;
      dirtyMat(i,j) = residual_ptr[index];
      //cout << "dirty i " << i << " j " << j << " " << dirtyMat(i,j) << endl;
    }
  }
  
  
  Matrix<T> modelMat(size_x, size_y, 0);  
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * size_x;
      modelMat(i,j) = model_ptr[index];
      //cout << "model i " << i << " j " << j << " " << modelMat(i,j) << endl;
    }
  }
  

  // perform "restore"
  // Before restoring, check for an empty model image and don't convolve (but still smooth residuals)
  bool emptyModel = false;
  
  if(fabs(sum(modelMat)) < 1e-08)
  { 
    os << LogIO::WARN << "Restoring with an empty model image. Only residuals will be processed to form the output restored image." << LogIO::POST;
    emptyModel = true;
  }

  if(max(dirtyMat) == 0.0)  
  {
    os << LogIO::WARN << "Cannot restore without a residual image" << LogIO::POST;
    return;
  }

  
  std::vector<double> beam(3);
  beam[0] = majaxis;
  beam[1] = minaxis;
  beam[2] = pa;

  double cospa = cos(beam[2]);
  double sinpa = sin(beam[2]);
  AlwaysAssert(beam[0]>0.0,AipsError);
  AlwaysAssert(beam[1]>0.0,AipsError);
  double sbmaj, sbmin;
  sbmaj = 4.0*log(2.0)*square(1.0/beam[0]);
  sbmin = 4.0*log(2.0)*square(1.0/beam[1]);
  //std::vector<double>fd(fabs(d));
  Matrix<T> beamMat(size_x, size_y, 0);

  for (int j = 0; j < size_y; j++) {
    for (int i = 0; i < size_x; i++) {
      double x =   cospa * (double(i)-refi)*inci + sinpa * (double(j)-refj)*incj;
      double y = - sinpa * (double(i)-refi)*inci + cospa * (double(j)-refj)*incj;
      double radius = sbmaj*square(x) + sbmin*square(y);
      if (radius < 20.0) 
        beamMat(i,j) = exp(-radius);
      else 
        beamMat(i,j)=0.;
    }
  }

  // Initialize restored image
  Matrix<T> imageMat(size_x, size_y, 0);
  if( !emptyModel ) 
  { 
     // Copy model into it
     imageMat = modelMat;
     // Smooth model by beam
    
    // Make FFT machine
    FFTServer<Float,Complex> fft(IPosition(2, size_x, size_y));
    
    // smooth the model by the beam
    Matrix<Complex> xfr;
    Matrix<Complex> cft;

    fft.fft0(xfr, beamMat);
    
    fft.fft0(cft, imageMat);
    cft *= xfr;
    
    fft.fft0(imageMat, cft, false);
    fft.flip(imageMat, false, false);

  }
  
    imageMat += dirtyMat; 

    if (pbcor)
    {

      if(pb.size() == 0)
      {
        // Cannot pbcor without pb
        os << LogIO::WARN << "Cannot pbcor without pb" << LogIO::POST;
        return;
      }

      py::buffer_info pb_buf = pb.request();
      T* pb_ptr = static_cast<T*>(pb_buf.ptr);
      
      Matrix<T> pbMat(size_x, size_y, 0); 
      for (int j = 0; j < size_y; j++)
      {
        for (int i = 0; i < size_x; i++)
        {
          size_t index = i + j * size_x;
          pbMat(i,j) = pb_ptr[index];
        }
      }
      
      Matrix<T> image_pbcorMat(size_x, size_y, 0);

      float pbmaxval = max(pbMat);

      if( pbmaxval<=0.0 )
      {
          os << LogIO::WARN << "Skipping PBCOR because pb max is zero " << LogIO::POST;
      }
      else
      {
          //LatticeExpr<Float> thepbcor( iif( *(pbsubim) > 0.0 , (*(restoredsubim))/(*(pbsubim)) , 0.0 ) );
          //pbcorsubim->copyData( thepbcor );

          for (std::size_t j = 0; j < size_y; ++j)
          {  
            for (std::size_t i = 0; i < size_x; ++i)
            {
              if (pbMat(i,j) > 0.0)
                image_pbcorMat(i,j) = imageMat(i,j) / pbMat(i,j);
            }
          }
      }

      // send back STL image_pbcor
      py::buffer_info image_pbcor_buf = image_pbcor.request();
      T* image_pbcor_ptr = static_cast<T*>(image_pbcor_buf.ptr);
      
      for (int j = 0; j < size_y; j++)
      {
        for (int i = 0; i < size_x; i++)
        {
          size_t index = i + j * size_x;
          image_pbcor_ptr[index] = image_pbcorMat(i,j);
        }
      }

    }

  // send back STL retored image
  for (int j = 0; j < size_y; j++)
  {
    for (int i = 0; i < size_x; i++)
    {
      size_t index = i + j * size_x;
      image_ptr[index] = imageMat(i,j);
    }
  }

}



/**
 * @brief pybind11-based restore python plugin
 */
/**
 * @brief This module provides a pybind11-based restore python plugin.
 *  
 * @param m The pybind11 module object.
 */
PYBIND11_MODULE(restore2py, m)
{
    // Using the simplest helper function to translate AipsError exceptions (from C++) to
    // PyAipsError (in Python).
    pybind11::register_exception<AipsError>(m, "PyAipsError");

    m.doc() = "pybind11-based restore python plugin"; // optional module docstring
 
    m.def("Restore2py", &Restore2py<float>, "A python interface for the restore API", 
        py::arg("model"), 
        py::arg("residual"), 
        py::arg("image"),
        py::arg("size_x"), py::arg("size_y"), 
        py::arg("refi"), py::arg("refj"),
        py::arg("inci"), py::arg("incj"), 
        py::arg("majaxis"), py::arg("minaxis"),
        py::arg("pa"), 
        py::arg("pbcor") = false, 
        py::arg("pb") = nullptr,
        py::arg("image_pbcor") = nullptr);

}

