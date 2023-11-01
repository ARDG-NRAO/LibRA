#include <imageanalysis/ImageAnalysis/ImageHistory.h>

#include <stdcasa/variant.h>
#include <iomanip>

//#include <casa/OS/Time.h>

namespace casa {

template<class T> ImageHistory<T>::ImageHistory(
        const SPIIT image
) : _image(image) {
    ThrowIf(! _image, "Image pointer cannot be null");
}

template<class T> void ImageHistory<T>::addHistory(
    const String& origin,
    const String& history
) {
    std::vector<String> x { history };
    //x.push_back(history);
    addHistory(origin, x);
}

template<class T> void ImageHistory<T>::addHistory(
        const LogOrigin& origin,
        const String& history
) {
    std::vector<String> x { history };
    // x.push_back(history);
    addHistory(origin.toString(), x);
}

template<class T> void ImageHistory<T>::addHistory(
    const std::vector<std::pair<LogOrigin, String> >& history
) {
    auto& log = _image->logger();
    //
    // Make sure we can write into the history table if needed
    //
    log.reopenRW();
    auto& sink = log.sink();
    for (const auto& el: history) {
        auto x = el.second;
        x.trim();
        if (! x.empty()) {
            LogMessage msg(el.second, el.first);
            sink.postLocally(msg);
        }
    }
}

template<class T> void ImageHistory<T>::addHistory(
    const String& origin, const std::vector<String>& history
) {
    auto lor = origin.empty()
        ? LogOrigin(getClass(), __func__)
		: LogOrigin(origin);
    auto& log = _image->logger();
    //
    // Make sure we can write into the history table if needed
    //
    log.reopenRW();
    auto& sink = log.sink();
    for(const auto& line: history) {
        auto x = line;
        x.trim();
        if (! x.empty()) {
            LogMessage msg(line, lor);
            sink.postLocally(msg);
        }
    }
}

template<class T> void ImageHistory<T>::addHistory(
    const casacore::String& origin,
    const std::vector<string>& history
) {
    std::vector<casacore::String> x;
    for( casacore::String h: history ) {
        x.push_back(h);
    }
    addHistory(origin, x);
}

template<class T> void ImageHistory<T>::addHistory(
    const casacore::LogOrigin& origin,
    const std::vector<casacore::String>& history
) {
    addHistory(origin.toString(), history);
}

template<class T> void ImageHistory<T>::clear() {
    _image->logger().clear();
}

template <class T>
void ImageHistory<T>::addHistory(
    const std::vector<std::pair<casacore::String, casacore::String>>& history
) {
    for (const auto& line: history) {
        addHistory(line.first, line.second);
    }
}

template<class T> casacore::LogIO& ImageHistory<T>::getLogSink() {
    return _image->logSink();
}

template<class T> std::vector<String> ImageHistory<T>::get(
    Bool list
) const {
    std::vector<String> t;
    const LoggerHolder& logger = _image->logger();
    LogIO log;
    LogMessage msg;
    auto iter = logger.begin();
    for (; iter != logger.end(); ++iter) {
        auto msgString = iter->message();
        if (list) {
            auto lor = iter->location().empty()
                ? LogOrigin(getClass(), __func__)
                : LogOrigin(iter->location());
            msg.origin(lor);
            Double jdn = iter->time()/C::day + C::MJD0;
            Time mytime(jdn);
            msg.messageTime(mytime);
            msg.message(msgString, true);
            log.post(msg);
        }
        t.push_back(msgString);
    }
    return t;
}

template <class T> std::vector<std::pair<casacore::String, casacore::String>> 
ImageHistory<T>::getApplicationHistory(
    const casacore::LogOrigin& origin, const casacore::String& taskname,
    const vector<casacore::String>& paramNames, const vector<casac::variant>& paramValues,
    const casacore::String& imageName
) {
    ThrowIf(
        paramNames.size() != paramValues.size(),
        "paramNames and paramValues must have the same number of elements"
    );
    std::pair<casacore::String, casacore::String> x;
    x.first = origin.fullName();
    x.second = "Ran " + taskname + " on " + imageName;
    std::vector<std::pair<casacore::String, casacore::String>> appHistory;
    appHistory.push_back(x);
    vector<std::pair<casacore::String, casac::variant>> inputs;
    vector<casacore::String>::const_iterator end = paramNames.end();
    auto begin = paramNames.cbegin();
    auto value = paramValues.cbegin();
    //auto end = paramNames.cend();
    casacore::String out = taskname + "(";
    casacore::String quote;
    // auto name = begin;
    for (auto name = begin; name != end; ++name, ++value) {
        if (name != begin) {
            out += ", ";
        }
        quote = value->type() == casac::variant::STRING ? "'" : "";
        out += *name + "=" + quote;
        out += value->toString();
        out += quote;
        /*
        name++;
        value++;
        */
    }
    x.second = out + ")";
    appHistory.push_back(x);
    return appHistory;
}

template<class T> vector<string> ImageHistory<T>::getAsStdStrings(
        casacore::Bool list
) const {
    vector<casacore::String> hist = get(list);
    vector<string> x;
    for( casacore::String s: hist ) {
        x.push_back(s.c_str());
    }
    return x;
}

template<class T> template <class U>  void ImageHistory<T>::append(
    SPCIIU image
) {
    _image->logger().append(image->logger());
}

template<class T> template <class U>  void ImageHistory<T>::append(
    SPIIU image
) {
    append(std::const_pointer_cast<const casacore::ImageInterface<U>>(image));
    //_image->logger().append(image->logger());
}

}
