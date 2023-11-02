/* -*- mode: c++ -*- */
//# SerialPartitionMixin.h: Parallel imaging framework data partitioning for
//#                         serial imaging
//# Copyright (C) 2016
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
#ifndef SERIAL_PARTITION_MIXIN_H_
#define SERIAL_PARTITION_MIXIN_H_

#include <synthesis/ImagerObjects/ParallelImagerParams.h>
#include <synthesis/ImagerObjects/MPIGlue.h>
#include <string>

namespace casa {

/**
 * Parameter and input data partitioning for serial imaging (in
 * ParallelImagerMixin).
 */
template <class T>
class SerialPartitionMixin
	: public T {

public:
	void concat_images(const std::string &type __attribute__((unused))) {};

protected:
	ParallelImagerParams
	get_params(MPI_Comm worker_comm __attribute__((unused)),
	           ParallelImagerParams &initial) {
		return initial;
	}
};

} // namespace casa

#endif // SERIAL_PARTITION_MIXIN_H_
