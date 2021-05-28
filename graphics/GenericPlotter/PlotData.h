//# PlotData.h: Classes to represent data for plots.
//# Copyright (C) 2008
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
//# $Id: $
#ifndef PLOTDATA_H_
#define PLOTDATA_H_

#include <graphics/GenericPlotter/PlotOptions.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>

#include <casa/BasicSL/String.h>
#include <casa/Utilities/CountedPtr.h>
#include <casacore/casa/BasicMath/Math.h>

namespace casa {

// Typedef for a point, which is two doubles (x and y).
typedef std::pair<double, double> ppoint_t;

//////////////////////
// ABSTRACT CLASSES //
//////////////////////

// Deliberately vague to be general enough to allow for many different types
// of data, but is it too vague to be useful?  Since the interface is meant
// to be a simple plotter, it may be better to restrict PlotData to be more
// like PlotPointData, which would eliminate possibilities for data like
// functions but would also eliminate a lot of vaguery.
class PlotData {
public:
    PlotData() { }
    
    virtual ~PlotData() { }
    
    
    // ABSTRACT METHODS //
    
    // Returns whether the contained data is valid or not.
    virtual bool isValid() const = 0;
    
    // Returns whether this object will delete its underlying data structures
    // upon deconstruction or not.
    virtual bool willDeleteData() const = 0;
    
    // Sets whether this object will delete its underlying data structures upon
    // deconstruction or not.
    virtual void setDeleteData(bool del = true) = 0;
};
typedef casacore::CountedPtr<PlotData> PlotDataPtr;


// A single source of data that basically provides indexing of its values.
class PlotSingleData : public virtual PlotData {
public:
    PlotSingleData() { }
    
    virtual ~PlotSingleData() { }
    
    
    // ABSTRACT METHODS //
    
    // Returns the number of points.
    virtual unsigned int size() const = 0;
    
    // Returns the value at given index.
    virtual double at(unsigned int i) const = 0;
    
    // Gets the minimum and maximum values.  Returns false for error.
    virtual bool minMax(double& min, double& max) = 0;
};
INHERITANCE_POINTER2(PlotSingleData, PlotSingleDataPtr, PlotData, PlotDataPtr)


// A source of data used to supply x and y values.  Basically consists of
// indexing its values.
class PlotPointData : public virtual PlotData {
public:
    PlotPointData() { }
    
    virtual ~PlotPointData() { }
    
    
    // ABSTRACT METHODS //
    
    // Returns the number of points.
    virtual unsigned int size() const = 0;
    
    // Returns the x value at the given index.
    virtual double xAt(unsigned int i) const = 0;
    
    // Returns the y value at the given index.
    virtual double yAt(unsigned int i) const = 0;
    
    // Gets the minimum and maximum values.  Returns false for error.
    virtual bool minsMaxes(double& xMin, double& xMax, double& yMin,
                           double& yMax) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Gets the x and y values at the given index.  Default implementation
    // just calls xAt and yAt, but in subclasses where performance could be
    // gained, this method should be overridden.  Implementations for plots
    // that use PlotPointData should use this method by default in case there
    // are performance gains.
    virtual void xAndYAt(unsigned int index, double& x, double& y) const;
};
INHERITANCE_POINTER2(PlotPointData, PlotPointDataPtr, PlotData, PlotDataPtr)


// casacore::Data that adds masking functionality on top of normal point data.
class PlotMaskedPointData : public virtual PlotPointData {
public:
    PlotMaskedPointData() { }
    
    virtual ~PlotMaskedPointData() { }
    
    
    // ABSTRACT METHODS //
    
    // Returns the number of masked points.
    virtual unsigned int sizeMasked() const = 0;
    
    // Returns the number of unmasked points.
    virtual unsigned int sizeUnmasked() const = 0;
    
    // Returns whether the data is masked at the given point or not.
    virtual bool maskedAt(unsigned int index) const = 0;
    
    // Gets the mins/maxes for just the masked points.
    virtual bool maskedMinsMaxes(double& xMin, double& xMax, double& yMin,
                                 double& yMax) = 0;
    
    // Gets the mins/maxes for just the unmasked points.
    virtual bool unmaskedMinsMaxes(double& xMin, double& xMax, double& yMin,
                                   double& yMax) = 0;

    // Returns whether data is plotted in reverse order (right to left),
	// needed when connecting points
    virtual bool reverseConnect(unsigned int index) const = 0;

    // Returns whether to plot conjugate data (e.g. UV plots)
    virtual bool plotConjugates() const = 0;
    
    // IMPLEMENTED METHODS //
    
    // Gets the x and y values and the mask at the given index.  See
    // PlotPointData::xAndYAt().
    virtual void xyAndMaskAt(unsigned int index, double& x, double& y,
                             bool& mask) const;
};
INHERITANCE_POINTER(PlotMaskedPointData, PlotMaskedPointDataPtr, PlotPointData,
                    PlotPointDataPtr, PlotData, PlotDataPtr)


// casacore::Data that adds error functionality on top of normal plot data.
class PlotErrorData : public virtual PlotPointData {
public:
    PlotErrorData() { }
    
