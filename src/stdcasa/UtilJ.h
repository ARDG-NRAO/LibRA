/*
 * UtilJ.h
 *
 *  Created on: Nov 4, 2010
 *      Author: jjacobs
 */

#ifndef UTILJ_H_
#define UTILJ_H_

// Casa Includes

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Exceptions/Error.h>

// C++ and System Includes

#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <sys/time.h>
#include <sys/resource.h>
// STL Includes
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <set>
#include <vector>

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
///#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

#ifdef __GNUC__
#define DEPRECATED_METHOD(comment) __attribute__ ((deprecated))
#else
///#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED_METHOD(comment)
#endif

#define Assert AssertCc
#define Throw ThrowCc

#define UnusedVariable(x) ((void) x);

namespace casacore{

class String;
}

namespace casa {

namespace utilj {

class AipsErrorTrace : public casacore::AipsError {

public:

    AipsErrorTrace ( const casacore::String &msg, const casacore::String &filename, casacore::uInt lineNumber,
                     Category c = GENERAL);

};

class Strings : public std::vector<casacore::String> {};

//template <typename Element, typename Container>
//bool
//contains (const Element & e, const Container & c)
//{
//	return c.find(e) != c.end();
//}


template <typename Container>
bool
containsKey (const typename Container::key_type & key,
             const Container & container)
{
    return container.find(key) != container.end();
}

template <typename Container>
bool
contains (const typename Container::value_type & e,
          const Container & c)
{
    // For set and map use containsKey; will work for set but
    // use with map requires specifying a pair as the first argument

    return std::find(c.begin(), c.end(), e) != c.end();
}

template <typename F, typename S>
F & first (std::pair<F,S> & pair) { return pair.first;}

template <typename F, typename S>
const F & first (const std::pair<F,S> & pair) { return pair.first;}

template <typename F, typename S>
class FirstFunctor : public std::unary_function<std::pair<F,S>, F>{
public:
    F & operator() (std::pair<F,S> & p) { return p.first; }
    const F & operator() (const std::pair<F,S> & p) { return p.first; }
};

template <typename Container, typename Element>
Container
fillContainer (Element sentinel, ...)
{
    using namespace std;

    Container container;

    va_list vaList;
    va_start (vaList, sentinel);

    Element e = va_arg (vaList, Element);

    insert_iterator<Container> i = inserter (container, container.begin());

    while (e != sentinel){

        * i ++ = e;

        e = va_arg (vaList, Element);
    }

    va_end (vaList);

    return container;
}

template <typename F, typename S>
FirstFunctor<F,S> firstFunctor () { return FirstFunctor<F,S> ();}


//DEPRECATED (casacore::String format (const char * formatString, ...) /* "Use casacore::String::format"*/);
casacore::String formatV (const casacore::String & formatString, va_list vaList);

template<typename T>
T
getEnv (const casacore::String & name, const T & defaultValue)
{
	char * value = getenv (name.c_str());

	if (value == NULL){
		return defaultValue;
	}
	else{
		return T (value);
	}
}

bool
getEnv (const casacore::String & name, const bool & defaultValue);

int
getEnv (const casacore::String & name, const int & defaultValue);


casacore::String getTimestamp ();

bool isEnvDefined (const casacore::String & name);

std::vector<casacore::String> split (const casacore::String & string, const casacore::String & splitter,
                           bool ignoreConsecutiveSplitters = false);

template <typename Itr>
casacore::String
join (Itr begin, Itr end, const casacore::String & delimiter)
{
    casacore::String result;
    Itr i = begin;

    if (i != end){

        result = * i ++;

        for (; i != end; i++){
            result += delimiter + * i;
        }
    }

    return result;
}

template <typename T>
casacore::String
join (const T & strings, const casacore::String & delimiter)
{
    return join (strings.begin(), strings.end(), delimiter);
}

template <typename Itr, typename F>
casacore::String
join (Itr begin, Itr end, F f, const casacore::String & delimiter)
{
    casacore::String result;
    Itr i = begin;

    if (i != end){

        result = f(* i);
        ++ i;

        for (; i != end; i++){
            result += delimiter + f (* i);
        }
    }

    return result;
}

template <typename K, typename V>
std::vector<K>
mapKeys (const std::map<K,V> & aMap)
{
    std::vector<K> result;

    std::transform (aMap.begin(), aMap.end(), back_inserter (result), firstFunctor<K,V>());

    return result;
}

casacore::AipsError repackageAipsError (casacore::AipsError & error,
                                        const casacore::String & message,
                                        const casacore::String & file,
                                        int line, const casacore::String & func);

template <typename F, typename S>
F & second (std::pair<F,S> & pair) { return pair.second;}

template <typename F, typename S>
const F & second (const std::pair<F,S> & pair) { return pair.second;}

template <typename F, typename S>
class SecondFunctor : public std::unary_function<std::pair<F,S>, F>{
public:
    S & operator() (std::pair<F,S> & p) { return p.second; }
};

template <typename F, typename S>
SecondFunctor<F,S> secondFunctor () { return SecondFunctor<F,S> ();}

template <typename K, typename V>
std::vector<V>
mapValues (const std::map<K,V> & aMap)
{
    std::vector<K> result (aMap.size());

    std::transform (aMap.begin(), aMap.end(), back_inserter (result), second<K,V>);

    return result;
}

void printBacktrace (std::ostream & os, const casacore::String & prefix = "");

long round (double d);

void sleepMs (int milliseconds);
void toStdError (const casacore::String & m, const casacore::String & prefix = "*E* ");
void throwIf (bool condition, const casacore::String & message, const casacore::String & file,
              int line, const casacore::String & func = casacore::String());
void throwIfError (int errorCode, const casacore::String & prefix, const casacore::String & file,
                   int line, const casacore::String & func = casacore::String());

template <typename It, typename Obj>
casacore::String
containerToString (It begin, It end, casacore::String (Obj::* func) () const, const casacore::String & delimiter = ",",
                   const casacore::String & wrapper = "")
{
    casacore::String result;
    casacore::String d = "";

    for (It i = begin; i != end; i++){
        result += d + wrapper + ((* i) .* func) () + wrapper;
        d = delimiter;
    }

    return result;
}

class MemoryStatistics {

public:

