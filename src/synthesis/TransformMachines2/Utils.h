// -*- C++ -*-
//# Utils.h: Definition of global functions in Utils.cc
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
#ifndef SYNTHESIS_TRANSFORM2_UTILS_H
#define SYNTHESIS_TRANSFORM2_UTILS_H


#include <casacore/casa/aips.h>
#include <casacore/casa/Exceptions/Error.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageOpener.h>
//#include <ms/MeasurementSets/MeasurementSet.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <synthesis/TransformMachines/CFCell.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Logging/LogIO.h>
#include <iostream>

// Code for filterByFirstColumn() largly contributed by
// bard.google.com/chat.  The code below is after (a) correction, and
// (b) adpotation (see comments below)
#include <vector>
#include <algorithm>
#include <unordered_set> // Bard did not think this was required.
template <typename T>
casacore::Matrix<T> filterByFirstColumn(const casacore::Matrix<T>& matrix,
					const std::vector<int>& indices)
{
  std::vector< std::vector<T>> filteredMatrix;
  // Create a set for faster lookup of valid indices.
  std::unordered_set<size_t> valid_indices(indices.begin(), indices.end());

  for (size_t i = 0; i < matrix.nelements(); ++i)
    {
      if (valid_indices.count(matrix(i,0)))
	{
	  // Pointing Bard to CASACore doc page for Matrix did not
	  // produce the following code.  Bard could only produce with
	  // STL containers.
	  filteredMatrix.push_back(matrix.row(i).tovector());
	}
    }

  casacore::Matrix<T> mssFreqSelVB;
  mssFreqSelVB.resize(filteredMatrix.size(),4);
  for(int i=0;i<filteredMatrix.size();i++)
    {
      mssFreqSelVB.row(i)=casacore::Vector<double>(filteredMatrix[i]);
    }

  return mssFreqSelVB;
}



namespace casa
{
  using namespace vi;
  namespace refim {
    casacore::Int getPhaseCenter(casacore::MeasurementSet& ms, casacore::MDirection& dir0, casacore::Int whichField=-1);
    casacore::Bool findMaxAbsLattice(const casacore::ImageInterface<casacore::Float>& lattice,
			   casacore::Float& maxAbs,casacore::IPosition& posMaxAbs);
    casacore::Bool findMaxAbsLattice(const casacore::ImageInterface<casacore::Float>& masklat,
			   const casacore::Lattice<casacore::Float>& lattice,
			   casacore::Float& maxAbs,casacore::IPosition& posMaxAbs, 
			   casacore::Bool flip=false);
    casacore::Double getCurrentTimeStamp(const VisBuffer2& vb);
    void makeStokesAxis(casacore::Int npol_p, casacore::Vector<casacore::String>& polType, casacore::Vector<casacore::Int>& whichStokes);
    casacore::Double getPA(const vi::VisBuffer2& vb);
    void storeImg(casacore::String fileName,casacore::ImageInterface<casacore::Complex>& theImg, casacore::Bool writeReIm=false);
    // void storeImg(casacore::String fileName,casacore::PagedImage<casacore::Complex>& theImg, casacore::Bool writeReIm=false);
    void storeImg(casacore::String fileName,casacore::ImageInterface<casacore::Float>& theImg);
    void storeArrayAsImage(casacore::String fileName, const casacore::CoordinateSystem& coords, const casacore::Array<casacore::Complex>& cf);
    void storeArrayAsImage(casacore::String fileName, const casacore::CoordinateSystem& coords, const casacore::Array<casacore::DComplex>& cf);
    void storeArrayAsImage(casacore::String fileName, const casacore::CoordinateSystem& coords, const casacore::Array<casacore::Float>& cf);
    void storeArrayAsImage(casacore::String fileName, const casacore::CoordinateSystem& coords, const casacore::Array<casacore::Double>& cf);
    
    casacore::Bool isVBNaN(const VisBuffer2& vb, casacore::String& mesg);
    namespace SynthesisUtils
    {
      //using namespace vi;
      void rotateComplexArray(casacore::LogIO& logIO, casacore::Array<casacore::Complex>& inArray, 
			      casacore::CoordinateSystem& inCS,
			      casacore::Array<casacore::Complex>& outArray, 
			      casacore::Double dAngleRad, 
			      casacore::String interpMathod=casacore::String("CUBIC"),
			      casacore::Bool modifyInCS=true);
      void findLatticeMax(const casacore::Array<casacore::Complex>& lattice,
			  casacore::Vector<casacore::Float>& maxAbs,
			  casacore::Vector<casacore::IPosition>& posMaxAbs) ;
      void findLatticeMax(const casacore::ImageInterface<casacore::Complex>& lattice,
			  casacore::Vector<casacore::Float>& maxAbs,
			  casacore::Vector<casacore::IPosition>& posMaxAbs) ;
      void findLatticeMax(const casacore::ImageInterface<casacore::Float>& lattice,
			  casacore::Vector<casacore::Float>& maxAbs,
			  casacore::Vector<casacore::IPosition>& posMaxAbs) ;
      inline  casacore::Int nint(const casacore::Double& v) {return (casacore::Int)std::floor(v+0.5);}
      inline  casacore::Int nint(const casacore::Float& v) {return (casacore::Int)std::floor(v+0.5);}
      inline  casacore::Bool near(const casacore::Double& d1, const casacore::Double& d2, 
			const casacore::Double EPS=1E-6) 
      {
	casacore::Bool b1=(fabs(d1-d2) < EPS)?true:false;
	return b1;
      }
      template <class T>
      inline void SETVEC(casacore::Vector<T>& lhs, const casacore::Vector<T>& rhs)
      {lhs.resize(rhs.shape()); lhs = rhs;};
      template <class T>
      inline void SETVEC(casacore::Array<T>& lhs, const casacore::Array<T>& rhs)
      {lhs.resize(rhs.shape()); lhs = rhs;};
      
