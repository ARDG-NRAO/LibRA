//# ComponentListImage.h
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2000,2001,2003
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

#ifndef IMAGES_COMPONENTlISTIMAGE_H
#define IMAGES_COMPONENTlISTIMAGE_H


//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/lattices/Lattices/TempLattice.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/TableRecord.h>

#include <components/ComponentModels/ComponentList.h>

#include <set>

namespace casa {

// <summary>
// Read, store, and manipulate an astronomical image based on a component list.
// </summary>

// <prerequisite>
//   <li> <linkto class=CoordinateSystem>CoordinateSystem</linkto>
//   <li> <linkto class=ImageInterface>ImageInterface</linkto>
//   <li> <linkto class=Lattice>Lattice</linkto>
//   <li> <linkto class=LatticeIterator>LatticeIterator</linkto>
//   <li> <linkto class=LatticeNavigator>LatticeNavigator</linkto>
//   <li> <linkto class=ImageRegion>ImageRegion</linkto>
// </prerequisite>

// <etymology>
// An Image comprised of a ComponentList.
// </etymology>

// <synopsis> 
// A ComponentListImage is an image based on a ComponentList table. Upon construction,
// the pixel values are not known, and are computed on the fly when doGetSlice() is called.
//
// AXES
// A ComponetListImage must have 2, 3, or 4 axes. At least two of these axes must be associated
// with a direction coordinate. Optionally, a ComponentListImage can also have a frequency and/or
// polarization axis. The maximum length of the polarization axis is four pixels, and the
// associated polarization values are constrained to be in the set of stokes parameters I, Q, U,
// and/or V.
//
// UNITS
// A ComponentListImage must have units of Jy/pixel, and setUnit() will thrown an exception if
// directed to set the brightness unit to something other than this.
//
// BEAMS
// ComponentListImages do not support beams, and setImageInfo() will throw an exception if beam
// information is included in the passed object. One can of course create another type of image
// using the pixel values of a ComponentListImage and modify metadata of the
// non-ComponentListImage to their heart's content.
//
// CACHING PIXEL VALUES
// The pixel values can be cached for the life of the object for fast retrieval if they
// are needed again. In this case, a TemporaryImage is created to hold the pixel values,
// The associated mask of this TempImage indicates if the corresponding pixel values have
// been computed or not; pixel values are computed as needed. In addition, the world
// coordinate values of all the direction and frequency pixels in the image are cached if
// caching is requested, as these computations can be expensive.
//
// CACHING OF POINT SOURCE PIXEL POSITIONS
// Computing samples for point source sky components on a pixel by pixel basis can be expensive
// (cf CAS-5688), and in comparison, caching their values for later use takes relatively little
// memory (I estimate 16 bytes * nchan * nstokes per point source). For
// this reason, if the associated component list contains point sources, the pixel coordinates
// of all the point sources are computed in a highly optimized way on the first call to doGetSlice()
// and are cached for later use; computing their values on a pixel by pixel basis is not done. Pixel
// coordinates for point sources which fall outside the image boundaries are not cached.
//
// MODIFYING PIXEL VALUES
// Pixel values are computed from sky components; one cannot set pixel values explicitly. Any method
// that ultimately calls doPutSlice will throw an exception. Note however, that masks can be added,
// removed, and modified in the usual ways.
// </synopsis> 

// <example>

// </example>

// <motivation>
// Kumar Golap is the motivation. See CAS-5837 and CAS-7309. The idea being that an image which can
// be represented by sky components takes a small fraction of the space to store compared to a
// PagedImage with identical pixel values.
// </motivation>

class ComponentListImage: public casacore::ImageInterface<casacore::Float> {
public:

    static const casacore::String IMAGE_TYPE;

    // <group>
    // Create a new ComponentListImage from a component list, coordinate system, and shape.
    // The input component list will be copied (via the ComponentList::copy() method, so
    // that the constructed object has a unique ComponentList that is not referenced by
    // any other objects. Exceptions are thrown if the shape has fewer than 2 or more than
    // 4 elements, the number of pixel axes in csys is not equal to the number of elements
    // in shape, or if csys does not have a direction coordinate. The brightness units are
    // automatically set to Jy/pixel.

    // This constructor creates a persistent ComponentListImage of name tableName.
    // If tableName is empty, an exception is thrown.
    ComponentListImage(
        const ComponentList& compList, const casacore::CoordinateSystem& csys,
        const casacore::IPosition& shape, const casacore::String& tableName,
        const casacore::Bool doCache=casacore::True
    );

    // This constructor creates a temporary ComponentListImage.
    ComponentListImage(
        const ComponentList& compList, const casacore::CoordinateSystem& csys,
        const casacore::IPosition& shape, const casacore::Bool doCache=casacore::True
    );
    // </group>

