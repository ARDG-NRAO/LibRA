/* -*- mode: c++ -*- */
//# ParallelImagerMixin.h: Main class for parallel imaging
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
#ifndef PARALLEL_IMAGER_MIXIN_H_
#define PARALLEL_IMAGER_MIXIN_H_

#include <string>
#include <vector>

#include <synthesis/ImagerObjects/MPIGlue.h>
#include <casacore/casa/Containers/Record.h>
#include <synthesis/ImagerObjects/ParallelImagerParams.h>
#include <synthesis/ImagerObjects/ParamFieldIterator.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <synthesis/ImagerObjects/SynthesisDeconvolverMixin.h>
#include <synthesis/ImagerObjects/SynthesisNormalizerMixin.h>
#include <synthesis/ImagerObjects/SynthesisImagerMixin.h>
#include <synthesis/ImagerObjects/IterationControl.h>
#include <synthesis/ImagerObjects/ContinuumPartitionMixin.h>
#include <synthesis/ImagerObjects/CubePartitionMixin.h>
#include <synthesis/ImagerObjects/SerialPartitionMixin.h>

namespace casa {

/**
 * ParallelImagerMixin
 *
 * This class ties together mixin classes for imager, normalizer, deconvolver,
 * iteration control and data partitioning to provide the high level
 * functionality of synthesis imaging. The functionality embodied by this class,
 * together with its parent mixin classes, replaces all of the functionality in
 * task_tclean.py, refimagerhelper.py, synthesisimager_cmpt.cc,
 * synthesisnormalizer_cmpt.cc, synthesisdeconvolver_cmpt.cc (and maybe
 * others). Some rearrangement of the method calls spread across task_tclean.py
 * and refimagerhelper.py was necessary to create a single class embodying only
 * the highest level algorithmic structure common to the three cases of serial,
 * continuum parallel and cube parallel imaging.
 *
 * Various implementation classes are defined via typedefs at the end of this
 * file. The ParallelImagerMixin implementation classes are all defined using
 * other mixin classes that simply wrap pre-existing, non-mixin imager,
 * normalizer, and deconvolver component instances for use in this
 * framework. [We refer to classes such as SynthesisNormalizer as "component"
 * classes, and the mixin classes that wrap such components as "component mixin"
 * classes]. These component mixin classes are intended to provide a sufficient
 * design that will support changes in the component classes transparently; if
 * this proves insufficient, new implementations of the component mixin classes
 * could be supported with minimal refactoring.
 */
template <class T>
class ParallelImagerMixin
	: public T {

public:

	// ParallelImagerMixin constructor
	//
	// The various MPI communicators passed to the constructor are intended to
	// represent the following process groups:
	//
    // * worker_comm: all processes used for significant computation (likely
    //   just excluding a "front-end" or user-facing process).
	//
	// * imaging/normalization/deconvolution_comm: the groups of processes for
	//   each of these components. Note that the scope of each of these
	//   communicators defines the set of communicating processes that comprise
	//   the component; for example, in cube imaging each imaging_comm comprises
	//   a single process, whereas in continuum imaging a single imaging_comm
	//   comprises all (worker) processes. Such differences are determined by
	//   design decisions, and do not reflect inherent features of the
	//   framework.
	//
	// * iteration_comm: processes taking part in iteration control, should be
	//   those processes in worker_comm with the possible addition of a process
	//   for the tclean "front-end".
	//
	// Note that there may be overlap in the process groups for each of these
	// communicators. The mixin classes in this framework support such usage,
	// although the viability of that support depends on the usage of these
	// communicators by the wrapped components in addition to the mixin
	// classes. The current component classes make no use of these
	// communicators, so overlapping process groups are supported. When
	// component classes are using their respective communicators, it is
	// sufficient for safely calling MPI routines from component code to ensure
	// that concurrent access to a communicator by this framework and any
	// component threads is avoided. [One way this can be achieved is by
	// limiting calls to MPI routines on the provided communicator only to
	// methods directly called by this framework.]
	//
	ParallelImagerMixin(MPI_Comm worker_comm,
	                    MPI_Comm imaging_comm,
	                    MPI_Comm normalization_comm,
	                    MPI_Comm deconvolution_comm,
	                    MPI_Comm iteration_comm,
	                    int niter,
	                    bool calculate_psf,
	                    bool calculate_residual,
	                    string save_model,
	                    ParallelImagerParams &params)
	: niter(niter)
	, calculate_psf(calculate_psf)
	, calculate_residual(calculate_residual)
	, save_model(save_model)
	, worker_comm(worker_comm)
	, imaging_comm(imaging_comm)
	, deconvolution_comm(deconvolution_comm)
	, normalization_comm(normalization_comm)
	, iteration_comm(iteration_comm) {
		// Get parameters for this process
		ParallelImagerParams my_params = T::get_params(worker_comm, params);

		// Convert parameters to other formats used by synthesis imaging
		// components, putting them into vectors by field index (not field key
		// as used by the Records).
		auto to_synthesis_params_select = [] (const casacore::Record &r) {
			SynthesisParamsSelect pars;
			pars.fromRecord(r);
			return pars;
		};
		std::vector<SynthesisParamsSelect> selection_params =
			transformed_by_field<SynthesisParamsSelect>(
				my_params.selection, to_synthesis_params_select, "ms");

		auto to_synthesis_params_image = [] (const casacore::Record &r) {
			SynthesisParamsImage pars;
			pars.fromRecord(r);
			return pars;
		};
		std::vector<SynthesisParamsImage> image_params =
			transformed_by_field<SynthesisParamsImage>(
				my_params.image, to_synthesis_params_image);

		auto to_synthesis_params_grid = [] (const casacore::Record &r) {
			SynthesisParamsGrid pars;
			pars.fromRecord(r);
			return pars;
		};
		std::vector<SynthesisParamsGrid> grid_params =
			transformed_by_field<SynthesisParamsGrid>(
				my_params.grid, to_synthesis_params_grid);

		auto to_synthesis_params_deconv = [] (const casacore::Record &r) {
			SynthesisParamsDeconv pars;
			pars.fromRecord(r);
			return pars;
		};
		std::vector<SynthesisParamsDeconv> deconvolution_params =
			transformed_by_field<SynthesisParamsDeconv>(
				my_params.deconvolution, to_synthesis_params_deconv);

		auto to_vector_params = [] (const casacore::Record &r) {
			casacore::Record result = r;
			return result;
		};
		std::vector<casacore::Record> normalization_params =
			transformed_by_field<casacore::Record>(my_params.normalization, to_vector_params);

		// Configure components
		T::setup_imager(imaging_comm, selection_params, image_params,
		                grid_params, my_params.weight);
		T::setup_normalizer(normalization_comm, normalization_params);
		T::setup_deconvolver(deconvolution_comm, deconvolution_params);
		// don't initialize iteration control on any rank until all workers have
		// completed initialization (need second barrier for case in which there
		// are processes in iteration_comm that are not in worker_comm)
		MPI_Barrier(worker_comm);
		MPI_Barrier(iteration_comm);
		T::setup_iteration_controller(iteration_comm, my_params.iteration);
	}

	~ParallelImagerMixin() {
		T::teardown_imager();
		T::teardown_normalizer();
		T::teardown_deconvolver();
		T::teardown_iteration_controller();
		auto free_comm = [](MPI_Comm *comm) {
			if (*comm != MPI_COMM_NULL
			    && *comm != MPI_COMM_SELF
			    && *comm != MPI_COMM_WORLD)
				MPI_Comm_free(comm);
		};
		free_comm(&worker_comm);
		free_comm(&imaging_comm);
		free_comm(&normalization_comm);
		free_comm(&deconvolution_comm);
		free_comm(&iteration_comm);
	}

	// Top level imaging method. Note that differences in parallel continuum,
	// parallel cube, and serial imaging are not apparent at this level.
	casacore::Record clean() {
		if (calculate_psf) {
			T::make_psf();
			T::normalize_psf();
		}
		if (niter >= 0) {
			if (calculate_residual) {
				run_major_cycle();
			}
			else if (niter == 0 && save_model != "none") {
				T::normalize_model();
				T::predict_model();
			}
			if (niter > 0)
				while (run_minor_cycle())
					run_major_cycle();
		}
		T::restore_images();
		T::concat_images("virtualnomove");
		casacore::Record result = T::get_summary(); // includes plot_report
		MPI_Barrier(worker_comm);
		return result;
	}

protected:

	void run_major_cycle() {
		T::normalize_model();
		T::execute_major_cycle();
		T::normalize_residual();
		T::denormalize_model();
	}

	bool run_minor_cycle() {
		T::initialize_minor_cycle();
		bool result = !T::is_clean_complete();
		if (result) T::execute_minor_cycle();
		return result;
	}

	int niter;

	bool calculate_psf;

	bool calculate_residual;

	string save_model;

	MPI_Comm worker_comm;

	MPI_Comm imaging_comm;

	MPI_Comm deconvolution_comm;

	MPI_Comm normalization_comm;

	MPI_Comm iteration_comm;

	// Convenience function for transforming input parameter casacore::Record fields.
	template<class T1>
	static std::vector<T1> transformed_by_field(casacore::Record &rec,
	                                       T1 (*fn)(const casacore::Record &),
	                                       const string &prefix = "") {
		std::vector<T1> result;
		auto add_to_result = [&](const casacore::Record &rec) {
			result.push_back(fn(rec));
		};
		std::for_each(ParamFieldIterator::begin(&rec, prefix),
		              ParamFieldIterator::end(&rec, prefix),
		              add_to_result);
		return result;
	};
};

// Parallel continuum imager class
typedef ParallelImagerMixin<
	ContinuumPartitionMixin<
		SynthesisImagerMixin<
			SynthesisNormalizerMixin<
				SynthesisDeconvolverMixin<
					IterationControl> > > > >
ContinuumParallelImagerImpl;

// Parallel cube imager class
typedef ParallelImagerMixin<
	CubePartitionMixin<
		SynthesisImagerMixin<
			SynthesisNormalizerMixin<
				SynthesisDeconvolverMixin<
					IterationControl> > > > >
CubeParallelImagerImpl;

// Serial (non-MPI) imager class -- allows ParallelImagerMixin type to be used
// regardless of serial vs parallel CASA. However, the naming is
// unfortunate...suggestions are welcome!
typedef ParallelImagerMixin<
	SerialPartitionMixin<
		SynthesisImagerMixin<
			SynthesisNormalizerMixin<
				SynthesisDeconvolverMixin<
					IterationControl> > > > >
SerialParallelImagerImpl;

} // namespace casa

#endif // PARALLEL_IMAGER_H_
