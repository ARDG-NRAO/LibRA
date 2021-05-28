/*
 * SidebandSeparator.cc
 *
 *  Created on: 2017/07/19
 *      Author: kana
 */

// STL
#include <ctype.h>

// cascore
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Logging/LogIO.h>

#include <imageanalysis/ImageAnalysis/ImageFactory.h>
#include <imageanalysis/ImageAnalysis/ImageMetaData.h>
#include <imageanalysis/ImageAnalysis/ImageMaskAttacher.h>

// casa
#include <synthesis/MeasurementEquations/SideBandSeparator.h>

using namespace std ;
using namespace casacore ;

namespace casa {

// constructors
SideBandSeparatorBase::SideBandSeparatorBase(const vector<string>& inputname)
{
  init();
  setInput(inputname);
  {// logging
	LogIO os(LogOrigin("SideBandSeparatorBase","SideBandSeparatorBase()", WHERE));
    os << "Found " << inputNames_.size() << " images." << LogIO::POST;
    os << "Input Data to be processed:" << LogIO::POST;
    for (size_t i = 0; i < inputNames_.size(); ++i) {
      os << "\t" << inputNames_[i] << LogIO::POST;
    }
  }
};

SideBandSeparatorBase::~SideBandSeparatorBase()
{
};

void SideBandSeparatorBase::init()
{
  // shifts
  initshift();
  // image list
  inputNames_.resize(0);
  // solution parameters
  otherside_ = false;
  doboth_ = false;
  rejlimit_ = 0.2;
};

void SideBandSeparatorBase::initshift()
{
  // shifts
  nshift_ = 0;
  nchan_ = 0;
  sigShift_.resize(0);
  imgShift_.resize(0);
};

void SideBandSeparatorBase::setInput(const vector<string>& inputname) {
  inputNames_.resize(0);
  // check existence of images
  for (size_t i = 0 ; i < inputname.size(); ++i) {
    if (checkFile(inputname[i], "d")) {
	  inputNames_.push_back(inputname[i]);
	} else {
	  inputNames_.resize(0);
	  throw( AipsError("Could not find "+inputname[i]) );
	}
  }
  LogIO os(LogOrigin("SideBandSeparatorBase","setImage()", WHERE));
  if (inputNames_.size() < 2)
	  throw( AipsError("At least two valid input data are required for processing") );
}

void SideBandSeparatorBase::setShift(const vector<double> &shift, const bool signal)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","setShift()", WHERE));
  if (shift.size() != 0 && shift.size() != inputNames_.size())
	  throw( AipsError("The number of shift should match that of images") );
  vector<double> *target = signal ? &sigShift_ : &imgShift_;
  target->resize(shift.size());
  for (unsigned int i = 0; i < shift.size(); i++)
	  target->at(i) = - shift[i]; /// NOTE if spw shifts +3ch, need to shift back -3ch in operation.

  if (target->size() == 0) {
    os << "Channel shifts of " << (signal ? "SIGNAL" : "IMAGE") << " sideband are cleared." << LogIO::POST;
  } else {
    os << "Channel shifts of " << (signal ? "SIGNAL" : "IMAGE") << " sideband are set: ( ";
    for (unsigned int i = 0; i < target->size(); i++) {
      os << (signal ? sigShift_[i] : imgShift_[i]);
      if (i != target->size()-1) os << " , ";
    }
    os << " ) [channels]" << LogIO::POST;
  }
};

void SideBandSeparatorBase::setThreshold(const double limit)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","setThreshold()", WHERE));
  if (limit <= 0 || limit >= 1.0)
    throw( AipsError("Rejection limit should be > 0.0 and < 1.0") );

  rejlimit_ = limit;
  os << "Rejection limit is set to " << rejlimit_ << LogIO::POST;
};

/////////////// PROTECTED FUNCTIONS //////////////////////

