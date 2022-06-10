//# VisibilityIterator.cc: Step through MeasurementEquation by visibility
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: VisIterator.cc,v 19.15 2006/02/01 01:25:14 kgolap Exp $

#include <msvis/MSVis/VisIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <stdcasa/UtilJ.h>
#include <casacore/scimath/Mathematics/InterpolateArray1D.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/ColDescSet.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/DataMan/TiledColumnStMan.h>
#include <casacore/tables/DataMan/TiledStManAccessor.h>
#include <msvis/MSVis/VisibilityIteratorImpl.h>
#include <cmath>

using std::ceil;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

class ROVisIteratorImpl : public VisibilityIteratorReadImpl {

    friend class ROVisIterator;
    friend class VisIterator;

public:

  // Default constructor - useful only to assign another iterator later
  ROVisIteratorImpl();

  // Construct from MS and a Block of MS column enums specifying the iteration
  // order, if none are specified, time iteration is implicit.  An optional
  // timeInterval can be given to iterate through chunks of time.  The default
  // interval of 0 groups all times together.  Every 'chunk' of data contains
  // all data within a certain time interval (in seconds) and with identical
  // values of the other iteration columns (e.g.  DATA_DESC_ID and FIELD_ID).
  // Using selectChannel(), a number of groups of channels can be requested.
  // At present the channel group iteration will always occur before the
  // interval iteration.
  ROVisIteratorImpl(ROVisIterator * rovi,
                    const MeasurementSet& ms,
                    const Block<Int>& sortColumns,
                    Double timeInterval=0);

  // Copy construct. This calls the assignment operator.
  ROVisIteratorImpl(const ROVisIteratorImpl & other);

  // Destructor
  virtual ~ROVisIteratorImpl();

  // Assignment. Any attached VisBuffers are lost in the assign.
  ROVisIteratorImpl & operator=(const ROVisIteratorImpl &other);

  // Members

  // Advance iterator through data
  ROVisIteratorImpl & operator++(int);
  ROVisIteratorImpl & operator++();

  // Selected spws and channel counts
  virtual void allSelectedSpectralWindows (Vector<Int> & spws, Vector<Int> & nvischan);

  // 
  virtual void lsrFrequency(const Int& spw, Vector<Double>& freq, Bool& convert, const  Bool ignoreconv=false);


  // The following throws an exception, because this isn't the
  // language of channel selection in VisIterator
  virtual void getChannelSelection(Block< Vector<Int> >&,
				   Block< Vector<Int> >&,
				   Block< Vector<Int> >&,
				   Block< Vector<Int> >&,
				   Block< Vector<Int> >&)
  { throw(AipsError("ROVisIteratorImpl::getChannelSelection: you can't do that!")); };

  // Return channel numbers in selected VisSet spectrum
  // (i.e. disregarding possible selection on the iterator, but
  //  including the selection set when creating the VisSet)
  Vector<Int>& channel(Vector<Int>& chan) const;
  Vector<Int>& chanIds(Vector<Int>& chanids) const;
  Vector<Int>& chanIds(Vector<Int>& chanids,Int spw) const;

  // Return selected correlation indices
  Vector<Int>& corrIds(Vector<Int>& corrids) const;

  // Return flag for each polarization, channel and row
  Cube<Bool>& flag(Cube<Bool>& flags) const;

  // Return current frequencies
  Vector<Double>& frequency(Vector<Double>& freq) const;

  // Return the correlation type (returns Stokes enums)
  Vector<Int>& corrType(Vector<Int>& corrTypes) const;

  // Return sigma matrix (pol-dep)
  Matrix<Float>& sigmaMat(Matrix<Float>& sigmat) const;

  // Return the visibilities as found in the MS, Cube(npol,nchan,nrow).
  Cube<Complex>& visibility(Cube<Complex>& vis,
			    DataColumn whichOne) const;
  // Return weight matrix
  Matrix<Float>& weightMat(Matrix<Float>& wtmat) const;

  // Return weightspectrum (a weight for each corr & channel)
  Cube<Float>& weightSpectrum(Cube<Float>& wtsp) const;

  // Set up new chan/corr selection via Vector<Slice>
  void selectChannel(const Vector<Vector<Slice> >& chansel);
  void selectCorrelation(const Vector<Vector<Slice> >& corrsel);

  // Set up/return channel averaging bounds
  Vector<Matrix<Int> >& setChanAveBounds(Float factor, Vector<Matrix<Int> >& bounds);

  // Return number of chans/corrs per spw/pol
  Int numberChan(Int spw) const;
  Int numberCorr(Int pol) const;

  // Return the row ids as from the original root table. This is useful
  // to find correspondance between a given row in this iteration to the
  // original ms row
  Vector<rownr_t>& rowIds(Vector<rownr_t>& rowids) const;

  // Need to override this and not use getColArray
  Vector<RigidVector<Double,3> >& uvw(Vector<RigidVector<Double,3> >& uvwvec) const;

protected:

  void setSelTable();

  virtual const Table attachTable() const;

  // update the DATA slicer
  void updateSlicer();
  // attach the column objects to the currently selected table

  // The ROVisibilityIterator version of this function sets the tile cache to 1
  // because of a feature in sliced data access that grows memory dramatically in
  // some cases.  However, ROVisibilityIterator, because it uses
  // ArrayColumn::getColumn(Vector<Vector<Slice> >&), is (1/28/2011) incredibly
  // slow if the tile cache does not span all the selected channels, and it will
  // read orders of magnitude more data than it needs to.  This sets the tile
  // cache to the minimum number of tiles required to span the selected channels.
  // Unlike ROVisibilityIterator, it does it for each hypercube, not just the
  // first one, and it does its work when the DDID has changed.
  void setTileCache();

  void getDataColumn(DataColumn whichOne, const Vector<Vector<Slice> >& slices,
			     Cube<Complex>& data) const;

  // Column access functions
  void getCol(const ScalarColumn<Bool> &column, Vector<Bool> &array, Bool resize = false) const;
  void getCol(const ScalarColumn<Int> &column, Vector<Int> &array, Bool resize = false) const;
  void getCol(const ScalarColumn<Double> &column, Vector<Double> &array, Bool resize = false) const;

  void getCol(const ArrayColumn<Bool> &column, Array<Bool> &array, Bool resize = false) const;
  void getCol(const ArrayColumn<Float> &column, Array<Float> &array, Bool resize = false) const;
  void getCol(const ArrayColumn<Double> &column, Array<Double> &array, Bool resize = false) const;
  void getCol(const ArrayColumn<Complex> &column, Array<Complex> &array, Bool resize = false) const;

  void getCol(const ArrayColumn<Bool> &column, const Slicer &slicer, Array<Bool> &array, Bool resize = false) const;
  void getCol(const ArrayColumn<Float> &column, const Slicer &slicer, Array<Float> &array, Bool resize = false) const;
  void getCol(const ArrayColumn<Complex> &column, const Slicer &slicer, Array<Complex> &array, Bool resize = false) const;


  // New slicer supports multiple Slices in channel and correlation

