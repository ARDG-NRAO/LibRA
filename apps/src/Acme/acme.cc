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
           << "Image values " << when << " normalization: "
           << "sumwt = " << sow << ", max(weight) = "
           << mwt << ", max(" << imType << ") = " << mim;
        logio << os.str() << LogIO::POST;
    }
}



void compute_pb(const string& pbName,
		const ImageInterface<Float>& weight,
		const ImageInterface<Float>& sumwt,
		const float& pblimit,
		LogIO& logio)
{
	double itsPBScaleFactor = sqrt(max(weight.get()));
	LatticeExpr<Float> norm_pbImage = sqrt(abs(weight)) / itsPBScaleFactor;
	LatticeExpr<Float> pbImage = iif(norm_pbImage > fabs(pblimit), norm_pbImage, 0.0);
        PagedImage<Float> tmp(weight.shape(), weight.coordinates(), pbName);
        tmp.copyData(pbImage);
        float mpb = max(tmp.get());
        stringstream os;
        os << fixed << setprecision(numeric_limits<float>::max_digits10)
           << "Max PB value is " << mpb;
        logio << os.str() << LogIO::POST;
}

template <class T>
void addImages(ImageInterface<T>& target,
	       const vector<string>& partImageNames,
	       const string& imExt,
	       const bool& reset_target)
{
  //	PagedImage<T> tmp(partImageNames[0] + imExt);
  //for (uint part = 1; part < partImageNames.size(); part++)

  //if (reset_target) FIGURE HOW TO ZERO THE TARGET IMAGE
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


void acme_func(std::string& imageName, std::string& deconvolver,
               string& normtype, string& workdir, string& mode, string& imType,
               float& pblimit, int& nterms, int& facets,
               float& psfcutoff,
               vector<float>& restoringbeam,
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
    cout << "Image name is " << targetName << endl;
  }
  else
    logio << "Unrecognized imtype. Allowed values are psf and residual." << LogIO::EXCEPTION;

  weightName   = imageName + ".weight";
  sumwtName    = imageName + ".sumwt";

  if (computePB)
    pbName   = imageName + ".pb";


  try
    {
      {
	Table table(targetName,TableLock(TableLock::AutoNoReadLocking));
	TableInfo& info = table.tableInfo();
	type=info.type();
	subType = info.subType();
      }

      if (type=="Image")
	{

	if (mode == "gather") {
          LatticeBase* lattPtr = ImageOpener::openImage (targetName);
          ImageInterface<Float> *targetImage;
          targetImage = dynamic_cast<ImageInterface<Float>*>(lattPtr);

	  LatticeBase* wPtr = ImageOpener::openImage(weightName);
          ImageInterface<Float> *wImage;
          wImage = dynamic_cast<ImageInterface<Float>*>(wPtr);

          LatticeBase* swPtr = ImageOpener::openImage(sumwtName);
          ImageInterface<Float> *swImage;
          swImage = dynamic_cast<ImageInterface<Float>*>(swPtr);

	  printImageMax(imType, *targetImage, *wImage, *swImage, logio, "before gather");

	  gather<float>(*targetImage, *wImage, *swImage, {"refim_n1", "refim_n2"}, "residual", true);

	  printImageMax(imType, *targetImage, *wImage, *swImage, logio, "after gather");
	} // refactor to avoid repeating commands to open images on both modes

	else if (mode == "normalize") { // just to test mode=gather; move normalize code to function later
	  LatticeBase* lattPtr = ImageOpener::openImage (targetName);
	  ImageInterface<Float> *targetImage;
	  targetImage = dynamic_cast<ImageInterface<Float>*>(lattPtr);

	  LatticeBase* wPtr = ImageOpener::openImage(weightName);
	  ImageInterface<Float> *wImage;
	  wImage = dynamic_cast<ImageInterface<Float>*>(wPtr);

	  LatticeBase* swPtr = ImageOpener::openImage(sumwtName);
	  ImageInterface<Float> *swImage;
	  swImage = dynamic_cast<ImageInterface<Float>*>(swPtr);


	  printImageMax(imType, *targetImage, *wImage, *swImage, logio, "before");


// Normalization equations implemented in this version:
// pb = sqrt(weight / max(weight))
// psf = psf / max(psf)
// weight = weight / SoW
// itsPBScaleFactor = max(weight)
// residual = residual / (Sow * (sqrt(weight) * itsPBScaleFactor))
// model = model / (sqrt(weight) * itsPBScaleFactor)
	      

	  IPosition pos(4,0,0,0,0);
	  float SoW = swImage->getAt(pos);
	  
          if (imType == "psf")
	  {
	      float psfFactor = max(targetImage->get());
	      LatticeExpr<Float> newPSF = (*targetImage) / psfFactor;
	      LatticeExpr<Float> newWeight = (*wImage) / SoW;
	      targetImage->copyData(newPSF);
	      wImage->copyData(newWeight);
	  }
	  else if ((imType == "residual") || (imType == "model"))
	  {
              LatticeExpr<Float> newIM;
	      double itsPBScaleFactor = sqrt(max(wImage->get()));
	      if (imType == "residual")
	          newIM = (*targetImage) / SoW;
	      else
		  newIM = LatticeExpr<Float> (*targetImage);

	      LatticeExpr<Float> ratio;
	      Float scalepb = 1.0;
	      LatticeExpr<Float> deno = sqrt(abs(*wImage)) * itsPBScaleFactor;

	      stringstream os;
              os << fixed << setprecision(numeric_limits<float>::max_digits10)
		 << "Dividing " << targetName << " by [ sqrt(weightimage) * " 
		 << itsPBScaleFactor << " ] to get flat noise with unit pb peak.";
	      logio << os.str() << LogIO::POST;

	      scalepb=fabs(pblimit)*itsPBScaleFactor*itsPBScaleFactor;
	      LatticeExpr<Float> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
	      LatticeExpr<Float> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
	      ratio = ((newIM) * mask / (deno + maskinv));
	      if (imType == "residual")
	          targetImage->copyData(ratio);
	      else
	      {
		  string newModelName = imageName + ".divmodel";
		  PagedImage<Float> tmp(wImage->shape(), wImage->coordinates(), newModelName);
		  tmp.copyData(ratio);
		  printImageMax(imType, tmp, *wImage, *swImage, logio, "after");
	      }
	  }
/*	  else if (imType == "model")
	  {
	      double itsPBScaleFactor = sqrt(max(wImage->get()));

              LatticeExpr<Float> deno = sqrt(abs(*wImage)) / itsPBScaleFactor;

	      stringstream os;
	      os << "Multiplying " << targetName << " by [ sqrt(weight) / "
		 << itsPBScaleFactor <<  " ] to take model back to flat noise with unit pb peak.";
	      logio << os.str() << LogIO::POST;

	      LatticeExpr<Float> mask( iif( (deno) > fabs(pblimit) , 1.0, 0.0 ) );
              LatticeExpr<Float> maskinv( iif( (deno) > fabs(pblimit) , 0.0, 1.0 ) );

	      LatticeExpr<Float> modelIM = (*targetImage) * mask * (deno + maskinv);
	      string newModelName = imageName + ".multiplymodel";
              PagedImage<Float> tmp(wImage->shape(), wImage->coordinates(), newModelName);
              tmp.copyData(modelIM);
	      printImageMax(imType, tmp, *wImage, *swImage, logio, "after");
	  }*/
 
	  printImageMax(imType, *targetImage, *wImage, *swImage, logio, "after");

	  if (computePB)
            compute_pb(pbName, *wImage, *swImage, pblimit, logio);
	    
/*	  if (imType == "residual"){
		  IPosition maxpos(4, 1158, 1384, 0, 0);
		  //float pbatpos = pbImage->getAt(maxpos);
		  //float pbatpos = deno.getAt(maxpos);  // in current implementation, deno = pb
		  float pbatpos = 1.0;
		  float reatpos = targetImage->getAt(maxpos);
		  stringstream os;
		  os << fixed << setprecision(numeric_limits<float>::max_digits10)
		     << "Values to verifiy residual normalization: pb@(1158,1384) = "
		     << pbatpos << ", residual@(1158,1384) = " << reatpos;
		  logio << os.str() << LogIO::POST;
	  }*/

	} // end mode=normalize
	}
      else
	logio << "imagename does not point to an image." << LogIO::EXCEPTION;
    }
  catch(AipsError& e)
    {
      logio << e.what() << LogIO::POST;
    }
}
