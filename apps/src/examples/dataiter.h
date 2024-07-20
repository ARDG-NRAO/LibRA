//# Copyright (C) 2024
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//
// Following are from the parafeed project (the UI library)
//

#ifndef DATAITER_H
#define DATAITER_H

#include <cl.h> // C++ized version
#include <clinteract.h>
#include <clgetBaseCode.h>
//
//================================================================================
//
//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}

#include <DataIterations.h>
#include <DataBase.h>


// Data column to use for the data fidelity term in a solver.
casa::refim::FTMachine::Type dataCol_l=casa::refim::FTMachine::OBSERVED;

//
// A plug-in lambda function for DataBase to run soon after opening the MS.
//
auto verifyMS=[](const MeasurementSet& ms)
 {
   if (
       ((dataCol_l == casa::refim::FTMachine::MODEL) && !(ms.tableDesc().isColumn("MODEL_DATA"))) ||
       ((dataCol_l == casa::refim::FTMachine::CORRECTED) && !(ms.tableDesc().isColumn("CORRECTED_DATA"))) ||
       ((dataCol_l == casa::refim::FTMachine::OBSERVED) && !(ms.tableDesc().isColumn("DATA")))
       )
     throw(AipsError("MS verification error: The requested data column not found.  Bailing out."));
 };

//
// Below is a plug-in (lambda) function passed to the
// DataItertor::dataIter() method, which actually consumes the data.
//
// Returns a std::vector<double> of length >=2.
// The first item contains the number of bytes of visibilities read/written.
// The second item contains the time taken to read/write the visibilities.
//
// The returned vector can be longer than 2 elements, with
// an application-specific interpretation of the rest of the
// elements of the vector.
//
std::chrono::time_point<std::chrono::steady_clock> dataIO_start;
auto dataConsumer = [](vi::VisBuffer2 *vb_l, vi::VisibilityIterator2 *vi2_l)
 {
   Cube<Complex> dataCube;
   std::chrono::duration<double> thisIOTime;
   dataIO_start = std::chrono::steady_clock::now();

   {
     // Read the data from a specific data column into the
     // in-memory buffer
     if (dataCol_l==casa::refim::FTMachine::CORRECTED)   {dataCube=vb_l->visCubeCorrected();}
     else if (dataCol_l==casa::refim::FTMachine::MODEL)  {dataCube=vb_l->visCubeModel();}
     else                                                {dataCube=vb_l->visCube();}
   }

   thisIOTime = std::chrono::steady_clock::now() - dataIO_start;
      
   std::vector<double> ret={(double)dataCube.shape().product()*sizeof(Complex), thisIOTime.count()};
   return ret;
 };

#endif
