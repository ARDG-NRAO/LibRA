//# AsdmStMan.cc: Storage Manager for the main table of a raw ASDM MS
//# Copyright (C) 2012
//# Associated Universities, Inc. Washington DC, USA.
//# (c) European Southern Observatory, 2012
//# Copyright by ESO (in the framework of the ALMA collaboration)
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
//# You should have receied a copy of the GNU Library General Public License
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
//# $Id: AsdmStMan.cc 21600 2012-07-16 09:59:20Z diepen $

#include <asdmstman/AsdmStMan.h>
#include <asdmstman/AsdmColumn.h>
#include <tables/Tables/Table.h>
#include <tables/DataMan/DataManError.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/BlockIO.h>
#include <casa/IO/AipsIO.h>
#include <casa/OS/CanonicalConversion.h>
#include <casa/OS/HostInfo.h>
#include <casa/OS/DOos.h>
#include <casa/Utilities/BinarySearch.h>
#include <casa/Utilities/Assert.h>
#include <casa/Logging/LogIO.h>

#include <map>

#if !defined(__clang__) && (__GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
#define CASA_ATTR_VECTORIZE __attribute__((optimize("tree-vectorize")))
#else
#define CASA_ATTR_VECTORIZE 
#endif

using namespace std;

using namespace casacore;
namespace casa {

  AsdmStMan::AsdmStMan (const String& dataManName)
  : DataManager    (),
    itsDataManName (dataManName),
    itsBDF         (0),
    itsOpenBDF     (-1),
    itsNBl(0)
  {}

  AsdmStMan::AsdmStMan (const String& dataManName,
			const Record&)
  : DataManager    (),
    itsDataManName (dataManName),
    itsBDF         (0),
    itsOpenBDF     (-1),
    itsNBl(0)
  {}

  AsdmStMan::AsdmStMan (const AsdmStMan& that)
  : DataManager    (),
    itsDataManName (that.itsDataManName),
    itsBDF         (0),
    itsOpenBDF     (-1),
    itsNBl(0)
  {}

  AsdmStMan::~AsdmStMan()
  {
    for (uInt i=0; i<ncolumn(); i++) {
      delete itsColumns[i];
    }
    closeBDF();
  }

  DataManager* AsdmStMan::clone() const
  {
    return new AsdmStMan (*this);
  }

  String AsdmStMan::dataManagerType() const
  {
    return "AsdmStMan";
  }

  String AsdmStMan::dataManagerName() const
  {
    return itsDataManName;
  }

  Record AsdmStMan::dataManagerSpec() const
  {
    return itsSpec;
  }


  DataManagerColumn* AsdmStMan::makeScalarColumn (const String& name,
						  int,
						  const String&)
  {
    throw DataManError(name + " is unknown scalar column for AsdmStMan");
  }

  DataManagerColumn* AsdmStMan::makeDirArrColumn (const String& name,
						  int dataType,
						  const String& dataTypeId)
  {
    return makeIndArrColumn (name, dataType, dataTypeId);
  }

  DataManagerColumn* AsdmStMan::makeIndArrColumn (const String& name,
						  int dtype,
						  const String&)
  {
    AsdmColumn* col;
    if (name == "DATA") {
      col = new AsdmDataColumn(this, dtype);
    } else if (name ==  "FLOAT_DATA") {
      col = new AsdmFloatDataColumn(this, dtype);
    } else if (name == "FLAG") {
      col = new AsdmFlagColumn(this, dtype);
    } else if (name == "WEIGHT") {
      col = new AsdmWeightColumn(this, dtype);
    } else if (name == "SIGMA") {
      col = new AsdmSigmaColumn(this, dtype);
    } else {
      throw DataManError (name + " is unknown array column for AsdmStMan");
    }
    itsColumns.push_back (col);
    return col;
  }

  DataManager* AsdmStMan::makeObject (const String& group, const Record& spec)
  {
    // This function is called when reading a table back.
    return new AsdmStMan (group, spec);
  }

  void AsdmStMan::registerClass()
  {
    DataManager::registerCtor ("AsdmStMan", makeObject);
  }

  Bool AsdmStMan::isRegular() const
  {
    return false;
  }
  Bool AsdmStMan::canAddRow() const
  {
    return true;
  }
  Bool AsdmStMan::canRemoveRow() const
  {
    return false;
  }
  Bool AsdmStMan::canAddColumn() const
  {
    return true;
  }
  Bool AsdmStMan::canRemoveColumn() const
  {
    return true;
  }