    virtual ~PlotErrorData() { }    
    
    
    // ABSTRACT METHODS //
    
    // Returns the "left" error for x at the given index.
    virtual double xLeftErrorAt(unsigned int i) const = 0;
    
    // Returns the "right" error for x at the given index.
    virtual double xRightErrorAt(unsigned int i) const = 0;
    
    // Returns the "bottom" error for y at the given index.
    virtual double yBottomErrorAt(unsigned int i) const = 0;
    
    // Returns the "top" error for y at the given index.
    virtual double yTopErrorAt(unsigned int i) const = 0;
    
    // Gets the maximum errors for the four sides.
    virtual bool errorMaxes(double& xLeft, double& xRight, double& yBottom,
                            double& yTop) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Gets the x and y values and error data at the given index.  See
    // PlotPointData::xAndYAt().
    virtual void xyAndErrorsAt(unsigned int index, double& x, double& y,
            double& xLeftError, double& xRightError, double& yBottomError,
            double& yTopError) const;
};
INHERITANCE_POINTER(PlotErrorData, PlotErrorDataPtr, PlotPointData,
                    PlotDataPtr, PlotData, PlotDataPtr)


// casacore::Data that differentiates different points into different "bins" on top of
// normal point data functionality.
class PlotBinnedData : public virtual PlotPointData {
public:
    // Constructor.
    PlotBinnedData() { }
    
    // Destructor.
    virtual ~PlotBinnedData() { }    
    
    
    // ABSTRACT METHODS //
    
    // Returns the total number of bins that the data is in.
    virtual unsigned int numBins() const = 0;
    
    // Returns the bin index number for the given index.  MUST be between 0 and
    // numBins().
    virtual unsigned int binAt(unsigned int i) const = 0;
    virtual unsigned int connectBinAt(unsigned int i) const {
        return binAt(i); };
    
    
    // IMPLEMENTED METHODS //
    
    // Returns true if the data is binned, false otherwise.
    virtual bool isBinned() const { return numBins() > 1; }
};
INHERITANCE_POINTER(PlotBinnedData, PlotBinnedDataPtr, PlotPointData,
                    PlotDataPtr, PlotData, PlotDataPtr)


// casacore::Data for raster plots, which can be thought of as three-dimensional.  Used
// for images, with the values being in one of the givne formats.
class PlotRasterData : public virtual PlotData {
public:
    // casacore::Format that the data is in
    enum Format {
        RGB32,       // data is an RBG integer, like 0x60A0C0
        ARGB32,      // data is an ARGB integer, like 0xFF60A0C0
        SPECTROGRAM  // data is meant for a spectrogram, not specific colors
    };
    
    // Origin point of the data - in other words, where (0,0) is located
    // visually on the canvas.  Default is LLEFT.
    enum Origin {
        LLEFT, LRIGHT, ULEFT, URIGHT
    };
    
    
    PlotRasterData() { }
    
    virtual ~PlotRasterData() { }
    
    
    // ABSTRACT METHODS //
    
    // Returns the data origin.
    virtual Origin origin() const = 0;
    
    // Sets the data origin.
    virtual void setOrigin(Origin o) = 0;
    
    // Returns the range of x.
    virtual prange_t xRange() const = 0;
    
    // Returns the range of y.
    virtual prange_t yRange() const = 0;    
    
    // Sets the range of x.
    virtual void setXRange(double from, double to) = 0;
    
    // Sets the range of y.
    virtual void setYRange(double from, double to) = 0;
    
    // Returns the range of the data values.
    virtual prange_t valueRange() const = 0;
    
    // Returns the data value at the given (x,y) coordinate.
    virtual double valueAt(double x, double y) const = 0;
    
    // Gets color bar values.
    virtual std::vector<double>* colorBarValues(unsigned int max = 1000) const = 0;
};
INHERITANCE_POINTER2(PlotRasterData, PlotRasterDataPtr, PlotData, PlotDataPtr)


/////////////////////////////
// DEFAULT IMPLEMENTATIONS //
/////////////////////////////

// Default implementation of PlotSingleData that supports raw arrays, vectors,
// and CASA Vectors.  The class is templated, but since data sources are
// expected in doubles it should be a numeric type that can be casted to a
// double.
template <class T>
class PlotSingleDataImpl : public virtual PlotSingleData {
public:
    // Invalid data constructor.
    PlotSingleDataImpl(): m_vector(NULL), m_cvector(NULL), m_array(NULL),
            m_arraySize(0), m_shouldDelete(false) { }
    
    // casacore::Data using different standard containers.
    // <group>
    PlotSingleDataImpl(std::vector<T>& value, bool shouldDelete = false):
            m_vector(&value), m_cvector(NULL), m_array(NULL), m_arraySize(0),
            m_shouldDelete(shouldDelete) {
        recalculateMinMax(); }
    PlotSingleDataImpl(casacore::Vector<T>& value, bool shouldDelete = false):
            m_vector(NULL), m_cvector(&value), m_array(NULL), m_arraySize(0),
            m_shouldDelete(shouldDelete) {
        recalculateMinMax(); }
    PlotSingleDataImpl(T*& value, unsigned int size, bool shouldDelete= false):
            m_vector(NULL), m_cvector(NULL), m_array(value), m_arraySize(size),
            m_shouldDelete(shouldDelete) {
        recalculateMinMax(); }
    // </group>
    