      template <class T>
      T getenv(const char *name, const T defaultVal);
      casacore::Float libreSpheroidal(casacore::Float nu);
      casacore::Double getRefFreq(const VisBuffer2& vb);
      void makeFTCoordSys(const casacore::CoordinateSystem& coords,
			  const casacore::Int& convSize,
			  const casacore::Vector<casacore::Double>& ftRef,
			  casacore::CoordinateSystem& ftCoords);
      
      void expandFreqSelection(const casacore::Matrix<casacore::Double>& freqSelection,
			       casacore::Matrix<casacore::Double>& expandedFreqList,
			       casacore::Matrix<casacore::Double>& expandedConjFreqList);
      
      template <class T>
      void libreConvolver(casacore::Array<T>& c1, const casacore::Array<T>& c2);
      inline casacore::Double conjFreq(const casacore::Double& freq, const casacore::Double& refFreq) 
      {return sqrt(2*refFreq*refFreq - freq*freq);};
      
      casacore::Double nearestValue(const casacore::Vector<casacore::Double>& list, const casacore::Double& val, casacore::Int& index);
      
      template <class T>
      T stdNearestValue(const std::vector<T>& list, const T& val, casacore::Int& index);
      
      casacore::CoordinateSystem makeUVCoords(casacore::CoordinateSystem& imageCoordSys,
				    casacore::IPosition& shape);
      
      casacore::Vector<casacore::Int> mapSpwIDToDDID(const VisBuffer2& vb, const casacore::Int& spwID);
      casacore::Vector<casacore::Int> mapSpwIDToPolID(const VisBuffer2& vb, const casacore::Int& spwID);
      void calcIntersection(const casacore::Int blc1[2], const casacore::Int trc1[2], const casacore::Float blc2[2], const casacore::Float trc2[2],
			    casacore::Float blc[2], casacore::Float trc[2]);
      casacore::Bool checkIntersection(const casacore::Int blc1[2], const casacore::Int trc1[2], const casacore::Float blc2[2], const casacore::Float trc2[2]);
      
      casacore::String mjdToString(casacore::Time& mjd);
      
      template<class Iterator>
      Iterator Unique(Iterator first, Iterator last);
      
      void showCS(const casacore::CoordinateSystem& cs, std::ostream& os, const casacore::String& msg=casacore::String());
      const casacore::Array<casacore::Complex> getCFPixels(const casacore::String& Dir, const casacore::String& fileName);
      void putCFPixels(const casacore::String& Dir, const casacore::String& fileName,
		       const casacore::Array<casacore::Complex>& srcpix);
      const casacore::IPosition getCFShape(const casacore::String& Dir, const casacore::String& fileName);

      void rotate2(const double& actualPA, CFCell& baseCFC, CFCell& cfc, const double& rotAngleIncr);
      
      casacore::TableRecord getCFParams(const casacore::String& dirName,const casacore::String& fileName,
					casacore::IPosition& cfShape,
					casacore::Array<casacore::Complex>& pixelBuffer,
					casacore::CoordinateSystem& coordSys, 
					casacore::Double& sampling,
					casacore::Double& paVal,
					casacore::Int& xSupport, casacore::Int& ySupport,
					casacore::Double& fVal, casacore::Double& wVal, casacore::Int& mVal,
					casacore::Double& conjFreq, casacore::Int& conjPoln,
					casacore::Bool loadPixels,
					casacore::Bool loadMiscInfo=true); 

      casacore::Vector<casacore::String> parseBandName(const casacore::String& fullName);

      casacore::CoordinateSystem makeModelGridFromImage(const std::string& modelImageName,
				  casacore::TempImage<casacore::DComplex>& modelImageGrid);

