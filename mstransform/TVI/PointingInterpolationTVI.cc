#include <iostream>
#include <set>
#include <memory>
#include <sstream>

#include <mstransform/TVI/PointingInterpolationTVI.h>

// Visibility Buffer
#include <msvis/MSVis/VisBuffer2.h>

// Visibility Iterator
#include <msvis/MSVis/VisibilityIteratorImpl2.h>
#include <msvis/MSVis/TransformingVi2.h>

// Measures
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MeasFrame.h>
#include <casacore/tables/TaQL.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Exceptions/Error.h>



// Messy
// !!! This include is required and must be after PI_TVI.h
// due to forward declarations
// of the MeasurementSet class in files included by PI_TVI.h
#include <casacore/ms/MeasurementSets.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>

using namespace casa::vi;
using namespace casacore;
using std::cout;
using std::endl;

using PI_TVI = PointingInterpolationTVI;
using PI_Vi2Factory = PointingInterpolationVi2Factory;
using PI_TVILayerFactory = PointingInterpolationTVILayerFactory;

PI_TVI::PointingInterpolationTVI(ViImplementation2 *inputVII) :
	TransformingVi2(inputVII) {
	// Initialize associated output VisBuffer2 object
	VisBufferOptions vbOptions {VbNoOptions};
	setVisBuffer(createAttachedVisBuffer(vbOptions));

	// Expecting a pointing table
	inputVII->ms().pointing().throwIfNull();

	// Input MS / pointing direction units and ref
	MSPointingColumns mspc(getVii()->ms().pointing());
	auto dirUnits = mspc.directionMeasCol().measDesc().getUnits();
	lonUnit_ = dirUnits[0];
	latUnit_ = dirUnits[1];

	MDirection::Ref ref(mspc.directionMeasCol().measDesc().getRefCode());
	dirRef_ = ref;
	toRef_.setType(MDirection::ICRS);

	setupInterpolator();
}

void PI_TVI::setupInterpolator(){

	try {
		// To clarify: behavior of TVIs iterating over multiple MSs = ?
		const auto & ms = this->getVii()->ms();

		// Total number of antennas
		auto nAnts = ms.antenna().nrow();

		// Selected, data-taking antennas
		Vector<Bool> isSelected(nAnts);
		MSColumns msCols(ms);
		cout << "Extracting active antennas ..." << endl;
		for (auto antId : msCols.antenna1().getColumn()) isSelected[antId] = true;
		for (auto antId : msCols.antenna2().getColumn()) isSelected[antId] = true;
		std::set<Int> selectedAntIds;
		for (uInt antId=0; antId<nAnts; antId++){
			if (isSelected[antId]) selectedAntIds.insert(antId);
		}

		//cout << "TVI input ms rows: " << ms.nrow() << endl;
		//cout << "TVI input ms antennas: " << nAnts << endl;
		//cout << "TVI input ms selected antennas: " << selectedAntIds.size() << endl;

		// Distinct pointings
		std::stringstream ss;
		ss << "select ANTENNA_ID, TIME from $1 "
		   << "where ANTENNA_ID in " << taQLSet(selectedAntIds) << " "
		   << "order by distinct ANTENNA_ID, TIME";
		String qryDistinctPointings(ss.str());

		//cout << "Query string:" << endl;
		//cout << qryDistinctPointings << endl;

		auto keys = tableCommand(qryDistinctPointings,ms.pointing()).table();
		auto distinctPointings = ms.pointing()(keys.rowNumbers());

		// Pointings per antenna
		auto distinctPointingsAntIds = ScalarColumn<Int>(distinctPointings,"ANTENNA_ID").getColumn();
		Vector<uInt> antsPointingsCount(nAnts,0);
		for ( auto antId : distinctPointingsAntIds ) ++antsPointingsCount[antId];
		Vector<Bool> hasPointings(nAnts,false);
		for (uInt antId=0; antId<nAnts; antId++){
			if (antsPointingsCount[antId] >0) hasPointings[antId] = true;
		}

		//for (uInt antId=0; antId < antsPointingsCount.nelements(); ++antId)
		//	cout << "ant: " << antId << " pointings: " << antsPointingsCount[antId] << endl;

		// Extract pointings data from distinctPointings table
		using PointingTimes = Vector<Double>;
		using PointingDir   = Vector<Double>;
		using PointingDirs  = Vector<PointingDir>;
		Vector<PointingTimes> antsPointingTimes(nAnts);
		Vector<PointingDirs> antsPointingDirs(nAnts);

		auto distinctPointingTimes = ScalarColumn<Double>(distinctPointings,"TIME").getColumn();
		ArrayMeasColumn<MDirection> distinctPointingDirsArray(distinctPointings,"DIRECTION");
		const auto & dirArray = distinctPointingDirsArray;

		uInt antStartRow = 0; // In distinct pointings table
		for ( uInt antId=0; antId<nAnts; antId++ ) {
			if ( not isSelected[antId] or not hasPointings[antId] ) continue;
			// Time
			auto antPointingsCount = antsPointingsCount[antId];
			antsPointingTimes[antId] = distinctPointingTimes(Slice(antStartRow,antPointingsCount));
			// Direction
			auto & antPointingDirs = antsPointingDirs[antId];
			antPointingDirs.resize(antPointingsCount);
			for ( uInt pIndex = 0 ; pIndex < antPointingsCount ; pIndex++ ){
				auto pointingRow = antStartRow + pIndex;
				auto rowDirArray = dirArray(pointingRow);
				auto rowDirection = rowDirArray.begin()->getAngle().getValue();
				antPointingDirs[pIndex] = rowDirection;
			}
			// Next antenna in distinct pointings table
			antStartRow += antPointingsCount;
		}

		interpolator_.setData(antsPointingTimes,antsPointingDirs,isSelected);

		// cout << "myTVI.constructor: done" << endl;
	} catch (const std::exception& e) {
		cout << "Unexcepted exception caught in " << __func__  << endl;
		cout << "Error message: " << endl;
		cout << e.what() << endl;
		exit(1);
	}
}

