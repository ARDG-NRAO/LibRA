// -*- C++ -*-
//# DataBase.h: Definition of the DataBase class
//# Copyright (C) 2021
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
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$

#ifndef ROADRUNNER_DATABASE_H
#define ROADRUNNER_DATABASE_H

#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
using namespace casacore;
using namespace std;

//
//-------------------------------------------------------------------
//
/**
 * @brief Loads a measurement set and applies a selection.
 *
 * This function loads a measurement set from the specified file, applies a selection based on the specified spw, field, and uvDist expressions, and returns the selected measurement set along with the spw and field IDs.
 *
 * @param msname The name of the measurement set file to load.
 * @param spwStr The spw expression to use for the selection.
 * @param fieldStr The field expression to use for the selection.
 * @param uvDistStr The uvDist expression to use for the selection.
 * @param thems The measurement set to load.
 * @param selectedMS The selected measurement set.
 * @param msSelection The MSSelection object to use for the selection.
 * @param verifyMS A function to verify the loaded measurement set.
 * @return A tuple containing the spw and field IDs of the selected measurement set.
 */
inline std::tuple<Vector<Int>, Vector<Int> > loadMS(const String& msname,
					     const String& spwStr,
					     const String& fieldStr,
					     const String& uvDistStr,
					     MeasurementSet& thems,
					     MeasurementSet& selectedMS,
					     MSSelection& msSelection,
					     std::function<void(const MeasurementSet& )> verifyMS=[](const MeasurementSet&){}
					     )
{
  //MeasurementSet thems;
  if (Table::isReadable(msname))
    thems=MeasurementSet(msname, Table::Update);
  else
    throw(AipsError(msname+" does not exist or is not readable"));
  verifyMS(thems);
  //
  //-------------------------------------------------------------------
  //
  msSelection.setSpwExpr(spwStr);
  //msSelection.setAntennaExpr(antStr);
  msSelection.setFieldExpr(fieldStr);
  msSelection.setUvDistExpr(uvDistStr);
  selectedMS = MeasurementSet(thems);
  Vector<int> spwid, fieldid;
  TableExprNode exprNode=msSelection.toTableExprNode(&thems);
  if (!exprNode.isNull())
    {
      selectedMS = MS(thems(exprNode));
      // TODO: should the following statements be moved outside this
      // block?
      spwid=msSelection.getSpwList();
      fieldid=msSelection.getFieldList();
    }
  return std::tuple<Vector<Int>, Vector<Int> >{spwid, fieldid};
}

/**
 * @brief Removes duplicate entries from a vector without sorting.
 *
 * This function removes duplicate entries from a vector without sorting.
 *
 * @tparam T The type of the vector elements.
 * @param vec The vector to remove duplicates from.
 */
template <typename ForwardIterator>
ForwardIterator remove_duplicates( ForwardIterator first,
				   ForwardIterator last )
{
    auto new_last = first;

    for ( auto current = first; current != last; ++current )
    {
        if ( std::find( first, new_last, *current ) == new_last )
        {
            if ( new_last != current ) *new_last = *current;
            ++new_last;
        }
    }

    return new_last;
}
/**
 * @brief A class for loading and manipulating measurement sets.
 *
 * This class provides functions for loading and manipulating measurement sets.
 */