  Vector<Matrix<Int> >   chanAveBounds_p;
  Vector<Vector<Slice> > chanSlices_p;
  Vector<Vector<Slice> > corrSlices_p;
  Vector<Vector<Slice> > newSlicer_p;
  Vector<Vector<Slice> > newWtSlicer_p;
  Table                  selTable_p;
  Bool                   useNewSlicer_p;

};

class VisIteratorImpl : public ROVisIteratorImpl {

    friend class ROVisIterator;
    friend class VisIterator;

public:

  // Default constructor - useful only to assign another iterator later
  VisIteratorImpl();

  // Construct from MS and a Block of MS column enums specifying the iteration
  // order, if none are specified, time iteration is implicit.  An optional
  // timeInterval can be given to iterate through chunks of time.  The default
  // interval of 0 groups all times together.  Every 'chunk' of data contains
  // all data within a certain time interval (in seconds) and with identical
  // values of the other iteration columns (e.g.  DATA_DESC_ID and FIELD_ID).
  // Using selectChannel(), a number of groups of channels can be requested.
  // At present the channel group iteration will always occur before the
  // interval iteration.
  VisIteratorImpl(ROVisIterator * rovi,
                    const MeasurementSet& ms,
                    const Block<Int>& sortColumns,
                    Double timeInterval=0);

  // Copy construct. This calls the assignment operator.
  VisIteratorImpl(const ROVisIteratorImpl & other);

  // Destructor
  ~VisIteratorImpl();

  // Assignment. Any attached VisBuffers are lost in the assign.
  VisIteratorImpl & operator=(const VisIteratorImpl &other);

  // Members

  // Advance iterator through data
  VisIteratorImpl & operator++(int);
  VisIteratorImpl & operator++();

  // Return channel numbers in selected VisSet spectrum
  // (i.e. disregarding possible selection on the iterator, but
  //  including the selection set when creating the VisSet)
  //Vector<Int>& channel(Vector<Int>& chan) const;
  //Vector<Int>& chanIds(Vector<Int>& chanids) const;
  //Vector<Int>& chanIds(Vector<Int>& chanids,Int spw) const;

  // Return selected correlation indices
  //Vector<Int>& corrIds(Vector<Int>& corrids) const;

  // Return flag for each polarization, channel and row
  //Cube<Bool>& flag(Cube<Bool>& flags) const;

  // Return current frequencies
  //Vector<Double>& frequency(Vector<Double>& freq) const;

  // Return the correlation type (returns Stokes enums)
  //Vector<Int>& corrType(Vector<Int>& corrTypes) const;

  // Return sigma matrix (pol-dep)
  //Matrix<Float>& sigmaMat(Matrix<Float>& sigmat) const;

  // Return the visibilities as found in the MS, Cube(npol,nchan,nrow).
  //Cube<Complex>& visibility(Cube<Complex>& vis,
			    //DataColumn whichOne) const;
  // Return weight matrix
  //Matrix<Float>& weightMat(Matrix<Float>& wtmat) const;

  // Return weightspectrum (a weight for each corr & channel)
  //Cube<Float>& weightSpectrum(Cube<Float>& wtsp) const;

  // Set up new chan/corr selection via Vector<Slice>
  //void selectChannel(const Vector<Vector<Slice> >& chansel);
  //void selectCorrelation(const Vector<Vector<Slice> >& corrsel);

  // Set up/return channel averaging bounds
  //Vector<Matrix<Int> >& setChanAveBounds(Float factor, Vector<Matrix<Int> >& bounds);

  // Return number of chans/corrs per spw/pol
  //Int numberChan(Int spw) const;
  //Int numberCorr(Int pol) const;

  // Return the row ids as from the original root table. This is useful
  // to find correspondance between a given row in this iteration to the
  // original ms row
  //Vector<uInt>& rowIds(Vector<uInt>& rowids) const;

  // Need to override this and not use getColArray
  //Vector<RigidVector<Double,3> >& uvw(Vector<RigidVector<Double,3> >& uvwvec) const;

  // Set/modify the flag row column; dimension Vector(nrow)
  void setFlagRow(const Vector<Bool>& rowflags);

  // Set/modify the flags in the data.
  // This sets the flags as found in the MS, Cube(npol,nchan,nrow),
  // where nrow is the number of rows in the current iteration (given by
  // nRow()).
  void setFlag(const Cube<Bool>& flag);

  // Set/modify the visibilities
  // This sets the data as found in the MS, Cube(npol,nchan,nrow).
  void setVis(const Cube<Complex>& vis, DataColumn whichOne);

  // Set the visibility and flags, and interpolate from velocities if needed
  void setVisAndFlag(const Cube<Complex>& vis, const Cube<Bool>& flag,
		     DataColumn whichOne);

  // Set/modify the weightMat
  void setWeightMat(const Matrix<Float>& wtmat);

  // Set/modify the weightSpectrum
  void setWeightSpectrum(const Cube<Float>& wtsp);

protected:

  //void setSelTable();

  void attachColumns (const Table & t);

  // update the DATA slicer
  //void updateSlicer();
  // attach the column objects to the currently selected table

  // The ROVisibilityIterator version of this function sets the tile cache to 1
  // because of a feature in sliced data access that grows memory dramatically in
  // some cases.  However, ROVisibilityIterator, because it uses
  // ArrayColumn::getColumn(Vector<Vector<Slice> >&), is (1/28/2011) incredibly
  // slow if the tile cache does not span all the selected channels, and it will
  // read orders of magnitude more data than it needs to.  This sets the tile
  // cache to the minimum number of tiles required to span the selected channels.
  // Unlike ROVisibilityIterator, it does it for each hypercube, not just the
  // first one, and it does its work when the DDID has changed.

  void getDataColumn(DataColumn whichOne, const Vector<Vector<Slice> >& slices,
			     Cube<Complex>& data) const;

  // Column access functions
//  void getCol(const ScalarColumn<Bool> &column, Vector<Bool> &array, Bool resize = false) const;
//  void getCol(const ScalarColumn<Int> &column, Vector<Int> &array, Bool resize = false) const;
//  void getCol(const ScalarColumn<Double> &column, Vector<Double> &array, Bool resize = false) const;
//
//  void getCol(const ArrayColumn<Bool> &column, Array<Bool> &array, Bool resize = false) const;
//  void getCol(const ArrayColumn<Float> &column, Array<Float> &array, Bool resize = false) const;
//  void getCol(const ArrayColumn<Double> &column, Array<Double> &array, Bool resize = false) const;
//  void getCol(const ArrayColumn<Complex> &column, Array<Complex> &array, Bool resize = false) const;
//
//  void getCol(const ArrayColumn<Bool> &column, const Slicer &slicer, Array<Bool> &array, Bool resize = false) const;
//  void getCol(const ArrayColumn<Float> &column, const Slicer &slicer, Array<Float> &array, Bool resize = false) const;
//  void getCol(const ArrayColumn<Complex> &column, const Slicer &slicer, Array<Complex> &array, Bool resize = false) const;

  void putDataColumn(DataColumn whichOne,
                     const Cube<Complex>& data);

  void putDataColumn(DataColumn whichOne,
                     const Vector<Vector<Slice> >& slices,
                     const Cube<Complex>& data);