PI_TVI::~PointingInterpolationTVI() {
	// cout << "myTVI: destructor: start" << endl;
	// delete interpolator_;
	// delete distinctPointingsCols_;
	inputVii_p = NULL; // Following TVI Development Guide
	//cout << "myTVI: destructor: done" << endl;
}

String PI_TVI::ViiType() const {
	return String("PointingInterpolation( ") + getVii()->ViiType() + " )";
}

void PI_TVI::origin(){
	// Drive underlying ViImplementation
	getVii()->origin();

	// Synchronize own output VisBuffer
	configureNewSubchunk();
}

void PI_TVI::next(){
	// Drive underlying ViImplementation
	getVii()->next();

	// Synchronize own output VisBuffer
	configureNewSubchunk();
}

PI_TVI::Interpolator &PI_TVI::getInterpolator() {
	return interpolator_;
}

void PI_TVI::setOutputDirectionFrame(MDirection::Types toRefType){
	toRef_.setType(toRefType);
}

std::pair<bool, MDirection> PI_TVI::getPointingAngle (int antId, double timeStamp) const {
	auto dirValue = interpolator_.pointingDir(antId,timeStamp);
	Quantity qLon(dirValue[0], lonUnit_);
	Quantity qLat(dirValue[1], latUnit_);
	MDirection fromDir(qLon,qLat,dirRef_);

	if (dirRef_.getType() == toRef_.getType())
		return std::make_pair(true,fromDir);

	using InterpMethod = PI_TVI::Interpolator::InterpMethod;
	double refTimeStamp = interpolator_.getInterpMethod() == InterpMethod::NEAREST ?
			  interpolator_.nearestPointingTimeStamp()
			: timeStamp;
	MEpoch refEpoch(Quantity(refTimeStamp,"s"),MEpoch::UTC);

	auto antPosition = getVisBuffer()->subtableColumns().antenna().positionMeas()(antId);
	auto toDir = MDirection::Convert(fromDir,
			MDirection::Ref(toRef_.getType(),MeasFrame(refEpoch,antPosition)))();

	return std::make_pair(true,toDir);
}