class DataBase
{
public:
  DataBase():
    msSelection(),theMS(),selectedMS(),spwidList(),fieldidList(),spwRefFreqList(),sortCols()
  {};
  /**
   * @brief Constructs a Database object with the specified parameters.
   *
   * This constructor constructs a Database object with the specified parameters.
   *
   * @param MSNBuf The name of the measurement set file to load.
   * @param fieldStr The field expression to use for the selection.
   * @param spwStr The spw expression to use for the selection.
   * @param uvDistStr The uvDist expression to use for the selection.
   * @param WBAwp Whether to use AW-Projection.
   * @param nW The number of W-planes to use for AW-Projection.
   * @param doSPWDataIter Whether to use spectral window data iteration.
   * @param verifyMS A function to verify the loaded measurement set.
   */
  DataBase(const string& MSNBuf, const string& fieldStr, const string& spwStr,
	   const string& uvDistStr, const bool& WBAwp, const int& nW,
	   bool& doSPWDataIter,
	   std::function<void(const MeasurementSet& )> verifyMS=[](const MeasurementSet&){}//NoOp
	   ):
    msSelection(),theMS(),selectedMS(),spwidList(),fieldidList(),spwRefFreqList(),sortCols()
  {
    LogIO log_l(LogOrigin("DataBase","DataBase"));
    log_l << "Opening the MS (\"" << MSNBuf << "\"), applying data selection, "
	  << "setting up data iterators...all that boring stuff."
	  << LogIO::POST;
    auto lists = loadMS(MSNBuf,
			spwStr,
			fieldStr,
			uvDistStr,
			theMS,
			selectedMS,
			msSelection,verifyMS);
    spwidList=std::get<0>(lists);
    fieldidList=std::get<1>(lists);

    if (WBAwp && (spwidList.nelements() > 1) && (nW > 1)) // AW-Projection.
      doSPWDataIter=true;  // Allow user to override the decision
    log_l << "No. of rows selected: " << selectedMS.nrow() << LogIO::POST;

    //-------------------------------------------------------------------
    // Set up the data iterator
    //
    float timeSpan=10.0;
    timeSpan=iterationAxis(doSPWDataIter,sortCols);
      // {
      // 	sortCols.resize(4);
      // 	sortCols[0]=MS::ARRAY_ID;
      // 	sortCols[1]=MS::DATA_DESC_ID; // Proxy for SPW (in most MEse anyway!)
      // 	sortCols[2]=MS::FIELD_ID;
      // 	sortCols[3]=MS::TIME;
      // 	timeSpan=100000.0;
      // }
    //
    // Construct the vis iterator Vi2.  Set up the frequency selection
    // in it.  Freq. selection is a two-part process.  The SPW level
    // selection is done at the MS level (i.e., the subMS has only the
    // selected SPWs in it).  The selection of frequency channels
    // *within* each selected SPW is done in the Vi2 below.
    //
    // The weight scheme is also set up in the Vi2 via the weightor()
    // call after making the empty sky images below.
    //
    vi2_l = new vi::VisibilityIterator2(selectedMS,vi::SortColumns(sortCols),true,0,timeSpan);

    // vi2_cfsrvr = new vi::VisibilityIterator2(subMS,vi::SortColumns(sortCols),true,0,timeSpan);
    // vb_cfsrvr=vi2_cfsrvr->getVisBuffer();
    // vi2_cfsrvr->originChunks();
    // vi2_cfsrvr->origin();
    // cerr << "First SPWID from vi2_cfsrvr: " << vb_cfsrvr->spectralWindows()(0) << endl;
    {
      Matrix<Double> freqSelection= msSelection.getChanFreqList(NULL,true);
      //      vi2_l.setInterval(50.0);

      FrequencySelectionUsingFrame fsel(MFrequency::Types::LSRK);
      for(unsigned i=0;i<freqSelection.shape()(0); i++)
	fsel.add(freqSelection(i,0), freqSelection(i,1), freqSelection(i,2));
      vi2_l->setFrequencySelection (fsel);
    }
    // 
    // Rebuild the spwList using the MS iterator.  This will get the actual SPW IDs in the MS,
    // rather than rely on getting this info. from the sub-tables (since the latter aren't always
    // made consistent with the main-table (this is not a deficiency of the MS structure)).  This
    // may be a tad slower in the absolute sense, specially for large monolith MSes.  In a relative
    // sense, its cost will remain insignificant for most imaging cases.
    //
    {
      log_l << "Getting SPW ID list using VI..." << LogIO::POST;
      std::vector<int> vb_SPWIDList;
      std::unordered_set<double> pa_set;
      vb_l=vi2_l->getVisBuffer();
      vi2_l->originChunks();
      for (vi2_l->originChunks();vi2_l->moreChunks(); vi2_l->nextChunk())
	{
	  vi2_l->origin(); // So that the global vb is valid
	  vb_SPWIDList.push_back(vb_l->spectralWindows()(0));

	  pa_set.insert(getPA(*vb_l));
	  //vb_PAList.push_back(getPA(*vb_l));
	}
      std::vector<double> vb_PAList(pa_set.begin(), pa_set.end());
      //
      // Since the SPWIDList is determined by iterating over the
      // database, this list could have duplicate entries (e.g. with
      // multiple FIELDs all of which have the same frequency setup).
      // So make the determined list a list of unique entries.
      //
      // std::sort(vb_SPWIDList.begin(), vb_SPWIDList.end());
      // vb_SPWIDList.erase(std::unique(vb_SPWIDList.begin(),
      // 				     vb_SPWIDList.end()),
      // 			 vb_SPWIDList.end());

      // Remove duplicates without sorting.  This will preserve the
      // order in which SPW IDs are presented via the VI iterator.
      vb_SPWIDList.erase(remove_duplicates(vb_SPWIDList.begin(),
					   vb_SPWIDList.end()),
			 vb_SPWIDList.end());

      spwidList.resize(vb_SPWIDList.size());

      vb_PAList.erase(remove_duplicates(vb_PAList.begin(),
					vb_PAList.end()),
		      vb_PAList.end());

      log_l << "...done." << endl
	    << vb_SPWIDList.size() << " SPWs, " << vb_PAList.size() << " PA values found."
	    << LogIO::POST;

      //for(uint i=0; auto id : vb_SPWIDList) spwidList[i++]=id; // Works only in C++-20
      uint i=0; for(auto id : vb_SPWIDList) spwidList[i++]=id;
    }

    log_l << "Selected SPW ID list: " << spwidList << endl;
    // Global VB (seems to be needed for multi-threading)
    vb_l=vi2_l->getVisBuffer();
  };

