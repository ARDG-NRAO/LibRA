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
      
  ostringstream os;
  LogSink sink(LogMessage::NORMAL,&os);
  LogIO logio(sink);

  /********** Block to add simple normalization functionality **********/
  if ((imType == "residual") || (imType == "psf")) {
    residualName = imageName + String(".") + imType;
    cout << "Image name is " << residualName << endl;
  }
  else
    logio << "Unrecognized imtype. Allowed values are psf and residual." << LogIO::EXCEPTION;

  weightName   = imageName + String(".weight");
  sumwtName    = imageName + String(".sumwt");

  if (computePB)
    pbName   = imageName + String(".pb");

  /*********************************************************************/

  try
    {
      {
	Table table(residualName,TableLock(TableLock::AutoNoReadLocking));
	TableInfo& info = table.tableInfo();
	type=info.type();
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

	  cout << "Image values before normalization: "
	       << "sumwt = " << sow << ", max(weight) = "
	       << mwt << ", max(" << imType << ") = " << mim << endl;

	  LatticeExpr<Float> newIM = LatticeExpr<Float> ((*reImage) / abs(*swImage));
	  
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

          cout << "Image values after normalization:  "
	       << "sumwt = " << sow << ", max(weight) = "
	       << mwt << ", max(" << imType << ") = " << mim << endl;

	  if (computePB) {
	    LatticeExpr<Float> pbImage = sqrt(abs(*wImage) * abs(*swImage));
	    PagedImage<Float> tmp(wImage->shape(), wImage->coordinates(), pbName);
	    tmp.copyData(pbImage);
	    float mpb = max(tmp.get());
	    cout << "Max PB value is " << mpb << endl;
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