  // New slicer supports multiple Slices in channel and correlation

//  Vector<Matrix<Int> >   chanAveBounds_p;
//  Vector<Vector<Slice> > chanSlices_p;
//  Vector<Vector<Slice> > corrSlices_p;
//  Vector<Vector<Slice> > newSlicer_p;
//  Vector<Vector<Slice> > newWtSlicer_p;
//  Table                  selTable_p;
//  Bool                   useNewSlicer_p;

  ArrayColumn<Complex> rwColVis_p;
  ArrayColumn<Float> rwColFloatVis_p;
  ArrayColumn<Complex> rwColModelVis_p;
  ArrayColumn<Complex> rwColCorrVis_p;
  ArrayColumn<Float> rwColWeight_p;
  ArrayColumn<Float> rwColWeightSpectrum_p;
  ArrayColumn<Float> rwColSigma_p;
  ArrayColumn<Bool> rwColFlag_p;
  ScalarColumn<Bool> rwColFlagRow_p;
};

//     ********************
//     ********************
//     **                **
//     **  ROVisIterator **
//     **                **
//     ********************
//     ********************

ROVisIterator::ROVisIterator()
{
}

ROVisIterator::ROVisIterator(const MeasurementSet & ms, const Block<Int>& sortColumns, Double timeInterval)
: ROVisibilityIterator (ms, sortColumns, timeInterval, Factory (this))
{}

ROVisIterator::ROVisIterator(const MeasurementSet & ms, const Block<Int>& sortColumns,
                             Double timeInterval, const ROVisibilityIterator::Factory & factory)
: ROVisibilityIterator (ms, sortColumns, timeInterval, factory)
{}


ROVisIterator::ROVisIterator(const ROVisIterator & other)
: ROVisibilityIterator (other)
{}

ROVisIterator::~ROVisIterator()
{}

ROVisIterator &
ROVisIterator::operator=(const ROVisIterator & other)
{
    ROVisibilityIterator::operator= (other);

    return * this;
}

ROVisIterator &
ROVisIterator::operator++(int)
{
    advance();
    return * this;
}

ROVisIterator &
ROVisIterator::operator++()
{
    advance();
    return * this;
}

Vector<Int>&
ROVisIterator::chanIds(Vector<Int>& chanids) const
{
    return getReadImpl()->chanIds(chanids);
}

Vector<Int>&
ROVisIterator::chanIds(Vector<Int>& chanids, Int spw) const
{
    return getReadImpl()->chanIds(chanids, spw);
}

Vector<Int>&
ROVisIterator::corrIds(Vector<Int>& corrids) const
{
    return getReadImpl()->corrIds(corrids);
}


Vector<Int>&
ROVisIterator::corrType(Vector<Int>& corrTypes) const
{
    return getReadImpl()->corrType(corrTypes);
}

ROVisIteratorImpl *
ROVisIterator::getReadImpl () const
{
    ROVisIteratorImpl * impl = dynamic_cast<ROVisIteratorImpl *> (ROVisibilityIterator::getReadImpl ());

    Assert (impl != NULL);

    return impl;
}

void
ROVisIterator::selectChannel(const Vector<Vector<Slice> >& chansel)
{
    return getReadImpl()->selectChannel(chansel);
}

void
ROVisIterator::selectCorrelation(const Vector<Vector<Slice> >& corrsel)
{
    return getReadImpl()->selectCorrelation(corrsel);
}

Vector<Matrix<Int> >&
ROVisIterator::setChanAveBounds(Float factor, Vector<Matrix<Int> >& bounds)
{
    return getReadImpl()->setChanAveBounds(factor, bounds);
}

void
ROVisIteratorImpl::setTileCache()
{
  // Set the cache size when the DDID changes (as opposed to MS) to avoid
  // overreading in a case like:
  // hcubes: [2, 256], [4, 64]
  // tileshape: [4, 64]
  // spws (ddids): [4,64], [2, 256], [2, 256], [4,64], [2, 256], [4,64]
  // and spw = '0:1~7,1:1~7,2:100~200,3:20~40,4:200~230,5:40~50'
  //
  // For hypercube 0, spw 2 needs 3 tiles, but spw 4 only needs 1, AND the last
  // tile at that.  So if hypercube 0 used a cache of 3 tiles throughout, every
  // read of 4:200~230 would likely also read the unwanted channels 0~127 of
  // the next row.
  //
  if (curStartRow_p != 0 && ! msIter_p.newDataDescriptionId()){
    return;
  }

  const MeasurementSet& thems = msIter_p.ms();

  if(thems.tableType() == Table::Memory){
    return;
  }

  const ColumnDescSet& cds=thems.tableDesc().columnDescSet();

  // Get the first row number for this DDID.
  Vector<rownr_t> rownums;
  rowIds(rownums);
  uInt startrow = rownums[0];

  Vector<String> columns(8);
  // complex
  columns(0)=MS::columnName(MS::DATA);
  columns(1)=MS::columnName(MS::CORRECTED_DATA);
  columns(2)=MS::columnName(MS::MODEL_DATA);
  // boolean
  columns(3)=MS::columnName(MS::FLAG);
  // float
  columns(4)=MS::columnName(MS::WEIGHT_SPECTRUM);
  columns(5)=MS::columnName(MS::WEIGHT);
  columns(6)=MS::columnName(MS::SIGMA);
  // double
  columns(7)=MS::columnName(MS::UVW);
  //
  for(uInt k = 0; k < columns.nelements(); ++k){
    if(cds.isDefined(columns(k))){
      const ColumnDesc& cdesc = cds[columns(k)];
      String dataManType="";

      dataManType = cdesc.dataManagerType();
      // We have to check WEIGHT_SPECTRUM as it tends to exist but not have
      // valid data.
      if(columns[k] == MS::columnName(MS::WEIGHT_SPECTRUM) &&
         !existsWeightSpectrum())
        dataManType="";

      // Sometimes columns may not contain anything yet
      if((columns[k]==MS::columnName(MS::DATA) && (columns_p.vis_p.isNull() ||
                                                   !columns_p.vis_p.isDefined(0))) ||
         (columns[k]==MS::columnName(MS::MODEL_DATA) && (columns_p.modelVis_p.isNull() ||
                                                         !columns_p.modelVis_p.isDefined(0))) ||
         (columns[k]==MS::columnName(MS::CORRECTED_DATA) && (columns_p.corrVis_p.isNull() ||
                                                             !columns_p.corrVis_p.isDefined(0))) ||
         (columns[k]==MS::columnName(MS::FLAG) && (columns_p.flag_p.isNull() ||
                                                   !columns_p.flag_p.isDefined(0))) ||
         (columns[k]==MS::columnName(MS::WEIGHT) && (columns_p.weight_p.isNull() ||
                                                     !columns_p.weight_p.isDefined(0))) ||
         (columns[k]==MS::columnName(MS::SIGMA) && (columns_p.sigma_p.isNull() ||
                                                    !columns_p.sigma_p.isDefined(0))) ||
         (columns[k]==MS::columnName(MS::UVW) && (columns_p.uvw_p.isNull() ||
                                                  !columns_p.uvw_p.isDefined(0))) ){
        dataManType="";
      }

      if(dataManType.contains("Tiled") &&
         !String(cdesc.dataManagerGroup()).empty()){
        try {
          ROTiledStManAccessor tacc=ROTiledStManAccessor(thems,
                                                         cdesc.dataManagerGroup());

          // This is for the data columns, WEIGHT_SPECTRUM and FLAG only.
          if((columns[k] != MS::columnName(MS::WEIGHT)) &&
             (columns[k] != MS::columnName(MS::UVW))){
            // Figure out how many tiles are needed to span the selected channels.
            const IPosition tileShape(tacc.tileShape(startrow));
            Vector<Int> ids;
            chanIds(ids);
            uInt startTile = ids[0] / tileShape[1];
            uInt endTile = ids[ids.nelements() - 1] / tileShape[1];
            uInt cachesize = endTile - startTile + 1;

            // and the selected correlations.
            corrIds(ids);
            startTile = ids[0] / tileShape[0];
            endTile = ids[ids.nelements() - 1] / tileShape[0];
            cachesize *= endTile - startTile + 1;

            // The cache needs to hold enough tiles to provide the data
            // for a single row for all channels and all correlations.
            // Determine the number of tiles in the 0 and 1 directions,
            // round them up to the nearest integer and take the product
            // of the two to yield the number of tiles needed.
            //
            // This could be reduced a little bit if necessary by using the
            // actual frequency selection since the current calculation uses
            // all of the channels in the row, thus reserves space in the cache
            // for tiles which contain no selected data (however the logic doesn't
            // appear to read those into the cache so there is no performance
            // penalty).

            const IPosition hShape(tacc.hypercubeShape(startrow));

            float nTiles0 = hShape [0] / (float) (tileShape [0]);
            float nTiles1 = hShape [1] / (float) (tileShape [1]);

            cachesize = (uInt) (ceil (nTiles0) * ceil (nTiles1));

            tacc.setCacheSize(startrow, cachesize);
          }
          else
            tacc.setCacheSize(startrow, 1);
        }
        catch (AipsError x) {
          //It failed so leave the caching as is.
          continue;
        }
      }
    }
  }
}