String PI_TVI::taQLSet(const std::set<int> & inputSet){
	std::stringstream ss;
	ss << "[ ";
	if (not inputSet.empty()){
		auto it = inputSet.begin();
		ss << *it;
		while ( ++it != inputSet.end() ) ss << "," << *it ;
	}
	ss << " ]";
	return String(ss.str());
}

using PI_Interp = PI_TVI::Interpolator;

PI_Interp::Interpolator()
	: interp_(InterpMethod::CUBIC_SPLINE),
	  nearestPointingTimeStamp_(0.0)
{
}

void PI_Interp::setInterpMethod(InterpMethod m){
	interp_ = m;
}

PI_Interp::InterpMethod PI_Interp::getInterpMethod() const {
	return interp_ ;
}

void PI_Interp::setData(const Vector<PointingTimes> &antsTimes,
			 const Vector<PointingDirs> &antsDirs,
			 const Vector<bool> &antSelected){

	// TODO: check sizes of input vectors

	// Initialize
	clear();
	init(antSelected);

	// Count pointings per antenna
	auto nAnt = antsTimes.nelements();
	Vector<uInt> pointingsCount(nAnt,0);
	for (decltype(nAnt) antId = 0; antId < nAnt; antId++) {
		pointingsCount[antId] = antsTimes[antId].nelements();
	}

	// Compute spline coefficients for selected antennas,
	// where possible
	for (decltype(nAnt) antId = 0; antId < nAnt; ++antId) {
		if (not antSelected[antId]) continue;
		if (pointingsCount[antId] < 4) continue;

		// Resize private member: antsTimes_
		// auto antPointings = pointingsCount[antId];
		// antsTimes_[antId].resize(antPointings);

		// Store pointing times: required for interpolation
		antsTimes_[antId] = antsTimes[antId];

		// No need
		//dirPointing(i).resize(nPointingData(i));

		// Resize private member: antsSplinesCoeffs_
		//splineCoeff(i).resize(nPointingData(i) - 1);
		auto antPointings = pointingsCount[antId];
		auto antSplineSegments = antPointings - 1;
		antsSplinesCoeffs_[antId].resize(antSplineSegments);

		// No need
		//for (uInt j = 0; j < dirPointing(i).nelements(); ++j) {
		  //dirPointing(i)(j).resize(2);
		//}

		// Resize private member: antsSplinesCoeffs_: continued
		///for (uInt j = 0; j < splineCoeff(i).nelements(); ++j) {
		auto & antSplinesCoeffs = antsSplinesCoeffs_[antId];
		for (uInt s = 0; s < antSplineSegments; ++s){
			// splineCoeff(i)(j).resize(2);
			antSplinesCoeffs[s].resize(2);
			// splineCoeff(i)(j)(0).resize(4); // x
			// splineCoeff(i)(j)(1).resize(4); // y
			antSplinesCoeffs[s][0].resize(4);
			antSplinesCoeffs[s][1].resize(4);
		}

		// Copy input data into private members
		//Int npoi = nPointingData(i);
		//for (Int j = 0; j < npoi; ++j) {

			// Copy time into private member: done above
			//  timePointing(i)(j) = time(i)(j);

			// Not needed
			//for (Int k = 0; k < 2; ++k) {
			//	dirPointing(i)(j)(k) = dir(i)(j)(k);
			//}
		//}


		// calcSplineCoeff(timePointing(i), dirPointing(i), splineCoeff(i));
		const auto & antPointingTimes = antsTimes[antId];
		const auto & antPointingDirs  = antsDirs[antId];
		computeSplineCoeffs(antPointingTimes,antPointingDirs,antSplinesCoeffs);
		isInterpolated_[antId] = true;
	}

}