    MemoryStatistics ();

    void update (); // call to get the latest stats loaded
    double getRssInMB () const; // get resident set size
    int64_t getRssInBytes () const;

    double getVmInMB () const; // get the Virtual memory size
    int64_t getVmInBytes () const;

private:

    double bytesPerMb_p;
    string filename_p;
    int pageSize_p;
    int64_t rssPages_p; // in pages
    int64_t vmPages_p; // in pages
};

class IoStatistics {

public:

    IoStatistics ();

    IoStatistics operator- (const IoStatistics &) const;
    IoStatistics operator+ (const IoStatistics &) const;
    IoStatistics operator/ (const IoStatistics &) const;
    IoStatistics operator* (double factor) const;

    void capture ();

    double getBytesRead () const;
    double getBytesWritten () const;
    double getNReads () const;
    double getNWrites () const;

    casacore::String report (float scale = .001, const casacore::String & scaleTag = casacore::String ("K")) const;

private:

    double nBytesRead_p;
    double nBytesWritten_p;
    double nReads_p;
    double nWrites_p;
    casacore::String statFile_p;
};


// These two classes, Times and DeltaTimes should be moved out of this file and
// into casacore/casa/OS.  In the meantime, an ifdef should keep the apple from
// barfing.

// <summary>

// </summary>

// <use visibility=local>   or   <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <templating arg=T>
//    <li>
//    <li>
// </templating>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class DeltaThreadTimes;

class ThreadTimes {

public:

    ThreadTimes () { * this = getTime();}

    double cpu () const { return cpu_p;}
    void clear () { empty_p = true;}
    bool empty () const { return empty_p;}
    double elapsed () const { return elapsed_p;}

    static ThreadTimes
    getTime (){

        struct timeval tVal;
        gettimeofday (& tVal, NULL);

        double elapsed = tVal.tv_sec + tVal.tv_usec * 1e-6;

        //double cpu = ((double) clock ()) / CLOCKS_PER_SEC; // should be in seconds



#if     defined (RUSAGE_THREAD)
        struct rusage usage;

        int failed = getrusage (RUSAGE_THREAD, & usage);
        assert (! failed);

        double cpu = ! failed ? toSeconds (usage.ru_utime) + toSeconds (usage.ru_stime) : 0;
#else
        double cpu = 0;
#endif

        return ThreadTimes (elapsed, cpu);
    }

