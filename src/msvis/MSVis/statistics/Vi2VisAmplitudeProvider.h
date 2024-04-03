// -*- mode: c++ -*-
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003,2015
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
//
// casacore::Data providers for visibility amplitudes.
//
#ifndef MSVIS_STATISTICS_VI2_VIS_AMPLITUDE_PROVIDER_H_
#define MSVIS_STATISTICS_VI2_VIS_AMPLITUDE_PROVIDER_H_

#include <msvis/MSVis/statistics/Vi2StatsAmplitudeIterator.h>
#include <msvis/MSVis/statistics/Vi2ObservedVisDataProvider.h>
#include <msvis/MSVis/statistics/Vi2ModelVisDataProvider.h>
#include <msvis/MSVis/statistics/Vi2CorrectedVisDataProvider.h>

namespace casa {

typedef Vi2ObservedVisDataProvider<Vi2StatsAmplitudeIterator>
Vi2ObservedVisAmplitudeProvider;

typedef Vi2ModelVisDataProvider<Vi2StatsAmplitudeIterator>
Vi2ModelVisAmplitudeProvider;

typedef Vi2CorrectedVisDataProvider<Vi2StatsAmplitudeIterator>
Vi2CorrectedVisAmplitudeProvider;

} // namespace casa

#endif // MSVIS_STATISTICS_VI2_VIS_AMPLITUDE_PROVIDER_H_
