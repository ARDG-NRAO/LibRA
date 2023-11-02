//# HanningSmoothTVI.h: This file contains the implementation of the HanningSmoothTVI class.
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

#include <mstransform/TVI/HanningSmoothTVI.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// HanningSmoothTVI class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
HanningSmoothTVI::HanningSmoothTVI(	ViImplementation2 * inputVii):
								ConvolutionTVI (inputVii)
{
	initialize();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void HanningSmoothTVI::initialize()
{
	convCoeff_p.resize(3,false);
	convCoeff_p(0) = 0.25;
	convCoeff_p(1) = 0.5;
	convCoeff_p(2) = 0.25;

	return;
}

//////////////////////////////////////////////////////////////////////////
// ConvolutionTVIFactory class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
HanningSmoothTVIFactory::HanningSmoothTVIFactory (ViImplementation2 *inputVii)
{
	inputVii_p = inputVii;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * HanningSmoothTVIFactory::createVi(VisibilityIterator2 *) const
{
	return new HanningSmoothTVI(inputVii_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * HanningSmoothTVIFactory::createVi() const
{
	return new HanningSmoothTVI(inputVii_p);
}

//////////////////////////////////////////////////////////////////////////
// HanningSmoothTVILayerFactory class
//////////////////////////////////////////////////////////////////////////

HanningSmoothTVILayerFactory::HanningSmoothTVILayerFactory() :
  ViiLayerFactory()
{}

ViImplementation2* 
HanningSmoothTVILayerFactory::createInstance(ViImplementation2* vii0) const 
{
  // Make the HanningSmoothTVi2, using supplied ViImplementation2, and return it
  ViImplementation2 *vii = new HanningSmoothTVI(vii0);
  return vii;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