    // Destructor.
    ~PlotSingleDataImpl() {
        if(m_shouldDelete) {
            if(m_vector != NULL) delete m_vector;
            if(m_cvector != NULL) delete m_cvector;
            if(m_array != NULL) delete m_array;
        }
    }
    
    
    // Implements PlotData::isValid().
    bool isValid() const {
        return m_vector != NULL || m_cvector != NULL || m_array != NULL; }

    // Implements PlotData::willDeleteData().
    bool willDeleteData() const { return m_shouldDelete; }

    // Implements PlotData::setDeleteData().
    void setDeleteData(bool del = true) { m_shouldDelete = del; }
    
    // Implements PlotSingleData::size().
    unsigned int size() const {
        if(m_vector != NULL) return m_vector->size();
        if(m_cvector != NULL) return m_cvector->size();
        if(m_array != NULL) return m_arraySize;
        return 0;
    }

    // Implements PlotSingleData::at().
    double at(unsigned int i) const {
        if(m_vector != NULL) return (double)(*m_vector)[i];
        if(m_cvector != NULL) return (double)(*m_cvector)[i];
        if(m_array != NULL) return (double)m_array[i];
        return 0;
    }

    // Implements PlotSingleData::minMax().
    bool minMax(double& min, double& max) {
        if(!isValid() || size() == 0) return false;
        min = m_min; max = m_max;
        return true;
    }
    
    // Recalculates the cached min and max.  Should be used if the underlying
    // data structure changes.
    void recalculateMinMax() {
        if(!isValid()) return;
        unsigned int n = size();
        if(n == 0) return;
        double temp = (double)at(0);
        m_min = m_max = temp;
        if(m_vector != NULL) {
            for(unsigned int i = 1; i < n; i++) {
                temp = (double)(*m_vector)[i];
                if(temp < m_min) m_min = temp;
                if(temp > m_max) m_max = temp;
            }
        } else if(m_cvector != NULL) {
            for(unsigned int i = 1; i < n; i++) {
                temp = (double)(*m_cvector)[i];
                if(temp < m_min) m_min = temp;
                if(temp > m_max) m_max = temp;
            }
        } else if(m_array != NULL) {
            for(unsigned int i = 1; i < n; i++) {
                temp = (double)m_array[i];
                if(temp < m_min) m_min = temp;
                if(temp > m_max) m_max = temp;
            }
        }
    }
    
private:
    std::vector<T>* m_vector;
    casacore::Vector<T>* m_cvector;
    T* m_array;
    unsigned int m_arraySize;
    bool m_shouldDelete;
    double m_min, m_max;
};

typedef PlotSingleDataImpl<int> PlotSingleIntData;
typedef PlotSingleDataImpl<unsigned int> PlotSingleUIntData;
typedef PlotSingleDataImpl<float> PlotSingleFloatData;
typedef PlotSingleDataImpl<double> PlotSingleDoubleData;


// Default implementation of PlotPointData that supports raw arrays, vectors,
// and CASA Vectors.  The class is templated, but since data sources are
// expected in doubles it should be a numeric type that can be casted to a
// double.  It can either be both x and y data, or just y data (where x is the
// index).
template <class T>
class PlotPointDataImpl : public virtual PlotPointData {
public:
    // X/Y constructors.
    // <group>
    PlotPointDataImpl(std::vector<T>& x, std::vector<T>& y, bool shouldDelete = false) :
            m_xData(x, shouldDelete), m_yData(y, shouldDelete) { }
    PlotPointDataImpl(casacore::Vector<T>& x, casacore::Vector<T>& y, bool shouldDelete = false) :
            m_xData(x, shouldDelete), m_yData(y, shouldDelete) { }
    PlotPointDataImpl(T*& x, T*& y, unsigned int size, bool shouldDel = false):
            m_xData(x, size, shouldDel), m_yData(y, size, shouldDel) { }
    // </group>
    
    // Y constructors.
    // <group>
    PlotPointDataImpl(std::vector<T>& y, bool shouldDelete = false) :
            m_yData(y, shouldDelete) { }
    PlotPointDataImpl(casacore::Vector<T>& y, bool shouldDelete = false) :
            m_yData(y, shouldDelete) { }
    PlotPointDataImpl(T*& y, unsigned int size, bool shouldDel = false):
            m_yData(y, size, shouldDel) { }
    // </group>
    
    virtual ~PlotPointDataImpl() { }
    
    
    // Implements PlotData::isValid().
    bool isValid() const { return m_yData.isValid(); }

    // Implements PlotData::willDeleteData().
    virtual bool willDeleteData() const { 
        return (m_yData.isValid() && m_yData.willDeleteData()) &&
               (!m_xData.isValid() || m_xData.willDeleteData());
    }

