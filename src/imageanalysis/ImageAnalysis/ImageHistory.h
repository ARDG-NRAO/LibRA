#ifndef IMAGEANALYSIS_IMAGEHISTORY_H
#define IMAGEANALYSIS_IMAGEHISTORY_H

#include <imageanalysis/ImageTypedefs.h>

#include <casacore/casa/namespace.h>

namespace casac {

class variant;

}

namespace casa {

template <class T> class ImageHistory {
    // <summary>
    // Top level interface accessing image history data.
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Image History access
    // </etymology>

    // <synopsis>
    // Top level interface for accessing image history.
    // </synopsis>

public:

    ImageHistory() = delete;
    
    ImageHistory(const SPIIT image);

    // destructor
    ~ImageHistory() {}

    // add a line to the history
    void addHistory(
        const casacore::String& origin, const casacore::String& history
    );

    void addHistory(
        const casacore::LogOrigin& origin, const casacore::String& history
    );
    // add multiple history lines, all which have the same origin
    void addHistory(
        const casacore::String& origin,
        const std::vector<casacore::String>& history
    );

    void addHistory(
        const casacore::LogOrigin& origin,
        const std::vector<casacore::String>& history
    );

    void addHistory(
        const casacore::String& origin, const std::vector<string>& history
    );

    void addHistory(
        const std::vector<std::pair<casacore::LogOrigin, casacore::String> >& history
    );

    void addHistory(
        const std::vector<std::pair<casacore::String, casacore::String>>& history
    );

    // erase all messages. Cannot be undone!
    void clear();

    std::vector<casacore::String> get(casacore::Bool list) const;
    std::vector<string> getAsStdStrings(casacore::Bool list) const;

    // return a vector<String> containing a formatted application 
    // summary. Nothing is written to them image that is useful for
    // history writing.
    static std::vector<std::pair<casacore::String, casacore::String>> 
    getApplicationHistory(
        const casacore::LogOrigin& origin, const casacore::String& taskname,
        const vector<casacore::String>& paramNames,
        const vector<casac::variant>& paramValues,
        const casacore::String& imageName
    );

    // <group>
    //Append the specified image's history to this image's history
    template <class U> void append(SPCIIU image);

    template <class U> void append(SPIIU image);
    // </group>

    casacore::String getClass() const { const static casacore::String s = "ImageHistory"; return s; }

    casacore::LogIO& getLogSink();

private:

    const SPIIT _image;

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageHistory.tcc>
#endif

#endif