// Selected spws and channel counts
void ROVisIterator::allSelectedSpectralWindows (Vector<Int> & spws, Vector<Int> & nvischan)
{
  getReadImpl()->allSelectedSpectralWindows(spws,nvischan);
}

void ROVisIterator::lsrFrequency(const Int& spw, Vector<Double>& freq, Bool& convert, const  Bool ignoreconv)
{
  getReadImpl()->lsrFrequency(spw,freq,convert,ignoreconv);
}


Int
ROVisIterator::numberChan(Int spw) const
{
    return getReadImpl()->numberChan(spw);
}

Int
ROVisIterator::numberCorr(Int pol) const
{
    return getReadImpl()->numberCorr(pol);
}

void
ROVisIterator::getDataColumn(DataColumn whichOne, const Vector<Vector<Slice> >& slices, Cube<Complex>& data) const
{
    getReadImpl()->getDataColumn(whichOne, slices, data);
}

VisibilityIteratorReadImpl *
ROVisIterator::Factory::operator() (const asyncio::PrefetchColumns * /*prefetchColumns*/,
                                    const Block<MeasurementSet>& mss,
                                    const Block<Int>& sortColumns,
                                    const Bool /*addDefaultSortCols*/,
                                    Double timeInterval) const
{
    return new ROVisIteratorImpl (vi_p, mss[0], sortColumns, timeInterval);
}

//     *************************
//     *************************
//     **                     **
//     **  ROVisIteratorImpl  **
//     **                     **
//     *************************
//     *************************

ROVisIteratorImpl::ROVisIteratorImpl() {}

ROVisIteratorImpl::ROVisIteratorImpl(ROVisIterator * rovi,
                                     const MeasurementSet &ms,
                                     const Block<Int>& sortColumns,
                                     Double timeInterval)
: VisibilityIteratorReadImpl(rovi, Block<MeasurementSet> (1, ms), sortColumns, true, timeInterval),
  useNewSlicer_p(false)
{

    // Initialize multi-slicers with empty slices
    chanSlices_p.resize(numberSpw());
    chanSlices_p.set(Vector<Slice>());
    corrSlices_p.resize(numberPol());
    corrSlices_p.set(Vector<Slice>());

}

ROVisIteratorImpl::ROVisIteratorImpl(const ROVisIteratorImpl& other)
: VisibilityIteratorReadImpl (other)
{
    operator=(other);
}

ROVisIteratorImpl::~ROVisIteratorImpl() {}

ROVisIteratorImpl&
ROVisIteratorImpl::operator=(const ROVisIteratorImpl& other)
{
  if (this!=&other) {

    VisibilityIteratorReadImpl::operator=(other);

    chanAveBounds_p = other.chanAveBounds_p;
    chanSlices_p = other.chanSlices_p;
    corrSlices_p = other.corrSlices_p;
    newSlicer_p = other.newSlicer_p;
    newWtSlicer_p = other.newWtSlicer_p;
    selTable_p = other.selTable_p;
    useNewSlicer_p = other.useNewSlicer_p;

  }

  return *this;
}

ROVisIteratorImpl & ROVisIteratorImpl::operator++(int)
{
  if (! more ()){
      advance();
  }
  return *this;
}

ROVisIteratorImpl & ROVisIteratorImpl::operator++()
{
  if (! more ()){
      advance();
  }
  return *this;
}



void ROVisIteratorImpl::updateSlicer()
{
  useNewSlicer_p=true;

  //    cout << "Using new slicer!..." << flush;
  
  newSlicer_p.resize(2);
  newWtSlicer_p.resize(1);
  
  //    cout << "newSlicer_p.shape() = " << newSlicer_p.shape() << endl;
  
  useSlicer_p=false;
  
  // Refer to correct slices for the current spwid/polid
  newSlicer_p(0).reference(corrSlices_p(this->polarizationId()));
  newSlicer_p(1).reference(chanSlices_p(this->spectralWindow()));
  
  newWtSlicer_p(0).reference(corrSlices_p(this->polarizationId()));

  setTileCache();
}

// 

// Selected spws and channel counts
void ROVisIteratorImpl::allSelectedSpectralWindows (Vector<Int> & spws, Vector<Int> & nvischan) {

  Vector<Int> ddids;
  msColumns().dataDescId().getColumn(ddids);
  Int ndd=genSort(ddids, Sort::Ascending, (Sort::QuickSort | Sort::NoDuplicates));
  ddids.resize(ndd,true);
  //  cout << "ddids = " << ddids << endl;
  Vector<Int> spwperdd;
  msColumns().dataDescription().spectralWindowId().getColumn(spwperdd);
  //  cout << "spwperdd = " << spwperdd << endl;
  spws.resize(ndd);
  nvischan.resize(numberSpw());
  nvischan.set(-1);
  for (Int idd=0;idd<ndd;++idd) {
    spws(idd)=spwperdd(ddids(idd));
    nvischan(spws(idd))=this->numberChan(spws(idd));
  }

}

