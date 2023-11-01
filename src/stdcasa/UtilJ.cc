/*
 * Utilj.cc
 *
 *  Created on: Nov 4, 2010
 *      Author: jjacobs
 */

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <errno.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/aipstype.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/CountedPtr.h>
#include <sys/time.h>
#include <execinfo.h>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

using std::max;
using std::min;

using namespace casacore;
using namespace std;

#include "UtilJ.h"

using namespace casacore;
namespace casa {

namespace utilj {

// Leave a tag to allow manual verification of build setting

#if defined (CASA_THREAD_NEUTRAL)
      String CasaThreadNeutral = "CasaThreadNeutral:YES";
#else
      String CasaThreadNeutral = "CasaThreadNeutral:NO";
#endif // defined (CASA_THREAD_NEUTRAL)

String
formatV (const String & formatString, va_list vaList)
{
	char buffer [4096];
	int nPrinted = vsnprintf (buffer, sizeof (buffer), formatString.c_str(), vaList);

	if (nPrinted >= (int) sizeof (buffer) - 1){
		buffer [sizeof (buffer) - 2] = '|'; // mark as truncated
	}

	return buffer;
}

/*String
format (const char * formatString, ...)
{

	// Return a String object created using the printf-like format string
	// and the provided arguments.  If the text is truncated because the
	// internal buffer is too small, the last character will be a pipe "|".

	va_list vaList;

	va_start (vaList, formatString);

	String result = formatV (formatString, vaList);

	va_end (vaList);

	return result;
}*/


Bool
getEnv (const String & name, const Bool & defaultValue)
{
	char * value = getenv (name.c_str());

	if (value == NULL){
		return defaultValue;
	}
	else{
		String stringValue = value;
		stringValue.downcase();
		Bool truthValue = true;

		if (stringValue == "false" ||
			stringValue == "f" ||
			stringValue == "0" ||
			stringValue == "no"){

			truthValue = false;
		}

		return truthValue;
	}
}

Int
getEnv (const String & name, const Int & defaultValue)
{
	char * value = getenv (name.c_str());

	if (value == NULL){
		return defaultValue;
	}
	else{

		char * next;
		long longValue = strtol (value, & next, 10);

		// If all of the value characters weren't consumed, assume
		// an error occurred and use the default value.

		if (* next != '\0')
			longValue = defaultValue;

		return longValue;
	}
}


String
getTimestamp ()
{
	// Get a possibly decent resolution time value

	struct timeval now;
	gettimeofday (& now, NULL);

	// Convert from UTC to local time

	struct tm localNow = * localtime (& now.tv_sec);

	// Output the seconds portion of the time in the format
	// hh:mm:ss

	char buffer [128];
	strftime (buffer, sizeof(buffer), "%X", & localNow);

	// Add on the higher resolution portion (if any) of the time
	// as milliseconds

	char buffer2 [128];

	snprintf (buffer2, sizeof(buffer2), "%s.%03d", buffer, (int) now.tv_usec/1000);

	// Return the final result in the format "hh:mm:ss.ttt"

	return buffer2;
}

Bool
isEnvDefined (const String & name)
{
	char * value = getenv (name.c_str());

	return value != NULL;
}

void
printBacktrace (ostream & os, const String & prefix)
{
    void * stack [512];
    int nUsed = backtrace (stack, 512);
    char ** trace = backtrace_symbols (stack, nUsed);
    if (! prefix.empty()){
        os << prefix << endl;
    }
    os << "*** Stack trace (use c++filt to demangle):" << endl;
    for (int i = 0; i < nUsed; i++){
        os << trace[i] << endl;
    }
    os.flush();
    delete trace;
}

AipsError
repackageAipsError (AipsError & error, const String & message, const String & file, Int line, const String & func)
{
    ostringstream os;

    AipsError tmp (message, file, line);
    os << func << ": " << tmp.what() << "\n   " << error.what();

    return AipsError (os.str());
}

long
round (Double d)
{
    Double sign = (d < 0) ? -1.0 : 1.0;

    d += 0.5 * sign;

    long result = (long) d;

    return result;
}


void
sleepMs (Int milliseconds)
{
    struct timespec t, tRemaining;
    t.tv_sec = milliseconds / 1000;
    t.tv_nsec = (milliseconds - t.tv_sec) * 1000000;

    // Because nanosleep can be interrupted by a signal, it is necessary
    // to continue the wait if errno is EINTR.  When interrupted, nanosleep
    // copies the amount of time remaining in the wait into tRemaining; so
    // the remainder of one interation becomes the wait value proper on the
    // next iteration.

    Bool done;
    do {
        done = nanosleep (& t, & tRemaining) == 0 || errno != EINTR;
        t = tRemaining;
    } while (! done);

}

vector<String>
split (const String & string, const String & splitter, Bool ignoreConsecutiveSplitters)
{
    vector<String> result;

    Int start = 0;

    while (true){

        Int matchStart = string.find (splitter, start);

        if (matchStart == (int) String::npos){

            // No match: put rest of string into the result

            String text = string.substr (start);

            if (! text.empty()){

                result.push_back (string.substr (start));
            }

            break;
        }

        String text = string.substr (start, matchStart - start);

        if (! (text.empty() &&  ignoreConsecutiveSplitters)){

            // If the text is nonempty or we're not ignored consecutive
            // occurrences of splitters, then push text onto the result.

            result.push_back (text);
        }

        start = matchStart + splitter.length();
    }

    return result;
}

void
toStdError (const String & m, const String & prefix)
{
    cerr << prefix << m << endl;
    cerr.flush();
}


void
throwIf (bool condition, const String & message, const String & file, Int line, const String & func)
{

	// If the condition is met then throw an AipsError

	if (condition) {
	    String m = func + ": " + message;
	    AipsErrorTrace e (m.c_str(), file.c_str(), line);

#       if defined (NDEBUG)
	        toStdError (e.what());
#       endif

	    throw e;
	}
}

void
throwIfError (int errorCode, const String & prefix, const String & file, Int line, const String & func)
{
	// If the provided error code is not equal to success (0) then
	// throw an AipsError using the provided suffix and then details
	// of the error.

	if (errorCode != 0) {
		AipsErrorTrace e (String::format ("%s: %s (errno=%d):%s", func.c_str(), prefix.c_str(),
		                                  errorCode, strerror (errorCode)), file.c_str(), line);

#       if defined (NDEBUG)
	        toStdError (e.what());
#       endif

	    throw e;
	}
}

DeltaThreadTimes
ThreadTimes::operator- (const ThreadTimes & tEarlier) const
{
    return DeltaThreadTimes (this->elapsed() - tEarlier.elapsed(),
                             this->cpu() - tEarlier.cpu());
}

DeltaThreadTimes &
DeltaThreadTimes::operator+= (const DeltaThreadTimes & other)
{
    cpu_p += other.cpu();
    elapsed_p += other.elapsed();
    n_p += 1;

    if (doStats_p){
        cpuSsq_p += other.cpu() * other.cpu();
        cpuMin_p = min (cpuMin_p, other.cpu());
        cpuMax_p = max (cpuMax_p, other.cpu());
        elapsedSsq_p += other.elapsed() * other.elapsed();
        elapsedMin_p = min (elapsedMin_p, other.elapsed());
        elapsedMax_p = max (elapsedMax_p, other.elapsed());
    }

    return * this;
}


String
DeltaThreadTimes::formatAverage (const String & floatFormat,
                           Double scale,
                           const String & units) const // to convert to ms
{
    String realFormat = String::format ("(el=%s,cp=%s,%%4.1f%%%%) %s",
                                        floatFormat.c_str(), floatFormat.c_str(), units.c_str());
    Int n = n_p != 0 ? n_p : 1;
    Double c = cpu_p / n * scale;
    Double e = elapsed_p / n * scale;
    Double p = c / e * 100;

    String result = String::format (realFormat.c_str(), e, c, p);

    return result;
}

String
DeltaThreadTimes::formatStats (const String & floatFormat,
                         Double scale,
                         const String & units) const  // to convert to ms
{
    String realFormat = String::format ("(el=%s {%s-%s,%s}, cp=%s {%s-%s,%s}, %%4.1f%%%%) %s",
                                         floatFormat.c_str(),
                                         floatFormat.c_str(),
                                         floatFormat.c_str(),
                                         floatFormat.c_str(),
                                         floatFormat.c_str(),
                                         floatFormat.c_str(),
                                         floatFormat.c_str(),
                                         floatFormat.c_str(),
                                         units.c_str());
    Int n = n_p != 0 ? n_p : 1;
    Double c = cpu_p / n * scale;
    Double cS = sqrt (cpuSsq_p / n_p - c * c);
    Double e = elapsed_p / n * scale;
    Double eS = sqrt (elapsedSsq_p / n_p - e * e);
    Double p = c / e * 100;

    String result = String::format (realFormat.c_str(), e, elapsedMin_p, elapsedMax_p, eS,
                                    c, cpuMin_p, cpuMax_p, cS, p);

    return result;
}

AipsErrorTrace::AipsErrorTrace ( const String &msg, const String &filename, uInt lineNumber,
                                 Category c)
: AipsError (msg, filename, lineNumber, c)
{
    void * stack [512];
    int n = backtrace (stack, 512);
    char ** trace = backtrace_symbols (stack, n);

    message += "\nStack trace (use c++filt to demangle):\n";
    for (int i = 0; i < n; i++){
        message += trace[i] + String ("\n");
    }
    free (trace);
}



MemoryStatistics::MemoryStatistics ()
{
    char buffer [128];

    pid_t pid = getpid ();

    sprintf (buffer, "/proc/%d/statm", pid);

    filename_p = buffer;

    pageSize_p = getpagesize ();

    bytesPerMb_p = 1024 * 1024.0;
}

double
MemoryStatistics::getVmInMB() const
{
    return getVmInBytes () / bytesPerMb_p;
}

int64_t
MemoryStatistics::getVmInBytes() const
{
    return vmPages_p * pageSize_p;
}

double
MemoryStatistics::getRssInMB() const
{
    return getRssInBytes () / bytesPerMb_p;
}

int64_t
MemoryStatistics::getRssInBytes() const
{
    return rssPages_p * pageSize_p;
}


void
MemoryStatistics::update ()
{
    ifstream is (filename_p.c_str());

    is >> vmPages_p >> rssPages_p;

    is.close ();
}

IoStatistics::IoStatistics ()
{
    Int pid = getpid();
    ostringstream os;

    statFile_p = String::format ("/proc/%d/io", pid);

    capture ();
}

IoStatistics
IoStatistics::operator- (const IoStatistics & other) const
{
    IoStatistics result;
    result.nBytesRead_p = nBytesRead_p - other.nBytesRead_p;
    result.nBytesWritten_p = nBytesWritten_p - other.nBytesWritten_p;
    result.nReads_p = nReads_p - other.nReads_p;
    result.nWrites_p = nWrites_p - other.nWrites_p;

    return result;
}

IoStatistics
IoStatistics::operator+ (const IoStatistics & other) const
{
    IoStatistics result;

    result.nBytesRead_p = nBytesRead_p + other.nBytesRead_p;
    result.nBytesWritten_p = nBytesWritten_p + other.nBytesWritten_p;
    result.nReads_p = nReads_p + other.nReads_p;
    result.nWrites_p = nWrites_p + other.nWrites_p;

    return result;
}

IoStatistics
IoStatistics::operator* (Double f) const
{
    IoStatistics result;

    result.nBytesRead_p = nBytesRead_p * f;
    result.nBytesWritten_p = nBytesWritten_p * f;
    result.nReads_p = nReads_p * f;
    result.nWrites_p = nWrites_p * f;

    return result;
}

IoStatistics
IoStatistics::operator/ (const IoStatistics & other) const
{
    IoStatistics result;

    result.nBytesRead_p = nBytesRead_p / other.nBytesRead_p;
    result.nBytesWritten_p = nBytesWritten_p / other.nBytesWritten_p;
    result.nReads_p = nReads_p / other.nReads_p;
    result.nWrites_p = nWrites_p / other.nWrites_p;

    return result;
}


void
IoStatistics::capture ()
{
    ifstream is (statFile_p.c_str());

    ThrowIf (! is.good(), String::format ("Failed to open %s file", statFile_p.c_str()));

    String tag;

    is >> tag;
    ThrowIf (tag != "rchar:",
             String::format ("Expected 'rchar:', got '%s'", tag.c_str()));
    is >> nBytesRead_p;

    is >> tag;
    ThrowIf (tag != "wchar:",
             String::format ("Expected 'wchar:', got '%s'", tag.c_str()));
    is >> nBytesWritten_p;


    is >> tag;
    ThrowIf (tag != "syscr:",
             String::format ("Expected 'syscr:', got '%s'", tag.c_str()));
    is >> nReads_p;


    is >> tag;
    ThrowIf (tag != "syscw:",
             String::format ("Expected 'syscw:', got '%s'", tag.c_str()));
    is >> nWrites_p;

    is.close();
}

Double
IoStatistics::getBytesRead () const
{
    return nBytesRead_p;
}

Double
IoStatistics::getBytesWritten () const
{
    return nBytesWritten_p;
}

Double
IoStatistics::getNReads () const
{
    return nReads_p;
}

Double
IoStatistics::getNWrites () const
{
    return nWrites_p;
}

String
IoStatistics::report (float scale, const String & scaleTag) const
{
    IoStatistics t = (* this) * scale;

    return String::format ("read: %.3f %sB, %.3f %sOps; write: %.3f %sB, %.3f %sOps",
                           t.nBytesRead_p, scaleTag.c_str(), t.nReads_p, scaleTag.c_str(),
                           t.nBytesWritten_p, scaleTag.c_str(), t.nWrites_p, scaleTag.c_str());


}
} // end namespace utilj

using namespace casacore;
} // end namespace casa
