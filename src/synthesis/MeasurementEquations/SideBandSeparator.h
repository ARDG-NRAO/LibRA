/*
 * SideBandSeparator.h
 *
 *  Created on: 2017/07/19
 *      Author: kana
 */

#ifndef SIDEBANDSEPARATOR_H_
#define SIDEBANDSEPARATOR_H_
// STL
#include <iostream>
#include <string>
#include <vector>
// casacore
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Quanta/Quantum.h>

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/scimath/Mathematics/FFTServer.h>

// casa
#include <imageanalysis/ImageTypedefs.h>

namespace casa { //# NAMESPACE CASA - BEGIN

/**
 * The base class of side band separation algorithm using FFT.
 * Data model independent functions are defined.
 * Data model dependent functions should be defined in derived class.
 **/
class SideBandSeparatorBase {
public:

	  /**
	   * @brief The constructor
	   *
	   * @param[in] inputname	A vector of file names of input data
	   *
	   **/
	  SideBandSeparatorBase(const std::vector<std::string>& inputname);
	  /**
	   * @brief The destructor
	   **/
	  virtual ~SideBandSeparatorBase();

	  /**
	   * @brief Set the number of channels shifted in each input data
	   *
	   * @param[in] shift	The number of channels shifted in each input data.
	   * The number of elements must be equal to the number of input data.
	   * @param[in] signal	If true, the @a shift is interpreted as that of
	   * signal sideband. If false, image sideband is assumed.
	   **/
	  void setShift(const std::vector<double> &shift, const bool signal = true);

	  /**
	   * @brief Set rejection limit of solution.
	   *
	   * @param[in] limit	Rejection limit of channels with poor solution.
	   **/
	  void setThreshold(const double limit);

	  /**
	   * @brief Resolve both image and signal sideband when true is set.
	   *
	   * @param[in] flag	if true, both singnal and image sidebands are
	   * solved. If false, sideband suppression is invoked and only signal
	   * sideband is solved.
	   **/
	  void solveBoth(const bool flag) { doboth_ = flag; };

	  /**
	   * @brief Obtain spectra by subtracting the solution of the other sideband.
	   *
	   * This is an experimental feature to investigate the algorithm.
	   *
	   * @param[in] flag	if false, the solution of its own sideband is adopted.
	   * if true, the solution is obtained for the other sideband and subtracted
	   * from an average of observed spectrum.
	   **/
	  void solvefromOther(const bool flag) { otherside_ = flag; };

	  /**
	  * @brief invoke sideband separation
	  *
	  * The function should be defined in derived class.
	  *
	  * @param[in] outfile	the prefix of output file names.
	  * Suffixes which indicates sideband, i.e., 'signalband' and 'imageband',
	  * are added to @a outfile .
	  * @param[in] overwrite	if true overwrite existing output files.
	  * if false, an error is raised if an output file already exists.
	  **/
	  virtual void separate(const std::string& outfile, const bool overwrite) = 0;

protected:
	  /** Initialize member variables **/
	  void init();
	  void initshift();

	  void setInput(const std::vector<std::string>& inputname);

	  /** Return if the path exists (optionally, check file type) **/
	  casacore::Bool checkFile(const std::string name, std::string type="");

	  casacore::Vector<float> solve(const casacore::Matrix<float> &specMat,
			      const std::vector<casacore::uInt> &inIdvec,
			      const bool signal = true);

	  casacore::Vector<bool> collapseMask(const casacore::Matrix<bool> &flagMat,
				    const std::vector<casacore::uInt> &inIdvec,
				    const bool signal = true);
	  void shiftSpectrum(const casacore::Vector<float> &invec, double shift,
			     casacore::Vector<float> &outvec);

	  void shiftFlag(const casacore::Vector<bool> &invec, double shift,
			     casacore::Vector<bool> &outvec);

	  void deconvolve(casacore::Matrix<float> &specmat, const std::vector<double> shiftvec,
			  const double threshold, casacore::Matrix<float> &outmat);

	  void aggregateMat(const casacore::Matrix<float> &inmat, std::vector<float> &outvec);

	  void subtractFromOther(const casacore::Matrix<float> &shiftmat,
				 const std::vector<float> &invec,
				 const std::vector<double> &shift,
				 std::vector<float> &outvec);
	  ////
	  size_t setupShift();

	  /*
	   * Interpolate masked spectrum channels
	   * In-place interpolation is performed to elements with mask being false.
	   * This function interpolates masked channels in the following rules.
	   * o nearest interpolation for edge channels
	   * o linear interpolation for intermediate masked ranges
	   *
	   * @param[inout] spectrum an 1-D array to be interpolated (in-place).
	   * the number of elements should be equal to that of @a mask.
	   * @param[in] mask an 1-D mask array. the elements in spectrum is valid
	   * when corresponding elements in mask is true.
	   * the number of elements should be equal to that of @a spectrum.
	   *
	   * the function returns false if no valid channel (mask is all false)
	   */
	  bool interpolateMaskedChannels(casacore::Array<float> spectrum,
			  const casacore::Array<bool> maskp);

	  /** Member variables **/
	  // name of images
	  std::vector<std::string> inputNames_;
	  // frequency and direction setup to select data.
	  std::vector<double> sigShift_, imgShift_;
	  unsigned int nshift_, nchan_;
	  // solution parameters
	  bool otherside_, doboth_;
	  double rejlimit_;

	  casacore::FFTServer<casacore::Float, casacore::Complex> fftsf, fftsi;


}; // SideBandSeparatorBase

/**
 * Data model dependent side band separator class.
 * Input: CASA image
 * Output : CASA image
 **/
class SideBandSeparatorII : public SideBandSeparatorBase {
public:
	  /**
	   * @brief The constructor
	   *
	   * @param[in] inputname	A vector of file names of input data
	   *
	   **/
	  SideBandSeparatorII(const std::vector<std::string>& imagename);
	  /**
	   * @brief The destructor
	   **/
	  virtual ~SideBandSeparatorII(){};

	  /**
	  * @brief invoke sideband separation
	  *
	  * @param[in] outfile	the prefix of output file names.
	  * Suffixes which indicates sideband, i.e., 'signalband' and 'imageband',
	  * are added to @a outfile .
	  * @param[in] overwrite	if true overwrite existing output files.
	  * if false, an error is raised if an output file already exists.
	  **/
	  virtual void separate(const std::string& outfile, const bool overwrite);

	  /**
	   * @brief set frequency information of output image in image sideband
	   *
	   * @param[in] refpix	reference channel of image sideband
	   * @param[in' refval	frequency in reference channel
	   **/
	  void setImageBandFrequency(const double refpix, const casacore::Quantity refval);

	  //protected:
private:
	  void initLocal();
	  void checkImage();

	  bool getImageCoordinate(const string& imagename, casacore::CoordinateSystem &csys, casacore::IPosition &npix);
	  bool compareImageAxes(const string& imagename, const casacore::CoordinateSystem &refcsys, const casacore::IPosition &refnpix);
	  bool getSpectraToSolve(const std::vector<casa::SPIIF> &images, const casacore::Slicer &slicer,
			  casacore::Matrix<float>& specMat, casacore::Matrix<bool>& maskMat, std::vector<casacore::uInt>& imgIdvec);
	  double refFreqPix_, refFreqHz_;

};


} //# NAMESPACE CASA - END
#endif /* SIDEBANDSEPARATOR_H_ */
