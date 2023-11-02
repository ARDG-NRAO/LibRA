//# AsciiRegionFile.cc
//# Copyright (C) 1998,1999,2000,2001
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

#include <imageanalysis/Annotations/RegionTextList.h>

#include <casacore/casa/OS/File.h>
#include <imageanalysis/Annotations/AnnRegion.h>
#include <casacore/images/Regions/WCDifference.h>
#include <casacore/casa/BasicSL/STLIO.h>

using namespace casacore;
namespace casa {

RegionTextList::RegionTextList() {}


RegionTextList::RegionTextList(
    const CoordinateSystem& csys,
    const IPosition shape
) : _csys(csys), _shape(shape), _canGetRegion(true) {}

RegionTextList::RegionTextList(
    const String& filename, const CoordinateSystem& csys,
    const IPosition shape,
    const String& prependRegion, const String& globalOverrideChans, const String& globalOverrrideStokes,
    const Int requireAtLeastThisVersion, Bool verbose, Bool requireImageRegion
) : _csys(csys), _shape(shape), _canGetRegion(true) {
    RegionTextParser parser(
        filename, csys, shape, requireAtLeastThisVersion,
        prependRegion,
        globalOverrideChans, globalOverrrideStokes,
        verbose, requireImageRegion
    );
    const auto lines = parser.getLines();
    for (const auto& line: lines) {
        addLine(line);
    }
}

RegionTextList::RegionTextList(
    const CoordinateSystem& csys, const String& text,
    const IPosition shape, const String& prependRegion,
    const String& globalOverrideChans, const String& globalOverrrideStokes,
    Bool verbose, Bool requireImageRegion
) : _csys(csys), _shape(shape), _canGetRegion(true) {
    RegionTextParser parser(
        csys, shape, text, prependRegion, globalOverrideChans,
        globalOverrrideStokes, verbose, requireImageRegion
    );
    const auto lines = parser.getLines();
    for (const auto& line: lines) {
        addLine(line);
    }
}

RegionTextList::~RegionTextList() {}

void RegionTextList::addLine(const AsciiAnnotationFileLine& line) {
    const auto x = line;
    _lines.resize(_lines.size()+1, true);
    _lines[_lines.size()-1] = x;
    if (x.getType() == AsciiAnnotationFileLine::ANNOTATION && _canGetRegion) {
        const auto annotation = x.getAnnotationBase();
        if (annotation->isRegion()) {
            const auto *region = dynamic_cast<const AnnRegion *>(annotation.get());
            if (! region->isAnnotationOnly()) {
                auto wcregion = region->getRegion2();
                if (region->isDifference() && _regions.size() == 0) {
                    Vector<Double> blc, trc;
                    _csys.toWorld(blc, IPosition(_csys.nPixelAxes(), 0));
                    _csys.toWorld(trc, _shape);
                    Vector<Quantity> qblc(blc.size()), qtrc(trc.size());
                    const auto wUnits = _csys.worldAxisUnits();
                    Vector<Int> absRel(blc.size(), RegionType::Abs);
                    for (uInt i=0; i<qblc.size(); i++) {
                        qblc[i] = Quantity(blc[i], wUnits[i]);
                        qtrc[i] = Quantity(blc[i], wUnits[i]);
                    }
                    _regions.push_back(
                        std::shared_ptr<const WCRegion>(
                            new WCBox(qblc, qtrc, _csys, absRel)
                        )
                    );
                    _union.push_back(true);
                }
                _regions.push_back(wcregion);
                _union.push_back(! region->isDifference());
                _composite = NULL;
            }
        }
    }
}

Record RegionTextList::regionAsRecord() const {
    ThrowIf(_regions.size() == 0, "No regions found");
    return getRegion()->toRecord("");
}

CountedPtr<const WCRegion> RegionTextList::getRegion() const {
    if (_composite) {
        return _composite;
    }
    ThrowIf(
        ! _canGetRegion,
        "Object constructed with too little information "
        "for forming composite region. Use another constructor."
    );
    if (_regions.size() == 0) {
        return nullptr;
    }
    if (_regions.size() == 1) {
        _composite = _regions[0];
        return _composite;
    }
    const auto end = _union.cend();
    const auto foundDifference = std::find(_union.cbegin(), end, false) != end;
    PtrBlock<const WCRegion *> unionRegions;
    if (! foundDifference) {
        // no complementary regions, just union the whole lot
        unionRegions.resize(_regions.size());
        for (uInt i=0; i<_regions.size(); ++i) {
            unionRegions[i] = _regions[i].get();
        }
        _composite.reset(new WCUnion(false, unionRegions));
        return _composite;
    }
    uInt count = 0;
    for (const auto isUnion: _union) {
        if (isUnion) {
            auto newSize = unionRegions.size() + 1;
            unionRegions.resize(newSize);
            unionRegions[newSize - 1] = _regions[count].get();
        }
        else {
            WCUnion myUnion(false, unionRegions);
            const WCDifference *myDiff = new WCDifference(myUnion, *_regions[count]);
            // _ptrMgr is used solely for pointer management, so that the pointers are
            // deleted when this object goes out of scope, because PtrBlocks do no
            // memory management
            _ptrMgr.push_back(std::shared_ptr<const WCDifference>(myDiff));
            unionRegions.resize(1, true);
            unionRegions[0] = myDiff;
        }
        ++count;
    }
    if (unionRegions.size() == 1) {
        _composite = _ptrMgr[_ptrMgr.size() - 1];
    }
    else {
        _composite.reset(new WCUnion(false, unionRegions));
    }
    return _composite;
}

uInt RegionTextList::nLines() const {
    return _lines.size();
}

AsciiAnnotationFileLine RegionTextList::lineAt(
    const uInt i
) const {
    ThrowIf(i >= _lines.size(), "Index out of range");
    return _lines[i];
}

ostream& RegionTextList::print(ostream& os) const {
    const auto vString = String::toString(RegionTextParser::CURRENT_VERSION);
    os << "#CRTFv" + vString
        << " CASA Region Text Format version "
        << vString << endl;
    for (
        auto iter=_lines.cbegin();
        iter != _lines.cend(); iter++
    ) {
        if (
            iter == _lines.cbegin()
            && iter->getType() == AsciiAnnotationFileLine::COMMENT
            && iter->getComment().contains(
                Regex(RegionTextParser::MAGIC.regexp() + "v[0-9]+")
            )
        ) {
            // skip writing header line if it already exists, we write
            // our own here to avoid clashes with previous spec versions
            continue;
        }
        os << *iter << endl;
    }
    return os;
}

}
