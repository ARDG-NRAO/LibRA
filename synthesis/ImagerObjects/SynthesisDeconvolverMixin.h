/* -*- mode: c++ -*- */
//# SynthesisDeconvolverMixin.h: Mixin for using SynthesisDeconvolver class in
//#                              parallel imaging framework
//#                              (ParallelImagerMixin)
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
#ifndef SYNTHESIS_DECONVOLVER_MIXIN_H_
#define SYNTHESIS_DECONVOLVER_MIXIN_H_

#include <casa/Containers/Record.h>
#include <synthesis/ImagerObjects/MPIGlue.h>
#include <synthesis/ImagerObjects/SynthesisDeconvolver.h>
#include <memory>
#include <algorithm>
#include <vector>

namespace casa {

/**
 * Simple mixin class to put SynthesisDeconvolver into ParallelImagerMixin
 * framework.
 */
template<class T>
class SynthesisDeconvolverMixin
	: public T {

private:
	std::vector< std::shared_ptr<SynthesisDeconvolver> > deconvolvers;

	casacore::Record controls;

protected:
	void
	setup_deconvolver(MPI_Comm comm,
	                  std::vector<SynthesisParamsDeconv> &deconv_pars) {
		// Create all deconvolver components on rank 0 of comm. TODO: Could we
		// distribute deconvolvers in a round-robin fashion across processes in
		// comm?

		teardown_deconvolver();
		if (T::effective_rank(comm) == 0) {
			size_t num_fields = deconv_pars.size();
			for (size_t i = 0; i < num_fields; ++i) {
				SynthesisDeconvolver *sd = new SynthesisDeconvolver();
				sd->setupDeconvolution(deconv_pars[i]);
				deconvolvers.push_back(
					std::shared_ptr<SynthesisDeconvolver>(sd));
			}
		}
	};

	void
	teardown_deconvolver() {
		deconvolvers.clear();
	};

public:
	void
	initialize_minor_cycle() {
		std::vector<casacore::Record> init_records;
		for (auto sd : deconvolvers)
			init_records.push_back(sd->initMinorCycle());
		T::merge_initialization_records(init_records);
		controls = T::get_minor_cycle_controls();
	};

	void
	execute_minor_cycle() {
		std::vector<casacore::Record> exec_records;
		for (auto sd : deconvolvers)
			exec_records.push_back(sd->executeMinorCycle(controls));
		T::merge_execution_records(exec_records);
	};

	void
	restore_images() {
		for (auto sd : deconvolvers)
			sd->restore();
	};
};

} // namespace casa

#endif // SYNTHESIS_DECONVOLVER_MIXIN_H_
