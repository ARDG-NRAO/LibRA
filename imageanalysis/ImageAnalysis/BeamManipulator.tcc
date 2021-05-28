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
//# $Id: $

#include <imageanalysis/ImageAnalysis/BeamManipulator.h>

#include <imageanalysis/ImageAnalysis/ImageHistory.h>
#include <imageanalysis/ImageAnalysis/ImageMetaData.h>

namespace casa {

template <class T> BeamManipulator<T>::BeamManipulator(SPIIT image)
	: _image(image), _log() {}

template <class T> void BeamManipulator<T>::remove() {
	casacore::ImageInfo ii = _image->imageInfo();

	ii.removeRestoringBeam();
	ThrowIf(
		! _image->setImageInfo(ii),
		"Failed to remove restoring beam"
	);
	if (_log) {
		*_log << casacore::LogIO::NORMAL << "Deleted restoring beam(s)"
			<< casacore::LogIO::POST;
	}
}

template <class T> void BeamManipulator<T>::rotate(const casacore::Quantity& angle) {
	casacore::ImageInfo ii = _image->imageInfo();
	casacore::ImageBeamSet beams = ii.getBeamSet();
	beams.rotate(angle, True);
	ii.setBeams(beams);
	_image->setImageInfo(ii);
}

template <class T> void BeamManipulator<T>::rotate(
    const casacore::Quantity& angle, const vector<String>& prependMsgs
) {
   	ImageInfo ii = _image->imageInfo();
    ImageBeamSet oBeams = ii.getBeamSet();
    rotate(angle);
    ImageBeamSet beams = _image->imageInfo().getBeamSet();
    vector<String> msgs;
    ostringstream oss;
    auto name = _image->name();
    if (ii.hasSingleBeam()) {
        oss << "Original " << name << " restoring beam: " << oBeams.getBeam();
        msgs.push_back(oss.str());
        oss.str("");
        oss << "New " << name << " restoring beam: " << beams.getBeam();
        msgs.push_back(oss.str());
    }
    else {
        String s = "This image has multiple beams. Reporting the rotation of the minimum and maximum area beams:";
        msgs.push_back(s);
        oss << "Original " << name << " minimum area restoring beam: " << oBeams.getMinAreaBeam();
        msgs.push_back(oss.str());
        oss.str("");
        oss << "New " << name << " minimum area restoring beam: " << beams.getMinAreaBeam();
        msgs.push_back(oss.str());
        oss.str("");
        oss << "Original " << name << " maximum area restoring beam: " << oBeams.getMaxAreaBeam();
        msgs.push_back(oss.str());
        oss.str("");
        oss << "New " << name << " maximum area restoring beam: " << beams.getMaxAreaBeam();
        msgs.push_back(oss.str());
    }
    LogIO log;
    LogOrigin lor("BeamManipulator", __func__);
    for (const auto& m: msgs) {
        log << lor << LogIO::NORMAL << m << LogIO::POST;
    }
    msgs.insert(msgs.begin(), prependMsgs.begin(), prependMsgs.end()); 
    ImageHistory<T> ih(_image);
    ih.addHistory(lor, msgs);
}

template <class T> void BeamManipulator<T>::setVerbose(casacore::Bool v) {
	if (v && ! _log) {
		_log.reset(new casacore::LogIO());
	}
	else {
		_log.reset();
	}
}

template <class T> void BeamManipulator<T>::set(const casacore::ImageBeamSet& beamSet) {
	ThrowIf(
		beamSet.empty(),
		"Input beam set cannot be empty"
	);
	casacore::ImageInfo ii = _image->imageInfo();
	ii.setBeams(beamSet);
	ThrowIf(
		! _image->setImageInfo(ii),
		"Failed to set beams"
	);
	if(_log) {
		*_log << casacore::LogOrigin("BeamManipulator", __func__);
		*_log << casacore::LogIO::NORMAL << "Set image beam set" << casacore::LogIO::POST;
	}
}

template <class T> void BeamManipulator<T>::set(
	const casacore::Quantity& major, const casacore::Quantity& minor,
	const casacore::Quantity& pa, const casacore::Record& rec,
	casacore::Int channel, casacore::Int polarization
) {
	if (_log) {
		*_log << casacore::LogOrigin("BeamManipulator", __func__);
	}
	casacore::ImageInfo ii = _image->imageInfo();
	casacore::Quantity bmajor, bminor, bpa;
	if (rec.nfields() != 0) {
		if (
			rec.isDefined("beams") && rec.isDefined("nChannels")
			&& rec.isDefined("nStokes")
		) {
			ImageMetaData<T> md(_image);
			casacore::uInt nChanIm = md.nChannels();
			casacore::uInt nStokesIm = md.nStokes();
			casacore::uInt nChanBeam = rec.asuInt("nChannels");
			casacore::uInt nStokesBeam = rec.asuInt("nStokes");
			if (nChanIm == nChanBeam && nStokesIm == nStokesBeam) {
				if (ii.hasBeam()) {
					if (_log) {
						*_log << casacore::LogIO::WARN << "Overwriting existing beam(s)" << casacore::LogIO::POST;
					}
					ii.removeRestoringBeam();
				}
			}
		}
		else {
			casacore::String error;
			// instantiating this object will do implicit consistency checks
			// on the passed-in record
			casacore::GaussianBeam beam = casacore::GaussianBeam::fromRecord(rec);

			bmajor = beam.getMajor();
			bminor = beam.getMinor();
			bpa = beam.getPA(true);
		}
	}
	else {
		bmajor = major;
		bminor = minor;
		bpa = pa;
	}
	if (bmajor.getValue() == 0 || bminor.getValue() == 0) {
		casacore::GaussianBeam currentBeam = ii.restoringBeam(channel, polarization);
		if (! currentBeam.isNull()) {
			bmajor = major.getValue() == 0 ? currentBeam.getMajor() : major;
			bminor = minor.getValue() == 0 ? currentBeam.getMinor() : minor;
			bpa = pa.isConform("rad") ? pa : casacore::Quantity(0, "deg");
		}
		else {
			ThrowIf(
				ii.hasMultipleBeams(),
				"This image does not have a corresponding per plane "
				"restoring beam that can be "
				"used to set missing input parameters"
			);
			ThrowCc(
				"This image does not have a restoring beam that can be "
				"used to set missing input parameters"
			);
		}
	}
	if (ii.hasMultipleBeams()) {
		if (channel < 0 && polarization < 0) {
			if (_log) {
				*_log << casacore::LogIO::WARN << "This image has per plane beams"
					<< "but no plane (channel/polarization) was specified. All beams will be set "
					<< "equal to the specified beam." << casacore::LogIO::POST;
			}
			ImageMetaData<T> md(_image);
			ii.setAllBeams(
				md.nChannels(), md.nStokes(),
				casacore::GaussianBeam(bmajor, bminor, bpa)
			);
		}
		else {
			ii.setBeam(channel, polarization, bmajor, bminor, bpa);
		}
	}
	else if (channel >= 0 || polarization >= 0) {
		if (ii.restoringBeam().isNull()) {
			if (_log) {
				*_log << casacore::LogIO::NORMAL << "This image currently has no beams of any kind. "
					<< "Since channel and/or polarization were specified, "
					<< "a set of per plane beams, each equal to the specified beam, "
					<< "will be created." << casacore::LogIO::POST;
			}
			ImageMetaData<T> md(_image);
			ii.setAllBeams(
				md.nChannels(), md.nStokes(),
				casacore::GaussianBeam(bmajor, bminor, bpa)
			);
		}
		else {
			ThrowCc(
				"Channel and/or polarization has "
				"been specified, but this image has a single (global restoring "
				"beam. This beam will not be altered. If you really want to modify "
				"the global beam, rerun setting both channel and "
				"polarization less than zero"
			);
		}
	}
	else {
		if (_log) {
			*_log << casacore::LogIO::NORMAL
				<< "Setting (global) restoring beam." << casacore::LogIO::POST;
		}
		ii.setRestoringBeam(casacore::GaussianBeam(bmajor, bminor, bpa));
	}
	ThrowIf(
		! _image->setImageInfo(ii),
		"Failed to set restoring beam"
	);
	if (_log) {
		*_log << casacore::LogIO::NORMAL << "Beam parameters:"
			<< "  Major          : " << bmajor.getValue() << " " << bmajor.getUnit() << endl
			<< "  Minor          : " << bminor.getValue() << " " << bminor.getUnit() << endl
			<< "  Position Angle : " << bpa.getValue() << " " << bpa.getUnit() << endl
			<< casacore::LogIO::POST;
	}
}

}

