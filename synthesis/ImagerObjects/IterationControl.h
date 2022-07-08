/* -*- mode: c++ -*- */
//# IterationControl.h: Iteration control for parallel imaging framework
//#                     (ParallelImagerMixin)
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
#ifndef ITERATION_CONTROL_H_
#define ITERATION_CONTROL_H_

#include <casacore/casa/Containers/Record.h>
#include <synthesis/ImagerObjects/MPIGlue.h>
#if ! defined(CASATOOLS)
#include <synthesis/ImagerObjects/DistributedSynthesisIterBot.h>
#endif
#include <synthesis/ImagerObjects/ParallelImager.h>

#include <memory>

namespace casa {

class IterationControl {

private:
#if ! defined(CASATOOLS)
	std::unique_ptr<DistributedSynthesisIterBot> it;
#endif
protected:
	void
	setup_iteration_controller(MPI_Comm comm, casacore::Record &iter_pars) {
		teardown_iteration_controller();
		// Instantiate an iterbot. Use DistributedSynthesisIterBot when
		// interaction with new GUI works; for now,
		// DistributedSynthesisIterBotWithOldGUI works.
#if ! defined(CASATOOLS)
		it = std::unique_ptr<DistributedSynthesisIterBot>(
			new DistributedSynthesisIterBotWithOldGUI(comm));
		it->setupIteration(iter_pars);
#endif
	};

	void
	teardown_iteration_controller() {
#if ! defined(CASATOOLS)
		it.reset();
#endif
	}

public:
	void
	end_major_cycle() {
#if ! defined(CASATOOLS)
		it->endMajorCycle();
#endif
	};

	casacore::Record
	get_minor_cycle_controls() {
#if ! defined(CASATOOLS)
		return it->getSubIterBot();
#else
        return casacore::Record( );
#endif
	};

	void
	merge_execution_records(const casacore::Vector<casacore::Record> &recs) {
#if ! defined(CASATOOLS)
		it->endMinorCycle(recs);
#endif
	};

	void
	merge_initialization_records(const casacore::Vector<casacore::Record> &recs) {
#if ! defined(CASATOOLS)
		it->startMinorCycle(recs);
#endif
	};

	bool
	is_clean_complete() {
#if ! defined(CASATOOLS)
		return it->cleanComplete() > 0;
#else
        return false;
#endif
	};

	casacore::Record
	get_summary() {
#if ! defined(CASATOOLS)
		return it->getIterationSummary();
#else
        return casacore::Record( );
#endif
	};

	static int effective_rank(MPI_Comm comm) {
		int result;
		if (comm != MPI_COMM_NULL)
			MPI_Comm_rank(comm, &result);
		else
			result = -1;
		return result;
	}

	static int effective_size(MPI_Comm comm) {
		int result;
		if (comm != MPI_COMM_NULL)
			MPI_Comm_size(comm, &result);
		else
			result = 0;
		return result;
	}
};

} // namespace casa

#endif // ITERATION_CONTROL_H_
