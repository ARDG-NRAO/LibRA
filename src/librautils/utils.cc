#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <casacore/images/Images/PagedImage.h>
#include <utils.h>
#include <sys/stat.h>
#include <stdcasa/Quantity.h>
#include <algorithm>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>  // For checkAndGetImageInfo()
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>    // For checkAndGetImageInfo()


using namespace std;
using namespace casacore;
namespace librautils
{
  bool imageExists(const std::string &img)
  {
      // Check if image exists and is readable
      Directory dirObj(img);
      if (img.empty() || !dirObj.exists() || !dirObj.isReadable())
      {
          LogIO log_l(LogOrigin("LibraUtils", "imageExists"));
          std::string msg = "Image \"" + img + "\" not found.";
          log_l << msg << LogIO::WARN;
          return false; // Return false if image does not exist
      }
      return true;
  }

    void printImageMax(const std::string &name, LogIO &logio)
    {
        PagedImage<float> target(name);
        checkImageMax(name, target, logio);
    }

    void printImageMax(const vector<string> &nameList, LogIO &logio)
    {
        for (auto name : nameList)
        {
            PagedImage<float> target(name);
            checkImageMax(name, target, logio);
        }
    }
    std::string removeExtension(const std::string& path) {
        if (path == "." || path == "..")
          return path;
      
        size_t pos = path.find_last_of("\\/.");
        if (pos != std::string::npos && path[pos] == '.')
          return path.substr(0, pos);
      
        return path;
    }

    std::string getExtension(const std::string& path)
    {
      string ext("");
      size_t pos = path.find_last_of("\\/.");
      if (pos != std::string::npos)
        {
      string ss=path.substr(pos);
      if ((pos=ss.find_last_of("."))!=std::string::npos)
        ext=ss.substr(pos + 1);
        }
  
      return ext;
    }