void PI_Interp::init(const Vector<bool> &antSelected){
	isSelected_ = antSelected;
	auto nAnt = antSelected.nelements();
	antsTimes_.resize(nAnt);
	antsSplinesCoeffs_.resize(nAnt);
	isInterpolated_.resize(nAnt);
	isInterpolated_ = false;
}

void PI_Interp::clear(){
	init();
}

void PI_Interp::computeSplineCoeffs(const PointingTimes& time,
                                        const PointingDirs& dir,
										SplineCoeffs& coeff) {
	Vector<Double> h, vx, vy;
	Vector<Double> a;
	Vector<Double> c;
	Vector<Double> alpha, beta, gamma;
	Vector<Double> wx, wy;
	Vector<Double> ux, uy;

	const auto num_data = time.nelements();
	h.resize(num_data-1);
	vx.resize(num_data-1);
	vy.resize(num_data-1);
	a.resize(num_data-1);
	c.resize(num_data-1);
	alpha.resize(num_data-1);
	beta.resize(num_data-1);
	gamma.resize(num_data-1);
	wx.resize(num_data-1);
	wy.resize(num_data-1);
	ux.resize(num_data);
	uy.resize(num_data);

	h[0] = time[1] - time[0];
	for (uInt i = 1; i < num_data-1; ++i) {
		h[i] = time[i+1] - time[i];
		vx[i] = 6.0*((dir[i+1][0]-dir[i][0])/h[i] - (dir[i][0]-dir[i-1][0])/h[i-1]);
		vy[i] = 6.0*((dir[i+1][1]-dir[i][1])/h[i] - (dir[i][1]-dir[i-1][1])/h[i-1]);
		a[i] = 2.0*(time[i+1] - time[i-1]);
		c[i] = h[i];
		gamma[i] = c[i];
	}
	alpha[2] = c[1]/a[1];
	for (uInt i = 3; i < num_data-1; ++i) {
		alpha[i] = c[i-1]/(a[i-1] - alpha[i-1]*c[i-2]);
	}
	beta[1] = a[1];
	for (uInt i = 2; i < num_data-2; ++i) {
		beta[i] = c[i]/alpha[i+1];
	}
	beta[num_data-2] = a[num_data-2] - alpha[num_data-2] * c[num_data-3];
	wx[0] = 0.0;
	wx[1] = vx[1];
	wy[0] = 0.0;
	wy[1] = vy[1];
	for (uInt i = 2; i < num_data-1; ++i) {
		wx[i] = vx[i] - alpha[i]*wx[i-1];
		wy[i] = vy[i] - alpha[i]*wy[i-1];
	}
	ux[num_data-1] = 0.0;
	uy[num_data-1] = 0.0;
	for (uInt i = num_data-2; i >= 1; --i) {
		ux[i] = (wx[i] - gamma[i]*ux[i+1])/beta[i];
		uy[i] = (wy[i] - gamma[i]*uy[i+1])/beta[i];
	}
	ux[0] = 0.0;
	uy[0] = 0.0;

	for (uInt i = 0; i < num_data-1; ++i) {
		coeff[i][0][0] = dir[i][0];
		coeff[i][1][0] = dir[i][1];
		const auto dt = time[i+1]-time[i];
		coeff[i][0][1] = (dir[i+1][0]-dir[i][0])/dt - dt*(2.0*ux[i]+ux[i+1])/6.0;
		coeff[i][1][1] = (dir[i+1][1]-dir[i][1])/dt - dt*(2.0*uy[i]+uy[i+1])/6.0;
		coeff[i][0][2] = ux[i]/2.0;
		coeff[i][1][2] = uy[i]/2.0;
		coeff[i][0][3] = (ux[i+1]-ux[i])/dt/6.0;
		coeff[i][1][3] = (uy[i+1]-uy[i])/dt/6.0;
	}
}


