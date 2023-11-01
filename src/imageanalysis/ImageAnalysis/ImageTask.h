#ifndef IMAGEANALYSIS_IMAGETASK_H
#define IMAGEANALYSIS_IMAGETASK_H

#include <casacore/casa/Containers/Record.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <imageanalysis/ImageTypedefs.h>
#include <imageanalysis/IO/OutputDestinationChecker.h>
#include <imageanalysis/Regions/CasacRegionManager.h>

#include <memory>

#include <components/ComponentModels/C11Timer.h>

namespace casacore{

template <class T> class ArrayLattice;
}

namespace casac {
class variant;
}

namespace casa {

class LogFile;

template <class T> class ImageTask {

    // <summary>
    // Virtual base class for image analysis tasks.
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Image tasking
    // </etymology>

    // <synopsis>
    // Virtual base class for image tasking.
    // </synopsis>

public:

    // verbosity levels
    enum Verbosity {
        QUIET,
        WHISPER,
        LOW,
        NORMAL,
        HIGH,
        NOISY,
        DEAFENING
    };

    virtual ~ImageTask();

    virtual casacore::String getClass() const = 0;

    inline void setStretch(const casacore::Bool stretch) { _stretch = stretch;}

    // tacitly does nothing if <src>lf</src> is the empty string.
    void setLogfile(const casacore::String& lf);

    void setLogfileAppend(const casacore::Bool a);

    void setRegion(const casacore::Record& region);

    void setMask(const casacore::String& mask) { _mask = mask; }

    void setVerbosity(Verbosity verbosity) { _verbosity = verbosity; }

    // <group>
    // These messages will appear in the product image history upon the call to
    // _prepareOutputImage(). They will be located immediately after the input
    // image's copied history. The first value in the pair is the log origin.
    // The second is the associated message. If this method is called more than once
    // on the same object, messages from subsequent calls are appended to the
    // end of messages set in prior calls.
    void addHistory(const std::vector<std::pair<casacore::String, casacore::String> >& msgs) const;

    void addHistory(const casacore::LogOrigin& origin, const casacore::String& msg) const;

    void addHistory(const casacore::LogOrigin& origin, const std::vector<casacore::String>& msgs) const;

    // This adds standard history messages regarding the task that was run and
    // input parameters used. The vectors must have the same length
    void addHistory(
        const casacore::LogOrigin& origin, const casacore::String& taskname,
        const std::vector<casacore::String>& paramNames, const std::vector<casac::variant>& paramValues
    ) const;
    // </group>

    // suppress writing the history on _prepareOutputImage() call. Useful for
    // not writing history to intermediate image products.
    void suppressHistoryWriting(casacore::Bool b) { _suppressHistory = b; }

    // get the history associated with the task. Does not include the
    // history of the input image.
    std::vector<std::pair<casacore::String, casacore::String> > getHistory() {return _newHistory;}

    void setDropDegen(casacore::Bool d) { _dropDegen = d; }

protected:

    // if <src>outname</src> is empty, no image will be written
    // if <src>overwrite</src> is True, if image already exists it will be removed
    // if <src>overwrite</src> is False, if image already exists exception will be thrown
    ImageTask(
        const SPCIIT image,
        const casacore::String& region, const casacore::Record *const &regionPtr,
        const casacore::String& box, const casacore::String& chanInp,
        const casacore::String& stokes, const casacore::String& maskInp,
        const casacore::String& outname, casacore::Bool overwrite
    );

    ImageTask(
        const SPCIIT image, const casacore::Record *const &regionPtr,
        const casacore::String& mask,
        const casacore::String& outname, casacore::Bool overwrite
    );

    virtual CasacRegionManager::StokesControl _getStokesControl() const = 0;

    virtual std::vector<OutputDestinationChecker::OutputStruct> _getOutputStruct();

    // does the lion's share of constructing the object, ie checks validity of
    // inputs, etc.

    virtual void _construct(casacore::Bool verbose=true);

    inline const SPCIIT _getImage() const {return _image;}

    inline const casacore::String& _getMask() const {return _mask;}

    inline const casacore::Record* _getRegion() const {return &_regionRecord;}

    inline void _setStokes(const casacore::String& stokes) { _stokesString = stokes; }

    inline const casacore::String& _getStokes() const {return _stokesString;}

    inline const casacore::String& _getChans() const {return _chan;}

    inline const casacore::String& _getOutname() const {return _outname; }

    // Represents the minimum set of coordinates necessary for the
    // task to function.
    virtual std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const = 0;

    // Is the attached image a position-velocity (PV) image?
    casacore::Bool _isPVImage() const;

    void _removeExistingOutfileIfNecessary() const;

    // if warnOnly is true, log a warning message if file exists and
    // overwrite is true, else throw an exception.
    void _removeExistingFileIfNecessary(
        const casacore::String& filename, const casacore::Bool overwrite, casacore::Bool warnOnly=false
    ) const;

    casacore::String _summaryHeader() const;

    inline const std::shared_ptr<casacore::LogIO> _getLog() const {return _log;}

    // by default, derived classes are configured to have no log file
    // support.
    virtual casacore::Bool _hasLogfileSupport() const {return false;}

    inline casacore::Bool _getStretch() const {return _stretch;}

    Bool _getSuppressHistory() const { return _suppressHistory; }

    const std::shared_ptr<LogFile> _getLogFile() const;

    casacore::Bool _writeLogfile(
        const casacore::String& output, const casacore::Bool open=true,
        const casacore::Bool close=true
    );

    casacore::Bool _openLogfile();

    void _closeLogfile() const;

    virtual inline casacore::Bool _supportsMultipleRegions() const {return false;}

    // does this task support images with multiple beams? false means it never does.
    // true means it does, but not necessarily in all cases (in which case, the
    // derived class is responsible for checking for and throwing exceptions in those cases).
    virtual inline casacore::Bool _supportsMultipleBeams() const {return true;}

    // If outname != NULL, use the value supplied. If is NULL, use the value of _outname.
    // Create a casacore::TempImage or casacore::PagedImage depending if outname/_outname is empty or not. Generally meant
    // for the image to be returned to the UI or the final image product that the user will want.
    // values=0 => the pixel values from the image will be used
    // mask=0 => the mask attached to the image, if any will be used, outShape=0 => use image shape, coordsys=0 => use image coordinate
    // system. overwrite is only used if outname != NULL.

    SPIIT _prepareOutputImage(
        const casacore::ImageInterface<T>& image, const casacore::Array<T> *const values,
        const casacore::ArrayLattice<casacore::Bool> *const mask=nullptr,
        const casacore::IPosition *const outShape=nullptr, const casacore::CoordinateSystem *const coordsys=nullptr,
        const casacore::String *const outname=nullptr, casacore::Bool overwrite=false, casacore::Bool dropDegen=false
    ) const;

    SPIIT _prepareOutputImage(
        const casacore::ImageInterface<T>& image, casacore::Bool dropDegen=false
    ) const;

    // if warnOnly is true, only log a warning message if the file exists and
    // overwrite is true, else throw an excepction
    SPIIT _prepareOutputImage(
        const casacore::ImageInterface<T>& image, const casacore::String& outname,
        casacore::Bool overwrite, casacore::Bool warnOnly
    ) const;

    // data are copied to the output image from the <src>data</src>
    // lattice. The mask is copied from the input image.
    SPIIT _prepareOutputImage(
        const casacore::ImageInterface<T>& image, const casacore::Lattice<T>& data
    ) const;

    Verbosity _getVerbosity() const { return _verbosity; }

    casacore::Bool _getOverwrite() const { return _overwrite; }

    virtual casacore::Bool _mustHaveSquareDirectionPixels() const {return false;}

    casacore::Bool _getDropDegen() const { return _dropDegen; }

    static void _copyMask(casacore::Lattice<casacore::Bool>& mask, const casacore::ImageInterface<T>& image);

    static void _copyData(casacore::Lattice<T>& data, const casacore::Lattice<T>& image);

    template <class U> void _doHistory(std::shared_ptr<casacore::ImageInterface<U>>& image) const;

    void _reportOldNewImageShapes(const ImageInterface<T>& out) const;
    
    void _reportOldNewImageShapes(const IPosition& outShape) const;

private:
    const SPCIIT _image;
    mutable std::shared_ptr<casacore::LogIO> _log = std::shared_ptr<casacore::LogIO>(new casacore::LogIO());
    const casacore::Record *const _regionPtr;
    casacore::Record _regionRecord;
    casacore::String _region = "";
    casacore::String _box = "";
    casacore::String _chan = "";
    casacore::String _stokesString = "";
    casacore::String _mask = "";
    casacore::String _outname = "";
    casacore::Bool _overwrite = false;
    casacore::Bool _stretch = false;
    casacore::Bool _logfileAppend = false;
    casacore::Bool _suppressHistory = false;
    casacore::Bool _dropDegen = false;
    std::unique_ptr<casacore::FiledesIO> _logFileIO;
    Verbosity _verbosity = NORMAL;
    std::shared_ptr<LogFile> _logfile;
    mutable std::vector<std::pair<casacore::String, casacore::String> > _newHistory;

    mutable C11Timer _timer;
};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageTask.tcc>
#endif

#endif