void ROVisIteratorImpl::lsrFrequency(const Int& spw, Vector<Double>& freq, 
				     Bool& convert, const  Bool ignoreconv) {


  MFrequency::Types obsMFreqType = (MFrequency::Types) msIter_p.msColumns ().spectralWindow ().measFreqRef ()(spw);
  convert = obsMFreqType != MFrequency::LSRK; // make this parameter write-only
  if (ignoreconv) convert=false;  // override if user says so

  // Set up the frequency converter
  MEpoch ep;
  ROScalarMeasColumn<MEpoch>(msIter_p.table (), MS::columnName (MS::TIME)).get (curStartRow_p, ep); // Setting epoch to iteration's first one
  MPosition obsPos = msIter_p.telescopePosition ();
  MDirection dir = msIter_p.phaseCenter ();
  MeasFrame frame (ep, obsPos, dir);
  MFrequency::Convert tolsr (obsMFreqType,
			     MFrequency::Ref (MFrequency::LSRK, frame));


  // Get the requested spw's nominal frequencies (all of them)
  //  (we will index these with channel ids later)
  Vector<Double> chanFreq(0);
  chanFreq = msIter_p.msColumns ().spectralWindow ().chanFreq ()(spw);

  // The selected channel ids for the requested spw
  Vector<Int> chans(0);
  this->chanIds(chans,spw);
  Int nchan=chans.nelements();

  // Create output frequencies
  freq.resize(nchan);
  for (Int ich=0;ich<nchan;++ich) {
    if (convert)
      freq[ich]=tolsr(chanFreq(chans(ich))).getValue().getValue();
    else
      freq[ich]=chanFreq(chans(ich));

  }

}


// (Alternative syntax for ROVisIter::chanIds)
Vector<Int>& ROVisIteratorImpl::channel(Vector<Int>& chan) const
{ return this->chanIds(chan); }


Vector<Int>& ROVisIteratorImpl::chanIds(Vector<Int>& chanids) const
{
  return chanIds(chanids,msIter_p.spectralWindowId());
}

Vector<Int>& ROVisIteratorImpl::chanIds(Vector<Int>& chanids, Int spw) const
{

  chanids.resize(this->numberChan(spw));

  // if non-trivial slices available
  if (chanSlices_p(spw).nelements() > 0 ) {
    
    Vector<Slice> slices(chanSlices_p(spw));
    Int nslices=slices.nelements();
    
    Int ich0=0;
    for (Int i=0;i<nslices;++i) {
      Int nchan=slices(i).length();
      Int start=slices(i).start();
      for (Int ich=0;ich<nchan;++ich,++ich0) 
	chanids(ich0)=start+ich;
    }
  }
  else {
    // all channels selected...
    indgen(chanids);
  }
  return chanids;
}

void ROVisIteratorImpl::setSelTable()
{
    VisibilityIteratorReadImpl::setSelTable();

    // The following code (which uses Table::operator() to create
    // a new RefTable) is computationally expensive. This could
    // be optimized by using the same method as in the
    // VisibilityIterator base class (which is to not create
    // RefTables but instead access the table column directly in
    // getReadImpl()->msIter_p.table() using VisibilityIterator::getReadImpl()->selRows_p).

    // Doing so would mean replacing calls like
    //     colSigma.getColumn(newWtSlicer_p,sigmat,true);
    // with
    //     colSigma.getColumnCells(getReadImpl()->selRows_p,newWtSlicer_p,sigmat,true);
    //
    // However the ArrayColumn class does allow passing both a 
    // Vector<Vector<Slice> > and a RefRows parameter to get-/putColumn.
    // Such put/get functions must be first implemented.

    Vector<rownr_t> rows(curNumRow_p);
    indgen(rows,curStartRow_p);
    selTable_p=msIter_p.table()(rows);
    attachColumns(attachTable());
}

// Return native correlation _indices_
Vector<Int>& ROVisIteratorImpl::corrIds(Vector<Int>& corrids) const
{
  Int pol = msIter_p.polarizationId();

  corrids.resize(this->numberCorr(pol));

  Vector<Slice> slices(corrSlices_p(pol));
  Int nslices=slices.nelements();
  
  // if non-trivial slices available
  if (nslices > 0 ) {
    
    Int icor0=0;
    for (Int i=0;i<nslices;++i) {
      Int ncorr=slices(i).length();
      Int start=slices(i).start();
      for (Int icor=0;icor<ncorr;++icor,++icor0) 
	corrids(icor0)=start+icor;
    }
  }
  else {
    // all corrs selected...
    indgen(corrids);
  }

  return corrids;
}

Vector<Int>&
ROVisIteratorImpl::corrType(Vector<Int>& corrTypes) const
{

  // Get the nominal corrType list
  Int polId = msIter_p.polarizationId();
  Vector<Int> nomCorrTypes;
  msIter_p.msColumns().polarization().corrType().get(polId,nomCorrTypes,true);

  // Get the 0-based corr indices
  Vector<Int> corrids;
  corrIds(corrids);

  // Set the corrType values by the corrids
  Int nCor=corrids.nelements();
  corrTypes.resize(nCor);
  for (Int icor=0;icor<nCor;++icor)
    corrTypes[icor]=nomCorrTypes[corrids[icor]];

  return corrTypes;
}

Cube<Bool>& ROVisIteratorImpl::flag(Cube<Bool>& flags) const
{
  if (useNewSlicer_p){
      columns_p.flag_p.getColumn(newSlicer_p,flags,true);
  }
  else {
      columns_p.flag_p.getColumn(flags,true);
  }
  return flags;
}

Vector<Double>& ROVisIteratorImpl::frequency(Vector<Double>& freq) const
{
  
  // We need to change some internals transparently
  //ROVisIteratorImpl* self = const_cast<ROVisIteratorImpl*>(this);

  if (! cache_p.freqCacheOK_p) {
    cache_p.freqCacheOK_p=true;
    const Vector<Double>& chanFreq=msIter_p.frequency();
    Vector<Int> chan;
    channel(chan);
    Int nch=chan.nelements();
    cache_p.frequency_p.resize(nch);

    for (Int i=0;i<nch;++i){
      cache_p.frequency_p(i)=chanFreq(chan(i));
    }
  }
  // Always copy to output
  freq.resize(cache_p.frequency_p.nelements());
  freq=cache_p.frequency_p;

  return freq;
}

Cube<Complex>& 
ROVisIteratorImpl::visibility(Cube<Complex>& vis, DataColumn whichOne) const
{
  if (useNewSlicer_p){
      getDataColumn(whichOne,newSlicer_p,vis);
  }
  else {
      VisibilityIteratorReadImpl::getDataColumn(whichOne,vis);
  }
  return vis;
}

void ROVisIteratorImpl::getDataColumn(DataColumn whichOne,
				  const Vector<Vector<Slice> >& slices,
				  Cube<Complex>& data) const
{ 
  // Return the visibility (observed, model or corrected);
  // deal with DATA and FLOAT_DATA seamlessly for observed data.
  switch (whichOne) {
  case ROVisIterator::Observed:
    if (floatDataFound_p) {
      Cube<Float> dataFloat;
      columns_p.floatVis_p.getColumn(slices,dataFloat,true);
      data.resize(dataFloat.shape());
      convertArray(data,dataFloat);
    } else {
      columns_p.vis_p.getColumn(slices,data,true);
    };
    break;
  case ROVisIterator::Corrected:
    columns_p.corrVis_p.getColumn(slices,data,true);
    break;
  case ROVisIterator::Model:
    columns_p.modelVis_p.getColumn(slices,data,true);
    break;
  };
 
};