  void AsdmStMan::addRow (uInt)
  {}
  void AsdmStMan::removeRow (uInt)
  {
    throw DataManError ("AsdmStMan cannot remove rows");
  }
  void AsdmStMan::addColumn (DataManagerColumn*)
  {}
  void AsdmStMan::removeColumn (DataManagerColumn*)
  {}

  Bool AsdmStMan::flush (AipsIO&, Bool)
  {
    return false;
  }

  void AsdmStMan::create (uInt)
  {}

  void AsdmStMan::open (uInt, AipsIO&)
  {
    // Read the index file.
    init();
  }

  void AsdmStMan::prepare()
  {
    for (uInt i=0; i<ncolumn(); i++) {
      itsColumns[i]->prepareCol();
    }
  }

  void AsdmStMan::resync (uInt)
  {}

  void AsdmStMan::reopenRW()
  {}

  void AsdmStMan::deleteManager()
  {
    closeBDF();
    // Remove index file.
    DOos::remove (fileName()+"asdmindex", false, false);
  }

  void AsdmStMan::closeBDF()
  {
    if (itsOpenBDF >= 0) {
      delete itsBDF;
      itsBDF = 0;
      FiledesIO::close (itsFD);
      itsOpenBDF = -1;
    }
  }

  void AsdmStMan::init()
  {
    // Open index file and check version.
    AipsIO aio(fileName() + "asdmindex");
    itsVersion = aio.getstart ("AsdmStMan");
    if (itsVersion > 1) {
      throw DataManError ("AsdmStMan can only handle up to version 1");
    }
    // Read the index info.
    Bool asBigEndian;
    aio >> asBigEndian >> itsBDFNames;
    aio.get (itsIndex);
    aio.getend();
    itsDoSwap = (asBigEndian != HostInfo::bigEndian());
    // Fill the vector with rows from the index.
    itsIndexRows.resize (itsIndex.size());
    for (uInt i=0; i<itsIndex.size(); ++i) {
      itsIndexRows[i] = itsIndex[i].row;
    }
    // Fill the specification record (only used for reporting purposes).
    itsSpec.define ("version", itsVersion);
    itsSpec.define ("bigEndian", asBigEndian);
    itsSpec.define ("BDFs", Vector<String>(itsBDFNames.begin(),itsBDFNames.end()));
    // Set to nothing read yet.
    itsStartRow   = -1;
    itsEndRow     = -1;
    itsIndexEntry = 0;

    if(itsIndex.size()>0){
      // test if the referenced ASDM seems to be present
      try{
	itsFD  = FiledesIO::open (itsBDFNames[0].c_str(), false);
	itsBDF = new FiledesIO (itsFD, itsBDFNames[0]);
	itsOpenBDF = 0;
	closeBDF();
      }
      catch (AipsError x){
	LogIO os(LogOrigin("AsdmStMan", "init()"));
	os <<  LogIO::WARN 
	   << "An error occured when accessing the ASDM referenced by this table:" << endl
	   << x.what() << endl
	   << "This means you will not be able to access the columns" << endl;
	for(uInt i=0; i<itsColumns.size(); i++){
	  os << itsColumns[i]->columnName() << endl;
	}
	os << "You may have (re)moved the ASDM from its import location." << endl
	   << "If you want to access the above columns, please restore the ASDM\nor correct the reference to it." 
	   << LogIO::POST;
      }
    }

  }

  uInt AsdmStMan::searchIndex (Int64 rownr)
  {
    // Search index entry matching this rownr.
    // If not exactly found, it is previous one.
    ///  vector<uInt>::const_iterator low = std::lower_bound (itsIndexRows.begin(),
    ///                                              itsIndexRows.end(),
    ///                                                  rownr);
    ///return low - itsIndexRows.begin();
    Bool found;
    uInt v = binarySearchBrackets (found, itsIndexRows, rownr,
				   itsIndexRows.size());
    if (!found){
      if(v>0){
	v--;
      }
      else{
	throw DataManError ("AsdmStMan: index empty.");
      }
    }

    return v;
  }

  const AsdmIndex& AsdmStMan::findIndex (Int64 rownr)
  {
    // Only get if not current.
    if (rownr < itsStartRow  ||  rownr >= itsEndRow) {
      itsIndexEntry = searchIndex (rownr);
      const AsdmIndex& ix = itsIndex[itsIndexEntry];
      // Resize to indicate not read yet.
      // Note that reserved size stays the same, so no extra mallocs needed.
      // itsData.resize (0);  commented out by Michel Caillat 03 Dec 2012
      itsStartRow = ix.row;
      itsEndRow   = ix.row + ix.nrow();
    }
    return itsIndex[itsIndexEntry];
  }

