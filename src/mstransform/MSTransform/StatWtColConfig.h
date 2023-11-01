//# StatWt.h: Class which implements statistical reweighting
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef STATWTCOLCONFIG_H_
#define STATWTCOLCONFIG_H_

#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>

namespace casacore {
    class LogIO;
}

namespace casac {
    class variant;
}

namespace casa {

// This class is necessary to ensure partial initialization of SIGMA_SPECTRUM
// and WEIGHT_SPECTRUM in the MS is not done. It should be constructed using
// the entire MS (not an MS that has had a selection applied to it). This
// class will do the complete initialization of SIGMA_SPECTRUM and
// WEIGHT_SPECTRUM if necessary.

class StatWtColConfig {

public:

    StatWtColConfig() = delete;

    // It is the caller's responsibility to ensure that the input
    // ms represents the entire MS, and is not an MS that has had
    // a selection applied to it. The full MS is specified by the ms parameter,
    // the selected MS is specified by the selMS parameter.
    // The caller is responsible for pointer management.
	StatWtColConfig(
	    casacore::MeasurementSet* ms, casacore::MeasurementSet* selMS,
	    casacore::Bool preview, const casacore::String& dataColumn,
	    const casac::variant& chanbin
	);

    ~StatWtColConfig();

    void getColWriteFlags(
        casacore::Bool& mustWriteWt, casacore::Bool& mustWriteWtSp,
        casacore::Bool& mustWriteSig, casacore::Bool& mustWriteSigSp
    ) const;

private:
    casacore::MeasurementSet* _ms;
    casacore::MeasurementSet* _selMS;
    casacore::Bool _mustWriteWt = false;
    casacore::Bool _mustWriteWtSp = false;
    casacore::Bool _mustInitWtSp = false;
    casacore::Bool _mustWriteSig = false;
    casacore::Bool _mustWriteSigSp = false;
    casacore::Bool _mustInitSigSp = false;
    casacore::Bool _possiblyWriteSigma = false;
    casacore::Bool _preview = false;
    casacore::String _dataColumn = "";
    casacore::Bool _doChanBin = false;

    // determine if the MS has a WEIGHT/SIGMA_SPECTRUM column, if it must
    // be written, and/or if it must be initialized.
    void _dealWithSpectrumColumn(
        casacore::Bool& hasSpec, casacore::Bool& mustWriteSpec,
        casacore::Bool& mustInitSpec, casacore::Bool mustWriteNonSpec,
        const casacore::String& colName, const casacore::String& descName,
        casacore::Bool specIsInitialized, const casacore::String& mgrName
    );

    void _determineFlags();

    void _hasSpectrumIsSpectrumInitialized(
        casacore::Bool& hasSpectrum, casacore::Bool& spectrumIsInitialzied,
        casacore::MS::PredefinedColumns col
    ) const;

    void _initSpecColsIfNecessary();

    static void _setEqual(
        casacore::Cube<casacore::Float>& newsp, 
        const casacore::Matrix<casacore::Float>& col
    );
};

}

#endif 

