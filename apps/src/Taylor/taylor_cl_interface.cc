// # Copyright (C) 2021,2026
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

// @file This file contains the command-line interface implementation
// taylor application.  The purpose of this application is to
// accumulate a list of images and output a taylor
// expansion given the number of taylor terms..
// 
#include <cl.h>
#include <clinteract.h>
#include <vector>
#include <taylor.h>


// -------------------------------------------------------------------------

#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}


void setuptaylor(std::vector<std::string> &taylorImages,
                 std::vector<std::string> &cubeImage,
                 std::vector<std::string> &pbImage,
                 std::vector<std::string> &sumwtImage,
                 const bool &overWrite,
                 const std::string &mode,
                 const std::string &reffreq,
                 const float &pblimit,
                 const int &nTerms,
                 const std::string &imType,
                 const std::string &pbMode,
                 const std::string &avgpbname,
                 const std::string &minfreqpbname,
                 const std::string &avgpbmode)
{
  LogIO log_l(LogOrigin("taylor", "taylor"));

  log_l << "Taylor: " << LogIO::NORMAL << endl;
  log_l << "  taylorImages: ";
  for (const auto &img : taylorImages)
  {
    log_l << img << " ";
  }

  log_l << LogIO::NORMAL << endl;

  log_l << "  cubeImage: ";
  for (const auto &img : cubeImage)
  {
    log_l << img << " ";
  }
  log_l << endl << " pbImage: ";
  for (const auto &img : pbImage)
  {
    log_l << img << " ";
  }

  log_l << LogIO::NORMAL << endl;
  log_l << "  overWrite: " << overWrite << LogIO::NORMAL << endl;
  log_l << "  mode: " << mode << LogIO::NORMAL << endl;
  log_l << "  reffreq: " << reffreq << LogIO::NORMAL << endl;
  log_l << "  pblimit: " << pblimit << LogIO::NORMAL << endl;
  log_l << "  nTerms: " << nTerms << LogIO::NORMAL << endl;
  log_l << "  imType: " << imType << LogIO::NORMAL << endl;

  // 1. Check if the output image already exists and overWrite is false
  if (!overWrite && !cubeImage.empty() && casacore::Table::isReadable(cubeImage[0]))
  {
    log_l << "Output image already exists and overWrite is set to false." << LogIO::EXCEPTION;
  }

  // 2. Warn that output image will be overwritten if it already exists and overWrite is true
  if (overWrite && !cubeImage.empty() && casacore::Table::isReadable(cubeImage[0]))
  {
    log_l << "Warning: Output image will be overwritten." << LogIO::WARN;
  }

  // 3. Check reffreq and nTerms for modes that require them
  if ((mode == "cube2taylor" || mode == "taylor2cube") && (reffreq.empty() || nTerms <= 0))
  {
    log_l << "reffreq and nTerms must be set for mode " << mode << "." << LogIO::EXCEPTION;
  }

  // 4. For mode taylor2cube check if taylor images exist on disk
  if (mode == "taylor2cube")
  {
    for (const auto &img : taylorImages)
    {
      if (!librautils::imageExists(img))
      {
        log_l << "Taylor image " << img << " does not exist on disk." << LogIO::EXCEPTION;
      }
    }
  }

  // 5. For mode cube2taylor: validate cube exists and convert imtype string to enum
  taylor::ImageType imTypeEnum = taylor::INVALID;
  if (mode == "cube2taylor")
  {
    if (cubeImage.empty())
    {
      log_l << "cubeImage must be set for mode cube2taylor." << LogIO::EXCEPTION;
    }
    if (!librautils::imageExists(cubeImage[0]))
    {
      log_l << "Cube image " << cubeImage[0] << " does not exist on disk." << LogIO::EXCEPTION;
    }

    if      (imType == "psf")      imTypeEnum = taylor::PSF;
    else if (imType == "residual") imTypeEnum = taylor::RESIDUAL;
    else if (imType == "pb")       imTypeEnum = taylor::PB;
    else if (imType == "sumwt")    imTypeEnum = taylor::SUMWT;
    else if (imType == "model")    imTypeEnum = taylor::MODEL;
    else
    {
      log_l << "Invalid imtype '" << imType << "' for cube2taylor." << LogIO::EXCEPTION;
    }

    for (const auto &img : taylorImages)
    {
      if (!librautils::imageExists(img))
        log_l << "MT image " << img << " does not exist on disk." << LogIO::WARN;
    }
  }

  // 6. For mode computealpha check the Taylor terms exist on disk.  taylorImages[0]
  // is a PREFIX here, as it is for cube2taylor; the terms are <prefix>.ttN.
  if (mode == "computealpha")
  {
    if (taylorImages.empty() || taylorImages[0].empty())
    {
      log_l << "taylorImages must be set for mode computealpha." << LogIO::EXCEPTION;
    }
    for (int tt = 0; tt < nTerms; tt++)
    {
      const std::string term = taylorImages[0] + ".tt" + std::to_string(tt);
      if (!librautils::imageExists(term))
      {
        log_l << "Taylor image " << term << " does not exist on disk." << LogIO::EXCEPTION;
      }
    }
  }

  // Post all log messages
  log_l.post();

  // Configuration is constructor state; the methods take only image names.
  // Every method returns void and reports failure by throwing AipsError.
  taylor::Taylor taylorObj(reffreq, nTerms, pblimit, imTypeEnum, overWrite);

  if (mode == "cube2taylor")
  {
    taylorObj.cubeToTaylorSum(cubeImage, taylorImages, pbImage, sumwtImage);
  }
  else if (mode == "taylor2cube")
  {
    taylorObj.taylorToCube(taylorImages, cubeImage, pbImage);
  }
  else if (mode == "computealpha")
  {
    taylorObj.computeAlpha(taylorImages, cubeImage);
  }
  else if (mode == "computeavgpb")
  {
    taylorObj.computeAvgPB(pbImage, avgpbname, minfreqpbname, avgpbmode);
  }
  else if (mode == "applyPB")
  {
    if (pbImage.empty())
    {
      log_l << "pbimage must be set for mode applyPB." << LogIO::EXCEPTION;
    }
    taylorObj.applyPB(pbImage[0], cubeImage, pbMode);
  }
  else if (mode == "removefreqdepPB")
  {
    taylorObj.removeFreqDepPB(avgpbname, cubeImage, pbImage);
  }
  else if (mode == "applyfreqdepPB")
  {
    taylorObj.applyFreqDepPB(avgpbname, cubeImage, pbImage);
  }
  else
  {
    log_l << "Invalid mode." << LogIO::EXCEPTION;
  }
};

