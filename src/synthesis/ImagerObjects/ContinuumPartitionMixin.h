/* -*- mode: c++ -*- */
//# ContinuumPartitionMixin.h: Parallel continuum imaging data partitioning
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
#ifndef CONTINUUM_PARTITION_MIXIN_H_
#define CONTINUUM_PARTITION_MIXIN_H_

#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <synthesis/ImagerObjects/ParallelImagerParams.h>
#include <synthesis/ImagerObjects/MultiParamFieldIterator.h>
#include <synthesis/ImagerObjects/MPIGlue.h>
#include <algorithm>
#include <unistd.h>
#include <vector>
#include <string>

namespace casa {

/**
 * Parameter and input data partitioning for parallel continuum imaging (in
 * ParallelImagerMixin).
 */
template <class T>
class ContinuumPartitionMixin
	: public T {

public:
	void concat_images(const std::string &type __attribute__((unused))) {};

protected:
	MPI_Comm worker_comm;

	int num_workers;

	int worker_rank;

	ParallelImagerParams
	get_params(MPI_Comm wcomm, ParallelImagerParams &initial) {

		// Determine rank among parallel imaging worker processes.
		worker_comm = wcomm;
		if (worker_comm != MPI_COMM_NULL) {
			MPI_Comm_size(worker_comm, &num_workers);
			MPI_Comm_rank(worker_comm, &worker_rank);
		} else {
			num_workers = 0;
			worker_rank = -1;
		}

		std::string cwd(getcwd(nullptr, 0));
		std::vector<std::string> all_worker_suffixes;
		for (int r = 0; r < num_workers; ++r)
			all_worker_suffixes.push_back(".n" + std::to_string(r));
		std::string my_worker_suffix =
			((num_workers > 1 && worker_rank >= 0)
			 ? all_worker_suffixes[worker_rank]
			 : "");
		SynthesisUtilMethods util;
		ParallelImagerParams result;

		// selection params
		result.selection =
			((worker_rank >= 0)
			 ? util.continuumDataPartition(initial.selection, num_workers).
			 rwSubRecord(std::to_string(worker_rank))
			 : casacore::Record());

		// image params
		if (worker_rank >= 0) {
			auto modify_imagename = [&](const char *field_val) {
				return cwd + "/" + field_val + my_worker_suffix;
			};
			result.image = convert_fields(initial.image, "imagename",
			                              modify_imagename);
		} else {
			result.image = empty_fields(initial.image, "imagename");
		}

		// grid params
		if (worker_rank >= 0) {
			auto modify_cfcache = [&](const char *field_val) {
				return field_val + my_worker_suffix;
			};
			result.grid =
				convert_fields(initial.grid, "cfcache", modify_cfcache);
		} else {
			result.grid = empty_fields(initial.grid, "cfcache");
		}

		// normalization params
		if (worker_rank == 0 && num_workers > 1) {
			auto accumulate_part_names =
				[&] (std::array<casacore::Record *,2> im_norm_par) {
				std::vector<casacore::String> part_names;
				std::string image_path =
				cwd + "/" + im_norm_par[0]->asString("imagename").c_str();
				for (auto s : all_worker_suffixes) {
					part_names.push_back(casacore::String(image_path + s));
				}
				im_norm_par[1]->define("partimagenames", casacore::Vector<casacore::String>(part_names));
			};
			std::array<casacore::Record *,2> im_norm_params =
				{ &initial.image, &initial.normalization };
			std::for_each(MultiParamFieldIterator<2>::begin(im_norm_params),
			              MultiParamFieldIterator<2>::end(im_norm_params),
			              accumulate_part_names);
			result.normalization = *(im_norm_params[1]);
		} else {
			result.normalization =
				empty_fields(initial.normalization, "partimagenames");
		}

		// deconvolution params
		result.deconvolution =
			((worker_rank == 0) ? initial.deconvolution : casacore::Record());

		// weight params
		result.weight = ((worker_rank >= 0) ? initial.weight : casacore::Record());

		// iteration params
		result.iteration = initial.iteration;

		return result;
	}

private:

	// Convenience method to transform certain record fields
	casacore::Record convert_fields(casacore::Record &rec, const char *field,
	                      std::function<std::string(const char *)> fn) {
		auto modify_field_val = [&](casacore::Record &msRec) {
			msRec.define(field, fn(msRec.asString(field).c_str()));
		};
		casacore::Record result(rec);
		std::for_each(ParamFieldIterator::begin(&result),
		              ParamFieldIterator::end(&result),
		              modify_field_val);
		return result;
	}

	// Convenience method to clear certain record fields
	casacore::Record empty_fields(casacore::Record &rec, const char *field) {
		auto modify_field_val = [&](casacore::Record &msRec) {
			msRec.defineRecord(field, casacore::Record());
		};
		casacore::Record result(rec);
		std::for_each(ParamFieldIterator::begin(&result),
		              ParamFieldIterator::end(&result),
		              modify_field_val);
		return result;
	}
};

} // namespace casa

#endif // CONTINUUM_PARTITION_MIXIN_H_