size_t SideBandSeparatorBase::setupShift()
{
	  LogIO os(LogOrigin("SideBandSeparatorBase","setupShift()", WHERE));
	if (sigShift_.size() > 0 && imgShift_.size() > 0) {
		return sigShift_.size();
	} else if (sigShift_.size() > 0 && imgShift_.size() == 0) {
		os << "Channel shift set only for signal sideband. Assuming the same shift in the opposite direction." << LogIO::POST;
		imgShift_.resize(sigShift_.size());
		for (size_t i = 0; i < sigShift_.size(); ++i) {
			imgShift_[i] = - sigShift_[i];
		}
	} else if (sigShift_.size() == 0 && imgShift_.size() > 0) {
		os << "Channel shift set only for image sideband. Assuming the same shift in the opposite direction." << LogIO::POST;
		sigShift_.resize(imgShift_.size());
		for (size_t i = 0; i < imgShift_.size(); ++i) {
			sigShift_[i] = - imgShift_[i];
		}
	} else {
		throw( AipsError("Channel shift was not been set.") );
	}
	return sigShift_.size();
}

Vector<bool> SideBandSeparatorBase::collapseMask(const Matrix<bool> &flagMat,
					 const vector<uInt> &inIdvec,
					 const bool signal)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","collapseFlag()", WHERE));
  if (inIdvec.size() == 0)
    throw(AipsError("Internal error. Table index is not defined."));
  if (flagMat.ncolumn() != inIdvec.size())
    throw(AipsError("Internal error. The row number of input matrix is not conformant."));
  if (flagMat.nrow() != nchan_)
    throw(AipsError("Internal error. The channel size of input matrix is not conformant."));

  const size_t nspec = inIdvec.size();
  vector<double> *thisShift;
  if (signal == otherside_) {
    // (solve signal && solveother = T) OR (solve image && solveother = F)
    thisShift = &imgShift_;
  } else {
    // (solve signal && solveother = F) OR (solve image && solveother = T)
    thisShift =  &sigShift_;
 }

  Vector<bool> outflag(nchan_, true);
  double tempshift;
  Vector<bool> shiftvec(nchan_, true);
  Vector<bool> accflag(nchan_, true);
  uInt shiftId;
  for (uInt i = 0 ; i < nspec; ++i) {
    shiftId = inIdvec[i];
    tempshift = - thisShift->at(shiftId);
    shiftFlag(flagMat.column(i), tempshift, shiftvec);
    // Now accumulate Mask (true only if all data is valid)
    for (uInt j = 0 ; j < nchan_ ; ++j)
//      accflag[j] |= shiftvec[j];
    	accflag[j] &= shiftvec[j];
  }
  outflag = accflag;
  // Shift back Flag
  //cout << "Shifting FLAG back to " << thisShift->at(0) << " channels" << endl;
  //shiftFlag(accflag, thisShift->at(0), outflag);

  return outflag;
}


Vector<float> SideBandSeparatorBase::solve(const Matrix<float> &specmat,
				   const vector<uInt> &inIdvec,
				   const bool signal)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","solve()", WHERE));
  if (inIdvec.size() == 0)
    throw(AipsError("Internal error. Table index is not defined."));
  if (specmat.ncolumn() != inIdvec.size())
    throw(AipsError("Internal error. The row number of input matrix is not conformant."));
  if (specmat.nrow() != nchan_)
    throw(AipsError("Internal error. The channel size of input matrix is not conformant."));


  os << LogIO::DEBUG1 << "Solving " << (signal ? "SIGNAL" : "IMAGE") << " sideband."
     << LogIO::POST;

  const size_t nspec = inIdvec.size();
  vector<double> *thisShift, *otherShift;
  if (signal == otherside_) {
    // (solve signal && solveother = T) OR (solve image && solveother = F)
    thisShift = &imgShift_;
    otherShift = &sigShift_;
    os << LogIO::DEBUG1 << "Image sideband will be deconvolved." << LogIO::POST;
  } else {
    // (solve signal && solveother = F) OR (solve image && solveother = T)
    thisShift =  &sigShift_;
    otherShift = &imgShift_;
    os << LogIO::DEBUG1 << "Signal sideband will be deconvolved." << LogIO::POST;
 }

  vector<double> spshift(nspec);
  Matrix<float> shiftSpecmat(nchan_, nspec, 0.);
  double tempshift;
  Vector<float> shiftspvec;
  uInt shiftId;
  for (uInt i = 0 ; i < nspec; i++) {
    shiftId = inIdvec[i];
    spshift[i] = otherShift->at(shiftId) - thisShift->at(shiftId);
    tempshift = - thisShift->at(shiftId);
    shiftspvec.reference(shiftSpecmat.column(i));
    shiftSpectrum(specmat.column(i), tempshift, shiftspvec);
  }

  Matrix<float> convmat(nchan_, nspec*(nspec-1)/2, 0.);
  vector<float> thisvec(nchan_, 0.);

  float minval, maxval;
  {//Debug
	  minMax(minval, maxval, shiftSpecmat);
	  os << LogIO::DEBUG1 << "Max/Min of input Matrix = (max: " << maxval
			  << ", min: " << minval << ")" << LogIO::POST;
  }

  os << LogIO::DEBUG1 <<  "starting deconvolution" << LogIO::POST;
  deconvolve(shiftSpecmat, spshift, rejlimit_, convmat);

  {//Debug
	  minMax(minval, maxval, convmat);
	  os << LogIO::DEBUG1 <<  "Max/Min of output Matrix = (max: " << maxval
			  << ", min: " << minval << ")" << LogIO::POST;
  }

  aggregateMat(convmat, thisvec);

  if (!otherside_) return thisvec;

  // subtract from the other side band.
  vector<float> othervec(nchan_, 0.);
  subtractFromOther(shiftSpecmat, thisvec, spshift, othervec);
  return Vector<float>(othervec);
};