    // Implements PlotData::setDeleteData().
    virtual void setDeleteData(bool del = true) {
        if(m_xData.isValid()) m_xData.setDeleteData(del);
        if(m_yData.isValid()) m_yData.setDeleteData(del);
    }
    
    // Implements PlotPointData::size().
    unsigned int size() const {
        if(!m_xData.isValid()) return m_yData.size();
        else return casacore::min(m_xData.size(), m_yData.size());
    }

    // Implements PlotPointData::xAt().  If no x data is given, the index is
    // returned.
    double xAt(unsigned int i) const {
        if(m_xData.isValid()) return m_xData.at(i);
        else                  return i;
    }

    // Implements PlotPointData::yAt().
    double yAt(unsigned int i) const { return m_yData.at(i); }

    // Implements PlotPointData::minsMaxes().
    bool minsMaxes(double& xMin, double& xMax, double& yMin, double& yMax) {
        if(!m_xData.isValid()) {
            xMin = 0;
            xMax = m_yData.size();
            return m_yData.minMax(yMin, yMax);
        } else {
            return m_xData.minMax(xMin, xMax) && m_yData.minMax(yMin, yMax);
        }
    }
    
private:
    PlotSingleDataImpl<T> m_xData;
    PlotSingleDataImpl<T> m_yData;
};

typedef PlotPointDataImpl<int> PlotPointIntData;
typedef PlotPointDataImpl<unsigned int> PlotPointUIntData;
typedef PlotPointDataImpl<float> PlotPointFloatData;
typedef PlotPointDataImpl<double> PlotPointDoubleData;


// Specialized subclass of PlotPointData that creates histogram data from
// single point data.  A histogram divides up the data into a number of "bins"
// and then counts the number of data that falls into each bin.  This class can
// act as both an interface for specializations or a concrete subclass of
// PlotPointData in itself.
class PlotHistogramData : public virtual PlotPointData {
public:    
    // Constructor which takes data and number of bins.
    PlotHistogramData(PlotSingleDataPtr data, unsigned int numBins);
    
    // Destructor.
    virtual ~PlotHistogramData();
    
    
    // Implements PlotData::isValid().
    virtual bool isValid() const;
    
    // Implements PlotData::willDeleteData().
    virtual bool willDeleteData() const;
    
    // Implements PlotData::setDeleteData().
    virtual void setDeleteData(bool del = true);
    
    
    // Implements PlotPointData::size().
    virtual unsigned int size() const { return numBins(); }
    
    // Implements PlotPointData::xAt().
    virtual double xAt(unsigned int i) const;
    
    // Implements PlotPointData::yAt().
    virtual double yAt(unsigned int i) const;
    
    // Implements PlotPointData::minsMaxes().
    virtual bool minsMaxes(double& xMin, double& xMax, double& yMin,
                           double& yMax);
    
    
    // Recalculates the histogram data into the given number of bins.
    virtual void recalculateBins(unsigned int numBins);
    
    // Returns the current number of histogram bins.
    virtual unsigned int numBins() const;
    