    // File system utilities
    bool pathExists(const std::string &path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    // Frequency processing utilities
    double parseRefFreq(LogIO &log_l, const std::string &reffreq_str)
    {
        casacore::Quantity reffreq_qa;
        if (!casacore::Quantity::read(reffreq_qa, reffreq_str))
        {
            log_l << "ERROR: Could not parse reference frequency string: " << reffreq_str << LogIO::SEVERE << LogIO::POST;
            log_l << "       Please provide a valid quantity string (e.g., '1.4GHz')." << LogIO::EXCEPTION;
        }
        return reffreq_qa.getValue("Hz");
    }

    std::vector<double> computeMidFrequencies(const std::vector<std::vector<double>> &freqs)
    {
        std::vector<double> midFreqs;
        midFreqs.reserve(freqs.size());

        for (const auto &f_list : freqs)
        {
            if (!f_list.empty())
            {
                std::vector<double> sortedFreqs = f_list;
                std::sort(sortedFreqs.begin(), sortedFreqs.end());
                size_t n = sortedFreqs.size();
                double mid;
                if (n % 2 == 0)
                    mid = (sortedFreqs[n / 2 - 1] + sortedFreqs[n / 2]) / 2.0;
                else
                    mid = sortedFreqs[n / 2];
                midFreqs.push_back(mid);
            }
            else
            {
                midFreqs.push_back(0.0);
            }
        }
        return midFreqs;
    }

    std::vector<double> computeWeights(const std::vector<double> &midFreqs, double refval)
    {
        std::vector<double> weights;
        weights.reserve(midFreqs.size());
        for (const auto &midFreq : midFreqs)
        {
            weights.push_back((midFreq - refval) / refval);
        }
        return weights;
    }

    // Image validation utilities
    bool findSpectralAxis(const casacore::CoordinateSystem &coords,
                          const casacore::IPosition &shape,
                          int &spectralCoordIndex, int &spectralPixelAxis)
    {
        spectralCoordIndex = -1;
        spectralPixelAxis = -1;

        int coordIdx = coords.findCoordinate(casacore::Coordinate::SPECTRAL);
        if (coordIdx < 0)
        {
            return false;
        }

        casacore::Vector<casacore::Int> pixelAxes = coords.pixelAxes(coordIdx);
        if (pixelAxes.nelements() == 0 || pixelAxes[0] < 0 ||
            pixelAxes[0] >= static_cast<int>(shape.size()))
        {
            // Spectral coordinate exists but is not attached to a pixel axis of
            // this shape (removed, or a degenerate/mismatched image).
            spectralCoordIndex = coordIdx;
            return false;
        }

        spectralCoordIndex = coordIdx;
        spectralPixelAxis = pixelAxes[0];
        return true;
    }

    void checkShapesConsistency(LogIO &log_l, const std::vector<std::vector<int>> &shapes,
                               const std::string &imageType)
    {
        if (shapes.empty())
        {
            return;
        }

        for (size_t i = 1; i < shapes.size(); ++i)
        {
            if (shapes[i] != shapes[0])
            {
                log_l << "ERROR: " << imageType << " images do not have the same shape." << LogIO::SEVERE << LogIO::POST;
                log_l << "       Shape of " << i << "-th image: [";
                for (int dim : shapes[i]) log_l << dim << " ";
                log_l << "]" << LogIO::SEVERE << LogIO::POST;
                log_l << "       Shape of 0-th image: [";
                for (int dim : shapes[0]) log_l << dim << " ";
                log_l << "]" << LogIO::SEVERE << LogIO::POST;
                log_l << "       All " << imageType << " images must have identical dimensions." << LogIO::EXCEPTION;
            }
        }
    }

    void checkAndGetImageInfo(LogIO &log_l, const std::vector<std::string> &names,
                             const std::string &imageType,
                             std::vector<std::vector<int>> &shapes,
                             std::vector<std::vector<double>> &freqs,
                             bool isTaylorCube, bool zeroImageContent)
    {
        shapes.clear();
        freqs.clear();

        for (const auto &name : names)
        {
            if (isTaylorCube && name.find(".tt") != std::string::npos)
            {
                log_l << "ERROR: The " << imageType << " argument contains a Taylor image name (" << name << ")." << LogIO::SEVERE << LogIO::POST;
                log_l << "       Please ensure " << imageType << " contains only cube image filenames, not Taylor image filenames." << LogIO::EXCEPTION;
            }

            if (!imageExists(name))
            {
                log_l << imageType << " image " << name << " does not exist on disk." << LogIO::EXCEPTION;
            }
            else
            {
                if (!isTaylorCube && !zeroImageContent) {
                     log_l << imageType << " image: " << name << LogIO::NORMAL << LogIO::POST;
                }

                try {
                    // Open the image to get its properties
                    casacore::PagedImage<float> image(name);

                    // Get shape
                    casacore::IPosition casacoreShape = image.shape();
                    std::vector<int> stdShape(casacoreShape.size());
                    for (size_t i = 0; i < casacoreShape.size(); ++i) {
                        stdShape[i] = casacoreShape[i];
                    }
                    shapes.push_back(stdShape);

                    // Get frequencies (if a spectral coordinate exists)
                    std::vector<double> currentFreqs;
                    casacore::CoordinateSystem coords = image.coordinates();
                    int spectralCoordIndex = -1, spectralPixelAxis = -1;
                    if (findSpectralAxis(coords, casacoreShape, spectralCoordIndex, spectralPixelAxis)) {
                        casacore::SpectralCoordinate specCoord = coords.spectralCoordinate(spectralCoordIndex);
                        int nChannels = casacoreShape[spectralPixelAxis];
                        for (int ch = 0; ch < nChannels; ++ch) {
                            double freq;
                            if (specCoord.toWorld(freq, double(ch))) {
                                currentFreqs.push_back(freq);
                            } else {
                                currentFreqs.push_back(0.0); // fallback if conversion fails
                            }
                        }
                    } else {
                        log_l << "WARNING: " << imageType << " image " << name
                              << " has no usable spectral pixel axis; its frequency list will be empty."
                              << LogIO::WARN << LogIO::POST;
                    }
                    freqs.push_back(currentFreqs);

                    if (zeroImageContent)
                    {
                        log_l << "Initializing " << imageType << " image: " << name << " to zero." << LogIO::NORMAL << LogIO::POST;
                        casacore::Array<float> imageArray(image.shape());
                        imageArray.set(0.0f);
                        image.put(imageArray);
                        image.unlock(); // Explicitly unlock if the image was locked on open
                        image.flush();
                    } else {
                        // For read-only images, ensure they are unlocked
                        image.unlock();
                    }

                } catch (const casacore::AipsError& e) {
                    log_l << "Error accessing image " << name << ": " << e.what() << LogIO::SEVERE << LogIO::POST;
                    log_l << "Failed to get info or initialize " << imageType << " image " << name << LogIO::EXCEPTION;
                }
            }
        }
    }

    // Pattern helper classes implementations
    void ImageValidator::validateAndGetInfo(LogIO &log_l, const std::vector<std::string> &names,
                                          const std::string &imageType,
                                          std::vector<std::vector<int>> &shapes,
                                          std::vector<std::vector<double>> &freqs,
                                          bool isTaylorCube, bool zeroImageContent)
    {
        checkAndGetImageInfo(log_l, names, imageType, shapes, freqs, isTaylorCube, zeroImageContent);
        checkShapesConsistency(log_l, shapes, imageType);
    }

    double FrequencyProcessor::parseFrequency(LogIO &log_l, const std::string &freq_str)
    {
        return parseRefFreq(log_l, freq_str);
    }

    std::vector<double> FrequencyProcessor::computeMidFrequencies(const std::vector<std::vector<double>> &freqs)
    {
        return librautils::computeMidFrequencies(freqs);
    }

    std::vector<double> FrequencyProcessor::computeWeights(const std::vector<double> &midFreqs, double refval)
    {
        return librautils::computeWeights(midFreqs, refval);
    }

    void FrequencyProcessor::validateFrequencyRange(LogIO &log_l, double refval, const std::string &reffreq_str,
                                                    const std::vector<double> &midFreqs)
    {
        if (midFreqs.empty()) return;

        double minF = *std::min_element(midFreqs.begin(), midFreqs.end());
        double maxF = *std::max_element(midFreqs.begin(), midFreqs.end());

        if (refval < minF || refval > maxF)
        {
            log_l << "WARNING: Reference frequency (" << reffreq_str << " = " << refval/1e9 << " GHz) "
                  << "is outside the range of cube frequencies [" << minF/1e9 << ", " << maxF/1e9 << "] GHz."
                  << LogIO::WARN << LogIO::POST;
        }
    }

    // Template specializations for ImageOperations
    template<>
    void ImageOperations<float>::applyLatticeExpression(ImageInterface<float>& image, const LatticeExpr<float>& expr)
    {
        image.copyData(expr);
        image.flush();
        image.unlock();
    }

    template<>
    void ImageOperations<float>::initializeToZero(const std::string& imgName, const IPosition& shape, 
                                                 const CoordinateSystem& coord)
    {
        if (!imageExists(imgName)) {
            PagedImage<float> image(shape, coord, imgName);
            Array<float> array(shape);
            array.set(0.0f);
            image.put(array);
            image.flush();
            image.unlock();
        }
    }

  std::string computeReferenceFrequency(const std::string& refFreqStr,
					const std::vector<double>& freqList,
					double& refFreqHz)
  {
    refFreqHz = 0;
    string newRefFreqStr(refFreqStr);
    if (refFreqStr == "mean")
      {
	refFreqHz = librautils::computeMean(freqList);
	newRefFreqStr = std::to_string(refFreqHz) + "Hz";
      }
    else if (refFreqStr == "median")
      {
	refFreqHz = librautils::computeMedian(freqList);
	newRefFreqStr = std::to_string(refFreqHz) + "Hz";
      }
    return newRefFreqStr;
  }

}; // namespace librautils