    // Construct an object by reading a persistent ComponentListImage from disk.
    // By default the image's default mask is used if it exists.
    explicit ComponentListImage(
        const casacore::String& filename,
        const casacore::Bool doCache=casacore::True,
        casacore::MaskSpecifier maskSpec=casacore::MaskSpecifier()
    );

    // Copy constructor uses reference semantics.
    ComponentListImage(const ComponentListImage& image);

    ~ComponentListImage();

    // Assignment operator using reference semantics.
    ComponentListImage& operator=(const ComponentListImage& other);

    // <group>
    // These override the methods in Lattice and always throw an exception because
    // there is not a general way to do this for a component list.
    void apply (casacore::Float (*function)(casacore::Float));
    void apply (casacore::Float (*function)(const casacore::Float&));
    void apply (const casacore::Functional<casacore::Float, casacore::Float>& function);
    // </group>

    // Create an ImageInterface clone using reference semantics.
    casacore::ImageInterface<casacore::Float>* cloneII() const;

    // get a const reference to the underlying ComponentList
    const ComponentList& componentList() const;

    casacore::Bool doGetMaskSlice(
        casacore::Array<casacore::Bool>& buffer, const casacore::Slicer& section
    );

    // This method overrides that in Lattice and always throw exceptions as there is not
    // general way to do this for component lists.
    void copyData (const casacore::Lattice<casacore::Float>&);

    bool doGetSlice(
        casacore::Array<casacore::Float>& buffer, const casacore::Slicer& section
    );

    // This method overrides that in Lattice and always throws an exception as there is not
    // general way to do this for component lists.
    void doPutSlice (
        const casacore::Array<casacore::Float>& buffer,
        const casacore::IPosition& where, const casacore::IPosition& stride
    );

    // Return the internal Table object to the RegionHandler.
    static casacore::Table& getTable (void* imagePtr, casacore::Bool writable);

    // Get a pointer the default pixelmask object used with this image.
    // It returns nullptr if no default pixelmask is used.
    const casacore::LatticeRegion* getRegionPtr() const;

    casacore::Bool hasPixelMask() const;

    casacore::String imageType() const;

    casacore::Bool isMasked() const;

    // returns True if there is a persistent table associated with this object.
    casacore::Bool isPaged() const;

    // returns True if there is a persistent table associated with this object.
    casacore::Bool isPersistent() const;

    // Overrides the LatticeBase method. The method refers to the Lattice
    // characteristic and therefore always returns False.
    casacore::Bool isWritable() const;

    // If there is no persistent table associated with this object, returns
    // "Temporary ComponentListImage". Returns the name of the table otherwise.
    casacore::String name(bool stripPath=false) const;

    casacore::Bool ok() const;

    // Open an existing ComponentListImage. This gets registered with ImageOpener. It simply calls and
    // returns the result of new ComponentListImage(name).
    static casacore::LatticeBase* openCLImage(const casacore::String& name, const casacore::MaskSpecifier&);

    // Throws an exception is there is no pixel mask.
    const casacore::Lattice<casacore::Bool>& pixelMask() const;

    // In general, clients shouldn't need to call this. This is for registering
    // how to open a ComponentListImage with casacore ImageOpener.
    static void registerOpenFunction();

    // overrides ImageInterface method
    void removeRegion(
        const casacore::String& name,
        casacore::RegionHandler::GroupType=casacore::RegionHandler::Any,
        casacore::Bool throwIfUnknown=casacore::True
    );

    void resize(const casacore::TiledShape& newShape);

    // Overrides Lattice method. Always throws exception; there is no way to
    // represent with a single component a source with constant intensity everywhere.
    void set(const casacore::Float& value);

    // controls if pixel values, directions, and frequencies are cached. If doCache is false,
    // existing cache values are destroyed if they exist.
    void setCache(casacore::Bool doCache);

    // Set the coordinate system. Flush the new coordinate system to disk if
    // the table exists is writable.
    casacore::Bool setCoordinateInfo (const casacore::CoordinateSystem& coords);

    void setDefaultMask(const casacore::String& regionName);

    casacore::Bool setImageInfo(const casacore::ImageInfo& info);

    casacore::Bool setMiscInfo(const casacore::RecordInterface& newInfo);

    // An exception is thrown if newUnits are anything but Jy/pixel.
    casacore::Bool setUnits (const casacore::Unit& newUnits);

    // return the shape of the image.
    casacore::IPosition shape() const;

    // Overrides ImageInterface method.
    void useMask(casacore::MaskSpecifier=casacore::MaskSpecifier());

protected:
    //Overrides Lattice. Always throws exception; arbitrary math cannot be represented with components.
    void handleMath(const casacore::Lattice<casacore::Float>& from, int oper);

private:

    enum PtFound {
        FOUND_INSIDE,
        FOUND_OUTSIDE,
        NOT_FOUND
    };

    ComponentList _cl, _modifiedCL;

