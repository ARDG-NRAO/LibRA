#ifndef IMAGEANALYSIS_IMAGEMASKHANDLER_H
#define IMAGEANALYSIS_IMAGEMASKHANDLER_H

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Containers/Block.h>
#include <imageanalysis/ImageTypedefs.h>

// #include <lattices/Lattices/LatticeExprNode.h>

#include <set>

namespace casacore {
class ImageRegion;
class LatticeExprNode;
template <class T> class  MaskedLattice;
}

namespace casa {

template <class T> class ImageMaskHandler {

    // <summary>
    // Perform various image mask operations
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // </etymology>

    // <synopsis>
    // </synopsis>

public:
	ImageMaskHandler(SPIIT);

    ~ImageMaskHandler();

    // set default mask. Empty string means unset default mask.
    void set(const casacore::String& name);

    // return the default mask name
    casacore::String defaultMask() const;

    // delete specified masks
    void deleteMasks(const std::set<casacore::String>& masks);

    // rename a mask
    void rename(const casacore::String& oldName, const casacore::String& newName);

    // get all mask names
    casacore::Vector<casacore::String> get() const;

    // copy mask
    void copy(const casacore::String& currentName, const casacore::String& newName);

    void calcmask(
    	const casacore::String& mask, casacore::Record& regions,
		const casacore::String& name, casacore::Bool asdefault=true
    );

    // copy the mask from the specified lattice to the image's (default) mask. The mask
    // and the image must have the same shape.
    template <class U> void copy(const casacore::MaskedLattice<U>& mask);

private:
    const SPIIT _image;

    void _calcmask(
    	const casacore::LatticeExprNode& node,
		const casacore::String& name, const casacore::Bool makedefault
    );

    // Make a block of regions from a Record
    static void _makeRegionBlock(
        casacore::PtrBlock<const casacore::ImageRegion*>& regions,
        const casacore::Record& Regions
    );
};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMaskHandler.tcc>
#endif

#endif