  void CASA_ATTR_VECTORIZE AsdmStMan::getShort (const AsdmIndex& ix, Complex* buf, uInt bl, uInt spw)
  {
    // Get pointer to the data in the block.
    Short* data = (reinterpret_cast<Short*>(&itsData[0]));
    data = data + 2 * ix.blockOffset + 2 * bl * ix.stepBl ;  // Michel Caillat - 21  Nov 2012

    //cout << "getShort works at this adress : " << (unsigned long long int) data << endl;

    if (itsDoSwap) {
      Short real,imag;
      for (uInt j=0; j<ix.nChan; ++j) {
	for (uInt i=0; i<ix.nPol; ++i) {
	  CanonicalConversion::reverse2 (&real, data);
	  CanonicalConversion::reverse2 (&imag, data+1);
	  *buf++ = Complex(real/ix.scaleFactors[spw],
			   imag/ix.scaleFactors[spw]);
	  data += 2;
	}
      }
    } else {
      // GCC < 5 fails vectorizing Complex so use floats (same layout c++11 26.4)
      Float * fbuf = reinterpret_cast<Float*>(buf);
      for (uInt j=0; j<ix.nChan * ix.nPol; ++j) {
	  *fbuf++ = data[0]/ix.scaleFactors[spw];
	  *fbuf++ = data[1]/ix.scaleFactors[spw];
	  data += 2;
      }
    }
  }

  void AsdmStMan::getInt (const AsdmIndex& ix, Complex* buf, uInt bl, uInt spw)
  {
    // Get pointer to the data in the block.
    Int* data = (reinterpret_cast<Int*>(&(itsData[0])));
    data = data + 2 * ix.blockOffset + 2 * bl * ix.stepBl;   // 21 Nov 2012 - Michel Caillat
    if (itsDoSwap) {
      Int real,imag;
      for (uInt j=0; j<ix.nChan; ++j) {
	for (uInt i=0; i<ix.nPol; ++i) {
	  CanonicalConversion::reverse4 (&real, data);
	  CanonicalConversion::reverse4 (&imag, data+1);
	  *buf++ = Complex(real/ix.scaleFactors[spw],
			   imag/ix.scaleFactors[spw]);
	  data += 2;
	}
      }
    } else {
      for (uInt j=0; j<ix.nChan; ++j) {
	for (uInt i=0; i<ix.nPol; ++i) {
	  *buf++ = Complex(data[0]/ix.scaleFactors[spw],
			   data[1]/ix.scaleFactors[spw]);
	  data += 2;
	}
      }
    }
  }

  void AsdmStMan::getFloat (const AsdmIndex& ix, Complex* buf, uInt bl, uInt spw)
  {
    // Get pointer to the data in the block.
    Float* data = (reinterpret_cast<Float*>(&(itsData[0])));
    data = data + 2 * ix.blockOffset + 2 * bl * ix.stepBl;   // 21 Nov 2012 Michel Caillat
    if (itsDoSwap) {
      Float real,imag;
      for (uInt j=0; j<ix.nChan; ++j) {
	for (uInt i=0; i<ix.nPol; ++i) {
	  CanonicalConversion::reverse4 (&real, data);
	  CanonicalConversion::reverse4 (&imag, data+1);
	  *buf++ = Complex(real/ix.scaleFactors[spw],
			   imag/ix.scaleFactors[spw]);
	  data += 2;
	}
      }
    } else {
      for (uInt j=0; j<ix.nChan; ++j) {
	for (uInt i=0; i<ix.nPol; ++i) {
	  *buf++ = Complex(data[0]/ix.scaleFactors[spw],
			   data[1]/ix.scaleFactors[spw]);
	  data += 2;
	}
      }
    }
  }