    // Returns the range at the given index.
    virtual prange_t rangeAt(unsigned int i) const;
    
private:
    PlotSingleDataPtr m_data;    // Data.
    std::vector<unsigned int> m_bins; // Bins with count.
    std::vector<prange_t> m_ranges;   // Cached bin ranges.
    unsigned int m_max;          // Highest bin count.
};


// Default implementation of PlotMaskedPointData using default containers.
template <class T>
class PlotMaskedPointDataImpl : public virtual PlotMaskedPointData,
                                public PlotPointDataImpl<T> {
public:
    // X/Y constructors.
    // <group>
    PlotMaskedPointDataImpl(std::vector<T>& x, std::vector<T>& y, std::vector<bool>& mask,
            bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, shouldDelete), m_maskVector(&mask),
            m_maskCVector(NULL), m_maskArray(NULL), m_maskArraySize(0),
            m_shouldDeleteMask(shouldDelete) { }
    PlotMaskedPointDataImpl(casacore::Vector<T>& x, casacore::Vector<T>& y, casacore::Vector<bool>& mask,
            bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, shouldDelete), m_maskVector(NULL),
            m_maskCVector(&mask), m_maskArray(NULL), m_maskArraySize(0),
            m_shouldDeleteMask(shouldDelete) { }
    PlotMaskedPointDataImpl(T*& x, T*& y, bool*& mask, unsigned int size,
            bool shouldDel = false) :
            PlotPointDataImpl<T>(x, y, size, shouldDel), m_maskVector(NULL),
            m_maskCVector(NULL), m_maskArray(mask), m_maskArraySize(size),
            m_shouldDeleteMask(shouldDel) { }
    // </group>
    
    // Y constructors.
    // <group>
    PlotMaskedPointDataImpl(std::vector<T>& y, std::vector<bool>& mask,
            bool shouldDelete = false) :
            PlotPointDataImpl<T>(y, shouldDelete), m_maskVector(&mask),
            m_maskCVector(NULL), m_maskArray(NULL), m_maskArraySize(0),
            m_shouldDeleteMask(shouldDelete) { }
    PlotMaskedPointDataImpl(casacore::Vector<T>& y, casacore::Vector<bool>& mask,
            bool shouldDelete = false) :
            PlotPointDataImpl<T>(y, shouldDelete), m_maskVector(NULL),
            m_maskCVector(&mask), m_maskArray(NULL), m_maskArraySize(0),
            m_shouldDeleteMask(shouldDelete) { }
    PlotMaskedPointDataImpl(T*& y, bool*& mask, unsigned int size,
            bool shouldDel = false) :
            PlotPointDataImpl<T>(y, size, shouldDel), m_maskVector(NULL),
            m_maskCVector(NULL), m_maskArray(mask), m_maskArraySize(size),
            m_shouldDeleteMask(shouldDel) { }
    // </group>
    
    // Destructor.
    ~PlotMaskedPointDataImpl() {
        if(m_shouldDeleteMask) {
            if(m_maskVector != NULL) delete m_maskVector;
            if(m_maskCVector != NULL) delete m_maskCVector;
            if(m_maskArray != NULL) delete m_maskArray;
        }
    }
    
    virtual bool reverseConnect(unsigned int /*index*/) const { return false; };
    virtual bool plotConjugates() const { return false; };

    // Overrides PlotPointDataImpl::willDeleteData().
    bool willDeleteData() const {
        return PlotPointDataImpl<T>::willDeleteData() && m_shouldDeleteMask; }

    // Overrides PlotPointDataImpl::setDeleteData().
    void setDeleteData(bool del = true) {
        PlotPointDataImpl<T>::setDeleteData(del);
        m_shouldDeleteMask = del;
    }
   
    // Implements PlotMaskedPointData::sizeMasked().
    unsigned int sizeMasked() const { return sizeMaskedOrUnmasked(true); }
    
    // Implements PlotMaskedPointData::sizeUnmasked().
    unsigned int sizeUnmasked() const { return sizeMaskedOrUnmasked(false); }
    
    // Implements PlotMaskedPointData::maskedAt().
    bool maskedAt(unsigned int index) const {
        if(m_maskVector != NULL)  return (*m_maskVector)[index];
        if(m_maskCVector != NULL) return (*m_maskCVector)[index];
        if(m_maskArray != NULL)   return m_maskArray[index];
        return false;
    }
    
    // Implements PlotMaskedPointData::maskedMinsMaxes().
    bool maskedMinsMaxes(double& xMin, double& xMax, double& yMin,
                            double& yMax) {
        return getMaskedOrUnmaskedMinsMaxes(xMin, xMax, yMin, yMax, true); }
    
    // Implements PlotMaskedPointData::unmaskedMinsMaxes().
    bool unmaskedMinsMaxes(double& xMin, double& xMax, double& yMin,
                            double& yMax) {
        return getMaskedOrUnmaskedMinsMaxes(xMin, xMax, yMin, yMax, false); }
    
private:
    std::vector<bool>* m_maskVector;
    casacore::Vector<bool>* m_maskCVector;
    bool* m_maskArray;
    unsigned int m_maskArraySize;
    bool m_shouldDeleteMask;
    
    // Helper for size.
    unsigned int sizeMaskedOrUnmasked(bool masked) const {
        unsigned int n = size();
        unsigned int count = 0;
        if(m_maskArray != NULL) {
            for(unsigned int i = 0; i < m_maskArraySize; i++)
                if(m_maskArray[i]) count++;
        } else if(m_maskVector != NULL) {
            for(unsigned int i = 0; i < m_maskVector->size(); i++)
                if((*m_maskVector)[i]) count++;
        } else if(m_maskCVector != NULL) {
            for(unsigned int i = 0; i < m_maskCVector->size(); i++)
                if((*m_maskCVector)[i]) count++;
        } else return n;
        if(masked) return casacore::min(count, n);
        else       return casacore::min(n - count, n);
    }
    
    // Helper for mins/maxes.
    bool getMaskedOrUnmaskedMinsMaxes(double& xMin, double& xMax, double& yMin,
                                      double& yMax, bool masked) {
        if(!isValid()) return false;
        unsigned int n = size();
        if(n == 0) return false;
        if(m_maskArray == NULL && m_maskVector == NULL &&
           m_maskCVector == NULL) return minsMaxes(xMin, xMax, yMin, yMax);
        
        unsigned int i = 0;
        bool m;
        for(; i < n; i++) {
            m = maskedAt(i);
            if((masked && m) || (!masked && !m)) {
                xMin = xMax = xAt(i);
                yMin = yMax = yAt(i);
                break;
            }
        }
        if(i == n) return false;
        double temp;
        for(; i < n; i++) {
            m = maskedAt(i);
            if((masked && m) || (!masked && !m)) {
                temp = xAt(i);
                if(temp < xMin) xMin = temp;
                if(temp > xMax) xMax = temp;
                temp = yAt(i);
                if(temp < yMin) yMin = temp;
                if(temp > yMax) yMax = temp;
            }
        }
        return true;
    }
};

typedef PlotMaskedPointDataImpl<int> PlotMaskedPointIntData;
typedef PlotMaskedPointDataImpl<unsigned int> PlotMaskedPointUIntData;
typedef PlotMaskedPointDataImpl<float> PlotMaskedPointFloatData;
typedef PlotMaskedPointDataImpl<double> PlotMaskedPointDoubleData;


// Default implementation of PlotErrorData using standard containers, plus
// scalars for the four errors.
template <class T>
class PlotScalarErrorDataImpl : public virtual PlotErrorData,
                                public PlotPointDataImpl<T> {
public:
    // Scalar error for top, bottom, left, and right.
    // <group>
    PlotScalarErrorDataImpl(std::vector<T>& x, std::vector<T>& y, T xLeftError,
            T xRightError, T yBottomError, T yTopError,
            bool shouldDelete=false): PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xLeftError(xLeftError), m_xRightError(xRightError),
            m_yBottomError(yBottomError), m_yTopError(yTopError) { }
    PlotScalarErrorDataImpl(casacore::Vector<T>& x, casacore::Vector<T>& y, T xLeftError,
            T xRightError, T yBottomError, T yTopError,
            bool shouldDelete=false): PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xLeftError(xLeftError), m_xRightError(xRightError),
            m_yBottomError(yBottomError), m_yTopError(yTopError) { }
    PlotScalarErrorDataImpl(T*& x, T*& y, unsigned int size, T xLeftError,
            T xRightError, T yBottomError, T yTopError,
            bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, size, shouldDelete),
            m_xLeftError(xLeftError), m_xRightError(xRightError),
            m_yBottomError(yBottomError), m_yTopError(yTopError) { }
    // </group>
    
    // Single error for x and y.
    // <group>
    PlotScalarErrorDataImpl(std::vector<T>& x, std::vector<T>& y, T xError, T yError,
            bool shouldDelete=false): PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xLeftError(xError), m_xRightError(xError),
            m_yBottomError(yError), m_yTopError(yError) { }
    PlotScalarErrorDataImpl(casacore::Vector<T>& x, casacore::Vector<T>& y, T xError, T yError,
            bool shouldDelete=false): PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xLeftError(xError), m_xRightError(xError),
            m_yBottomError(yError), m_yTopError(yError) { }
    PlotScalarErrorDataImpl(T*& x, T*& y,unsigned int size, T xError, T yError,
            bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, size, shouldDelete),
            m_xLeftError(xError), m_xRightError(xError),
            m_yBottomError(yError), m_yTopError(yError) { }
    // </group>
    
    // Single error for all values.
    // <group>
    PlotScalarErrorDataImpl(std::vector<T>& x, std::vector<T>& y, T error,
            bool shouldDelete=false): PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xLeftError(error), m_xRightError(error), m_yBottomError(error),
            m_yTopError(error) { }
    PlotScalarErrorDataImpl(casacore::Vector<T>& x, casacore::Vector<T>& y, T error,
            bool shouldDelete=false): PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xLeftError(error), m_xRightError(error), m_yBottomError(error),
            m_yTopError(error) { }
    PlotScalarErrorDataImpl(T*& x, T*& y, unsigned int size, T error,
            bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, size, shouldDelete),
            m_xLeftError(error), m_xRightError(error), m_yBottomError(error),
            m_yTopError(error) { }
    // </group>
    
    // Destructor.
    ~PlotScalarErrorDataImpl() { }
    
    // Implements PlotErrorData getter methods.
    // <group>
    double xLeftErrorAt(unsigned int ) const { return m_xLeftError; }    
    double xRightErrorAt(unsigned int ) const { return m_xRightError; }
    double yBottomErrorAt(unsigned int ) const { return m_yBottomError; }
    double yTopErrorAt(unsigned int ) const { return m_yTopError; }
    // </group>
    
    // Implements PlotErrorData::errorMaxes().
    bool errorMaxes(double& xLeft, double& xRight, double& yBottom,
            double& yTop) {
        xLeft   = m_xLeftError;
        xRight  = m_xRightError;
        yBottom = m_yBottomError;
        yTop    = m_yTopError;
        return true;
    }
    
private:
    T m_xLeftError, m_xRightError, m_yBottomError, m_yTopError;
};

typedef PlotScalarErrorDataImpl<int> PlotScalarErrorIntData;
typedef PlotScalarErrorDataImpl<unsigned int> PlotScalarErrorUIntData;
typedef PlotScalarErrorDataImpl<float> PlotScalarErrorFloatData;
typedef PlotScalarErrorDataImpl<double> PlotScalarErrorDoubleData;


// Default implementation of PlotErrorData using standard containers, plus
// PlotPointDataImpls for the errors.
template <class T>
class PlotErrorDataImpl : public virtual PlotErrorData,
                          public PlotPointDataImpl<T> {
public:
    // Symmetric error constructors.
    // <group>
    PlotErrorDataImpl(T*& x, T*& y, T*& xError, T*& yError, unsigned int size,
            bool shouldDelete = true) :
            PlotPointDataImpl<T>(x, y, size, shouldDelete),
            m_xError(xError, xError, size, shouldDelete),
            m_yError(yError, yError, size, shouldDelete) { }
    PlotErrorDataImpl(std::vector<T>& x, std::vector<T>& y, std::vector<T>& xError, 
            std::vector<T>& yError, bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xError(xError, xError, shouldDelete),
            m_yError(yError, yError, shouldDelete) { }
    PlotErrorDataImpl(casacore::Vector<T>& x, casacore::Vector<T>& y, casacore::Vector<T>& xError,
            casacore::Vector<T>& yError, bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xError(xError, xError, shouldDelete),
            m_yError(yError, yError, shouldDelete) { }
    // </group>
    
    // Asymmetric error constructors.
    // <group>
    PlotErrorDataImpl(T*& x, T*& y, T*& xLeftError, T*& xRightError,
            T*& yBottomError, T*& yTopError, unsigned int size,
            bool shouldDelete = true) :
            PlotPointDataImpl<T>(x, y, size, shouldDelete),
            m_xError(xLeftError, xRightError, size, shouldDelete),
            m_yError(yBottomError, yTopError, size, shouldDelete) { }
    PlotErrorDataImpl(std::vector<T>& x, std::vector<T>& y, std::vector<T>& xLeftError,
            std::vector<T>& xRightError, std::vector<T>& yBottomError,
            std::vector<T>& yTopError, bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xError(xLeftError, xRightError, shouldDelete),
            m_yError(yBottomError, yTopError, shouldDelete) { }
    PlotErrorDataImpl(casacore::Vector<T>& x, casacore::Vector<T>& y, casacore::Vector<T>& xLeftError,
            casacore::Vector<T>& xRightError, casacore::Vector<T>& yBottomError,
            casacore::Vector<T>& yTopError, bool shouldDelete = false) :
            PlotPointDataImpl<T>(x, y, shouldDelete),
            m_xError(xLeftError, xRightError, shouldDelete),
            m_yError(yBottomError, yTopError, shouldDelete) { }
    // </group>
    
    ~PlotErrorDataImpl() { }
    
    // Overrides PlotPointDataImpl::willDeleteData().
    bool willDeleteData() const {
        return PlotPointDataImpl<T>::willDeleteData() &&
               m_xError.willDeleteData() && m_yError.willDeleteData();
    }
    
    // Overrides PlotPointDataImpl::setDeleteData().
    void setDeleteData(bool del = true) {
        m_xError.setDeleteData(del);
        m_yError.setDeleteData(del);
        PlotPointDataImpl<T>::setDeleteData(del);
    }
    
    // Implements PlotErrorData getter methods.
    // <group>
    double xLeftErrorAt(unsigned int i) const { return m_xError.xAt(i); }
    double xRightErrorAt(unsigned int i) const { return m_xError.yAt(i); }
    double yBottomErrorAt(unsigned int i) const { return m_yError.xAt(i); }
    double yTopErrorAt(unsigned int i) const { return m_yError.yAt(i); }
    // </group>
    
    // Implements PlotErrorData::errorMaxes().
    bool errorMaxes(double& xLeft, double& xRight, double& yBottom,
            double& yTop) {
        double temp;
        return m_xError.minsMaxes(temp, xLeft, temp, xRight) &&
               m_yError.minsMaxes(temp, yBottom, temp, yTop);
    }
    
private:
    PlotPointDataImpl<T> m_xError, m_yError;
};

typedef PlotErrorDataImpl<int> PlotErrorIntData;
typedef PlotErrorDataImpl<unsigned int> PlotErrorUIntData;
typedef PlotErrorDataImpl<float> PlotErrorFloatData;
typedef PlotErrorDataImpl<double> PlotErrorDoubleData;


// Implementation of raster data using casa::Matrix.
template <class T>
class PlotRasterMatrixData : public virtual PlotRasterData {
public:
    // Whether the indexing is (row,col) or (x,y).  Default is (row,col).
    enum Indexing {
        ROW_COL, X_Y
    };
    
    PlotRasterMatrixData(casacore::Matrix<T>& data, bool shouldDelete = false) :
            m_data(&data), m_origin(LLEFT), m_indexing(ROW_COL),
            m_shouldDelete(shouldDelete) {
        casacore::IPosition shape = data.shape();
        unsigned int n0 = shape[0] - 1, n1 = shape[1] - 1;

        m_0From = 0;
        m_0To = n0 + 1;
        m_1From = 0;
        m_1To = n1 + 1;
        m_0Pieces = (n0 + 1) / (m_0To - m_0From);
        m_1Pieces = (n1 + 1) / (m_1To - m_1From);

        double val = static_cast<double>(data(0, 0));
        m_valFrom = m_valTo = val;
        for(casacore::uInt i = 0; i < data.nrow(); i++) {
            for(casacore::uInt j = 0; j < data.ncolumn(); j++) {
                val = static_cast<double>(data(i, j));
                if(val < m_valFrom) m_valFrom = val;
                if(val > m_valTo) m_valTo = val;
            }
        }
    }
    
    ~PlotRasterMatrixData() { if(m_shouldDelete) delete m_data; }
    
    // Implements PlotData::isValid().
    bool isValid() const { return true; }
    
    // Implements PlotData::willDeleteData().
    bool willDeleteData() const { return m_shouldDelete; }
    
    // Implements PlotData::setDeleteData().
    void setDeleteData(bool del = true) { m_shouldDelete = del; }
    
    // Implements PlotRasterData::origin().
    Origin origin() const { return m_origin; }
    
    // Implements PlotRasterData::setOrigin().
    void setOrigin(Origin o) {
        if(m_origin != o) {
            m_origin = o;
        }
    }
    
    // Implements PlotRasterData::xRange().
    prange_t xRange() const {
        if(m_indexing == X_Y) return prange_t(m_0From, m_0To);
        else return prange_t(m_1From, m_1To);
    }
    
    // Implements PlotRasterData::yRange().
    prange_t yRange() const {
        if(m_indexing == X_Y) return prange_t(m_1From, m_1To);
        else return prange_t(m_0From, m_0To);
    }
     
    // Implements PlotRasterData::setXRange().
    void setXRange(double from, double to) {
        if(from == to) return;
        if(from > to) {
            double temp = from;
            from = to;
            to = temp;
        }        

        if(m_indexing == X_Y) {
            m_0From = from;
            m_0To = to;
            m_0Pieces = (m_data->shape()[0]) / (m_0To - m_0From);
        } else {
            m_1From = from;
            m_1To = to;
            m_1Pieces = (m_data->shape()[1]) / (m_1To - m_1From);
        }
    }
        
    // Implements PlotRasterData::setYRange().
    void setYRange(double from, double to) {
        if(from == to) return;
        if(from > to) {
            double temp = from;
            from = to;
            to = temp;
        }

        if(m_indexing == X_Y) {
            m_1From = from;
            m_1To = to;
            m_1Pieces = (m_data->shape()[1]) / (m_1To - m_1From);
        } else {
            m_0From = from;
            m_0To = to;
            m_0Pieces = (m_data->shape()[0]) / (m_0To - m_0From);
        }
    }
    
    // Implements PlotRasterData::valueRange().
    prange_t valueRange() const { return prange_t(m_valFrom, m_valTo); }
    
    // Implements PlotRasterData::valueAt().
    double valueAt(double x, double y) const {
        if(m_indexing == X_Y) {
            if(x < m_0From || x > m_0To || y < m_1From || y > m_1To) return 0;

            int xi = (int)((x - m_0From) * m_0Pieces);
            int yi = (int)((y - m_1From) * m_1Pieces);
            if(xi >= m_data->shape()[0]) xi = m_data->shape()[0] - 1;
            if(yi >= m_data->shape()[1]) yi = m_data->shape()[1] - 1;
            
            return static_cast<double>((*m_data)(xi, yi));

        } else {
            if(x < m_1From || x > m_1To || y < m_0From || y > m_0To) return 0;

            int xi = (int)((x - m_1From) * m_1Pieces);
            int yi = (int)((y - m_0From) * m_0Pieces);
            if(xi >= m_data->shape()[1]) xi = m_data->shape()[1] - 1;
            if(yi >= m_data->shape()[0]) yi = m_data->shape()[0] - 1;
            
            return static_cast<double>((*m_data)(yi, xi));
        }
    }
    
    // Implements PlotRasterData::colorBarValues().
    std::vector<double>* colorBarValues(unsigned int max = 1000) const {
        std::vector<double>* v = new std::vector<double>();

        double val;
        bool found;
        for(unsigned int i = 0; i < m_data->nrow() && v->size() <= max; i++) {
            for(unsigned int j = 0; j < m_data->ncolumn() && v->size() <= max;
            j++) {
                val = static_cast<double>((*m_data)(i, j));
                found = false;
                for(unsigned int k = 0; k < v->size() && !found; k++)
                    if(v->at(k) == val) found = true;
                if(!found) v->push_back(val);
            }
        }

        return v;
    }
    
    // Gets/sets the indexing used for the matrix.
    // <group>
    Indexing indexing() const { return m_indexing; }    
    void setIndexing(Indexing i) { m_indexing = i; }
    // </group>
    
    // Gets/sets the matrix.
    // <group>
    casacore::Matrix<T>* matrix() { return m_data; }
    void setMatrix(casacore::Matrix<T>* m, bool shouldDelete = true) {
        if(m_shouldDelete) delete m_data;
        m_data = m;
        m_shouldDelete = shouldDelete;
    }
    // </group>
    
private:
    casacore::Matrix<T>* m_data;
    double m_0From, m_0To;
    double m_1From, m_1To;
    double m_0Pieces, m_1Pieces;
    double m_valFrom, m_valTo;
    Origin m_origin;
    Indexing m_indexing;
    bool m_shouldDelete;
};

}

#endif /*PLOTDATA_H_*/