void SideBandSeparatorBase::shiftSpectrum(const Vector<float> &invec,
				  double shift,
				  Vector<float> &outvec)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","shiftSpectrum()", WHERE));
  if (invec.size() != nchan_)
    throw(AipsError("Internal error. The length of input vector differs from nchan_"));
  if (outvec.size() != nchan_)
    throw(AipsError("Internal error. The length of output vector differs from nchan_"));

//  cout <<  "Start shifting spectrum for " << shift << " channels" << endl;

  // tweak shift to be in 0 ~ nchan_-1
  if ( fabs(shift) > nchan_ ) shift = fmod(shift, nchan_);
  if (shift < 0.) shift += nchan_;
  double rweight = fmod(shift, 1.);
  if (rweight < 0.) rweight += 1.;
  double lweight = 1. - rweight;
  uInt lchan, rchan;

  outvec = 0;
  for (uInt i = 0 ; i < nchan_ ; i++) {
    lchan = uInt( floor( fmod( (i + shift), nchan_ ) ) );
    if (lchan < 0.) lchan += nchan_;
    rchan = ( (lchan + 1) % nchan_ );
    outvec(lchan) += invec(i) * lweight;
    outvec(rchan) += invec(i) * rweight;

//    if (i == 2350 || i== 2930) {
//      cout << "Channel= " << i << " of input vector: " << endl;
//      cout << "L channel = " << lchan << endl;
//      cout << "R channel = " << rchan << endl;
//      cout << "L weight = " << lweight << endl;
//      cout << "R weight = " << rweight << endl;
//    }
  }
};


void SideBandSeparatorBase::shiftFlag(const Vector<bool> &invec,
				  double shift,
				  Vector<bool> &outvec)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","shiftFlag()", WHERE));
  if (invec.size() != nchan_)
    throw(AipsError("Internal error. The length of input vector differs from nchan_"));
  if (outvec.size() != nchan_)
    throw(AipsError("Internal error. The length of output vector differs from nchan_"));

//  cout << "Start shifting flag for " << shift << "channels" << endl;

  // shift is almost integer think it as int.
  // tolerance should be in 0 - 1
  double tolerance = 0.01;
  // tweak shift to be in 0 ~ nchan_-1
  if ( fabs(shift) > nchan_ ) shift = fmod(shift, nchan_);
  if (shift < 0.) shift += nchan_;
  double rweight = fmod(shift, 1.);
  bool ruse(true), luse(true);
  if (rweight < 0.) rweight += 1.;
  if (rweight < tolerance){
    // the shift is almost lchan
    ruse = false;
    luse = true;
  }
  if (rweight > 1-tolerance){
    // the shift is almost rchan
    ruse = true;
    luse = false;
  }
  uInt lchan, rchan;

  outvec = false;
  for (uInt i = 0 ; i < nchan_ ; i++) {
    lchan = uInt( floor( fmod( (i + shift), nchan_ ) ) );
    if (lchan < 0.) lchan += nchan_;
    rchan = ( (lchan + 1) % nchan_ );
    outvec(lchan) |= (invec(i) && luse);
    outvec(rchan) |= (invec(i) && ruse);

//    if (i == 2350 || i == 2930) {
//      cout << "Channel= " << i << " of input vector: " << endl;
//      cout << "L channel = " << lchan << endl;
//      cout << "R channel = " << rchan << endl;
//      cout << "L channel will be " << (luse ? "used" : "ignored") << endl;
//      cout << "R channel will be " << (ruse ? "used" : "ignored") << endl;
//    }
  }
};


