/*
 * Scantable2MSFiller.cc
 *
 *  Created on: Jan 5, 2016
 *      Author: nakazato
 */

#include <singledishfiller/Filler/SingleDishMSFiller.h>

#include <singledishfiller/Filler/Scantable2MSReader.h>
#include <singledishfiller/Filler/NRO2MSReader.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

template class SingleDishMSFiller<Scantable2MSReader>;
template class SingleDishMSFiller<NRO2MSReader>;

} //# NAMESPACE CASA - END