      //
      // Make a list of A- and W-term IDs for paging-in the CFs from
      // the CFCache.
      //
      // wVals and fVals is the list of pixel values along the w- and
      // frequency-axis of the CFCache.
      //
      // wbAWP==true indicates that WB A-Projection is requested.
      // nw > 1 indicates that W-term correction is requested.
      // imRefFreq is the reference frequency of the sky image frame.
      // spwRefFreq is the reference frequency of the current SPW.
      // wNdxList and spwNdxList are the return values of list of IDs for CFs in the CFCache.
      // vbSPW is the SPW ID in the current VB.
      void makeAWLists(const casacore::Vector<double>& wVals,
		       const casacore::Vector<double>& fVals,
		       const bool& wbAWP, const uint& nw,
		       const double& imRefFreq, const double& spwRefFreq,
		       const int vbSPW,
		       casacore::Vector<int>& wNdxList, casacore::Vector<int>& spwNdxList);
      //
      // A decision if new CFs are to be paged in for the AW line of
      // resamplers (AWVisResampler, AWVisResamplerHPG). This
      // evaluates to true if WB AW-P algorithm is requested and the
      // SPW ID has changed, or if forced==true.
      //
      // If the condition evaluates to true (i.e. a new CF is needed)
      // and current SPW is different, the value of the first argument
      // (cachedVBSpw) is replaced with the value of the second
      // argument (currentVBSpw).
      // 
      bool needNewCF(int& cachedVBSpw, const int currentVBSpw,
		     const int nWPlanes, const bool wbAWP,
		     const bool forced);

      bool matchPol(const vi::VisBuffer2& vb,
		    const casacore::CoordinateSystem& imCoords,
		    casacore::Vector<int>& polMap,
		    casacore::Vector<casacore::Stokes::StokesTypes>& visPolMap_p);

      casacore::Record readRecord(const casacore::String& fileName);
      void writeRecord(const casacore::String& fileName,
		      const casacore::Record& rec);

      void saveAsRecord(const casacore::CoordinateSystem& csys,
			const casacore::IPosition& imShape,
			const casacore::String& fileName,
			const casacore::String& keyName);
      void readFromRecord(casacore::CoordinateSystem& csys,
			  casacore::IPosition& imShape,
			  const casacore::String& fileName,
			  const casacore::String& keyName);
      casacore::Quantity makeFreqQuantity(const casacore::String& freqStr,
					  const casacore::String& unit);
    }
    
    void getHADec(casacore::MeasurementSet& ms, const VisBuffer2& vb, casacore::Double &HA, casacore::Double& RA, casacore::Double& Dec);

    /////////////////////////////////////////////////////////////////////////////
    // 
    // An interface class to detect changes in the VisBuffer
    // Exact meaning of the "change" is defined in the derived classes
    //
    struct IChangeDetector {
      // return true if a change occurs in the given row since the last call of update
      virtual casacore::Bool changed(const VisBuffer2 &vb, casacore::Int row) const = 0;
      // start looking for a change from the given row of the VisBuffer
      virtual void update(const VisBuffer2 &vb, casacore::Int row) = 0;
      
      // reset to the state which exists just after construction
      virtual void reset() = 0;
      
      // some derived methods, which use the abstract virtual function changed(vb,row)
      
      // return true if a change occurs somewhere in the buffer
      casacore::Bool changed(const VisBuffer2 &vb) const;
      // return true if a change occurs somewhere in the buffer starting from row1
      // up to row2 (row2=-1 means up to the end of the buffer). The row number, 
      // where the change occurs is returned in the row2 parameter
      casacore::Bool changedBuffer(const VisBuffer2 &vb, casacore::Int row1, casacore::Int &row2) const;
    protected:
      // a virtual destructor to make the compiler happy
      virtual ~IChangeDetector();
    };
    //
    //////////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////////
    //
    // ParAngleChangeDetector - a class to detect a change in the parallactic 
    //                          angle. 
    //
    class ParAngleChangeDetector : public IChangeDetector {
      casacore::Double pa_tolerance_p;   // a parallactic angle tolerance. If exeeded, 
      // the angle is considered to be changed.
      casacore::Double last_pa_p;        // last value of the parallactic angle
    public:
      // The default constructor
      ParAngleChangeDetector():pa_tolerance_p(0.0) {};
      // set up the tolerance, which determines how much the position angle should
      // change to report the change by this class
      ParAngleChangeDetector(const casacore::Quantity &pa_tolerance);
      
      virtual void setTolerance(const casacore::Quantity &pa_tolerance);
      // reset to the state which exists just after construction
      virtual void reset();
      
      // return parallactic angle tolerance
      casacore::Quantity getParAngleTolerance() const;
      
      // implementation of the base class' virtual functions
      
      // return true if a change occurs in the given row since the last call of update
      virtual casacore::Bool changed(const VisBuffer2 &vb, casacore::Int row) const;
      // start looking for a change from the given row of the VisBuffer
      virtual void update(const VisBuffer2 &vb, casacore::Int row);
    };
    
    //
    /////////////////////////////////////////////////////////////////////////////
    
  };
};
#endif
