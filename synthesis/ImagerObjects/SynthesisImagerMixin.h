// -*- mode: c++ -*-
//# SynthesisImagerMixin.h: Mixin for using SynthesisImager class in parallel
//#                         imaging framework (ParallelImagerMixin)
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
#ifndef SYNTHESIS_IMAGER_MIXIN_H_
#define SYNTHESIS_IMAGER_MIXIN_H_

#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/Array.h>
#include <synthesis/ImagerObjects/MPIGlue.h>
#include <synthesis/ImagerObjects/SynthesisImager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <memory>
#include <vector>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <dirent.h>
#include <system_error>

namespace casa {

/**
 * Simple mixin class to put SynthesisImager into ParallelImagerMixin framework.
 */
template <class T>
class SynthesisImagerMixin
	: public T {

private:
	std::unique_ptr<SynthesisImager> si;

	static casacore::Quantity asQuantity(const casacore::Record &rec, const char *field_name);

	static casacore::Quantity asQuantity(const casacore::String &field_name);

	static bool haveCFCache(const std::string &dirname);

	static int isCFS(const struct dirent *d);

	static std::vector<std::string> getCFCacheList(
		const SynthesisParamsGrid &grid_pars, int size, int rank);

	void
	set_weighting(const casacore::Record &weight_pars, 
	              const std::vector<SynthesisParamsImage> &image_pars) {
		casacore::String type =
			((weight_pars.fieldNumber("type") != -1)
			 ? weight_pars.asString("type")
			 : casacore::String("natural"));
		casacore::String rmode =
			((weight_pars.fieldNumber("rmode") != -1)
			 ? weight_pars.asString("rmode")
			 : casacore::String("norm"));
		casacore::Double robust =
			((weight_pars.fieldNumber("robust") != -1)
			 ? weight_pars.asDouble("robust")
			 : 0.0);
		casacore::Int npixels =
			((weight_pars.fieldNumber("npixels") != -1)
			 ? weight_pars.asInt("npixels")
			 : 0);
		casacore::Bool multifield =
			((weight_pars.fieldNumber("multifield") != -1)
			 ? weight_pars.asBool("multifield")
			 : false);
		casacore::Bool usecubebriggs =
			((weight_pars.fieldNumber("usecubebrigss") != -1)
			 ? weight_pars.asBool("usecubebriggs")
			 : false);
		casacore::Quantity noise =
			((weight_pars.fieldNumber("noise") != -1)
			 ? asQuantity(weight_pars, "noise")
			 : casacore::Quantity(0.0, "Jy"));
		casacore::Quantity field_of_view =
			((weight_pars.fieldNumber("fieldofview") != -1)
			 ? asQuantity(weight_pars, "fieldofview")
			 : casacore::Quantity(0.0, "arcsec"));
		const casacore::Array<casacore::String> &uv_taper_pars =
			((weight_pars.fieldNumber("uvtaper") != -1)
			 ? weight_pars.asArrayString("uvtaper")
			 : casacore::Array<casacore::String>());
		casacore::Quantity bmaj(0.0, "deg"), bmin(0.0, "deg"), bpa(0.0, "deg");
		casacore::String filter_type("");
		if (uv_taper_pars.nelements() > 0) {
			bmaj = asQuantity(uv_taper_pars(casacore::IPosition(1, 0)));
			filter_type = casacore::String("gaussian");
			if (uv_taper_pars.nelements() > 1) {
				bmin = asQuantity(uv_taper_pars(casacore::IPosition(1, 1)));
				if (uv_taper_pars.nelements() > 2)
					bpa = asQuantity(uv_taper_pars(casacore::IPosition(1, 2)));
			} else /* uv_taper_pars.nelements() == 1 */ {
				bmin = bmaj;
			}
		}
		// TODO: the following is the logic for setting 'filter_type' in
		// synthesisimager_cmpt.cc...verify that the check on uv_taper_pars[0]
		// length is not required here
		//
		// if (uv_taper_pars.nelements() > 0 && uv_taper_pars[0].length() > 0)
		//     filter_type = casacore::String("gaussian");
		si->weight(type, rmode, noise, robust, field_of_view, npixels,
		           multifield, usecubebriggs, filter_type, bmaj, bmin, bpa);
		if (image_pars.size() == 1
		    && image_pars[0].stokes == casacore::String("I")
		    && weight_pars.asString("type") != casacore::String("natural")) {
			si->getWeightDensity();
			T::reduce_weight_density();
			si->setWeightDensity();
		}
	};

protected:
	void
	setup_imager(MPI_Comm comm,
	             std::vector<SynthesisParamsSelect> &select_pars,
	             std::vector<SynthesisParamsImage> &image_pars,
	             std::vector<SynthesisParamsGrid> &grid_pars,
	             casacore::Record &weight_pars) {
		// Create a single imager component for every rank in comm.

		teardown_imager();
		int imaging_rank = T::effective_rank(comm);
		if (imaging_rank == 0) {
			si = std::unique_ptr<SynthesisImager>(new SynthesisImager());
			for (auto s : select_pars)
				si->selectData(s);
			for (size_t i = 0;
			     i < std::min(image_pars.size(), grid_pars.size());
			     ++i)
				si->defineImage(image_pars[i], grid_pars[i]);
		}
		int imaging_size = T::effective_size(comm);
		if (imaging_rank >= 0 && imaging_size > 1) {
			SynthesisParamsGrid &grid_pars0 = grid_pars.at(0);
			if (!haveCFCache(grid_pars0.cfCache)) {
				if (imaging_rank == 0) {
					Vector<String> const empty;
					si->dryGridding(empty);
				}
				std::vector<std::string> cf_list =
					getCFCacheList(grid_pars0, imaging_size, imaging_rank);
				if (cf_list.size() > 0) {
					if (si == nullptr)
						si = std::unique_ptr<SynthesisImager>(
							new SynthesisImager());
                    casacore::Vector<casacore::String> const cf_list_casavec(cf_list);
					si->fillCFCache(
						cf_list_casavec, grid_pars0.ftmachine, grid_pars0.cfCache,
						grid_pars0.psTermOn, grid_pars0.aTermOn,grid_pars0.conjBeams);
				}
			}
			// create new imager instance, scrapping any that already exists
			si = std::unique_ptr<SynthesisImager>(new SynthesisImager());
			si->selectData(select_pars.at(imaging_rank));
			si->defineImage(image_pars.at(imaging_rank),
			                grid_pars.at(imaging_rank));
		}
		if (imaging_rank >= 0)
			set_weighting(weight_pars, image_pars);
	};

	void teardown_imager() {
		si.reset();
	};

public:
	void
	make_psf() {
		// TODO: verify this is correct for all ranks
		if (si != nullptr) si->makePSF();
	};

	void
	execute_major_cycle() {
		casacore::Record rec;
		rec.define("lastcycle", T::is_clean_complete());
		// TODO: verify this is correct for all ranks
		if (si != nullptr) si->executeMajorCycle(rec);
		T::end_major_cycle();
	};

	void
	predict_model() {
		// TODO: verify this is correct for all ranks
		if (si != nullptr) si->predictModel();
	};
};

// TODO: this method is a utility function...move it into another module?
template <class T>
casacore::Quantity
SynthesisImagerMixin<T>::asQuantity(
	const casacore::Record &rec, const char *field_name) {
	casacore::Bool success = false;
	casacore::QuantumHolder qh;
	casacore::String err_str;
	switch (rec.dataType(field_name)) {
	case casacore::DataType::TpRecord:
		success = qh.fromRecord(err_str, rec.subRecord(field_name));
		break;
	case casacore::DataType::TpString:
		success = qh.fromString(err_str, rec.asString(field_name));
		break;
	default:
		break;
	}
	if (!(success && qh.isQuantity())) {
		ostringstream oss;
		oss << "Error in converting quantity: " << err_str;
		throw (casacore::AipsError(oss.str()));
	}
	return qh.asQuantity();
};

// TODO: this method is a utility function...move it into another module?
template <class T>
casacore::Quantity
SynthesisImagerMixin<T>::asQuantity(const casacore::String &field_name) {
	casacore::QuantumHolder qh;
	casacore::String err_str;
	casacore::Bool success = qh.fromString(err_str, field_name);
	if (!(success && qh.isQuantity())) {
		ostringstream oss;
		oss << "Error in converting quantity: " << err_str;
		throw (casacore::AipsError(oss.str()));
	}
	return qh.asQuantity();
};

template <class T>
bool
SynthesisImagerMixin<T>::haveCFCache(const std::string &dirname) {
	struct stat stat_buf;
	return (stat(dirname.c_str(), &stat_buf) == 0
	        && S_ISDIR(stat_buf.st_mode));
};

template <class T>
int
SynthesisImagerMixin<T>::isCFS(const struct dirent *d) {
	std::string name(d->d_name);
	return name.find("CFS") == 0;
};

template <class T>
std::vector<std::string>
SynthesisImagerMixin<T>::getCFCacheList(
	const SynthesisParamsGrid &grid_pars, int size, int rank) {
	// return vector for all ranks, even if it's empty
	std::vector<std::string> result;
	struct dirent **namelist;
	int nCFS = scandir(grid_pars.cfCache.c_str(), &namelist, 
	                   SynthesisImagerMixin::isCFS, alphasort);
	if (nCFS >= 0) {
		size = std::min(size, nCFS);
		// Note that with size having been redefined as the minimum of the
		// original size value and nCFS, if rank >= size, then no strings
		// are added to the result vector in the following loop.
		for (int n = rank; n < nCFS; n += size) {
			std::string name(namelist[n]->d_name);
			result.push_back(name);
		}
		free(namelist);
	} else {
		// errno == ENOMEM
		std::error_condition ec(errno, std::generic_category());
		throw casacore::AipsError(casacore::String("Failed to scan cf cache directory '")
		                + grid_pars.cfCache + casacore::String("': ")
		                + casacore::String(ec.message()));
	}
	return result;
};

} // namespace casa

#endif // SYNTHESIS_IMAGER_MIXIN_H_
