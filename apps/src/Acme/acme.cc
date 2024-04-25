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


#include <acme.h>

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <casacore/casa/OS/DirectoryIterator.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>


//
//-------------------------------------------------------------------------


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
           << "Image values " << when << " gather/normalization: "
           << "sumwt = " << sow << ", max(weight) = "
           << mwt << ", max(" << imType << ") = " << mim;
        logio << os.str() << LogIO::POST;
    }
}

bool imageExists(const string& imagename)
{
  Directory image(imagename);
  return image.exists();
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

template <class T>
void resetImage(ImageInterface<T>& target)
{
  target.set(0.0);
}

template <class T>
void addImages(ImageInterface<T>& target,
	       const vector<string>& partImageNames,
	       const string& imExt,
	       const bool& reset_target)
{
  //	PagedImage<T> tmp(partImageNames[0] + imExt);
  //for (uint part = 1; part < partImageNames.size(); part++)

  if (reset_target) // FIGURE HOW TO ZERO THE TARGET IMAGE
    resetImage(target);

  for (auto partName : partImageNames)
    target += PagedImage<T>(partName + imExt);
  // if (! reset_target)
  //   tmp += target;	
  // target.copyData(tmp);
}
template <class T>
void gather(ImageInterface<T>& target,
	    ImageInterface<T>& weight,
	    ImageInterface<T>& sumwt,
	    const vector<string>& partImageNames,
	    const std::string& imType,
	    const bool& reset_target)
{
  if (imType == "psf")
    {
      addImages<T>(sumwt, partImageNames, ".sumwt", reset_target);
      addImages<T>(weight, partImageNames, ".weight", reset_target);
      addImages<T>(target, partImageNames, ".psf", reset_target);
    }
  else if (imType == "residual")
    {
      addImages<T>(target, partImageNames, ".residual", reset_target);
    }
}

template <class T>
void normalize(const std::string& imageName,
	       ImageInterface<T>& target,
	       ImageInterface<T>& weight,
	       ImageInterface<T>& sumwt,
	       const std::string& imType,
	       const float& pblimit,
	       LogIO& logio)
// normtype, nterms, facets, psfcutoff and restoring beam are not functional yet, will add to interface as needed
// Normalization equations implemented in this version:
// pb = sqrt(weight / max(weight))
// psf = psf / max(psf)
// weight = weight / SoW
// itsPBScaleFactor = max(weight)
// residual = residual / (Sow * (sqrt(weight) * itsPBScaleFactor))
// model = model / (sqrt(weight) * itsPBScaleFactor)
{
  string targetName = imageName + "." + imType;

  IPosition pos(4,0,0,0,0);
  float SoW = sumwt.getAt(pos);

  if (imType == "psf")
  {
      float psfFactor = max(target.get());
      LatticeExpr<T> newPSF = target / psfFactor;
      LatticeExpr<T> newWeight = weight / SoW;
      target.copyData(newPSF);
      weight.copyData(newWeight);
  }
  else if ((imType == "residual") || (imType == "model"))
  {
      LatticeExpr<T> newIM;
      double itsPBScaleFactor = sqrt(max(weight.get()));
      if (imType == "residual")
          newIM = target / SoW;
      else
          newIM = LatticeExpr<T> (target);

      LatticeExpr<T> ratio;
      Float scalepb = 1.0;
      LatticeExpr<T> deno = sqrt(abs(weight)) * itsPBScaleFactor;

      stringstream os;
      os << fixed << setprecision(numeric_limits<float>::max_digits10)
         << "Dividing " << targetName << " by [ sqrt(weightimage) * "
         << itsPBScaleFactor << " ] to get flat noise with unit pb peak.";
      logio << os.str() << LogIO::POST;

      scalepb=fabs(pblimit)*itsPBScaleFactor*itsPBScaleFactor;
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

void acme_func(std::string& imageName, std::string& deconvolver,
               string& normtype, string& workdir, string& mode, string& imType,
               float& pblimit, int& nterms, int& facets,
               float& psfcutoff,
               vector<float>& restoringbeam,
	       vector<string>& partImageNames,
	       bool& resetImages,
	       bool& computePB)
{
  //
  //---------------------------------------------------
  //
  String type, targetName, weightName, sumwtName, pbName;
  String subType;
  
  LogIO logio(LogOrigin("acme","acme_func"));

  if (! ((mode == "gather") || (mode == "normalize")))
    logio << "Unrecognized mode. Allowed values are gather and normalize." << LogIO::EXCEPTION;

  if ((imType == "residual") || (imType == "psf") || (imType == "model")) {
    targetName = imageName + "." + imType;
    logio << "Running gather/normalization for " << targetName << LogIO::POST;
  }
  else
    logio << "Unrecognized imtype. Allowed values are psf and residual." << LogIO::EXCEPTION;

  weightName   = imageName + ".weight";
  sumwtName    = imageName + ".sumwt";

  if (computePB)
    pbName   = imageName + ".pb";

  try
  {
    string tableName = targetName;
    if ((! imageExists(targetName)) &&
	(mode == "gather") &&
	(partImageNames.size() > 0))
      tableName = partImageNames[0] + "." + imType;
    Table table(tableName,TableLock(TableLock::AutoNoReadLocking));
    TableInfo& info = table.tableInfo();
    type=info.type();
    subType = info.subType();
  }
  catch(AipsError& e)
  {
    logio << e.what() << LogIO::EXCEPTION;
  }

  if (type=="Image")
  {
    // checking if all necessary images exist before opening to avoid segfaults
    // consider to move code to open images to a function
    LatticeBase* targetPtr;
    ImageInterface<Float> *targetImage;
    if (imageExists(targetName)) {
      targetPtr = ImageOpener::openImage (targetName);
      targetImage = dynamic_cast<ImageInterface<Float>*>(targetPtr);
    }
    else
      logio << "Image " << targetName << " does not exist." << LogIO::EXCEPTION;

    LatticeBase* wPtr;
    ImageInterface<Float> *wImage;
    if (imageExists(weightName)) {
      wPtr = ImageOpener::openImage(weightName);
      wImage = dynamic_cast<ImageInterface<Float>*>(wPtr);
    }
    else
      logio << "Image " << weightName << " does not exist." << LogIO::EXCEPTION;

    LatticeBase* swPtr;
    ImageInterface<Float> *swImage;
    if (imageExists(sumwtName)) {
      swPtr = ImageOpener::openImage(sumwtName);
      swImage = dynamic_cast<ImageInterface<Float>*>(swPtr);
    }
    else
      logio << "Image " << sumwtName << " does not exist." << LogIO::EXCEPTION;

    printImageMax(imType, *targetImage, *wImage, *swImage, logio, "before");

    if (mode == "gather") {
      gather<float>(*targetImage, *wImage, *swImage, partImageNames, imType, resetImages);
    }

    if (mode == "normalize") {
      normalize<float>(imageName, *targetImage, *wImage, *swImage, imType, pblimit, logio); 
    
      if (computePB)
        compute_pb(pbName, *wImage, *swImage, pblimit, logio);
    }

    printImageMax(imType, *targetImage, *wImage, *swImage, logio, "after");

  }
  else
    logio << "imagename does not point to an image." << LogIO::EXCEPTION;
}
