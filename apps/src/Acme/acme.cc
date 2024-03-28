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

void compute_pb(const string& pbName,
		const ImageInterface<Float>& weight,
		const ImageInterface<Float>& sumwt,
		LogIO& logio)
{
	LatticeExpr<Float> pbImage = sqrt(abs(weight) / abs(sumwt));
        PagedImage<Float> tmp(weight.shape(), weight.coordinates(), pbName);
        tmp.copyData(pbImage);
        float mpb = max(tmp.get());
        stringstream os;
        os << fixed << setprecision(numeric_limits<float>::max_digits10)
           << "Max PB value is " << mpb;
        logio << os.str() << LogIO::POST;
}

void acme_func(std::string& imageName, std::string& deconvolver,
               string& normtype, string& workdir, string& imType,
               float& pblimit, int& nterms, int& facets,
               float& psfcutoff,
               vector<float>& restoringbeam,
	       bool& computePB)
{
  //
  //---------------------------------------------------
  //
  String type, residualName, weightName, sumwtName, pbName;
  String subType;
  
  // stringstream os;
  // LogSink sink(LogMessage::NORMAL,&os);
  LogIO logio(LogOrigin("acme","acme_func"));

  /********** Block to add simple normalization functionality **********/
  if ((imType == "residual") || (imType == "psf")) {
    residualName = imageName + "." + imType;
    cout << "Image name is " << residualName << endl;
  }
  else
    logio << "Unrecognized imtype. Allowed values are psf and residual." << LogIO::EXCEPTION;

  weightName   = imageName + ".weight";
  sumwtName    = imageName + ".sumwt";

  if (computePB)
    pbName   = imageName + ".pb";

  /*********************************************************************/

  try
    {
      {
	Table table(residualName,TableLock(TableLock::AutoNoReadLocking));
	TableInfo& info = table.tableInfo();
	type=info.type();
	subType = info.subType();
      }

      if (type=="Image")
	{
	  LatticeBase* lattPtr = ImageOpener::openImage (residualName);
	  ImageInterface<Float> *reImage;

	  reImage = dynamic_cast<ImageInterface<Float>*>(lattPtr);

	  /********** Block to add simple normalization functionality **********/

	  LatticeBase* wPtr = ImageOpener::openImage(weightName);
	  ImageInterface<Float> *wImage;
	  wImage = dynamic_cast<ImageInterface<Float>*>(wPtr);

	  LatticeBase* swPtr = ImageOpener::openImage(sumwtName);
	  ImageInterface<Float> *swImage;
	  swImage = dynamic_cast<ImageInterface<Float>*>(swPtr);

	  float itsPBScaleFactor = 1.0;

	  float sow = max(swImage->get());
	  float mwt = max(wImage->get());
	  float mim = max(reImage->get());

	  {
	    stringstream os;
	    os << fixed << setprecision(numeric_limits<float>::max_digits10)
	       << "Image values before normalization: "
	       << "sumwt = " << sow << ", max(weight) = "
	       << mwt << ", max(" << imType << ") = " << mim;
	    logio << os.str() << LogIO::POST;
	  }

	  LatticeExpr<Float> newIM = LatticeExpr<Float> ((*reImage) / abs(*swImage));

// Equations as currently implemented:
// pb = sqrt(weight/sumwt)
// deno = sqrt(weight/sumwt)
// newIM = image(psf, residual) / sumwt
// normalized image(psf, residual) = newIM * mask / (deno + maskinv) |||| mask, maskinv implementation below
	  
	  LatticeExpr<Float> ratio;
	  Float scalepb = 1.0;
	  LatticeExpr<Float> deno = LatticeExpr<Float> (sqrt(abs(*wImage)/abs(*swImage)));
	  scalepb=fabs(pblimit)*itsPBScaleFactor*itsPBScaleFactor;
	  LatticeExpr<Float> mask( iif( (deno) > scalepb , 1.0, 0.0 ) );
	  LatticeExpr<Float> maskinv( iif( (deno) > scalepb , 0.0, 1.0 ) );
	  ratio = ((newIM) * mask / (deno + maskinv));
	  reImage->copyData(ratio);

//	  IPosition loc(4,1000,1000,0,0);
//	  float pix = reImage->getAt(loc);
//	  cout << "Pixel value at (1000, 1000) is " << pix << endl;

	  sow = max(swImage->get());
          mwt = max(wImage->get());
          mim = max(reImage->get());

	  {
	    stringstream os;
	    os << fixed << setprecision(numeric_limits<float>::max_digits10)
	       << "Image values after normalization:  "
	       << "sumwt = " << sow << ", max(weight) = "
	       << mwt << ", max(" << imType << ") = " << mim;
	    logio << os.str() << LogIO::POST;
	  }

	  if (computePB)
            compute_pb(pbName, *wImage, *swImage, logio);
	    
	  if (imType == "residual"){
		  IPosition maxpos(4, 1158, 1384, 0, 0);
		  //float pbatpos = pbImage->getAt(maxpos);
		  float pbatpos = deno.getAt(maxpos);  // in current implementation, deno = pb
		  float reatpos = reImage->getAt(maxpos);
		  stringstream os;
		  os << fixed << setprecision(numeric_limits<float>::max_digits10)
		     << "Values to verifiy residual normalization: pb@(1158,1384) = "
		     << pbatpos << ", residual@(1158,1384) = " << reatpos;
		  logio << os.str() << LogIO::POST;
	  }

	}
      else
	logio << "imagename does not point to an image." << LogIO::EXCEPTION;
    }
  catch(AipsError& e)
    {
      logio << e.what() << LogIO::POST;
    }
}