    casacore::IPosition _shape = casacore::IPosition(4, 1);
    std::shared_ptr<casacore::LatticeRegion> _mask = nullptr;
    casacore::Int _latAxis, _longAxis, _freqAxis, _stokesAxis;
    casacore::MVAngle _pixelLongSize, _pixelLatSize;
    casacore::MeasRef<casacore::MDirection> _dirRef;
    casacore::Matrix<std::shared_ptr<casacore::MVDirection>> _dirVals;
    casacore::MeasRef<casacore::MFrequency> _freqRef;
    casacore::Vector<std::shared_ptr<casacore::MVFrequency>> _freqVals;
    // yes it really does need to be initialized here (or in the constructor)
    // because initializing to nullptr will have undesirable effects when copying.
    // The copied image will initialize this, but since it is a nullptr in the original,
    // it doesn't get initialized in the original. Trust me, you don't want to deal
    // with this issue.
    using PtStoreType = std::map<
        std::pair<casacore::uInt, casacore::uInt>,
        casacore::Matrix<casacore::Float>
    >;
    std::shared_ptr<PtStoreType> _ptSourcePixelVals = std::shared_ptr<PtStoreType>(
        new PtStoreType()
    );
    casacore::Vector<casacore::Int> _pixelToIQUV;
    casacore::Bool _cache = casacore::True;
    casacore::Bool _hasFreq = casacore::False;
    casacore::Bool _hasStokes = casacore::False;
    // Yes this really has to be a shared_ptr so that _ptSourcePixelVals can be computed
    // outside the constructor and copying the image will work properly
    std::shared_ptr<casacore::Bool> _computedPtSources = std::shared_ptr<casacore::Bool>(
        new casacore::Bool(casacore::False)
    );
    casacore::MFrequency _defaultFreq = casacore::MFrequency();
    std::shared_ptr<casacore::TempImage<casacore::Float>> _valueCache = nullptr;

    void _applyMask (const casacore::String& maskName);

    void _applyMaskSpecifier (const casacore::MaskSpecifier& spec);

    void _cacheCoordinateInfo(const casacore::CoordinateSystem& csys);

    // improve performance by caching point source pixel values. This is always done;
    // the value of _cache has no effect on it.
    void _computePointSourcePixelValues();

    void _deleteCache();

    void _fillBuffer(
        casacore::Array<casacore::Float>& buffer, const casacore::IPosition& chunkShape,
        const casacore::IPosition& secStart, casacore::uInt nFreqs,
        const casacore::Cube<casacore::Double>& pixelVals
    ) const;

    PtFound _findPixel(
        casacore::Cube<casacore::Double>& values, const casacore::IPosition& pixelPosition,
        const casacore::DirectionCoordinate& dirCoord, const SkyComponent& point,
        const casacore::Vector<casacore::MVFrequency>& freqValues
    ) const;

    PtFound _findPixelIn3x3Box(
        casacore::IPosition& pixelPosition, casacore::Cube<casacore::Double>& values,
        const casacore::DirectionCoordinate& dirCoord, const SkyComponent& point,
        const casacore::Vector<casacore::MVFrequency>& freqValues
    ) const;

    void _findPointSoures(
        std::vector<casacore::uInt>& foundPtSourceIdx, casacore::uInt& nInside,
        casacore::uInt& nOutside, const std::set<casacore::uInt>& pointSourceIdx
    );

    casacore::Vector<casacore::MVFrequency> _getAllFreqValues(casacore::uInt nFreqs);

    void _getDirValsDoCache(
        casacore::Vector<casacore::MVDirection>& dirVals,
        const casacore::IPosition& secStart, casacore::uInt endLong,
        casacore::uInt endLat, const casacore::DirectionCoordinate& dirCoord
    );

    void _getDirValsNoCache(
        casacore::Vector<casacore::MVDirection>& dirVals,
        const casacore::IPosition& secStart, casacore::uInt endLong,
        casacore::uInt endLat, const casacore::DirectionCoordinate& dirCoord
    ) const;

    void _getFreqValsDoCache(
        casacore::Vector<casacore::MVFrequency>& freqVals,
        const casacore::IPosition& secStart, casacore::uInt nFreqs,
        const casacore::SpectralCoordinate& specCoord
    );

    void _getFreqValsNoCache(
        casacore::Vector<casacore::MVFrequency>& freqVals,
        const casacore::IPosition& secStart, casacore::uInt nFreqs,
        const casacore::SpectralCoordinate& specCoord
    ) const;

    void _initCache();

    void _openLogTable();

    void _reopenRW();

    void _resize(const casacore::TiledShape& shape);

    void _restoreAll(const casacore::TableRecord& rec);

    void _restoreImageInfo(const casacore::TableRecord& rec);

    void _restoreMiscInfo(const casacore::TableRecord& rec);

    void _restoreUnits(const casacore::TableRecord& rec);

};

}

#endif
