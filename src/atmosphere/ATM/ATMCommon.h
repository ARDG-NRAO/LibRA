#ifndef _ATM_COMMON_H_
#define _ATM_COMMON_H_
/*******************************************************************************
 * ALMA - Atacama Large Millimiter Array
 * (c) Instituto de Estructura de la Materia, 2010
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * "@(#) $Id: ATMCommon.h Exp $"
 *
 * who       when      what
 * --------  --------  ----------------------------------------------
 * jroche    01/02/10  created
 */

#ifndef __cplusplus
#error "This is a C++ include file and cannot be used from plain C"
#endif

#ifdef ALMA_TELCAL
#define ATM_NAMESPACE_BEGIN namespace telcal { namespace atm {
#define ATM_NAMESPACE_END } }
#define ATM_NAMESPACE telcal::atm
#else
#define ATM_NAMESPACE_BEGIN namespace atm {
#define ATM_NAMESPACE_END }
#define ATM_NAMESPACE atm
#endif

#endif /*!_ATM_COMMON_H_*/
