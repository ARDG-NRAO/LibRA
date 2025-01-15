// # Copyright (C) 2021
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

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <casacore/casa/OS/DirectoryIterator.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>


//
//-------------------------------------------------------------------------

namespace Dale
{
  bool imageExists(const string& imagename)
  {
    Directory image(imagename);
    return image.exists();
  }
  
  void printImageMax(const string& imType,
		     const ImageInterface<Float>& target,
		     const ImageInterface<Float>& weight,
		     const ImageInterface<Float>& sumwt,
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
    weight.copyData(normalizedWeight);
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

  template <class T>
  void normalize(const std::string& imageName,
		 ImageInterface<T>& target,
		 ImageInterface<T>& weight,
		 ImageInterface<T>& sumwt,
		 const std::string& imType,
		 const float& pblimit,
		 const bool normalize_weight,
		 LogIO& logio)
  // normtype, nterms, facets, psfcutoff and restoring beam are not functional yet, will add to interface as needed
  // Normalization equations implemented in this version:
  // pb = sqrt(weight / max(weight))
  // psf = psf / max(psf)
  // weight = weight / SoW
  // itsPBScaleFactor = max(weight)
  // residual = residual / (Sow * (sqrt(weight) * itsPBScaleFactor))
  // model = model / (sqrt(weight) / itsPBScaleFactor)
  {
    string targetName = imageName + "." + imType;
    
    IPosition pos(4,0,0,0,0);
    float SoW = sumwt.getAt(pos);
    
    if (imType == "psf")
      {
	float psfFactor = max(target.get());
	LatticeExpr<T> newPSF = target / psfFactor;
	target.copyData(newPSF);
	if (normalize_weight)
	  {
	    LatticeExpr<T> newWeight = weight / SoW;
	    weight.copyData(newWeight);
	  }
      }
    else if ((imType == "residual") || (imType == "model"))
      {
	LatticeExpr<T> newIM;
	double itsPBScaleFactor = sqrt(max(weight.get()));
	if (imType == "residual")
          newIM = target / SoW;
	else
          newIM = LatticeExpr<T> (target);
	
	LatticeExpr<T> ratio, deno;
	Float scalepb = 1.0;
	if (imType == "residual")
	  {
	    deno = sqrt(abs(weight)) * itsPBScaleFactor;
	    stringstream os;
	    os << fixed << setprecision(numeric_limits<float>::max_digits10)
	       << "Dividing " << targetName << " by [ sqrt(weightimage) * "
	       << itsPBScaleFactor << " ] to get flat noise with unit pb peak.";
	    logio << os.str() << LogIO::POST;
	    
	    scalepb=fabs(pblimit)*itsPBScaleFactor*itsPBScaleFactor;
	  }
	else if (imType == "model")
	  {
	    deno = sqrt(abs(weight)) / itsPBScaleFactor;
	    
	    stringstream os;
	    os << fixed << setprecision(numeric_limits<float>::max_digits10)
	       << "Dividing " << targetName << " by [ sqrt(weightimage) / "
	       << itsPBScaleFactor << " ] to get to flat sky model before prediction.";
	    logio << os.str() << LogIO::POST;
	    
	    scalepb=fabs(pblimit);
	  }
	
	LatticeExpr<T> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
	LatticeExpr<T> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
	ratio = ((newIM) * mask / (deno + maskinv));
	
	if (imType == "residual")
          target.copyData(ratio);
	else
	  {
	    string newModelName = imageName + ".divmodel";
	    PagedImage<T> tmp(weight.shape(), weight.coordinates(), newModelName);
	    tmp.copyData(ratio);
	    printImageMax(imType, tmp, weight, sumwt, logio, "after");
	  }
      }
  }
  
  template <class T>
  ImageInterface<T>* checkAndOpen(const string& name)
  {
    if (!imageExists(name))
      throw(AipsError("Image " + name + " does not exist."));

    LatticeBase *imPtr;
	
    imPtr = ImageOpener::openImage (name);
    return dynamic_cast<ImageInterface<T>*>(imPtr);
  }

  void dale(std::string& imageName,
	    std::string& normtype,
	    std::string& imType,
	    float& pblimit, 
	    float& psfcutoff,
	    bool& computePB,
	    bool& normalize_weight)
  {
    //
    //---------------------------------------------------
    //
    string type, targetName, weightName, sumwtName, pbName;
    
    LogIO logio(LogOrigin("Dale","dale"));
    
    try
      {
	if ((imType == "residual") || (imType == "psf") || (imType == "model"))
	  {
	    targetName = imageName + "." + imType;
	    logio << "Running normalization for " << targetName << LogIO::POST;
	  }
	else
	  throw(AipsError("Unrecognized imtype (" + imType +"). Allowed values are psf and residual."));
	
	weightName   = imageName + ".weight";
	sumwtName    = imageName + ".sumwt";
	
	{
	  Table table(targetName,TableLock(TableLock::AutoNoReadLocking));
	  TableInfo& info = table.tableInfo();
	  type = info.type();
	  if (type != "Image")
	    throw(AipsError("imagename does not point to an image."));
	}
	// checking if all necessary images exist before opening to avoid segfaults
	// consider to move code to open images to a function
	
	ImageInterface<Float>* targetImage = checkAndOpen<float>(targetName);
	ImageInterface<Float>* wImage = checkAndOpen<float>(weightName);
	ImageInterface<Float>* swImage = checkAndOpen<float>(sumwtName);
	
	printImageMax(imType, *targetImage, *wImage, *swImage, logio, "before");
	normalize<float>(imageName, *targetImage, *wImage, *swImage, imType, pblimit, normalize_weight, logio); 
	printImageMax(imType, *targetImage, *wImage, *swImage, logio, "after");
	
	if (computePB)
	  {
	    pbName   = imageName + ".pb";
	    compute_pb(pbName, *wImage, *swImage, pblimit, logio);
	  }
      }
    catch(AipsError& e)
      {
	logio << e.what() << LogIO::EXCEPTION;
      }
    
  }
}