Vector<Double> PI_Interp::pointingDir(int antId, double timeStamp) const {
//const MSPointingColumns& mspc,
//const Double& time,
//const Int& index,
//const Int& antid) {


	if ( not isInterpolated_[antId] ) {
		Vector<Double> pole(2,0.0);
		return pole;
	}
	// Search the segment to which the timestamp belongs
	// TODO: std::lowerbound

	//Int lastIndex = timePointing(antid).nelements() - 1;
	auto antPointingsCount = antsTimes_[antId].nelements();
	Int lastIndex = antPointingsCount - 1;

	Int aindex = lastIndex;
	//for (uInt i = 0; i < timePointing(antId).nelements(); ++i) {
	for (uInt i = 0; i < antPointingsCount; ++i) {
		if (timeStamp < antsTimes_[antId][i]) {
			aindex = i-1;
			break;
		}
	}
	if (aindex < 0) aindex = 0;
	if (lastIndex <= aindex) aindex = lastIndex - 1;
	nearestPointingTimeStamp_ = antsTimes_[antId][aindex];

	//auto const &coeff = splineCoeff(antid)(aindex);
	const auto & coeff = antsSplinesCoeffs_[antId][aindex];
	// Double dt = time - timePointing(antid)(aindex);
	Double dt = interp_ == InterpMethod::NEAREST ? 0.0 :
				timeStamp - antsTimes_[antId][aindex];

	Vector<Double> newdir(2);
	//newdir(0) = coeff(0)(0) + coeff(0)(1)*dt + coeff(0)(2)*dt*dt + coeff(0)(3)*dt*dt*dt;
	//newdir(1) = coeff(1)(0) + coeff(1)(1)*dt + coeff(1)(2)*dt*dt + coeff(1)(3)*dt*dt*dt;
	newdir(0) = coeff(0)(0) + dt*(coeff(0)(1) + dt*(coeff(0)(2) + dt*coeff(0)(3)));
	newdir(1) = coeff(1)(0) + dt*(coeff(1)(1) + dt*(coeff(1)(2) + dt*coeff(1)(3)));

	return newdir;

	// Quantity rDirLon(newdir(0), "rad");
	// Quantity rDirLat(newdir(1), "rad");
	// auto const &directionMeasColumn = mspc.directionMeasCol();
	// MDirection::Ref rf(directionMeasColumn.measDesc().getRefCode());
	// if (directionMeasColumn.measDesc().isRefCodeVariable()) {
	// rf = mspc.directionMeas(index).getRef();
	// }
	//return MDirection(rDirLon, rDirLat, rf);
}

double PI_Interp::nearestPointingTimeStamp() const {
	return nearestPointingTimeStamp_;
}
// ------------------------- Factories ---------------------------------

// ---- Vi2Factory
PI_Vi2Factory::PointingInterpolationVi2Factory(Record const &configuration,
		ViImplementation2 *inputVII) :
		inputVII_p(inputVII),
		configuration_(configuration)
{

}

PI_Vi2Factory::PointingInterpolationVi2Factory(const Record & /*configuration*/,
		const MeasurementSet *ms,
		const SortColumns &sortColumns,
		Double timeInterval,
		Bool isWritable) :
		inputVII_p(nullptr) {

	inputVII_p = new VisibilityIteratorImpl2(
			Block<const MeasurementSet *>(1, ms),
			sortColumns, timeInterval, isWritable);
}

PI_Vi2Factory::~PointingInterpolationVi2Factory() {
}

ViImplementation2 * PI_Vi2Factory::createVi() const {
	return new PI_TVI(inputVII_p);
}

// ---- TVILayerFactory
PI_TVILayerFactory::PointingInterpolationTVILayerFactory(
		Record const &configuration) :
		ViiLayerFactory()
{
	configuration_p = configuration;
}

PI_TVILayerFactory::~PointingInterpolationTVILayerFactory()
{

}

ViImplementation2*
PI_TVILayerFactory::createInstance(ViImplementation2* vii0) const {
	// Make the PI_TVI, using supplied ViImplementation2, and return it
	PointingInterpolationVi2Factory factory(configuration_p, vii0);
	ViImplementation2 *vii = nullptr;
	try {
		vii = factory.createVi();
	} catch (...) {
		if (vii0) {
			delete vii0;
		}
		throw;
	}
	return vii;
}




