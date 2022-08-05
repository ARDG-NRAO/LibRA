//# dImageHistograms.cc: This program generates histograms from images
//#
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: dImageHistograms.cc 20567 2009-04-09 23:12:39Z gervandiepen $
//
// IMHIST iterates through an image and creates and displays histograms from
//    data chunks specified by the axes keyword. 
//
//
//   in       The name on the input aips++ image.  Currently must be of type <Float>
//            and can be of any dimension.
//
//            There is no default.
//
//   axes     An array of integers (1 relative) specifying which axes are to be 
//            histogrammed and which ones the histograms will be displayed as 
//            a function of.  For example,  axes=3  would cause imhist to work
//            out histograms of the third (z) axis and display them as a function of
//              location along the first (x) and second (y) axes.   axes=1,3 would cause
//            histograms of 1-3 (x-z) planes to be made and then to be displayed as
//            a function of the second axis (y) location.  
// 
//            The default is to make a histogram of the entire image.
//    
// 
//   blc,trc  Region (1 relative)
//   inc      Increment to step through image
//
//   nbins    This specifies the number of bins, which is the same for each 
//            histogram.  Note that the bin width is worked out for each histogram 
//            separately from the data minimum and maximum for that data chunk
//            (unless you set the range keyword).
//          
//            The default is 25
//         
//   include  This specifies a range of pixel values to *include* in generating
//            the histograms.  If two values are given then include pixels in
//            the range include(1) to include(2).  If one value is given, then
//            include pixels in the range -abs(include) to abs(include)
//            This range applies to all histograms.
//             
//            The default is to include all data.
//
//   gauss    If true, a Gaussian overlay with the same mean, sigma
//            (of the pixels that were binned) and integral (of the histogram)
//            will be drawn on each histogram.
//
//            The default is true.
//
//   cumu     If true, a cumulative histogram is drawn.
//
//            The defaults is false.
//
//   log      If true, the log of the histogram values is drawn.
//
//            The default is false.
//
//   list     If true list some statistical information about each histogram
//
//            The default is false.
//
//   plotter  The PGPLOT device.
//
//            The default is /xs
//
//   nxy      The number of subplots to put on each page in x and y.  Each 
//            histogram takes one subplot.
//
//            The default is 1,1
//
// To do:
//   add keyword exclude ?  Bit hard as requires restucturing of min/max
//   storage image
//
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Inputs/Input.h>
#include <casacore/casa/Logging.h>
#include <casacore/casa/BasicSL/String.h>

#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <imageanalysis/ImageAnalysis/ImageHistograms.h>
#include <casacore/lattices/LRegions/LCSlicer.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/casa/System/PGPlotter.h>
#include <casacore/casa/OS/EnvVar.h>

#include <casacore/casa/iostream.h>


#include <casacore/casa/namespace.h>
enum defaults {AXES, REGION, RANGE, NDEFAULTS};


