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


#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Quanta/Quantum.h>
#include <images/Images/ImageInterface.h>
//#include <ms/MeasurementSets/MeasurementSet.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <synthesis/TransformMachines/CFCell.h>
#include <casa/Arrays/Array.h>
#include <casa/Logging/LogIO.h>
#include <casa/iostream.h>

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
    void storeImg(casacore::String fileName,casacore::ImageInterface<casacore::Float>& theImg);
    void storeArrayAsImage(casacore::String fileName, const casacore::CoordinateSystem& coords, const casacore::Array<casacore::Complex>& cf);
    void storeArrayAsImage(casacore::String fileName, const casacore::CoordinateSystem& coords, const casacore::Array<casacore::DComplex>& cf);
    void storeArrayAsImage(casacore::String fileName, const casacore::CoordinateSystem& coords, const casacore::Array<casacore::Float>& cf);
    
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
      const casacore::IPosition getCFShape(const casacore::String& Dir, const casacore::String& fileName);

      void rotate2(const double& actualPA, CFCell& baseCFC, CFCell& cfc, const double& rotAngleIncr);
      
      casacore::TableRecord getCFParams(const casacore::String& dirName,const casacore::String& fileName,
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
