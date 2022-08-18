// -*- C++ -*-
//# SigHandler.cc: Implementation of the signal handling  class
//# Copyright (C) 2012
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
//# $Id$

#include <synthesis/Utilities/SigHandler.h>
#include <casacore/casa/BasicSL/String.h>

//DEDICATED TO HONGLIN YE 



using namespace casacore;
namespace casa { 
  bool SigHandler::killOn_p = false;
  SigHandler::SigHandler()
  {
    setupSignalHandlers();
    setStopSignal(false);
  }

  SigHandler::~SigHandler()
  {
  }

  void SigHandler::setupSignalHandlers(){
    
    signal(SIGABRT, &(SigHandler::theHandler));
    signal(SIGINT, &(SigHandler::theHandler));
	   
	   
  }
  void SigHandler::resetSignalHandlers(){
    signal(SIGABRT, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    killOn_p=false;
  }

  Bool SigHandler::gotStopSignal(){
    return killOn_p;    
  }

  void SigHandler::theHandler(int sig){
    String smoke="n ABORT ";
    if(sig==SIGINT) smoke="n INTERRUPT ";
    cerr << ("Caught a"+smoke+ " signal. Please wait ...");
    killOn_p=true;
  }

  void SigHandler::setStopSignal(Bool lala){
    killOn_p=lala;
  }
  


};//end namespace
