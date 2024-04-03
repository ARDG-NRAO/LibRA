/* -*- mode: c++ -*- */
//# CubePartitionMixin.h: Parallel cube imaging data partitioning
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
#ifndef CUBE_PARTITION_MIXIN_H_
#define CUBE_PARTITION_MIXIN_H_

#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <synthesis/ImagerObjects/ParallelImagerParams.h>
#include <synthesis/ImagerObjects/ParamFieldIterator.h>
#include <synthesis/ImagerObjects/MPIGlue.h>
#include <algorithm>
#include <vector>
#include <string>
#include <unistd.h>

namespace casa {

/**
 * Parameter and input data partitioning for parallel cube imaging (in
 * ParallelImagerMixin).
 */
template <class T>
class CubePartitionMixin
	: public T {

public:
	void concat_images(const string &type) {
		casacore::LogIO log(casacore::LogOrigin("CubePartitionMixin", "concat_images", WHERE));
		if (worker_rank >= 0) {
			const std::string image_types[] =
				{"image", "psf", "model", "residual", "mask", "pb", "weight"};
			string cwd(getcwd(nullptr, 0));
			// wait until all ranks have completed file modifications
			MPI_Barrier(worker_comm);
			// round-robin allocation of image concatenation tasks to worker
			// ranks
			for (casacore::uInt i = (casacore::uInt)worker_rank;
			     i < image_parameters.nfields();
			     i += (casacore::uInt)num_workers) {
				std::string imagename =
					image_parameters.subRecord(i).asString("imagename");
				std::string image_prefix = cwd + "/" + imagename;
				std::vector<std::string> images;
				for (auto ext : image_types) {
					images.clear();
					std::string ext_suffix = "." + ext;
					std::string concat_imagename =
						image_prefix + ext_suffix;
					for (casacore::uInt j = 0; j < (casacore::uInt)num_workers; ++j) {
						std::string component_imagename =
							image_prefix + ".n" + std::to_string(j) + ext_suffix;
						if (access(component_imagename.c_str(), F_OK) == 0)
							images.push_back(component_imagename);
					}
					if (!images.empty()) {
						std::string cmd = "imageconcat inimages='";
						for (auto im : images) cmd += im + " ";
						cmd += "' outimage='" + concat_imagename + "' type=";
						cmd += type;
						int rc = std::system(cmd.c_str());
						if (rc == -1 || WEXITSTATUS(rc) != 0)
							log << casacore::LogIO::WARN
							    << ("concatenation of " + concat_imagename
							        + " failed")
							    << casacore::LogIO::POST;
					}
				}
			}
		}
	};

protected:

	MPI_Comm worker_comm;

	int num_workers;

	int worker_rank;

	casacore::Record image_parameters;

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
		for (int r = 0; r < num_workers; ++r) {
			all_worker_suffixes.push_back(".n" + std::to_string(r));
		}
		std::string my_worker_suffix =
			((worker_rank >= 0) ? all_worker_suffixes[worker_rank] : "");

		image_parameters = casacore::Record(initial.image);

		SynthesisUtilMethods util;
		ParallelImagerParams result;

		casacore::Record data_image_partition;
		if (worker_rank >= 0) {
			std::string worker_rank_str = std::to_string(worker_rank);
			// need a SynthesisImager instance to do cube partitioning
			std::unique_ptr<SynthesisImager> si(new SynthesisImager());
			for (casacore::uInt i = 0; i < initial.selection.nfields(); ++i) {
				SynthesisParamsSelect selection_pars;
				selection_pars.fromRecord(initial.selection.subRecord(i));
				si->selectData(selection_pars);
			}
			for (casacore::uInt i = 0; i < initial.image.nfields(); ++i) {
				casacore::String i_name = initial.image.name(i);
				if (initial.grid.isDefined(i_name)) {
					SynthesisParamsImage image_pars;
					image_pars.fromRecord(initial.image.subRecord(i_name));
					SynthesisParamsGrid grid_pars;
					grid_pars.fromRecord(initial.grid.subRecord(i_name));
					si->defineImage(image_pars, grid_pars);
					casacore::Record csys = si->getcsys();
					if (csys.nfields() > 0) {
						int nchan = ((image_pars.nchan == -1)
						             ? si->updateNchan()
						             : image_pars.nchan);;
						casacore::Vector<casacore::Int> numchans;
						casacore::Vector<casacore::CoordinateSystem> csystems;
						// save only that part of the record returned from
						// util.cubeDataImagePartition that is handled by the
						// current rank
						data_image_partition.defineRecord(
							i_name,
							util.cubeDataImagePartition(
								initial.selection,
								*casacore::CoordinateSystem::restore(csys, "coordsys"),
								num_workers, nchan, csystems, numchans).
							rwSubRecord(worker_rank_str));
					}
				}
			}
		}

		// selection_params
		if (worker_rank >= 0) {
			casacore::Record sel;
			for (casacore::uInt i = 0; i < data_image_partition.nfields(); ++i) {
				casacore::Record &di = data_image_partition.rwSubRecord(i);
				for (casacore::uInt f = 0; f < di.nfields(); ++f) {
					casacore::String name = di.name(f);
					if (name.find("ms") == 0) {
						casacore::Record &ms = di.rwSubRecord(f);
						if (ms.isDefined("spw") && ms.asString("spw") == "-1")
							ms.define("spw", "");
						sel.defineRecord(name, ms);
					}
				}
			}
			result.selection = sel;
		} else {
			result.selection = casacore::Record();
		}

		// image_params
		if (worker_rank >= 0) {
			result.image = initial.image;
			for (casacore::uInt i = 0; i < data_image_partition.nfields(); ++i) {
				const casacore::Record &di = data_image_partition.subRecord(i);
				casacore::String i_name = data_image_partition.name(i);
				casacore::Record &i_image = result.image.rwSubRecord(i_name);
				i_image.define("csys", di.asString("coordsys"));
				i_image.define("nchan", di.asString("nchan"));
				i_image.define(
					"imagename",
					i_image.asString("imagename") + casacore::String(my_worker_suffix));
			}
		} else {
			result.image = empty_fields(initial.image, "imagename");
		}

		// FIXME: are grid parameters partitioned by node?
		//
		// grid params
		if (worker_rank >= 0) {
			auto modify_cfcache = [&](const char *field_val) {
				return *field_val + my_worker_suffix;
			};
			result.grid =
				convert_fields(initial.grid, "cfcache", modify_cfcache);
		} else {
			result.grid = empty_fields(initial.grid, "cfcache");
		}

		// normalization_params
		result.normalization =
			((worker_rank >= 0) ? initial.normalization : casacore::Record());

		// deconvolution params
		result.deconvolution =
			((worker_rank >= 0) ? initial.deconvolution : casacore::Record());

		// weight params
		result.weight =
			((worker_rank >= 0) ? initial.weight : casacore::Record());

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
		for_each(ParamFieldIterator::begin(&result),
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
		for_each(ParamFieldIterator::begin(&result),
		         ParamFieldIterator::end(&result),
		         modify_field_val);
		return result;
	}
};

} // namespace casa

#endif // CUBE_PARTITION_MIXIN_H_