  void AsdmStMan::getAuto (const AsdmIndex& ix, Complex* buf, uInt bl)
  {
    // Get pointer to the data in the block.
    Float* data = (reinterpret_cast<Float*>(&(itsData[0])));
    data = data + ix.blockOffset + bl * ix.stepBl;   // 21 Nov 2012 . Michel Caillat

    // The autocorr can have 1, 2, 3 or 4 npol.
    // 1 and 2 are XX and/or YY which are real numbers.
    // 3 are all 4 pols with XY a complex number and YX=conj(XY).
    // 4 are all 4 pols with XX,YY real and XY,YX complex.
    if (itsDoSwap) {
      Float valr, vali;
      if (ix.nPol == 3) {
	for (uInt i=0; i<ix.nChan; ++i) {
	  CanonicalConversion::reverse4 (&valr, data++);
	  *buf++ = Complex(valr);          // XX
	  CanonicalConversion::reverse4 (&valr, data++);
	  CanonicalConversion::reverse4 (&vali, data++);
	  *buf++ = Complex(valr, vali);    // XY
	  *buf++ = Complex(valr, -vali);   // YX
	  CanonicalConversion::reverse4 (&valr, data++);
	  *buf++ = Complex(valr);          // YY
	}
      } else if (ix.nPol == 4) {
	for (uInt i=0; i<ix.nChan; ++i) {
	  CanonicalConversion::reverse4 (&valr, data++);
	  *buf++ = Complex(valr);          // XX
	  CanonicalConversion::reverse4 (&valr, data++);
	  CanonicalConversion::reverse4 (&vali, data++);
	  *buf++ = Complex(valr, vali);    // XY
	  CanonicalConversion::reverse4 (&valr, data++);
	  CanonicalConversion::reverse4 (&vali, data++);
	  *buf++ = Complex(valr, vali);   // YX
	  CanonicalConversion::reverse4 (&valr, data++);
	  *buf++ = Complex(valr);          // YY
	}
      } else {
	for (uInt i=0; i<ix.nChan * ix.nPol; ++i) {
	  CanonicalConversion::reverse4 (&valr, data++);
	  *buf++ = Complex(valr);
	}
      }
    } else {
      // No byte swap needed.
      if (ix.nPol == 3) {
	for (uInt i=0; i<ix.nChan; ++i) {
	  *buf++ = Complex(data[0]);
	  *buf++ = Complex(data[1], data[2]);
	  *buf++ = Complex(data[1], -data[2]);
	  *buf++ = Complex(data[3]);
	  data += 4;
	}
      } else if (ix.nPol == 4) {
	for (uInt i=0; i<ix.nChan; ++i) {
	  *buf++ = Complex(data[0]);
	  *buf++ = Complex(data[1], data[2]);
	  *buf++ = Complex(data[3], data[4]);
	  *buf++ = Complex(data[5]);
	  data += 6;
	}
      } else {
	for (uInt i=0; i<ix.nChan * ix.nPol; ++i) {
	  *buf++ = Complex(data[i]);
	}
      }
    }
  }

  void AsdmStMan::getAuto (const AsdmIndex& ix, Float* buf, uInt bl)
  {
    // Get pointer to the data in the block.
    Float* data = (reinterpret_cast<Float*>(&(itsData[0])));
    data = data + ix.blockOffset + bl * ix.stepBl; 

    // This can only apply to the FLOAT_DATA column, and in that
    // case nPol can only be 1 or 2, anything else is an error.
    // No complex values should be possible here.

    if (ix.nPol > 2) {
      throw DataManError ("AsdmStMan: more than 2 polarizations found for FLOAT_DATA, can not accomodate.");
    }

    if (itsDoSwap) {
      Float valr;
      for (uInt i=0; i<ix.nChan * ix.nPol; ++i) {
	CanonicalConversion::reverse4 (&valr, data++);
	*buf++ = valr;
      }
    } else {
      // No byte swap needed.
      for (uInt i=0; i<ix.nChan * ix.nPol; ++i) {
	*buf++ = data[i];
      }
    }
  }

  IPosition AsdmStMan::getShape (uInt rownr)
  {
    // Here determine the shape from the rownr.
    /// For now fill in some shape.
    uInt inx = searchIndex (rownr);
    const AsdmIndex& ix = itsIndex[inx];
    if (ix.dataType == 10  &&  ix.nPol == 3) {
      // 3 autocorrs means 4 (YX = conj(XY));
      return IPosition(2, 4, ix.nChan);
    }
    return IPosition(2, ix.nPol, ix.nChan);
  }

