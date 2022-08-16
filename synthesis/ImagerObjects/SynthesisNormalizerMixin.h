/* -*- mode: c++ -*- */
//# SynthesisNormalizerMixin.h: Mixin for using SynthesisNormalizer class in
//#                             parallel imaging framework (ParallelImagerMixin)
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
#ifndef SYNTHESIS_NORMALIZER_MIXIN_H_
#define SYNTHESIS_NORMALIZER_MIXIN_H_

#include <synthesis/ImagerObjects/MPIGlue.h>
#include <synthesis/ImagerObjects/SynthesisNormalizer.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/OS/RegularFile.h>
#include <memory>
#include <algorithm>
#include <vector>

namespace casa {

/**
 * Simple mixin class to put SynthesisNormalizer into ParallelImagerMixin
 * framework.
 */
template<class T>
class SynthesisNormalizerMixin
	: public T {

private:
	std::vector< std::shared_ptr<SynthesisNormalizer> > normalizers;

protected:
	void
	setup_normalizer(MPI_Comm comm, std::vector<casacore::Record> &norm_pars) {
		// Create all normalizer components on rank 0 of comm. TODO: Could we
		// distribute normalizers in a round-robin fashion across processes in
		// comm?

		teardown_normalizer();
		if (T::effective_rank(comm) == 0)
			for (auto pars : norm_pars) {
				SynthesisNormalizer *sn = new SynthesisNormalizer();
				sn->setupNormalizer(pars);
				normalizers.push_back(std::shared_ptr<SynthesisNormalizer>(sn));
				// FIXME: check whether we're to remove any files or directories
				// in addition to those provided in "partimagenames", i.e, those
				// with file name extensions
				//
				// To ensure restarts work correctly, remove existing files.
				if (pars.isDefined("partimagenames")) {
					const std::vector<casacore::String> &part_names =
						pars.asArrayString("partimagenames").tovector();
					for (size_t p = 0; p < part_names.size(); ++p) {
						casacore::File f(part_names[p]);
						if (f.isDirectory(false))
							casacore::Directory(f).removeRecursive();
						else
							casacore::RegularFile(f).remove();
					}
				}
			}
	};

	void
	teardown_normalizer() {
		normalizers.clear();
	};

public:
	void
	normalize_psf() {
		for (auto sn : normalizers) {
			sn->gatherImages(/*dopsf*/true, /*doresidual*/false,
			                 /*density*/false);
			sn->dividePSFByWeight();
		}
	};

	void
	normalize_model() {
		for (auto sn : normalizers) {
			sn->divideModelByWeight();
			sn->scatterModel();
		}
	};

	void
	normalize_residual() {
		for (auto sn : normalizers) {
			sn->gatherImages(/*dopsf*/false, /*doresidual*/true,
			                 /*density*/false);
			sn->divideResidualByWeight();
		}
	};

	void
	denormalize_model() {
		for (auto sn : normalizers)
			sn->multiplyModelByWeight();
	};

	void
	reduce_weight_density() {
		for (auto sn : normalizers) {
			sn->gatherImages(/*dopsf*/false, /*doresidual*/false,
			                 /*density*/true);
			sn->scatterWeightDensity();
		}
	};
};

} // namespace casa

#endif // SYNTHESIS_NORMALIZER_MIXIN_H_
