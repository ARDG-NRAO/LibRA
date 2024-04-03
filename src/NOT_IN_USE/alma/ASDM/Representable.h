
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
 * Warning!
 *  -------------------------------------------------------------------- 
 * | This is generated code!  Do not modify this file.                  |
 * | If you do, all changes will be lost when the file is re-generated. |
 *  --------------------------------------------------------------------
 *
 * File Representable.h
 */
 
#ifndef _Representable_H
#define _Representable_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif
 
#include <alma/ASDM/Entity.h>
#include <alma/ASDM/ConversionException.h>

namespace asdm {


/**
 * The Representable interface is implemented by all tables
 * and by the container.  It represents methods that are 
 * required to convert between the internal and the various 
 * external representations of tables and containers.
 */
class Representable {
	friend class ASDM;

public:

	virtual ~Representable() {}
	virtual std::string toXML()   = 0;
	virtual void fromXML(std::string& xml) = 0;

	virtual std::string getName() const = 0;
	virtual unsigned int size() const = 0;
		
protected:

	virtual Entity getEntity() const = 0;
	virtual void setEntity(Entity e) = 0;

	bool presentInMemory;
	bool loadInProgress;
	uint32_t declaredSize;
};
	
} // End namespace asdm

#endif /* _Representable_CLASS */