Matrix<Float>& ROVisIteratorImpl::sigmaMat(Matrix<Float>& sigmat) const
{
  if (useNewSlicer_p) columns_p.sigma_p.getColumn(newWtSlicer_p,sigmat,true);
  else {
    sigmat.resize(nPol_p,curNumRow_p);
    columns_p.sigma_p.getColumn(sigmat);
  }
  return sigmat;
}

Matrix<Float>& ROVisIteratorImpl::weightMat(Matrix<Float>& wtmat) const
{
  if (useNewSlicer_p) columns_p.weight_p.getColumn(newWtSlicer_p,wtmat,true);
  else {
    wtmat.resize(nPol_p,curNumRow_p);
    columns_p.weight_p.getColumn(wtmat);
  }
  return wtmat;
}

Cube<Float>& ROVisIteratorImpl::weightSpectrum(Cube<Float>& wtsp) const
{
  if (this->existsWeightSpectrum()) {
    if (useNewSlicer_p) columns_p.weightSpectrum_p.getColumn(newSlicer_p,wtsp,true);
    else {
      wtsp.resize(nPol_p,nChan_p,curNumRow_p);
      columns_p.weightSpectrum_p.getColumn(wtsp);
    }
  } else {
    wtsp.resize(0,0,0);
  }
  return wtsp;
}

void ROVisIteratorImpl::selectChannel(const Vector<Vector<Slice> >& chansel) {
  //  cout << "selectChannel!...vss..." << flush;

  if (chansel.nelements() != uInt(numberSpw()))
    throw(AipsError("Specified channel slices has incorrect number of spws."));
  
  chanSlices_p.resize(numberSpw(),false);
  chanSlices_p=chansel;

  // Enforce use of the new slicer downstream
  useNewSlicer_p=true;
    
  //  cout << "done." << endl;

}

void ROVisIteratorImpl::selectCorrelation(const Vector<Vector<Slice> >& corrsel) {

  //  cout << "selectCorrelation!...vvs..." << flush;

  if (corrsel.nelements() != uInt(numberPol()))
    throw(AipsError("Specified correlation slices has incorrect number of polIds."));
  
  corrSlices_p.resize(numberPol(),false);
  corrSlices_p=corrsel;

  // Enforce use of the new slicer downstream
  useNewSlicer_p=true;
    
  //  cout << "done." << endl;

}

Vector<Matrix<Int> >& ROVisIteratorImpl::setChanAveBounds(Float factor,
						      Vector<Matrix<Int> >& bounds)
{
  if(!useNewSlicer_p)
    throw(AipsError("Help!"));

  // For every spw...
  bounds.resize(numberSpw());

  // If factor greater than zero, fill the bounds non-trivially
  if (factor > 0) {
 
    // Decipher averaging factor
    Int width = 1;
    if(factor > 1.0)
      width = Int(factor); // factor supplied in channel units
 
    // Calculate bounds for each spw
    for(Int ispw = 0; ispw < numberSpw(); ++ispw){
   
      // Number of SELECTED channels PRIOR to averaging
      Int nChan0 = numberChan(ispw);
   
      // factor might have been supplied as a fraction;
      //  width is then potentially spw-dependent
      if(factor <= 1.0)
	width = Int(factor * Float(nChan0));
   
      // Get the selected channel list
      Vector<Int> chans;
      chanIds(chans, ispw);

      // The nominal number of output channels depends on the full
      // range of channels selected (not the number of them)
      //  (will be revised later, if nec.)
      Int nChanOut0 = 1 + (chans[nChan0 - 1] - chans[0]) / width;
   
      // Initialize the bounds container for this spw
      Matrix<Int>& currBounds(bounds[ispw]);
      currBounds.resize(nChanOut0, 4);
      //currBounds.set(0);
   
      Int outChan = 0;
      Int firstchan = chans[0];
      Int lastchan = chans[nChan0 - 1];

      // Index of input channel in SELECTED list, i.e.
      // ich = vi.chanIds(chanids, spw)[selchanind].
      uInt selchanind = 0;

      for(Int ich = firstchan; ich <= lastchan; ich += width){
        Int w = width;

        // Use a clamped width in case (lastchan - firstchan + 1) % width != 0.
        if(ich + w - 1 > lastchan)
          w = lastchan + 1 - ich;

        // The start and end in MS channel #s.
        currBounds(outChan, 0) = ich;
        currBounds(outChan, 1) = ich + w - 1;

        // The start and end in selected reckoning.
        currBounds(outChan, 2) = selchanind;
        selchanind += w;
        currBounds(outChan, 3) = selchanind - 1;

        // for(uInt ii = 0; ii < 4; ++ii)
        //   cerr << "currBounds(" << outChan << ", " << ii << ") = "
        //        << currBounds(outChan, ii) << endl;
        ++outChan;
      }
    } // ispw

  } // factor > 0

  // Internal reference  (needed?)
  chanAveBounds_p.reference(bounds);

  // Return the bounds Vector reference
  return bounds;
}

// Vector<Matrix<Int> >& ROVisIteratorImpl::setChanAveBounds(Float factor,
// 						      Vector<Matrix<Int> >& bounds) {

//   if (!useNewSlicer_p) throw(AipsError("Help!"));

//   // For every spw...
//   bounds.resize(numberSpw());

//   // If factor greater than zero, fill the bounds non-trivially
//   if (factor>0) {
    
//     // Decipher averaging factor
//     Int width(1);
//     if (factor>1.0) width=Int(factor); // factor supplied in channel units
    
//     // Calculate bounds for each spw
//     for (Int ispw=0;ispw<numberSpw();++ispw) {
      
//       // Number of SELECTED channels PRIOR to averaging
//       Int nChan0=numberChan(ispw);
      
//       // factor might have been supplied in factional units;
//       //  width is then potentially spw-dependent
//       if (factor<=1.0)
// 	width=Int(factor*Float(nChan0));
      
//       // Get the selected channel list
//       Vector<Int> chans;
//       chanIds(chans,ispw);

//       // The nominal number of output channels depends on the full
//       // range of channels selected (not the number of them)
//       //  (will be revised later, if nec.)
//       Int nChanOut0((chans(nChan0-1)-chans(0)+1+width)/width);
      
//       // Initialize the bounds container for this spw
//       Matrix<Int>& currBounds(bounds(ispw));
//       currBounds.resize(nChanOut0,4);
//       currBounds.set(0);
      
//       // Actual output channel count; initialization
//       Int nChanOut=1;
//       Int lo(chans(0));
//       currBounds(0,0)=lo;
//       currBounds(0,2)=0;
//       for (Int ich=0;ich<nChan0;++ich) 
// 	if ( (chans(ich)-lo+1)>width ) {
// 	  currBounds(nChanOut-1,1)=chans(ich-1);   // end of previous
// 	  currBounds(nChanOut-1,3)=ich-1;
// 	  lo=currBounds(nChanOut,0)=chans(ich);    // start of next
// 	  currBounds(nChanOut,2)=ich;    // start of next
// 	  ++nChanOut;
// 	}
//       currBounds(nChanOut-1,1)=chans(nChan0-1);    // end of final set
//       currBounds(nChanOut-1,3)=(nChan0-1);    // end of final set
      
