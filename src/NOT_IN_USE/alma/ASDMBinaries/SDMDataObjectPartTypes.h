#ifndef SDMDataObjectPartType_H
/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 */
#include <stdint.h>

namespace asdmbinaries {
  /*
   * Definitions of the types of the different possible parts in a multipart MIME message
   * containing data conform to the BDF format.
   *
   */
  typedef int64_t          ACTUALDURATIONSTYPE; /**< Type of actual durations. */
  typedef int64_t          ACTUALTIMESTYPE;     /**< Type of actual times. */
  typedef float            AUTODATATYPE;        /**< Type of auto data. */
  typedef int32_t          INTCROSSDATATYPE;    /**< Type of cross data when those are encoded as 32bits unsigned integers. */
  typedef int16_t          SHORTCROSSDATATYPE;  /**< Type of cross data when those are encoded as 16bits unsigned integers. */
  typedef float            FLOATCROSSDATATYPE;  /**< Type of cross data when those are encoded as 32bits float. */
  typedef uint32_t         FLAGSTYPE;           /**< Type of flags. */
  typedef float            ZEROLAGSTYPE;        /**< Type of zero lags. */
}

#endif // SDMDataObjectPartType_H
