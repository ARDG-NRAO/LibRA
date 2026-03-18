#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <casacore/images/Images/PagedImage.h>
#include <utils.h>


using namespace std;
using namespace casacore;
namespace librautils
{
    bool imageExists(const std::string &imagename)
    {
        Directory image(imagename);
        return image.exists();
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
    double parseRefFreq(LogIO &log_l, const std::string &reffreq_str)
    {
        casacore::Quantity reffreq_qa;
        if (!casacore::Quantity::read(reffreq_qa, reffreq_str))
        {
            log_l << "Could not parse reference frequency string: " << reffreq_str << LogIO::SEVERE << LogIO::POST;
            log_l << "Please provide a valid quantity string (e.g., '1.4GHz')." << LogIO::EXCEPTION;
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
                std::vector<double> sorted = f_list;
                std::sort(sorted.begin(), sorted.end());
                size_t n = sorted.size();
                double mid = (n % 2 == 0)
                    ? (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0
                    : sorted[n / 2];
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
        for (const auto &f : midFreqs)
            weights.push_back((f - refval) / refval);
        return weights;
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