// //       for(uInt ii = 0; ii < 4; ++ii)
// //         cerr << "currBounds(" << nChanOut - 1 << ", " << ii << ") = "
// //              << currBounds(nChanOut - 1, ii) << endl;

//       // contract bounds container, if necessary
//       if (nChanOut<nChanOut0)
// 	currBounds.resize(nChanOut,4,true);
      
//     } // ispw


//   } // factor > 0

//   // Internal reference  (needed?)
//   chanAveBounds_p.reference(bounds);

//   // Return the bounds Vector reference
//   return bounds;

// }

Int ROVisIteratorImpl::numberChan(Int spw) const {

  // Nominally all channels this spw
  Int nchan=msColumns().spectralWindow().numChan()(spw);

  if (useNewSlicer_p) {
    Int nslices=chanSlices_p(spw).nelements();
    if (nslices > 0 ) {
      nchan=0;
      for (Int isl=0;isl<nslices;++isl) 
	nchan+=chanSlices_p(spw)(isl).length();
    }
  }

  return nchan;

}


Int ROVisIteratorImpl::numberCorr(Int pol) const {

  // Nominally all correlations this pol
  Int ncorr=msColumns().polarization().numCorr()(pol);

  if (useNewSlicer_p) {
    Int nslices=corrSlices_p(pol).nelements();
    if (nslices > 0 ) {
      // Accumulate from slice lengths
      ncorr=0;
      for (Int isl=0;isl<nslices;++isl) 
	ncorr+=corrSlices_p(pol)(isl).length();
    }
  }

  return ncorr;

}

void ROVisIteratorImpl::getCol(const ScalarColumn<Bool> &column, Vector<Bool> &array, Bool resize) const
{
    column.getColumn(array, resize);
}

void ROVisIteratorImpl::getCol(const ScalarColumn<Int> &column, Vector<Int> &array, Bool resize) const
{
    column.getColumn(array, resize);
}

void ROVisIteratorImpl::getCol(const ScalarColumn<Double> &column, Vector<Double> &array, Bool resize) const
{
    column.getColumn(array, resize);
}

void ROVisIteratorImpl::getCol(const ArrayColumn<Bool> &column, Array<Bool> &array, Bool resize) const
{
    column.getColumn(array, resize);
}

void ROVisIteratorImpl::getCol(const ArrayColumn<Float> &column, Array<Float> &array, Bool resize) const
{
    column.getColumn(array, resize);
}

void ROVisIteratorImpl::getCol(const ArrayColumn<Double> &column, Array<Double> &array, Bool resize) const
{
    column.getColumn(array, resize);
}

void ROVisIteratorImpl::getCol(const ArrayColumn<Complex> &column, Array<Complex> &array, Bool resize) const
{
    column.getColumn(array, resize);
}

void ROVisIteratorImpl::getCol(const ArrayColumn<Bool> &column, const Slicer &slicer, Array<Bool> &array, Bool resize) const
{
    column.getColumn(slicer, array, resize);
}

void ROVisIteratorImpl::getCol(const ArrayColumn<Float> &column, const Slicer &slicer, Array<Float> &array, Bool resize) const
{
    column.getColumn(slicer, array, resize);
}

void ROVisIteratorImpl::getCol(const ArrayColumn<Complex> &column, const Slicer &slicer, Array<Complex> &array, Bool resize) const
{
    column.getColumn(slicer, array, resize);
}


Vector<RigidVector<Double,3> >& 
ROVisIteratorImpl::uvw(Vector<RigidVector<Double,3> >& uvwvec) const
{
    uvwvec.resize(curNumRow_p);
    getCol(columns_p.uvw_p, cache_p.uvwMat_p,true);
    // get a pointer to the raw storage for quick access
    Bool deleteIt;
    Double* pmat = cache_p.uvwMat_p.getStorage(deleteIt);
    for (uInt row=0; row<curNumRow_p; row++, pmat+=3) uvwvec(row)=pmat;
    return uvwvec;
}


const Table
ROVisIteratorImpl::attachTable() const
{
    return selTable_p;
}

//     ******************
//     ******************
//     **              **
//     **  VisIterator **
//     **              **
//     ******************
//     ******************


VisIterator::VisIterator() {}

VisIterator::VisIterator(MeasurementSet &MS, 
			 const Block<Int>& sortColumns, 
			 Double timeInterval)
  : ROVisIterator (MS, sortColumns, timeInterval, Factory (this))
{}

VisIterator::VisIterator(const VisIterator & other)
: ROVisIterator (other)
{
    operator=(other);
}

VisIterator::~VisIterator() {}

VisIterator& 
VisIterator::operator=(const VisIterator& other)
{
    if (this!=&other) {

        ROVisIterator::operator= (other);
    }

        return * this;
}

VisIterator &
VisIterator::operator++(int)
{
  advance();

  return *this;
}

VisIterator &
VisIterator::operator++()
{
  advance();

  return *this;
}

void
VisIterator::attachColumns(const Table &table)
{
    getImpl() -> attachColumns (table);
}

VisIteratorImpl *
VisIterator::getImpl () const
{
    VisIteratorImpl * impl = dynamic_cast<VisIteratorImpl *> (ROVisibilityIterator::getReadImpl ());

    Assert (impl != NULL);

    return impl;
}

void VisIterator::setFlagRow(const Vector<Bool>& rowflags)
{
    getImpl() -> setFlagRow (rowflags);
};

void VisIterator::setVis(const Cube<Complex>& vis, DataColumn whichOne)
{
    getImpl () -> setVis (vis, whichOne);
}

void VisIterator::setFlag(const Cube<Bool>& flags)
{
    getImpl () -> setFlag (flags);
}

void VisIterator::setVisAndFlag(const Cube<Complex>& vis,
				       const Cube<Bool>& flag,
				       DataColumn whichOne)
{
    getImpl () -> setVisAndFlag (vis, flag, whichOne);
}

void VisIterator::setWeightMat(const Matrix<Float>& weightMat)
{
    getImpl () -> setWeightMat (weightMat);
}

void VisIterator::setWeightSpectrum(const Cube<Float>& weightSpectrum)
{
    getImpl () -> setWeightSpectrum (weightSpectrum);
}

void VisIterator::putDataColumn(DataColumn whichOne,
				const Vector<Vector<Slice> >& slices,
				const Cube<Complex>& data)
{
    getImpl () -> putDataColumn (whichOne, slices, data);
};  

void VisIterator::putDataColumn(DataColumn whichOne,
				const Cube<Complex>& data)
{
    getImpl () -> putDataColumn (whichOne, data);
};  

Vector<rownr_t>& ROVisIteratorImpl::rowIds(Vector<rownr_t>& rowids) const
{
  rowids.resize(curNumRow_p);
  rowids=selTable_p.rowNumbers();
  return rowids;
}


void VisIterator::putCol(ScalarColumn<Bool> &column, const Vector<Bool> &array)
{
    column.putColumn(array);
}

void VisIterator::putCol(ArrayColumn<Bool> &column, const Array<Bool> &array)
{
    column.putColumn(array);
}