  //
  //-------------------------------------------------------------------
  //
  inline float iterationAxis(const bool& spwSort, Block<int>& sortCols_l)
  {
    //-------------------------------------------------------------------
    // Set up the data iterator
    //

    float timeSpan=10.0;
    if (spwSort)
    {
      sortCols_l.resize(4);
      sortCols_l[0]=MS::ARRAY_ID;
      sortCols_l[1]=MS::DATA_DESC_ID; // Proxy for SPW (in most MEse anyway!)
      sortCols_l[2]=MS::FIELD_ID;
      sortCols_l[3]=MS::TIME;
      timeSpan=100000.0;
    }
    return timeSpan;
  }
  //
  //-------------------------------------------------------------------
  // Open the MS, apply the user-supplied verifyMS functor to it,
  // and use the internal msSelection object to setup the selected MS.
  // Any setup of the msSelection object before calling this method will
  // be applied to make the selected MS.
  //
  inline void openDB(const String& msname, const bool& spwSort,
		     std::function<void(const MeasurementSet& )> verifyMS=[](const MeasurementSet&){}, // default is to a NoOp
		     std::function<float(Block<int>& sortColumns)> setupIterAxis=[](Block<int>&){return -1.0;} // default is to call internal iterationAxis_SPW()
		     )
  {
    //MeasurementSet thems;
    if (Table::isReadable(msname))
      theMS=MeasurementSet(msname, Table::Update);
    else
      throw(AipsError(msname+" does not exist or is not readable"));
    verifyMS(theMS);

    // Use the internal msSelection object to make a selected MS.  The
    // msSelection object can be setup by the client before calling
    // the OpenDB() method.
    //
    // THIS SHOULD ALSO BE DONE VIA A FUNCTOR.  BUT FIRST TEST
    // MSSelection::operator=()
    //
    selectedMS = MeasurementSet(theMS);
    TableExprNode exprNode=msSelection.toTableExprNode(&theMS);
    if (!exprNode.isNull())
      selectedMS = MS(theMS(exprNode));

    // Use the supplied functor to setup the sort columns.  The
    // default is to call the internal
    float timeSpan=setupIterAxis(sortCols);
    if (timeSpan < 0.0) // User did not supply a functor
      timeSpan=iterationAxis(spwSort,sortCols);

    initVisIterator(timeSpan);
  }
  //
  //-------------------------------------------------------------------
  //
  inline void initVisIterator(const float& timeSpan_l)
  {
    //
    // Construct the vis iterator Vi2.  Set up the frequency selection
    // in it.  Freq. selection is a two-part process.  The SPW level
    // selection is done at the MS level (i.e., the subMS has only the
    // selected SPWs in it).  The selection of frequency channels
    // *within* each selected SPW is done in the Vi2 below.
    //
    // The weight scheme is also set up in the Vi2 via the weightor()
    // call after making the empty sky images below.
    //
    vi2_l = new vi::VisibilityIterator2(selectedMS,vi::SortColumns(sortCols),true,0,timeSpan_l);

    {
      Matrix<Double> freqSelection= msSelection.getChanFreqList(NULL,true);
      //      vi2_l.setInterval(50.0);

      FrequencySelectionUsingFrame fsel(MFrequency::Types::LSRK);
      for(unsigned i=0;i<freqSelection.shape()(0); i++)
	fsel.add(freqSelection(i,0), freqSelection(i,1), freqSelection(i,2));
      vi2_l->setFrequencySelection (fsel);
    }

    vb_l=vi2_l->getVisBuffer();
    vi2_l->originChunks();
  }
  //
  //-------------------------------------------------------------------
  //
  ~DataBase()
  {
    LogIO log_l(LogOrigin("DataBase","~DataBase"));
    log_l << "Closing the database.  Detaching the MS for cleanup." << LogIO::POST;
    //detach the ms for cleaning up
    theMS = MeasurementSet();
    selectedMS = MeasurementSet();
  };
  //private:

  // Ideally the following could be private members with the class
  // providing runtime-cheap access methods.  That's for the next
  // version of this class.

  vi::VisBuffer2 *vb_l;
  vi::VisibilityIterator2 *vi2_l;
  MSSelection msSelection;
  MS theMS, selectedMS;
  Block<Int> sortCols;

  Vector<int> spwidList, fieldidList;
  Vector<double> spwRefFreqList;
};
#endif