void UI(bool restart, int argc, char **argv, bool interactive,
        std::vector<std::string> &taylorImages, std::vector<std::string> &cubeImage,
        std::vector<std::string> &pbImage, std::vector<std::string> &sumwtImage,
        bool &overWrite, string &mode, string &reffreq, float &pblimit,
        int &nTerms, string &imType, string &pbMode, string &avgpbname,
        string &minfreqpbname, string &avgpbmode)
{
  clSetPrompt(interactive);

  if (!restart)
  {
    BeginCL(argc, argv);
    clInteractive(0);
  }
  else
    clRetry();
  try
  {
    int i;
    SMap watchPoints;
    VString exposedKeys;

  

    InitMap(watchPoints, exposedKeys);
    exposedKeys.push_back("nTerms");
    exposedKeys.push_back("reffreq");

    watchPoints["taylor2cube"] = exposedKeys;
    watchPoints["cube2taylor"] = exposedKeys;

    exposedKeys.resize(0);

    watchPoints["applyfreqdepPB"] = exposedKeys;
    watchPoints["removefreqdepPB"] = exposedKeys;

    i = 1;
    i = clgetNSValp("taylorImages", taylorImages, i);
    i = 1;
    clgetNSValp("cubeImage", cubeImage, i);
    i = 1;
    clgetNSValp("pbimage", pbImage, i);
    i = 1;
    clgetNSValp("sumwtImage", sumwtImage, i);
    i = 1;
    clgetBValp("overwrite", overWrite, i);
    i = 1;
    clgetSValp("mode", mode, i);
    i = 1;
    clgetSValp("reffreq", reffreq, i);

    i = 1;
    clgetIValp("nTerms", nTerms, i);

    i = 1;
    clgetSValp("mode", mode, i, watchPoints);
    clSetOptions("mode", {"taylor2cube", "cube2taylor", "applyfreqdepPB", "removefreqdepPB", "computealpha", "computeavgpb", "applyPB"});
    i = 1;
    clgetFValp("pblimit", pblimit, i);
    i = 1;
    clgetSValp("imtype", imType, i);

    // imtype names an image type.  The applyPB direction used to ride on this
    // same field, which made the option list the union of two unrelated
    // vocabularies and let parafeed accept combinations both branches reject.
    clSetOptions("imtype", {"residual", "pb", "sumwt", "psf", "model"});

    i = 1;
    clgetSValp("pbmode", pbMode, i);
    clSetOptions("pbmode", {"multiply", "divide"});

    i = 1;
    clgetSValp("avgpbname", avgpbname, i);

    i = 1;
    clgetSValp("minfreqpbname", minfreqpbname, i);

    i = 1;
    clgetSValp("avgpbmode", avgpbmode, i);
    clSetOptions("avgpbmode", {"mean", "median"});

    EndCL();
  }
  catch (clError x)
  {
    x << x << endl;
    clRetry();
  }
  // if (taylorImages.empty())
  //   throw(AipsError("Taylor images not set."));
  // if (cubeImage.empty())
  //   throw(AipsError("Cube image not set."));
  // if (pbImage.empty())
  //   throw(AipsError("PB image not set."));
  // if (mode == "")
  //   throw(AipsError("Mode not set."));
  // if (reffreq == "")
  //   throw(AipsError("Reference frequency not set."));
}
//
//-------------------------------------------------------------------------
//
int main(int argc, char **argv)
{
  //
  //---------------------------------------------------
  //
  std::vector<std::string> taylorImages, cubeImage, pbImage, sumwtImage;
  bool resetImages = false;
  bool restartUI = false;
  bool interactive = true;
  bool overWrite = false;
  string mode = "cube2taylor";
  string reffreq = "";
  float pblimit = 0.0;
  int nTerms = 0;
  string imType = "residual";
  string pbMode = "multiply";
  string avgpbname = "";
  string minfreqpbname = "";
  string avgpbmode = "mean";

RENTER: // UI re-entry point.
  try
  {
    UI(restartUI, argc, argv, interactive,
       taylorImages, cubeImage, pbImage, sumwtImage,
       overWrite, mode, reffreq,
       pblimit, nTerms, imType, pbMode,
       avgpbname, minfreqpbname,
       avgpbmode);
    //      restartUI = False;
    //
    //---------------------------------------------------
    //
    setuptaylor(taylorImages, cubeImage, pbImage, sumwtImage, overWrite, mode, reffreq, pblimit, nTerms, imType, pbMode, avgpbname, minfreqpbname, avgpbmode);
  }
  catch (clError &x)
  {
    x << x.what() << endl;
    restartUI = True;
  }
  //
  // Catch any exception thrown by AIPS++ libs.  Do your cleanup here
  // before returning to the UI (if you choose to).  Without this, all
  // exceptions (AIPS++ or otherwise) are caught in the default
  // exception handler clDefaultErrorHandler (installed by the CLLIB).
  //
  catch (AipsError &x)
  {
    cerr << "###AipsError: " << x.getMesg() << endl;
    restartUI = True;
    //      exit(0);
  }
  if (restartUI)
    RestartUI(RENTER);
}