void SideBandSeparatorBase::deconvolve(Matrix<float> &specmat,
			       const vector<double> shiftvec,
			       const double threshold,
			       Matrix<float> &outmat)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","deconvolve()", WHERE));
  if (specmat.nrow() != nchan_)
    throw(AipsError("Internal error. The length of input matrix differs from nchan_"));
  if (specmat.ncolumn() != shiftvec.size())
    throw(AipsError("Internal error. The number of input shifts and spectrum  differs."));

  float minval, maxval;
  {//Debug
	  minMax(minval, maxval, specmat);
	  os << LogIO::DEBUG1 <<  "Max/Min of input Matrix = (max: " << maxval
			  << ", min: " << minval << ")" << LogIO::POST;
  }

  uInt ninsp = shiftvec.size();
  outmat.resize(nchan_, ninsp*(ninsp-1)/2, 0.);
  Matrix<Complex> fftspmat(nchan_/2+1, ninsp, Complex(0.));
  Vector<float> rvecref(nchan_, 0.);
  Vector<Complex> cvecref(nchan_/2+1, Complex(0.));
  uInt icol = 0;
  unsigned int nreject = 0;

  os << LogIO::DEBUG1 <<  "Starting initial FFT. The number of input spectra = " << ninsp << LogIO::POST;
  os << LogIO::DEBUG1 <<  "out matrix has ncolumn = " << outmat.ncolumn() << LogIO::POST;

  for (uInt isp = 0 ; isp < ninsp ; isp++) {
    rvecref.reference( specmat.column(isp) );
    cvecref.reference( fftspmat.column(isp) );

    {//Debug
    	minMax(minval, maxval, rvecref);
    	os << LogIO::DEBUG1 << "Max/Min of inv FFTed Matrix = (max: " << maxval
    			<< ", min: " << minval << ")" << LogIO::POST;
    }

    fftsf.fft0(cvecref, rvecref, true);
  }

  //Liberate from reference
  rvecref.unique();

  Vector<Complex> cspec(nchan_/2+1, Complex(0.));
  const double PI = 6.0 * asin(0.5);
  const double nchani = 1. / (float) nchan_;
  const Complex trans(0., 1.);

  os << LogIO::DEBUG1 <<  "starting actual deconvolution" << LogIO::POST;
  for (uInt j = 0 ; j < ninsp ; j++) {
    for (uInt k = j+1 ; k < ninsp ; k++) {
      const double dx = (shiftvec[k] - shiftvec[j]) * 2. * PI * nchani;

      for (uInt ichan = 0 ; ichan < cspec.size() ; ichan++){
    	  cspec[ichan] = ( fftspmat(ichan, j) + fftspmat(ichan, k) )*0.5;
    	  double phase = dx*ichan;
    	  if ( fabs( sin(phase) ) > threshold){
    		  cspec[ichan] += ( fftspmat(ichan, j) - fftspmat(ichan, k) ) * 0.5
    				  * trans * cos(0.5*phase) / sin(0.5*phase);
//	                    * trans * sin(phase) / ( 1. - cos(phase) );
    	  } else {
    		  nreject++;
    	  }
      } // end of channel loop
//      os << LogIO::DEBUG1 <<  "done calculation of cspec" << LogIO::POST;

      Vector<Float> rspec;
      rspec.reference( outmat.column(icol) );

//      os << LogIO::DEBUG1 <<  "Starting inverse FFT. icol = " << icol << LogIO::POST;
      fftsi.fft0(rspec, cspec, false);

      icol++;
    }
  }

  {//Debug
	  minMax(minval, maxval, outmat);
	  os << LogIO::DEBUG1 <<  "Max/Min of inv FFTed Matrix = (max: " << maxval << ", min: " << minval << ")" << LogIO::POST;
  }

  os << LogIO::DEBUG1 << "Threshold = " << threshold
		  << ", Rejected channels = " << nreject << LogIO::POST;
};


