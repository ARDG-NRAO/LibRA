// # Copyright (C) 2021, 2026
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$


#include <dale.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <casacore/casa/OS/DirectoryIterator.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>
#include <librautils/utils.h>

//
//-------------------------------------------------------------------------
//

namespace Dale
{
  void printImageMax(const string& imType,
		     const ImageInterface<float>& target,
		     const ImageInterface<float>& weight,
		     const ImageInterface<float>& sumwt,
		     LogIO& logio,
		     const string& when)
  {
    float sow = max(sumwt.get());
    float mwt = max(weight.get());
    float mim = max(target.get());
    
    {
      stringstream os;
      os << fixed << setprecision(numeric_limits<float>::max_digits10)
	 << "Image values " << when << " normalization: "
	 << "sumwt = " << sow << ", max(weight) = "
	 << mwt << ", max(" << imType << ") = " << mim;
      logio << os.str() << LogIO::POST;
    }
  }
  //
  //-------------------------------------------------------------------------
  //
  template <class T>
  void compute_pb(const string& pbName,
		  const ImageInterface<T>& weight,
		  const ImageInterface<T>& sumwt,
		  const float& pblimit,
		  LogIO& logio)
  {
    double itsPBScaleFactor = sqrt(max(weight.get()));
    LatticeExpr<T> norm_pbImage = sqrt(abs(weight)) / itsPBScaleFactor;
    LatticeExpr<T> pbImage = iif(norm_pbImage > fabs(pblimit), norm_pbImage, 0.0);
    PagedImage<T> tmp(weight.shape(), weight.coordinates(), pbName);
    tmp.copyData(pbImage);
    float mpb = max(tmp.get());
    stringstream os;
    os << fixed << setprecision(numeric_limits<float>::max_digits10)
       << "Max PB value is " << mpb;
    logio << os.str() << LogIO::POST;
  }
  //
  //-------------------------------------------------------------------------
  //
  // Normalizer for residual and model images.
  template <class T>
  void normalizeModel(const std::string& name,
		      ImageInterface<T>& target,
		      ImageInterface<T>& weight,
		      ImageInterface<T>& sumwt,
		      const float& pblimit,
		      LogIO& logio)
  {
    // weight = weight / SoW
    // itsPBScaleFactor = max(weight)
    // residual = residual / (Sow * (sqrt(weight) * itsPBScaleFactor))
    // model = model / (sqrt(weight) / itsPBScaleFactor)
    LatticeExpr<T> newTarget = LatticeExpr<T> (target);
    
    IPosition pos(4,0,0,0,0);
    T SoW = sumwt.getAt(pos);
    LatticeExpr<T> normalizedWeight = weight / SoW;
    //    weight.copyData(normalizedWeight);
    double itsPBScaleFactor = sqrt(max(weight.get()));
    
    LatticeExpr<T> ratio, deno;
    deno = sqrt(abs(weight)) / itsPBScaleFactor;
    
    stringstream os;
    os << fixed << setprecision(numeric_limits<float>::max_digits10)
       << "Dividing " << name << " by [ sqrt(weightimage) / "
       << itsPBScaleFactor << " ] to get to flat sky model before prediction.";
    logio << os.str() << LogIO::POST;
    
    
    float scalepb=fabs(pblimit);
    LatticeExpr<T> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
    LatticeExpr<T> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
    ratio = ((newTarget) * mask / (deno + maskinv));
    
    string newModelName = name + ".divmodel";
    PagedImage<T> tmp(weight.shape(), weight.coordinates(), newModelName);
    tmp.copyData(ratio);
    printImageMax(string("model"), tmp, weight, sumwt, logio, "after");
  }
  //
  //-------------------------------------------------------------------------
  //
  template <class T>
  void normalize(const std::string& imageName,
		 PagedImage<T>& target,
		 PagedImage<T>& weight,
		 PagedImage<T>& sumwt,
		 const std::string& imType,
		 const float& pblimit,
		 const bool normalize_weight,
		 LogIO& logio,
		 const float taylorWtValue)
  // normtype, nterms, facets, psfcutoff and restoring beam are not functional yet, will add to interface as needed
  // Normalization equations implemented in this version:
  // pb = sqrt(weight / max(weight))
  // psf = psf / max(psf)
  // weight = weight / SoW
  // itsPBScaleFactor = max(weight)
  // residual = residual / (Sow * (sqrt(weight) * itsPBScaleFactor))
  // model = model / (sqrt(weight) / itsPBScaleFactor)
  {
    //string targetName = imageName + "." + imType;
    
    IPosition pos(4,0,0,0,0);
    float SoW = sumwt.getAt(pos);

    // Taylor path: normalize this term's PSF by the tt0 PSF peak, supplied as
    // taylorWtValue, not by its own peak.  psf.tt_k and residual.tt_k are the
    // two sides of one set of normal equations, and the ratio between terms
    // is what carries the spectral index; scaling each term by its own peak
    // would destroy it.  A term's own weight, sum(w*fac_k), is a small signed
    // number and is emphatically not the right divisor.  taylorWtValue is
    // legitimately negative for odd terms, so NaN (not sign) selects the
    // ordinary, non-Taylor behaviour.
    //
    // Only the PSF needs this.  The residual branch below is already
    // term-independent: SoW cancels out of it exactly (newIM/deno reduces to
    // target/sqrt(weight*max(weight)), and the pblimit mask reduces to
    // sqrt(weight/max(weight)) > pblimit), and the weight image it does
    // depend on is gridded once at term 0.
    const bool useTaylorWt = ! std::isnan(taylorWtValue);

    if ((imType == "psf") || (imType == "taylorpsf"))
      {
	float psfFactor = useTaylorWt ? taylorWtValue : max(target.get());
	if (useTaylorWt)
	  logio << "Taylor PSF normalization by tt0 peak " << psfFactor
		<< " (this term's own peak is " << max(target.get()) << ")"
		<< LogIO::POST;
	LatticeExpr<T> newPSF = target / psfFactor;
	target.copyData(newPSF);
	// taylorpsf: emit the PSF peak to a dedicated .taylorwt image, leaving
	// the gridding sumwt untouched. HPG getSumWeights() = raw Σ w_ij (flat
	// across SPWs), which roadrunner writes to .sumwt and rewrites every
	// residual pass. The PSF peak = Σ w_ij |A_ij|²_max is frequency-varying
	// and matches what CASA awproject holds in memory as sumwt; taylor's
	// cube2taylor must weight by this for correct MTMFS spectral terms.
	// Keeping it in a separate file makes it the stable per-SPW weight that
	// no major-cycle roadrunner pass clobbers.
	if (imType == "taylorpsf")
	  {
	    string taylorWtName = librautils::removeExtension(imageName) + ".taylorwt";
	    casacore::File twf(taylorWtName);
	    if (twf.exists()) casacore::Directory(taylorWtName).removeRecursive();
	    PagedImage<T> taylorWt(sumwt.shape(), sumwt.coordinates(), taylorWtName);
	    casacore::Array<T> sowArr(sumwt.shape(), (T)psfFactor);
	    taylorWt.put(sowArr);
	    taylorWt.flush();
	    stringstream os;
	    os << "taylorpsf: wrote PSF peak=" << psfFactor << " to "
	       << taylorWtName << " (gridding sumwt SoW=" << SoW << " left intact)";
	    logio << os.str() << LogIO::POST;
	    std::cerr << "[dale] " << os.str() << "\n";
	  }
      }
    else if ((imType == "residual") || (imType == "model"))
      {
	LatticeExpr<T> newIM(target), normWt(weight);

	if (! librautils::isNormalized(weight)) normWt = weight / SoW;
	
	double itsPBScaleFactor = sqrt(max(normWt.get()));
	if (imType == "residual") newIM = target / SoW;
	
	LatticeExpr<T> ratio, deno;
	float scalepb = 1.0;
	if (imType == "residual")
	  {
	    deno = sqrt(abs(normWt)) * itsPBScaleFactor;
	    stringstream os;
	    os << fixed << setprecision(numeric_limits<float>::max_digits10)
	       << "Dividing " << imageName << " by [ sqrt(weightimage) * "
	       << itsPBScaleFactor << " ] to get flat noise with unit pb peak."
	       << " SoW=" << SoW
	       << " itsPBScaleFactor=" << itsPBScaleFactor
	       << " SoW*itsPBScaleFactor=" << SoW * itsPBScaleFactor;
	    logio << os.str() << LogIO::POST;
	    std::cerr << "[dale] " << os.str() << "\n";

	    scalepb=fabs(pblimit)*itsPBScaleFactor*itsPBScaleFactor;
	  }
	else if (imType == "model")
	  {
	    deno = sqrt(abs(normWt)) / itsPBScaleFactor;

	    stringstream os;
	    os << fixed << setprecision(numeric_limits<float>::max_digits10)
	       << "Dividing " << imageName << " by [ sqrt(weightimage) / "
	       << itsPBScaleFactor << " ] to get to flat sky model before prediction."
	       << " SoW=" << SoW
	       << " itsPBScaleFactor=" << itsPBScaleFactor
	       << " SoW*itsPBScaleFactor=" << SoW * itsPBScaleFactor;
	    logio << os.str() << LogIO::POST;
	    std::cerr << "[dale] " << os.str() << "\n";

	    scalepb=fabs(pblimit);
	  }
	
	LatticeExpr<T> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
	LatticeExpr<T> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
	ratio = ((newIM) * mask / (deno + maskinv));
	
	if (imType == "residual")
          target.copyData(ratio);
	else
	  {
	    string newModelName = librautils::removeExtension(imageName) + ".divmodel";
	    PagedImage<T> tmp(normWt.shape(), weight.coordinates(), newModelName);
	    tmp.copyData(ratio);
	    printImageMax(imType, tmp, weight, sumwt, logio, "after");
	  }
      }
  }
  //
  //-------------------------------------------------------------------------
  //
  template <class T>
  std::unique_ptr<PagedImage<T>> checkAndOpen(const string& name)
  {
    if (! librautils::imageExists(name))
      throw(AipsError("Image " + name + " does not exist."));
    
    LatticeBase *imPtr;
    
    imPtr = ImageOpener::openImage (name);

    std::unique_ptr<PagedImage<T>> sptr(dynamic_cast<PagedImage<T>*>(imPtr));
    return sptr;
  }
  //
  //-------------------------------------------------------------------------
  //
  void dale(const std::string& imageName,
	    const std::string& wtImageName,
	    const std::string& sowImageName,
	    const std::string& normtype,
	    const std::string& imType,
	    const float& pblimit, 
	    //const float& psfcutoff,
	    const bool& computePB,
	    const std::string& taylorWtName)
  //const bool& normalize_weight)
  {
    float psfcutoff=0.35;
    bool normalize_weight=false;
    //
    //---------------------------------------------------
    //
    string type="", subType="", targetName=imageName, weightName=wtImageName, sumwtName=sowImageName, pbName="";
    
    LogIO logio(LogOrigin("Dale","dale"));
    
    try
      {
	targetName = imageName;
	if ((imType == "residual") || (imType == "psf") || (imType == "taylorpsf") || (imType == "model"))
	  {
	    // Use name extension conventions only if targetName did not have an extension.
	    // taylorpsf operates on the .psf image (same file, extra sumwt update).
	    string extToAppend = (imType == "taylorpsf") ? "psf" : imType;
	    if ( librautils::getExtension(targetName)=="") targetName += "." + extToAppend;
	    logio << "Running normalization for " << targetName << LogIO::POST;
	  }
	else
	  throw(AipsError("Unrecognized imtype (" + imType +"). Allowed values are psf, taylorpsf, residual or model."));
	
	// Use a convention for image names only if the names aren't provided.
	if (weightName == "") weightName   = librautils::removeExtension(targetName) + ".weight";
	if (sumwtName  == "") sumwtName    = librautils::removeExtension(targetName) + ".sumwt";
	
	{
	  Table table(targetName,TableLock(TableLock::AutoNoReadLocking));
	  TableInfo& info = table.tableInfo();
	  type = info.type();
	  if (type != "Image")
	    throw(AipsError("imagename does not point to an image."));
	}
	// checking if all necessary images exist before opening
        std::unique_ptr<PagedImage<float>> targetImage = checkAndOpen<float>(targetName);
	if (! librautils::isNormalized<float>(*targetImage))
	  {
            std::unique_ptr<PagedImage<float>> wImage = checkAndOpen<float>(weightName);
            std::unique_ptr<PagedImage<float>> swImage = checkAndOpen<float>(sumwtName);
	
	    //PagedImage<float> pw(*wImage);
	    //	string wtype=PagedImage<float>(*wImage).table().tableInfo().type();
	    librautils::getImageType<float>(*targetImage, type, subType);
	    logio << "Target image type: "
		  << type << " "
		  << "subType: " << subType << " "
		  << targetImage->name() << " "
		  << endl;
	
	    printImageMax(imType, *targetImage, *wImage, *swImage, logio, "before");
	    // Read the tt0 weight (the term-0 PSF peak roadrunner wrote) if one
	    // was named; NaN keeps the ordinary, non-Taylor normalization. This
	    // value is legitimately negative for odd Taylor terms, so it cannot
	    // be validated by sign; only its presence matters.
	    float taylorWtValue = std::numeric_limits<float>::quiet_NaN();
	    if (taylorWtName != "")
	      {
		PagedImage<float> twImage(taylorWtName);
		taylorWtValue = twImage.getAt(IPosition(4,0,0,0,0));
	      }
	    normalize<float>(targetName, *targetImage, *wImage, *swImage, imType, pblimit, normalize_weight, logio, taylorWtValue);
	    printImageMax(imType, *targetImage, *wImage, *swImage, logio, "after");
	
	    librautils::setNormalized<float>(*targetImage);
	    librautils::getImageType<float>(*targetImage, type, subType);
	    // cerr << "Target image type: "
	    // 	 << type << " "
	    // 	 << subType << " "
	    // 	 << targetImage->name() << " "
	    // 	 << endl;
	    if (computePB)
	      {
		pbName = librautils::removeExtension(targetName) + ".pb";
		compute_pb(pbName, *wImage, *swImage, pblimit, logio);
	      }
	  }
	else
	  logio << targetImage->name() << " is already normalized" << LogIO::POST;
      }
    catch(AipsError& e)
      {
	logio << e.what() << LogIO::EXCEPTION;
      }
    
  }
}
