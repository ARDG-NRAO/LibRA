//# ParallelImagerFactory.cc: Factory for ParallelImager instances
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
#include <synthesis/ImagerObjects/ParallelImagerFactory.h>
#include <synthesis/ImagerObjects/MPIGlue.h>
#include <synthesis/ImagerObjects/ParallelImagerParams.h>
#include <synthesis/ImagerObjects/ParallelImagerAdaptor.h>
#include <string>

using namespace casacore;
namespace casa {

ParallelImager *
ParallelImagerFactory::make(MPI_Comm task_comm,
                            Record clean_params,
                            Record selection_params,
                            Record image_params,
                            Record grid_params,
                            Record weight_params,
                            Record normalization_params,
                            Record deconvolution_params,
                            Record iteration_params)
{
	ParallelImager *result;
	ParallelImagerParams params(selection_params, image_params, grid_params,
	                            weight_params, normalization_params,
	                            deconvolution_params, iteration_params);
	int niter = iteration_params.asInt("niter");
	bool calculate_psf = clean_params.asBool("calcpsf");
	bool calculate_residual = clean_params.asBool("calcres");
	const std::string &save_model = clean_params.asString("savemodel");
	bool interactive = iteration_params.asBool("interactive");
	int rank;
	MPI_Comm_rank(task_comm, &rank);
	int size;
	MPI_Comm_size(task_comm, &size);
	bool is_worker = !interactive || rank > 0 || size == 1;
	MPI_Comm worker_comm;
	MPI_Comm_split(task_comm, is_worker ? 1 : MPI_UNDEFINED, 0, &worker_comm);
	bool has_non_worker = interactive && size > 1;
	int num_workers = size - (has_non_worker ? 1 : 0);
	if (num_workers == 1) {
		// Serial imaging, whether or not MPI is available. The use of
		// communicators is the same as that for parallel continuum imaging
		// (below).
		MPI_Comm imcomm = dup_valid_comm(worker_comm);
		MPI_Comm ncomm = normalization_comm(worker_comm, niter);
		MPI_Comm dcomm = dup_valid_comm(worker_comm);
		MPI_Comm itcomm = dup_valid_comm(task_comm);
		result = new SerialParallelImager(
			worker_comm, imcomm, ncomm, dcomm, itcomm,
			niter, calculate_psf, calculate_residual, save_model, params);
	} else {
		const Record &image_field = image_params.subRecord(0);
		std::string mode(image_field.isDefined("mode")
		            ? image_field.asString("mode")
		            : "mfs");
		if (mode == "mfs") {
			// Parallel continuum imaging. Imaging, normalization and
			// deconvolution components each use a duplicate of 'worker_comm';
			// the imaging cycles are coordinated across all worker
			// processes. Iteration component uses a duplicate of 'task_comm',
			// which additionally coordinates interaction with a possible
			// user-facing process.
			MPI_Comm imcomm = dup_valid_comm(worker_comm);
			MPI_Comm ncomm = normalization_comm(worker_comm, niter);
			MPI_Comm dcomm = dup_valid_comm(worker_comm);
			MPI_Comm itcomm = dup_valid_comm(task_comm);
			result = new ContinuumParallelImager(
				worker_comm, imcomm, ncomm, dcomm, itcomm,
				niter, calculate_psf, calculate_residual, save_model, params);
		} else {
			// Parallel cube imaging. Imaging, normalization and deconvolution
			// components each use a duplicate of MPI_COMM_SELF; each process
			// manages the interaction of its components independently of the
			// other processes. Iteration component uses a duplicate of
			// 'task_comm', as that coordinates across all worker and
			// user-facing processes.
			MPI_Comm worker_self = (is_worker ? MPI_COMM_SELF : MPI_COMM_NULL);
			MPI_Comm itcomm = dup_valid_comm(task_comm);
			result = new CubeParallelImager(
				worker_comm,
				worker_self,
				normalization_comm(worker_self, niter),
				worker_self,
				itcomm,
				niter, calculate_psf, calculate_residual, save_model, params);
		}
	}
	return result;
}

using namespace casacore;
} // namespace casa
