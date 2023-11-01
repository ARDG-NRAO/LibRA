// -*- mode: c++ -*-
//# ParallelImagerFactory.h: Factory for ParallelImager instances
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
#ifndef PARALLEL_IMAGER_FACTORY_H_
#define PARALLEL_IMAGER_FACTORY_H_

#include <synthesis/ImagerObjects/MPIGlue.h>
#include <synthesis/ImagerObjects/ParallelImager.h>

namespace casa {


/**
 * Factory class for ParallelImager instances. Instances of this class are used
 * to create the appropriate ParallelImager instance for a set of given
 * parameters.
 */
class ParallelImagerFactory {
public:

	/**
	 * Make a ParallelImager for the given set of parameters. Decide whether to
	 * instantiate a parallel continuum, parallel cube or serial imager, and
	 * create an appropriate instance of that type. This method does not spawn
	 * any processes, it just creates the imager instance for the current
	 * process given the communicator 'task_comm' and the values provided by the
	 * other parameters. The method works in a serial CASA environment through
	 * the definitions provided by MPIGlue.h. The various imaging component
	 * communicators required by ParallelImagerMixin are created by this method;
	 * they are expected to be freed by the returned instance at an appropriate
	 * time. No reference to 'task_comm' is maintained by the returned instance,
	 * allowing the caller to use 'task_comm' freely after this method returns.
	 */
	static ParallelImager *make(MPI_Comm task_comm,
	                            casacore::Record clean_params,
	                            casacore::Record selection_params,
	                            casacore::Record image_params,
	                            casacore::Record grid_params,
	                            casacore::Record weight_params,
	                            casacore::Record normalization_params,
	                            casacore::Record deconvolution_params,
	                            casacore::Record iteration_params);

private:
	// Convenience method for creating communicator for normalization
	// components. Significantly, return the null communicator when 'niter'
	// value is less than 1, which provides the indication to
	// SynthesisNormalizerMixin that the normalization component is not
	// required.
	static MPI_Comm normalization_comm(MPI_Comm comm, int niter) {
		MPI_Comm result;
		if (niter > 0 && comm != MPI_COMM_NULL) MPI_Comm_dup(comm, &result);
		else result = MPI_COMM_NULL;
		return result;
	}

	// Convenience method for duplicating communicators (incl. MPI_COMM_NULL)
	static MPI_Comm dup_valid_comm(MPI_Comm comm) {
		MPI_Comm result;
		if (comm != MPI_COMM_NULL) MPI_Comm_dup(comm, &result);
		else result = MPI_COMM_NULL;
		return result;
	}
};

} // namespace casa

#endif // PARALLEL_IMAGER_FACTORY_H_
