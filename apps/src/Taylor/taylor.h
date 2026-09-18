// # Copyright (C) 2021,2026
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$

#ifndef TAYLOR_H
#define TAYLOR_H

#include <casa/aips.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/StreamLogSink.h>
#include <images/Images/TempImage.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/ImageUtilities.h>
#include <images/Images/ImageOpener.h>
#include <images/Images/ImageSummary.h>
#include <images/Images/ImageInfo.h>
#include <casacore/tables/Tables/TableUtil.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/lattices/LatticeMath/LatticeMathUtil.h>
#include <casacore/lattices/Lattices/LatticeLocker.h>
#include <casa/Containers/Record.h>
#include <lattices/Lattices/PagedArray.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
#include <librautils/utils.h>
#include <vector>
#include <string>
#include <memory>

using namespace casacore;

namespace taylor
{
	/**
	 * Image types, numbered to match CASA's SynthesisUtilMethods::cubeToTaylorSum
	 * `imtype` argument.  The ordering is load bearing: the PB premultiply during
	 * accumulation applies to types below PB, exactly as CASA's `imtype < 2`.
	 */
	enum ImageType
	{
		PSF = 0,
		RESIDUAL,
		PB,
		SUMWT,
		MODEL,
		INVALID = -1
	};

	/**
	 * Taylor-term conversions for the mtmfs-via-cube pipeline.
	 *
	 * Configuration (reference frequency, term count, pblimit, image type,
	 * overwrite) is constructor state; the methods take only image names.  All
	 * methods return void and report failure by throwing AipsError through
	 * LogIO::EXCEPTION, which is the error channel the CLI and the tests are
	 * already built around.
	 */
	class Taylor
	{
	public:
		Taylor(const std::string &reffreq, int nterms, float pblimit,
			   ImageType imtype, bool overwrite);

		/**
		 * Way In: accumulate per-SPW images into Taylor terms.
		 *
		 * @param cubenames  one image per SPW.
		 * @param mtnames    mtnames[0] is a PREFIX; terms are written as
		 *                   <prefix>.tt0 ... <prefix>.ttN.
		 * @param pbnames    one per-SPW PB per cube, same order, with the avgPB
		 *                   appended last, so size() == cubenames.size() + 1.
		 * @param sumwtnames one per SPW; ignored when imtype is SUMWT.
		 */
		void cubeToTaylorSum(const std::vector<std::string> &cubenames,
							 const std::vector<std::string> &mtnames,
							 const std::vector<std::string> &pbnames,
							 const std::vector<std::string> &sumwtnames);

		/**
		 * Way Out: evaluate Taylor terms back onto the per-SPW images.
		 *
		 * When pbnames is non-empty the accumulated cubes are divided by
		 * pbnames[0], removing the avgPB baked in on the Way In so that what
		 * leaves here is TRUE SKY: roadrunner's awp degridder applies PB(nu)
		 * itself.
		 */
		void taylorToCube(const std::vector<std::string> &mtnames,
						  const std::vector<std::string> &cubenames,
						  const std::vector<std::string> &pbnames);

		void computeAlpha(const std::vector<std::string> &mtnames,
						  const std::vector<std::string> &alphaimages);

		void computeAvgPB(const std::vector<std::string> &pbnames,
						  const std::string &avgpbname,
						  const std::string &minfreqpbname,
						  const std::string &avgpbmode);

		void applyPB(const std::string &pbname,
					 const std::vector<std::string> &cubenames,
					 const std::string &mode);

		void removeFreqDepPB(const std::string &avgpbname,
							 const std::vector<std::string> &cubenames,
							 const std::vector<std::string> &pbnames);

		void applyFreqDepPB(const std::string &avgpbname,
							const std::vector<std::string> &cubenames,
							const std::vector<std::string> &pbnames);

		/**
		 * Facet / channel / polarization slice of an image on disk.  Public
		 * because the tests drive it directly.
		 */
		static std::shared_ptr<ImageInterface<float>>
		getImageSlice(const std::string &imgName, const int facet, const int nfacets,
					  const int chan, const int nchanchunks, const int pol, const int npolchunks);

		int nterms() const { return nterms_; }
		float pblimit() const { return pblimit_; }
		ImageType imtype() const { return imtype_; }

	private:
		std::string reffreq_;
		int nterms_ = 0;
		float pblimit_ = 0.0f;
		ImageType imtype_ = INVALID;
		bool overwrite_ = false;

		/// Number of output Taylor terms: 2*nterms-1 for PSF and SUMWT, else nterms.
		int outputTermCount() const;

		void validateCubeToTaylorInputs(LogIO &log_l,
										const std::vector<std::string> &cubenames,
										const std::vector<std::string> &mtnames,
										const std::vector<std::string> &pbnames,
										const std::vector<std::string> &sumwtnames) const;

		void createTermImages(LogIO &log_l, const std::string &prefix,
							  const std::vector<int> &templateShape,
							  const CoordinateSystem &templateCoord,
							  double minFreq, double maxFreq, int spectralCoordIndex) const;

		void createPBMasks(LogIO &log_l, const std::string &prefix) const;

		static void checkRefFreqRange(LogIO &log_l, double refval,
									  const std::string &reffreq_str,
									  const std::vector<double> &midFreqs);
	};
} // namespace taylor

#endif
