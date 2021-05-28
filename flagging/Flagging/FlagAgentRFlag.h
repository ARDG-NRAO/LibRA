//# FlagAgentRFlag.h: This file contains the interface definition of the FlagAgentRFlag class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2012, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2012, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef FlagAgentRFlag_H_
#define FlagAgentRFlag_H_

#include <flagging/Flagging/FlagAgentBase.h>
#include <casa/Utilities/DataType.h>

namespace casa { //# NAMESPACE CASA - BEGIN

/**
 * <summary>
 * A flag agent that implements the 'rflag' flagdata mode.
 * </summary>
 *
 * Uses the FlagAgentBase::ANTENNA_PAIRS iteration approach.
 *
 * Note that the user interface provided by flagdata is
 * complex/misleading in trying to replicate the use patterns of RFlag
 * in AIPS. This produces a potentially confusing pattern when
 * implementing the virtuals of FlagAgentBase. The per-channel
 * RMS/variance values are calculated in
 * computeAntennaPairFlagsCore(). Then, the overall thresholds
 * (timedev, freqdev) are calculated in getReport().  The different
 * display modes are essentially implemented in getReportCore().
 *
 * This implementation takes as reference implementation the AIPS task RFlag
 * by Erich Greisen, see
 * The AIPS Cookbook
 * Appendix E : Special Considerations for EVLA data calibration and imaging in AIPS
 * E.5 Detailed Flagging
 * http://www.aips.nrao.edu/cook.html#CEE
 */
class FlagAgentRFlag : public FlagAgentBase {

	enum optype {

		MEAN,
		ROBUST_MEAN,
		MEDIAN,
		ROBUST_MEDIAN
	};

public:

	FlagAgentRFlag(FlagDataHandler *dh, casacore::Record config, casacore::Bool writePrivateFlagCube = false, casacore::Bool flag = true);
	~FlagAgentRFlag();

protected:

	// Parse configuration parameters
	void setAgentParameters(casacore::Record config);

	// Compute flags for a given (time,freq) map
	bool computeAntennaPairFlags(const vi::VisBuffer2 &visBuffer, VisMapper &visibilities,FlagMapper &flags,casacore::Int antenna1,casacore::Int antenna2,std::vector<casacore::uInt> &rows);

	// Extract automatically computed thresholds to use them in the next pass
	void passIntermediate(const vi::VisBuffer2 &visBuffer);

	// Remove automatically computed thresholds for the following scans
	void passFinal(const vi::VisBuffer2 &visBuffer);

	// Convenience function to get simple averages
	casacore::Double mean(std::vector<casacore::Double> &data,std::vector<casacore::Double> &counts);

	// Convenience function to compute median
	casacore::Double median(std::vector<casacore::Double> &data);

	//
	void noiseVsRef(std::vector<casacore::Double> &data, casacore::Double ref);

	// Convenience function to get simple averages
	casacore::Double computeThreshold(std::vector<casacore::Double> &data, std::vector<casacore::Double> &dataSquared, std::vector<casacore::Double> &counts);

	// casacore::Function to be called for each timestep/channel
	void computeAntennaPairFlagsCore(std::pair<casacore::Int,casacore::Int> spw_field,
										casacore::Double noise,
										casacore::Double scutoff,
										casacore::uInt timeStart,
										casacore::uInt timeStop,
										casacore::uInt centralTime,
										VisMapper &visibilities,
										FlagMapper &flags);

	void robustMean(	casacore::uInt timestep_i,
						casacore::uInt pol_k,
						casacore::uInt nChannels,
						casacore::Double &AverageReal,
						casacore::Double &AverageImag,
						casacore::Double &StdReal,
						casacore::Double &StdImag,
						casacore::Double &SumWeightReal,
						casacore::Double &SumWeightImag,
						VisMapper &visibilities,
						FlagMapper &flags);

	void simpleMedian(	casacore::uInt timestep_i,
						casacore::uInt pol_k,
						casacore::uInt nChannels,
						casacore::Double &AverageReal,
						casacore::Double &AverageImag,
						casacore::Double &StdReal,
						casacore::Double &StdImag,
						casacore::Double &SumWeightReal,
						casacore::Double &SumWeightImag,
						VisMapper &visibilities,
						FlagMapper &flags);

	// casacore::Function to return histograms
	FlagReport getReport();

	// casacore::Function to return histograms
	FlagReport getReportCore(	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > &data,
								map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > &dataSquared,
								map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > &counts,
								map< std::pair<casacore::Int,casacore::Int>,casacore::Double > &threshold,
								FlagReport &totalReport,
								string label,
								casacore::Double scale);

	// Dedicated method to generate threshold values
	void generateThresholds(	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > &data,
								map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > &dataSquared,
								map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > &counts,
								map< std::pair<casacore::Int,casacore::Int>,casacore::Double > &threshold,
								string label);

private:

	// General parameters
	casacore::Bool doflag_p;
	casacore::Bool doplot_p;
	casacore::uInt nTimeSteps_p;
	// flagdata task param timedevscale
	casacore::Double noiseScale_p;
	// flagdata task param freqdevscale
	casacore::Double scutoffScale_p;

	// Spectral Robust fit
	casacore::uInt nIterationsRobust_p;
	std::vector<casacore::Double> thresholdRobust_p;
	casacore::Double spectralmin_p;
	casacore::Double spectralmax_p;
	casacore::uInt optype_p;
	void (casa::FlagAgentRFlag::*spectralAnalysis_p)(casacore::uInt,casacore::uInt,casacore::uInt,casacore::Double&,casacore::Double&,casacore::Double&,casacore::Double&,casacore::Double&,casacore::Double&,VisMapper&,FlagMapper&);

	// Store frequency to be used in Reports
	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > field_spw_frequency_p;
	map< std::pair<casacore::Int,casacore::Int>,casacore::Double > field_spw_frequencies_p;

	// casacore::Time-direction analysis
	casacore::Double noise_p;
	// holds the timedev thresholds for every field-SPW pair
	map< std::pair<casacore::Int,casacore::Int>,casacore::Double > field_spw_noise_map_p;
	map< std::pair<casacore::Int,casacore::Int>,casacore::Bool > user_field_spw_noise_map_p;
	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > field_spw_noise_histogram_sum_p;
	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > field_spw_noise_histogram_sum_squares_p;
	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > field_spw_noise_histogram_counts_p;

	// Spectral analysis
	casacore::Double scutoff_p;
	// holds the freqdev thresholds for every field-SPW pair
	map< std::pair<casacore::Int,casacore::Int>,casacore::Double > field_spw_scutoff_map_p;
	map< std::pair<casacore::Int,casacore::Int>,casacore::Bool > user_field_spw_scutoff_map_p;
	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > field_spw_scutoff_histogram_sum_p;
	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > field_spw_scutoff_histogram_sum_squares_p;
	map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::Double> > field_spw_scutoff_histogram_counts_p;
};


} //# NAMESPACE CASA - END

#endif /* FlagAgentRFlag_H_ */