int main (int argc, const char* argv[])
{
try {

   Input inputs(1);
   inputs.version ("$Revision: 20567 $");


// Get inputs
    String casadata = EnvironmentVariable::get("CASADATA");
    if (casadata.empty()) {
        cerr << "CASADATA env variable not defined. Can't find fixtures. Did you source the casainit.(c)sh file?" << endl;
        return 1;
    }
    String *parts = new String[2];
    split(casadata, parts, 2, String(" "));
    String datadir = parts[0] + "/unittest/image/";
    delete [] parts;

   String name = datadir + "test_image.im";
   inputs.create("in", name, "Input image name");
   inputs.create("axes", "-10", "Cursor axes");
   inputs.create("blc", "-10", "blc");
   inputs.create("trc", "-10", "trc");
   inputs.create("inc", "-10", "inc");
   inputs.create("nbins", "25", "Number of bins");
   inputs.create("include", "0.0", "Pixel range to include");
   inputs.create("gauss", "true", "Plot Gaussian equivalent ?");
   inputs.create("cumu", "false", "Plot cumulative histogram ?");
   inputs.create("log", "false", "Take log of y axis ?");
   inputs.create("list", "false", "List statistics for each histogram");
   inputs.create("plotter", "", "Plot device");
   inputs.create("nxy", "1,1", "Number of subplots in x & y");
   inputs.create("disk", "false", "Force storage image to be disk based");
   inputs.readArguments(argc, argv);

   const String in = inputs.getString("in");
   const Block<Int> cursorAxesB(inputs.getIntArray("axes"));
   const Block<Int> blcB(inputs.getIntArray("blc"));
   const Block<Int> trcB(inputs.getIntArray("trc"));
   const Block<Int> incB(inputs.getIntArray("inc"));
   const Int nBins = inputs.getInt("nbins");
   const Bool doGauss = inputs.getBool("gauss");
   const Bool doCumu = inputs.getBool("cumu");
   const Bool doLog = inputs.getBool("log");
   const Block<Double> includeB = inputs.getDoubleArray("include");
   const Bool doList = inputs.getBool("list");
   const Block<Int> nxyB(inputs.getIntArray("nxy"));
   const String device = inputs.getString("plotter");
   const Bool force = inputs.getBool("disk");


// Create defaults array
 
   Vector<Bool> validInputs(NDEFAULTS);
   validInputs = false;
   LogOrigin lor("imhist", "main()", WHERE);
   LogIO os(lor);

// Check inputs

   if (in.empty()) {
     os << LogIO::SEVERE << "You must give an input image" << LogIO::POST;
     return 1;
   }

// Convert cursor axes array to a vector (0 relative)
  
   Vector<Int> cursorAxes;
   if (cursorAxesB.nelements() == 1 && cursorAxesB[0] == -10) {
      cursorAxes.resize(0);   
   } else {
      cursorAxes.resize(cursorAxesB.nelements());
      for (uInt i=0; i<cursorAxes.nelements(); i++) cursorAxes(i) = cursorAxesB[i] - 1;
      validInputs(AXES) = true;
   }

// Convert region things to IPositions (0 relative)
   
   IPosition blc;
   IPosition trc;
   IPosition inc;
   if (blcB.nelements() == 1 && blcB[0] == -10) {
      blc.resize(0);
   } else {
      blc.resize(blcB.nelements());
      for (uInt i=0; i<blcB.nelements(); i++) blc(i) = blcB[i] - 1;
      validInputs(REGION) = true;
   }
   if (trcB.nelements() == 1 && trcB[0] == -10) {
      trc.resize(0);
   } else {
      trc.resize(trcB.nelements());
      for (uInt i=0; i<trcB.nelements(); i++) trc(i) = trcB[i] - 1;
      validInputs(REGION) = true;
   }
   if (incB.nelements() == 1 && incB[0] == -10) {
      inc.resize(0);
   } else {
      inc.resize(incB.nelements());
      for (uInt i=0; i<incB.nelements(); i++) inc(i) = incB[i];
      validInputs(REGION) = true;
   }

// Convert inclusion range to vector
   
   Vector<Float> include(includeB.nelements());
   uInt i;
   for (i=0;i<include.nelements(); i++) {
     include(i) = includeB[i]; 
   }
   if (include.nelements() == 1 && include(0)==0) {
      include.resize(0);
   } else {
      validInputs(RANGE) = true;
   }


// Plotting things
 
   Vector<Int> nxy;
   if (nxyB.nelements() == 1 && nxyB[0] == -1) {
      nxy.resize(0);
   } else {
      nxy.resize(nxyB.nelements());
      for (i=0;i<nxy.nelements(); i++) nxy(i) = nxyB[i];
   }


// Do the work 
    
   os << LogIO::NORMAL << "Using image " << in << LogIO::POST;
   DataType imageType = imagePixelType(in);
   if (imageType==TpFloat) {
   
// Construct image
     
      PagedImage<Float> inImage(in);
      SubImage<Float>* pSubImage2 = 0;
    
      if (validInputs(REGION)) {  
         LCBox::verify(blc, trc, inc, inImage.shape());
         cout << "Selected region : " << blc+1<< " to "  
              << trc+1 << endl;
         const LCSlicer region(blc, trc);
//
         SubImage<Float>* pSubImage = 0;
         if (inImage.isMasked()) {
            ImageRegion mask =
              inImage.getRegion(inImage.getDefaultMask(),
                                RegionHandler::Masks);
            pSubImage = new SubImage<Float>(inImage, mask);
         } else {
            pSubImage = new SubImage<Float>(inImage);
         }
         pSubImage2 = new SubImage<Float>(*pSubImage, ImageRegion(region));
         delete pSubImage;
      } else {
         if (inImage.isMasked()) {
            ImageRegion mask =
              inImage.getRegion(inImage.getDefaultMask(),
                                RegionHandler::Masks);
            pSubImage2 = new SubImage<Float>(inImage, mask);
         } else {
            pSubImage2 = new SubImage<Float>(inImage);
         }
      }

// Construct histogram object
  
      casa::ImageHistograms<Float> histo(*pSubImage2, os, true, force);
      if (pSubImage2!=0) delete pSubImage2;

   
// Set state
      
      if (validInputs(AXES)) {
         if (!histo.setAxes(cursorAxes)) {
            os << histo.errorMessage() << LogIO::POST;
            return 1;
         }
      }
      if (!histo.setNBins(nBins)) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
      }
      if (validInputs(RANGE)) {
         if (!histo.setIncludeRange(include)) {
            os << histo.errorMessage() << LogIO::POST;
            return 1;
         }
      }
      if (!histo.setGaussian (doGauss)) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
      }
      if (!histo.setForm(doLog, doCumu)) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
      }
      if (!histo.setStatsList(doList)) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
      }
      if (!device.empty()) {
         PGPlotter plotter(device);
         if (!histo.setPlotting(plotter, nxy)) {
            os << histo.errorMessage() << LogIO::POST;
            return 1;
         }
      }

// Display histograms

      if (!histo.display()) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
      }

// Get histograms   

      Array<Float> values, counts;
      os << LogIO::NORMAL << "Recovering histogram arrays" << LogIO::POST;
      if (!histo.getHistograms(values,counts)) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
      }
//      cout << "values=" << values << endl;
//      cout << "counts=" << counts << endl;
 
      os << LogIO::NORMAL << "Recovering individual histogram arrays" << LogIO::POST;
      Vector<Float> valuesV, countsV;
      IPosition pos(histo.displayAxes().nelements(),0);
      if (!histo.getHistogram(valuesV,countsV,pos,false)) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
      }

//      cout << "values=" << valuesV << endl;
//      cout << "counts=" << countsV << endl;


// Test copy constructor

      os << LogIO::NORMAL << "Applying copy constructor" << endl;
      casa::ImageHistograms<Float> histo2(histo);

// Test assignment operator

      os << "Applying assignment operator" << LogIO::POST;
      histo = histo2;

// Test setNewImage
 
     os << "Test setNewImage" << LogIO::POST;
     if (!histo.setNewImage(inImage)) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
     } 
     if (!histo.display()) {
         os << histo.errorMessage() << LogIO::POST;
         return 1;
     }

   } else {
      os << "images of type " << Int(imageType)
	 << " not yet supported" << endl;
      return 1;
   }
}
   catch (AipsError x) {
      cerr << "aipserror: error " << x.getMesg() << endl;
      return 1;
  }

 return 0;

}
