#ifndef COMPONENTS_C11TIMER_H
#define COMPONENTS_C11TIMER_H

#include <chrono>

#include <casacore/casa/namespace.h>

namespace casa {

class C11Timer {
	// <summary>
	// casacore::Timer based on C++11 chrono library
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// casacore::Timer based on C++11 chrono library
	// </etymology>

	// <synopsis>
	// casacore::Timer based on C++11 chrono library
	// </synopsis>

public:
	// create the timer but do not start it.
	C11Timer();

	~C11Timer();

	// return mean duration, in seconds, of all start/stop cycles
	casacore::Double meanDuration() const;

	// duration in seconds of most recent start/stop cycle
	casacore::Double duration() const;

	// number of start/stop cycles
	casacore::uInt nCycles() const;

	// (re)start the timer
	void start();

	// stop the timer
	void stop();

	// total duration in seconds, sum of all start/stop cycles
	// doesn't include the duration of the current cycle if the
	// timer is currently running
	casacore::Double totalDuration() const;

private:
    std::chrono::steady_clock::time_point _start;
	std::chrono::duration<double> _duration, _totalDuration;
	casacore::uInt _nCycles;


};
}

#endif