void SideBandSeparatorBase::aggregateMat(const Matrix<float> &inmat,
				 vector<float> &outvec)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","aggregateMat()", WHERE));
  if (inmat.nrow() != nchan_)
    throw(AipsError("Internal error. The row numbers of input matrix differs from nchan_"));
//   if (outvec.size() != nchan_)
//     throw(AipsError("Internal error. The size of output vector should be equal to nchan_"));

  os << LogIO::DEBUG1 << "Averaging " << inmat.ncolumn() << " spectra in the input matrix."
     << LogIO::POST;

  const uInt nspec = inmat.ncolumn();
  const double scale = 1./( (double) nspec );
  // initialize values with 0
  outvec.assign(nchan_, 0);
  for (uInt isp = 0 ; isp < nspec ; isp++) {
    for (uInt ich = 0 ; ich < nchan_ ; ich++) {
      outvec[ich] += inmat(ich, isp);
    }
  }

  vector<float>::iterator iter;
  for (iter = outvec.begin(); iter != outvec.end(); iter++){
    *iter *= scale;
  }
};

void SideBandSeparatorBase::subtractFromOther(const Matrix<float> &shiftmat,
				      const vector<float> &invec,
				      const vector<double> &shift,
				      vector<float> &outvec)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","subtractFromOther()", WHERE));
  if (shiftmat.nrow() != nchan_)
    throw(AipsError("Internal error. The row numbers of input matrix differs from nchan_"));
  if (invec.size() != nchan_)
    throw(AipsError("Internal error. The length of input vector should be nchan_"));
  if (shift.size() != shiftmat.ncolumn())
    throw(AipsError("Internal error. The column numbers of input matrix != the number of elements in shift"));

  const uInt nspec = shiftmat.ncolumn();
  Vector<float> subsp(nchan_, 0.), shiftsub;
  Matrix<float> submat(nchan_, nspec, 0.);
  for (uInt isp = 0 ; isp < nspec ; isp++) {
    for (uInt ich = 0; ich < nchan_ ; ich++) {
      subsp(ich) = shiftmat(ich, isp) - invec[ich];
    }
    shiftsub.reference(submat.column(isp));
    // shift back spectrum so that other side stays still.
    shiftSpectrum(subsp, -shift[isp], shiftsub);
  }

  aggregateMat(submat, outvec);
};

bool SideBandSeparatorBase::interpolateMaskedChannels(Array<float> spectrum,
		const Array<bool> mask)
{
//	if (spectrum.ndim() != 1 || mask.ndim() != 1)
//		throw AipsError("Array arguments must be 1-dimension.");
	if (spectrum.size() != mask.size())
		throw AipsError("Size mismatch between spectrum and mask.");
	size_t num_chan = spectrum.size();
	float* spectrum_p = spectrum.data();
	const bool* mask_p = mask.data();
	// get the first and the last valid channel IDs.
	size_t ledge=0, redge=num_chan-1;
	while (!mask_p[ledge] && ledge < num_chan-1) {
		++ledge;
	}
	while (!mask_p[redge] && redge > 0) {
		--redge;
	}
	// Return if no valid channel
	if (redge < ledge) return false;
	// Nearest-neighbor for edges;
	for (size_t i = 0; i < ledge; ++i) spectrum_p[i] = spectrum_p[ledge];
	for (size_t i = num_chan-1; i > redge; --i) spectrum_p[i] = spectrum_p[redge];
	// Linear interpolation for intermediate gaps
	size_t mstart = -1, mend = -1, i0;
	float slope;
	for (size_t i = ledge+1; i < redge; ++i) {
		if (!mask_p[i]) {
			// the first masked channel
			mstart=i;
			// the last valid channel
			i0 = mstart-1;
			// search for the end of masked channel range
			while(!mask_p[i] && i < redge) {
				mend = i;
				++i;
			}
			// 'mend' points to the last masked channel
			// while 'i' points to the next valid channel
			slope = (spectrum_p[i] - spectrum_p[i0])/static_cast<float>(i-i0);
			for (size_t j = mstart; j < mend+1; ++j) {
				spectrum_p[j] = slope*static_cast<float>(j-i0) + spectrum_p[i0];
			}
			mstart = -1;
			mend = -1;
		}
	}
	return true;
}