    DeltaThreadTimes operator- (const ThreadTimes & tEarlier) const;

    static double
    toSeconds (const struct timeval & t)
    {
        return t.tv_sec + t.tv_usec * 1e-6;
    }

protected:

    bool empty_p;
    double cpu_p;
    double elapsed_p;

    ThreadTimes (double elapsed, double cpu) : cpu_p (cpu), elapsed_p (elapsed) {}
};

// <summary>
// </summary>

// <use visibility=local>   or   <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <templating arg=T>
//    <li>
//    <li>
// </templating>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>
class DeltaThreadTimes : private ThreadTimes {

    friend class ThreadTimes;

public:

    DeltaThreadTimes () : ThreadTimes (0, 0), doStats_p (false), n_p (0) {}
    explicit DeltaThreadTimes (bool doStats) : ThreadTimes (0,0), doStats_p (doStats), n_p (0)
    {
        cpuSsq_p = 0;
        cpuMin_p = 1e20;
        cpuMax_p = -1e20;
        elapsedSsq_p = 0;
        elapsedMin_p = 1e20;
        elapsedMax_p = -1e20;
    }

    DeltaThreadTimes & operator += (const DeltaThreadTimes & other);

    double cpu () const { return ThreadTimes::cpu();}
    double cpuAvg () const { return n_p == 0 ? 0 : cpu() / n_p;}
    double elapsed () const { return ThreadTimes::elapsed();}
    double elapsedAvg () const { return n_p == 0 ? 0 : elapsed() / n_p;}
    casacore::String formatAverage (const casacore::String & floatFormat = "%6.1f",
                          double scale=1000.0,
                          const casacore::String & units = "ms")  const; // to convert to ms
    casacore::String formatStats (const casacore::String & floatFormat = "%6.1f",
                        double scale=1000.0,
                        const casacore::String & units = "ms")  const; // to convert to ms
    int n() const { return n_p;}

protected:

    DeltaThreadTimes (double elapsed, double cpu) : ThreadTimes (elapsed, cpu), n_p (0) {}

private:

    double cpuMin_p;
    double cpuMax_p;
    double cpuSsq_p;
    bool doStats_p;
    double elapsedMin_p;
    double elapsedMax_p;
    double elapsedSsq_p;
    int n_p;
};

// Global Functions

// <linkfrom anchor=unique-string-within-this-file classes="class-1,...,class-n">
//     <here> Global functions </here> for foo and bar.
// </linkfrom>

// A free function is provided that is useful for
// go here...

// <group name=accumulation>


// </group>

/*

Example of using composer and unary.  The composed functors have to be derived from std::unary_function

  int f(int x) { return x*x;}
  int g(int x) { return 2 * x;}
  int h(int x) { return 100 + x;}

  vector<int> a;
  a.push_back(1);
  a.push_back(2);
  a.push_back(3);

  transform (a.begin(), a.end(), std::ostream_iterator<int> (cout, "\n"), compose (unary(f), unary(f)));

  // prints out
  // 4
  // 16
  // 36

  transform (a.begin(), a.end(), std::ostream_iterator<int> (cout, "\n"),
             compose (unary(h), compose (unary(f), unary(f))));

  // prints out
  // 104
  // 116
  // 136

*/

template <typename F, typename G>
class ComposedFunctor : public std::unary_function <typename G::argument_type, typename F::result_type> {

public:

    ComposedFunctor (F f, G g) : f_p (f), g_p (g) {}

    typename F::result_type operator() (typename G::argument_type x) { return f_p ( g_p (x)); }

private:

    F f_p;
    G g_p;
};

template <typename F, typename G>
ComposedFunctor<F, G>
compose (F f, G g)
{
    return ComposedFunctor <F, G> (f, g);
}

template <typename D, typename R>
class UnaryFunctor : public std::unary_function<D,R> {
public:
    typedef R (* F) (D);

    UnaryFunctor (F f) : f_p (f) {}
    R operator() (D x) { return f_p (x); }

private:

    F f_p;
};

template <typename D, typename R>
UnaryFunctor <D, R>
unary (R (*f) (D)) { return UnaryFunctor<D, R> (f);}

class Z {
public:

    string getName () const { return name_p;}

    string name_p;
};



} // end namespace utilj

} // end namespace casa



#endif /* UTILJ_H_ */
