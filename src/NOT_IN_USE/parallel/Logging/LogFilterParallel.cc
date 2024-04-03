//# LogFilterParallel.h: This file contains the implementation of the LogFilterParallel class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
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

#include <parallel/Logging/LogFilterParallel.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


LogFilterParallel::LogFilterParallel (LogMessage::Priority lowest)
: LogFilter(lowest)
{
	filterOutVector_p.clear();
}


LogFilterParallel::LogFilterParallel (const LogFilterParallel& other)
: LogFilter(other),
  filterOutVector_p(other.filterOutVector_p)
{

}


LogFilterParallel& LogFilterParallel::operator=(const LogFilterParallel& other)
{
	if (this != &other)
	{
		LogFilter::operator =(other);
		filterOutVector_p = other.filterOutVector_p;
	}

	return *this;
}


LogFilterParallel::~LogFilterParallel()
{
	filterOutVector_p.clear();
}


LogFilterParallel* LogFilterParallel::clone() const
{
	return new LogFilterParallel(*this);
}


Bool LogFilterParallel::pass(const LogMessage& message) const
{
	Bool ret = false;
	Bool priorityPass = LogFilter::pass(message);

	if (priorityPass)
	{
		ret = true;
		for(std::vector<String>::const_iterator  it = filterOutVector_p.begin(); it != filterOutVector_p.end(); ++it)
		{
		    if (message.message().contains(*it))
		    {
		    	ret = false;
		    	break;
		    }
		}
	}
	else
	{
		ret = false;
	}

	return ret;
}


void LogFilterParallel::filterOut(const Char *text)
{
	filterOutVector_p.push_back(String(text));
}



} //# NAMESPACE CASA - END
