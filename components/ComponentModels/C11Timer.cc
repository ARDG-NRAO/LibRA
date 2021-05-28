//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

#include <components/ComponentModels/C11Timer.h>

using namespace std::chrono;

using namespace casacore;
namespace casa {
C11Timer::C11Timer()
	: _start(), _duration(), _totalDuration(), _nCycles(0) {}

C11Timer::~C11Timer() {}

Double C11Timer::duration() const {
	return _duration.count();
}

Double C11Timer::meanDuration() const {
	return _duration.count()/_nCycles;
}

uInt C11Timer::nCycles() const {
	return _nCycles;
}

void C11Timer::start() {
	_start = steady_clock::now();
}

void C11Timer::stop() {
	_duration = steady_clock::now() - _start;
	/*
	_duration = duration_cast<duration<double>> >(
		steady_clock::now() - _start
	);
	*/
	_totalDuration += _duration;
	++_nCycles;
}

Double C11Timer::totalDuration() const {
	return _totalDuration.count();
}

}