Bool SideBandSeparatorBase::checkFile(const string name, string type)
{
  File file(name);
  if (!file.exists()){
    return false;
  } else if (type.empty()) {
    return true;
  } else {
    // Check for file type
    switch (std::tolower(type[0])) {
    case 'f':
      return file.isRegular(True);
    case 'd':
      return file.isDirectory(True);
    case 's':
      return file.isSymLink();
    default:
      throw AipsError("Invalid file type. Available types are 'file', 'directory', and 'symlink'.");
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////
SideBandSeparatorII::SideBandSeparatorII(const std::vector<std::string>& imagename)
: SideBandSeparatorBase (imagename)
{
	initLocal();
	checkImage();
};

void SideBandSeparatorII::initLocal() {
	refFreqPix_ = 0.0;
	refFreqHz_ = -1.0;
}

void SideBandSeparatorII::checkImage() {
  LogIO os(LogOrigin("SideBandSeparatorBase","setImage()", WHERE));

  // check image axes (npix, incr, ref, ndim)
  os << "Image axes check. Using the first image as the template" << LogIO::POST;
  CoordinateSystem csys0;
  IPosition npix0;
  if (!getImageCoordinate(inputNames_[0], csys0, npix0)) {
	  throw( AipsError("Invalid image "+inputNames_[0]) );
  }
  // Summary
  os << "Template image coordinate:" << LogIO::POST;
  os << "\tndim\t" << csys0.nCoordinates() << LogIO::POST;
  os << "\tAxes\t" << csys0.worldAxisNames() << LogIO::POST;
  os << "\tnPix\t" << npix0 << LogIO::POST;
  os << "\tRefPix\t" << csys0.referencePixel() << LogIO::POST;
  os << "\tRefPValt" << csys0.referenceValue() << LogIO::POST;
  os << "\tIncr\t" << csys0.increment() << LogIO::POST;

  for (size_t i = 1; i < inputNames_.size(); ++i){
	  if(!compareImageAxes(inputNames_[i], csys0, npix0))
		  throw( AipsError("Image axes mismatch: "+inputNames_[0]) );
  }
}

bool SideBandSeparatorII::getImageCoordinate(const string& imagename, CoordinateSystem &csys, IPosition &npix) {
	  LogIO os(LogOrigin("SideBandSeparatorBase","setImage()", WHERE));
	  auto ret = ImageFactory::fromFile(imagename);
      auto imageF = std::get<0>(ret);
      if (imageF) { //float image
		  os << "Found float image" << LogIO::POST;
		  npix = imageF->shape();
		  ImageMetaData<Float> immd(imageF);
		  vector<Int> myAxes;
		  csys =  immd.coordsys(myAxes);
		  return true;
	  } else if (std::get<1>(ret)) { // complex image
		  os << "Found complex image" << LogIO::POST;
		  return false;
	  } else if (std::get<2>(ret)) { // double image
		  os << "Found double image" << LogIO::POST;
		  return false;
	  } else if (std::get<3>(ret)) { // complex double image
		  os << "Found complex double image" << LogIO::POST;
		  return false;
	  } else {
		  os << LogIO::WARN << "Failed to open " << imagename << LogIO::POST;
		  return false;
	  }
}

bool SideBandSeparatorII::compareImageAxes(const string& imagename, const CoordinateSystem &refcsys, const IPosition &refnpix)
{
	CoordinateSystem csys;
	IPosition npix;
	if (!getImageCoordinate(imagename, csys, npix)) {
		throw( AipsError("Invalid image "+imagename) );
	}
	uInt ndim = refcsys.nWorldAxes();
	if (csys.nWorldAxes() != ndim || refnpix.size() != ndim || npix.size() != ndim) {
		return false;
	}
	bool match = true;
	constexpr Double frac_tol = 0.1;
	for (uInt i = 0; i<refcsys.nCoordinates(); ++i) {
		Double tolerance = frac_tol*abs(refcsys.increment()[i]);
		match &= (npix[i]==refnpix[i]); // npix
		match &= (abs(csys.increment()[i]-refcsys.increment()[i]) < tolerance); // incr
		if (refcsys.type(i) == Coordinate::SPECTRAL) continue; // skip channel comparison
		match &= (abs(csys.referencePixel()[i]-refcsys.referencePixel()[i]) < frac_tol); // refpix
		match &= (abs(csys.referenceValue()[i]-refcsys.referenceValue()[i]) < tolerance); // refval
	}
	return match;
}

void SideBandSeparatorII::setImageBandFrequency(const double refpix, const Quantity refval)
{
    LogIO os(LogOrigin("SideBandSeparatorBase","setImageBandFrequency()", WHERE));
	double freq_hz = refval.getValue("Hz", True);
	if (freq_hz < 0.0) throw( AipsError("Frequency should be positive") );
	refFreqPix_ = refpix;
	refFreqHz_ = freq_hz;
	os << "Setting frequency axis of image band: " << refFreqHz_*1.e-9
			<< "GHz at channel " << refFreqPix_ << LogIO::POST;
}

void SideBandSeparatorII::separate(const string& outfile, const bool overwrite)
{
  LogIO os(LogOrigin("SideBandSeparatorBase","separate()", WHERE));
  /*** Check outputs ***/
  if (outfile.size() == 0)
	  throw( AipsError("Output file name is undefined."));
  string const signame = outfile + ".signalband";
  string const imgname = outfile + ".imageband";
  if (checkFile(signame, "d") && !overwrite) {
		  throw( AipsError("Image "+signame+" already exists.") );
  }
  if (doboth_ && checkFile(imgname, "d") && !overwrite) {
	  throw( AipsError("Image "+imgname+" already exists.") );
  }

  /*** Set up channel shift of image and signal sideband ***/
  nshift_ = setupShift();
  if (nshift_ < 2)
    throw( AipsError("At least 2 IFs are necessary for convolution.") );
  if (nshift_ != inputNames_.size())
	    throw( AipsError("Internal error: nshift_ and image number differs.") );

  /*** Open input images (data model dependent) ***/
  vector<SPIIF> images(nshift_);
  for (size_t i = 0; i < nshift_; ++i) {
	  auto ret = ImageFactory::fromFile(inputNames_[i]);
      auto imageF = std::get<0>(ret);
      if (! imageF)
		  throw( AipsError("Float image not found in "+inputNames_[i]) );
	  images[i] = imageF;
  }
  /*** analyze axis of reference image (data model dependent) ***/
  SPIIF refImage = images[0];
  IPosition const npix = refImage->shape();
  uInt const ndim = npix.size();
  ImageMetaData<Float> const immd(refImage);
  vector<Int> myAxes;
  CoordinateSystem const csys =  immd.coordsys(myAxes);
  if (!csys.hasSpectralAxis())
	  throw( AipsError("Could not find spectral axis.") );
  Int spax_id = csys.spectralAxisNumber();
  nchan_ = npix[spax_id];
  IPosition specArrayShape(ndim, 1), specVecShape(1, nchan_);
  specArrayShape[spax_id] = nchan_;

  /*** prepare output image (data model dependent) ***/
  SPIIF sigImg, imgImg;
  sigImg = ImageFactory::createImage<Float>(signame, csys, npix, True, overwrite, nullptr);
  if (doboth_) {
	  /*** define frequency coordinate of image sideband ***/
	  CoordinateSystem imgcsys(csys);
	  if (refFreqHz_ < 0.0) {
		  os << LogIO::WARN << "Frequency information of image sideband is not set. Using the value of signal sideband" << LogIO::POST;
	  } else {
		  Int spax_id = imgcsys.spectralAxisNumber();
		  Vector<Double> refpix = imgcsys.referencePixel();
		  Vector<Double> refval = imgcsys.referenceValue();
		  Vector<Double> incr = imgcsys.increment();
		  refpix[spax_id] = refFreqPix_;
		  refval[spax_id] = refFreqHz_;
		  incr[spax_id] *= -1.0;
		  imgcsys.setReferencePixel(refpix);
		  imgcsys.setReferenceValue(refval);
		  imgcsys.setIncrement(incr);
	  }
	  imgImg = ImageFactory::createImage<Float>(imgname, imgcsys, npix, True, overwrite, nullptr);
  }

  Matrix<float> specMat(nchan_, nshift_);
  Matrix<bool> maskMat(nchan_, nshift_);
  Array<float> sigSpec(specVecShape), imgSpec(specVecShape);
  Array<bool> sigMask(specVecShape), imgMask(specVecShape);
  vector<uInt> dataIdvec;

  /*** Generate FFTServer ***/
  fftsf.resize(IPosition(1, nchan_), FFTEnums::REALTOCOMPLEX);
  fftsi.resize(IPosition(1, nchan_), FFTEnums::COMPLEXTOREAL);

  /*** Loop over image pixel and separate sideband (data model dependent) ***/
  IPosition start(ndim), end(ndim), stride(ndim);
  for (int ipos = 0; ipos < npix.product()/nchan_; ipos++){
	  dataIdvec.resize(0);
	  /*** convert 1-D ipos to slicer in image coordinate ***/
	  uInt denominator = 1;
	  for (uInt i = 0; i < ndim; ++i) {
		  if ((Int) i == spax_id) {
			  start(i) = 0;
			  end(i) = nchan_-1;
			  stride(i) = 1;
			  denominator *= nchan_;
		  } else {
			  uInt pos = ((ipos/denominator) % npix[i]);
			  start(i) = pos;
			  end(i) = pos;
			  stride(i) = npix[i];
			  denominator *= npix[i];
		  }
	  }
	  Slicer const specSlicer(start, end, stride, Slicer::endIsLast);
    /*** Get a set of spectra to solve (data model dependent) ***/
    if (!getSpectraToSolve(images, specSlicer, specMat, maskMat, dataIdvec)){
    	sigSpec = 0.0f;
    	imgSpec = 0.0f;
    	sigMask = false;
    	imgMask = false;
    } else {
        /*** Solve signal sideband ***/
        sigSpec = solve(specMat, dataIdvec, true);
        /*** Aggregate channel mask  ***/
        sigMask = collapseMask(maskMat, dataIdvec, true);
        if (doboth_) {
      	  /*** Solve image sideband  ***/
          imgSpec = solve(specMat, dataIdvec, false);
          /*** Aggregate channel mask  ***/
          imgMask = collapseMask(maskMat, dataIdvec, false);
        }
    }
    /*** Signal side band ***/
    /*** now assign spec and mask to output image (data model dependent) ***/
    sigImg->putSlice(sigSpec.reform(specArrayShape), start, stride);
    /*** need to create a pixel mask if not any (data model dependent) ***/
    if (! sigImg->hasPixelMask()) {
        casacore::String maskName("");
        ImageMaskAttacher::makeMask(*sigImg, maskName, true, true, os, true);
    }
    sigImg->pixelMask().putSlice(sigMask.reform(specArrayShape), start, stride);
    /*** Image side band ***/
    if (doboth_) {
        /*** now assign spec and mask to output image  (data model dependent) ***/
    	imgImg->putSlice(imgSpec.reform(specArrayShape), start, stride);
        /*** need to create a pixel mask if not any  (data model dependent) ***/
        if (! imgImg->hasPixelMask()) {
            casacore::String maskName("");
            ImageMaskAttacher::makeMask(*imgImg, maskName, true, true, os, true);
        }
    	imgImg->pixelMask().putSlice(imgMask.reform(specArrayShape), start, stride);
    }
  } // end of row loop
  /*** Finally, save tables on disk (data model dependent) ***/
  sigImg.reset();
  imgImg.reset();

}

bool SideBandSeparatorII::getSpectraToSolve(const vector<SPIIF> &images, const Slicer &slicer,
		  Matrix<float>& specMat, Matrix<bool>& maskMat, vector<uInt>& imgIdvec){
	imgIdvec.resize(0);
	specMat.resize(nchan_, nshift_);
	maskMat.resize(nchan_, nshift_);
	Array<float> spec(IPosition(1,nchan_));
	Array<bool> mask(IPosition(1,nchan_));
	size_t nspec = 0;
	for (size_t i = 0; i < images.size(); ++i) {
		images[i]->getSlice(spec, slicer, False);
		images[i]->getMaskSlice(mask, slicer, False);
		// Do interpolation of masked channels.
		// The method return false if there is no valid data (mask is true for valid pixels)
		// Skip if no valid channel in this spectrum
		if (!interpolateMaskedChannels(spec, mask)) continue;
		// register this spectrum and mask to matrices to solve
		specMat.column(nspec) = spec;
		maskMat.column(nspec) = mask;
		imgIdvec.push_back((uInt) i);
		++nspec;
	} // end of image loop
	if (nspec < nshift_) {
		specMat.resize(nchan_, nspec, true);
		maskMat.resize(nchan_, nspec, true);
	}
	return nspec > 0;
}

} //# NAMESPACE CASA - END