  void AsdmStMan::getData (uInt rownr, Complex* buf)
  {
    const AsdmIndex& ix = findIndex (rownr);
  
    // If we are in front of Correlator data, do we have to update the baseline numbers tranposition ?
    if (ix.dataType != 10)
      if (ix.nBl != itsNBl)
	setTransposeBLNum(ix.nBl);
  
    // Open the BDF if needed.
    if (Int(ix.fileNr) != itsOpenBDF) {
      closeBDF();
      itsFD  = FiledesIO::open (itsBDFNames[ix.fileNr].c_str(), false);
      itsBDF = new FiledesIO (itsFD, itsBDFNames[ix.fileNr]);
      itsOpenBDF = ix.fileNr;
      itsFileOffset = ix.fileOffset;
      itsData.resize(0);
    }
  
    // Or we did not have open a new BDF but are aiming at a new position in the same BDF
    else if ( itsFileOffset != ix.fileOffset ) {
      itsFileOffset = ix.fileOffset;
      itsData.resize(0);
    }

    // Read data block if not done yet, i.e. if and only if we are in a new BDF or in the same
    // one but at a new position (fileOffset).
    //
    if (itsData.empty()) {
      itsData.resize (ix.dataSize());
      itsBDF->seek (ix.fileOffset);
      itsBDF->read (itsData.size(), &(itsData[0]));
    }
    // Determine the spw and baseline from the row.
    // The rows are stored in order of spw,baseline.
    uInt spw = ix.iSpw ; // 19 Feb 2014 : Michel Caillat changed this assignement;

    //
    // Attention !!! If we are in front of Correlator data we must apply the transposition function
    uInt bl;
    //cerr << "shape0 "<< itsTransposeBLNum_v.size() << endl; 
    if (ix.dataType != 10 ) 
      bl = itsTransposeBLNum_v[rownr - ix.row];
    else
      bl = (rownr - ix.row);

    switch (ix.dataType) {
    case 0:
      getShort (ix, buf, bl, spw);
      break;
    case 1:
      getInt (ix, buf, bl, spw);
      break;
    case 3:
      getFloat (ix, buf, bl, spw);
      break;
    case 10:
      getAuto (ix, buf, bl);
      break;
    default:
      throw DataManError ("AsdmStMan: Unknown data type");
    }
  }

  void AsdmStMan::getData (uInt rownr, Float* buf)
  {
    const AsdmIndex& ix = findIndex (rownr);

    // float data can only be fetched for type 10
    if (ix.dataType != 10) {
      throw DataManError ("AsdmStMan: illegal data type for FLOAT_DATA column");
    }
  
    // Open the BDF if needed.
    if (Int(ix.fileNr) != itsOpenBDF) {
      closeBDF();
      itsFD  = FiledesIO::open (itsBDFNames[ix.fileNr].c_str(), false);
      itsBDF = new FiledesIO (itsFD, itsBDFNames[ix.fileNr]);
      itsOpenBDF = ix.fileNr;
      itsFileOffset = ix.fileOffset;
      itsData.resize(0);
    }
  
    // Or we did not have open a new BDF but are aiming at a new position in the same BDF
    else if ( itsFileOffset != ix.fileOffset ) {
      itsFileOffset = ix.fileOffset;
      itsData.resize(0);
    }

    // Read data block if not done yet, i.e. if and only if we are in a new BDF or in the same
    // one but at a new position (fileOffset).
    //
    if (itsData.empty()) {
      itsData.resize (ix.dataSize());
      itsBDF->seek (ix.fileOffset);
      itsBDF->read (itsData.size(), &(itsData[0]));
    }
    // Determine the spw and baseline from the row.
    // The rows are stored in order of spw,baseline.
    // getAuto appears to not depend on spw.  R.Garwood.

    uInt bl = (rownr - ix.row);
    getAuto (ix, buf, bl);
  }

  void AsdmStMan::getBDFNames(Block<String>& bDFNames)
  {
    bDFNames = itsBDFNames;
    return;
  }

  Bool AsdmStMan::setBDFNames(Block<String>& bDFNames)
  {
    if(bDFNames.size() == itsBDFNames.size()){
      itsBDFNames = bDFNames;
      return true;
    }
    else{
      return false;
    }
  }
  
  void AsdmStMan::writeIndex(){
  
    AipsIO aio(fileName() + "asdmindex", ByteIO::New);
    aio.putstart("AsdmStMan", itsVersion);
    aio << HostInfo::bigEndian() << itsBDFNames;
    aio.put(itsIndex);
    aio.putend();
  
  }

  void AsdmStMan::setTransposeBLNum(uInt nBl) {
    
    itsTransposeBLNum_v.clear();

    // Deduce the number of antennas from the number of baselines
    uInt numAnt = (uInt) (floor((1 + sqrt(1 + 8*nBl)) / 2 + 0.5));

    //cerr << "AsdmStMan::setTransposeBLNum, numAnt=" << numAnt << endl;
    
    uInt blNum = 0;
    map<uInt, map<uInt, uInt> > _mm;
    for (uInt i2 = 1; i2 < numAnt; i2++) {
      map<uInt, uInt> _m;
      for (uInt i1 = 0; i1 < i2; i1++) {
	_m[i1] = blNum;
	blNum++;
      }
      _mm[i2] = _m;
    }

    for (uInt i1 = 0; i1 < numAnt; i1++)
      for (uInt i2 = i1+1; i2 < numAnt; i2++)
	itsTransposeBLNum_v.push_back(_mm[i2][i1]);

    itsNBl = nBl;
  }
  
} //# end namespace
