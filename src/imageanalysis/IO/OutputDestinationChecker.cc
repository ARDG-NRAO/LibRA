
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

#include <imageanalysis/IO/OutputDestinationChecker.h>

#include <casacore/casa/OS/File.h>


using namespace casacore;
namespace casa {

OutputDestinationChecker::OutputDestinationChecker() {}

OutputDestinationChecker::~OutputDestinationChecker() {}

void OutputDestinationChecker::checkOutput(OutputStruct& output, LogIO& log) {
	String label = output.label;
	String name = *(output.outputFile);
	Bool required = output.required;
	Bool replaceable = output.replaceable;
	if (name.empty()) {
		ThrowIf(
			required, label + " cannot be blank"
		);
		return;
	}
	LogIO::Command logLevel = required ? LogIO::SEVERE : LogIO::WARN;
	LogIO::Command logAction = required ? LogIO::EXCEPTION : LogIO::POST;
	File f(name);
	switch (f.getWriteStatus()) {
	case File::NOT_CREATABLE:
		log << logLevel << "Requested " << label << " " << name
			<< " cannot be created so will not be written" << logAction;
		*(output.outputFile) = "";
		break;
	case File::NOT_OVERWRITABLE:
		log << logLevel << "There is already a file or directory named "
			<< name << " which cannot be overwritten so the " << label
			<< " will not be written" << logAction;
		*(output.outputFile) = "";
		break;
	case File::OVERWRITABLE:
		if (! replaceable) {
			log << logLevel << "Replaceable flag is false and there is "
				<< "already a file or directory named " << name
				<< " so the " << label << " will not be written"
				<< logAction;
			*(output.outputFile) = "";
		}
		break;
	default:
		return;
	}
}

void OutputDestinationChecker::checkOutputs(
	std::vector<OutputStruct> * const output, LogIO& log
) {
	if (output) {
		for( OutputStruct elem: *output ) {
			checkOutput(elem, log);
		}
	}
}
} 