void VisIterator::putCol(ArrayColumn<Float> &column, const Array<Float> &array)
{
    column.putColumn(array);
}

void VisIterator::putCol(ArrayColumn<Complex> &column, const Array<Complex> &array)
{
    column.putColumn(array);
}

void VisIterator::putCol(ArrayColumn<Bool> &column, const Slicer &slicer, const Array<Bool> &array)
{
    column.putColumn(slicer, array);
}

void VisIterator::putCol(ArrayColumn<Float> &column, const Slicer &slicer, const Array<Float> &array)
{
    column.putColumn(slicer, array);
}

void VisIterator::putCol(ArrayColumn<Complex> &column, const Slicer &slicer, const Array<Complex> &array)
{
    column.putColumn(slicer, array);
}

VisibilityIteratorReadImpl *
VisIterator::Factory::operator() (const asyncio::PrefetchColumns * /*prefetchColumns*/,
                                  const Block<MeasurementSet>& mss,
                                  const Block<Int>& sortColumns,
                                  const Bool /*addDefaultSortCols*/,
                                  Double timeInterval) const
{
    return new VisIteratorImpl (vi_p, mss[0], sortColumns, timeInterval);
}

//     ***********************
//     ***********************
//     **                   **
//     **  VisIteratorImpl  **
//     **                   **
//     ***********************
//     ***********************

VisIteratorImpl::VisIteratorImpl(ROVisIterator * rovi,
                                 const MeasurementSet& ms,
                                 const Block<Int>& sortColumns,
                                 Double timeInterval)
: ROVisIteratorImpl (rovi, ms, sortColumns, timeInterval)
{
}

VisIteratorImpl::~VisIteratorImpl()
{}

VisIteratorImpl &
VisIteratorImpl::operator=(const VisIteratorImpl &other){

    if (this != & other){

        ROVisIteratorImpl::operator=(other);

        rwColFlag_p.reference(other.rwColFlag_p);
        rwColFlagRow_p.reference(other.rwColFlagRow_p);
        rwColVis_p.reference(other.rwColVis_p);
        rwColFloatVis_p.reference(other.rwColFloatVis_p);
        rwColModelVis_p.reference(other.rwColModelVis_p);
        rwColCorrVis_p.reference(other.rwColCorrVis_p);
        rwColWeight_p.reference(other.rwColWeight_p);
        rwColWeightSpectrum_p.reference(other.rwColWeightSpectrum_p);
        rwColSigma_p.reference(other.rwColSigma_p);

    }

    return * this;
}


void
VisIteratorImpl::attachColumns(const Table &table)
{

    ROVisIteratorImpl::attachColumns(table);

    const ColumnDescSet& cds = table.tableDesc().columnDescSet();

    if (cds.isDefined("CORRECTED_DATA")){
        rwColCorrVis_p.attach(table,"CORRECTED_DATA");
    }

    if (cds.isDefined(MS::columnName(MS::DATA))) {
        rwColVis_p.attach(table,MS::columnName(MS::DATA));
    }

    rwColFlag_p.attach(table,MS::columnName(MS::FLAG));

    rwColFlagRow_p.attach(table,MS::columnName(MS::FLAG_ROW));

    if (cds.isDefined(MS::columnName(MS::FLOAT_DATA))) {
        floatDataFound_p=true;
        rwColFloatVis_p.attach(table,MS::columnName(MS::FLOAT_DATA));
    }
    else {
        floatDataFound_p=false;
    }

    if (cds.isDefined("MODEL_DATA")){
        rwColModelVis_p.attach(table,"MODEL_DATA");
    }

    rwColSigma_p.attach(table,MS::columnName(MS::SIGMA));

    rwColWeight_p.attach(table,MS::columnName(MS::WEIGHT));

    if (cds.isDefined("WEIGHT_SPECTRUM")){
        rwColWeightSpectrum_p.attach(table,"WEIGHT_SPECTRUM");
    }
}
void
VisIteratorImpl::putDataColumn(DataColumn whichOne,
                               const Vector<Vector<Slice> >& slices,
                               const Cube<Complex>& data)
{
    // Set the visibility (observed, model or corrected);
    // deal with DATA and FLOAT_DATA seamlessly for observed data.

    switch (whichOne) {

    case VisibilityIterator::Observed:

        if (floatDataFound_p) {
            Cube<Float> dataFloat=real(data);
            rwColFloatVis_p.putColumn(slices,dataFloat);
        } else {
            rwColVis_p.putColumn(slices,data);
        };
        break;

    case VisibilityIterator::Corrected:

        rwColCorrVis_p.putColumn(slices,data);
        break;

    case VisibilityIterator::Model:

        rwColModelVis_p.putColumn(slices,data);
        break;
    };
};

void
VisIteratorImpl::putDataColumn(DataColumn whichOne,
                               const Cube<Complex>& data)
{
    // Set the visibility (observed, model or corrected);
    // deal with DATA and FLOAT_DATA seamlessly for observed data.

    switch (whichOne) {

    case VisibilityIterator::Observed:

        if (floatDataFound_p) {
            Cube<Float> dataFloat=real(data);
            rwColFloatVis_p.putColumn(dataFloat);
        } else {
            rwColVis_p.putColumn(data);
        };
        break;

    case VisibilityIterator::Corrected:

        rwColCorrVis_p.putColumn(data);
        break;

    case VisibilityIterator::Model:

        rwColModelVis_p.putColumn(data);
        break;
    };
};


void
VisIteratorImpl::setFlag(const Cube<Bool>& flags)
{
    if (useNewSlicer_p){
        rwColFlag_p.putColumn (newSlicer_p, flags);
    }
    else{
        rwColFlag_p.putColumn(flags);
    }
}

void
VisIteratorImpl::setFlagRow(const Vector<Bool>& rowflags)
{
    rwColFlagRow_p.putColumn(rowflags);
};

void
VisIteratorImpl::setVis(const Cube<Complex>& vis, DataColumn whichOne)
{

    if (useNewSlicer_p){
        putDataColumn (whichOne, newSlicer_p, vis);
    }
    else {
        putDataColumn (whichOne, vis);
    }

}

void
VisIteratorImpl::setVisAndFlag(const Cube<Complex>& vis,
                               const Cube<Bool>& flag,
                               DataColumn whichOne)
{
    this->setFlag(flag);
    this->setVis(vis,whichOne);
}

void
VisIteratorImpl::setWeightMat(const Matrix<Float>& weightMat)
{
    if (useNewSlicer_p){
        rwColWeight_p.putColumn (newWtSlicer_p, weightMat);
    }
    else{
        rwColWeight_p.putColumn (weightMat);
    }
}

void
VisIteratorImpl::setWeightSpectrum(const Cube<Float>& weightSpectrum)
{
    if (existsWeightSpectrum()) {

        if (useNewSlicer_p){
            rwColWeightSpectrum_p.putColumn(newSlicer_p,weightSpectrum);
        }
        else{
            rwColWeightSpectrum_p.putColumn(weightSpectrum);
        }
    }
    else {
        throw(AipsError("Can't set WEIGHT_SPECTRUM -- it doesn't exist!"));
    }
}

} //# NAMESPACE CASA - END

