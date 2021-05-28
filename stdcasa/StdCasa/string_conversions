#ifndef __casac_string_conversions__
#define __casac_string_conversions__

#include <cstdlib>

typedef unsigned int uInt;

bool stringtobool( const std::string &str ) {
    const char *data = str.data();
    while ( *data && isspace(*data) ) ++data;
    if (!strncasecmp(data,"true",4)) return true;
    if (!strncasecmp(data,"false",5)) return false;
    if (isdigit(*data) || *data == '+' || *data == '-') {
	const char *digit = data;
	while (*digit == '+' || *digit == '-') ++digit;
	if (isdigit(*digit)) {
	    while (*digit && (isdigit(*digit))) ++digit;
	    if (*digit == '.') return atof(data) != 0.0 ? true : false;
	    else return atoi(data) != 0 ? true : false;
	} else {
	    return false;
	}
    }
    return false;
}

inline int stringtoint( const std::string &str ) {
    const char *data = str.data();
    return atoi(data);
}

inline unsigned int stringtouInt( const std::string &str ) {
    const char *data = str.data();
    return strtoul(data, NULL, 10);
}

inline long long stringtolong( const std::string &str ) {
    const char *data = str.data();
    return (long long)atol(data);
}


inline double stringtodouble( const std::string &str ) {
    const char *data = str.data();
    return atof(data);
}

#define STRINGTOCOMPLEX_DEFINITION(TYPE,NAME)		\
inline TYPE NAME( const std::string &str ) {		\
    const char *data = str.data();			\
    return TYPE(atof(data),0.0);			\
}

STRINGTOCOMPLEX_DEFINITION(std::complex<double>,stringtocomplex)

#define TOSTRHELPERS(NAME,PTR,NEW)					\
inline std::string PTR bool ## NAME( bool b ) {				\
    return b ? NEW std::string("true") : NEW std::string("false");	\
}									\
inline std::string PTR NAME( bool b ) {					\
    return b ? NEW std::string("true") : NEW std::string("false");	\
}									\
inline std::string PTR int ## NAME( long i ) {			\
    char buff[256];							\
    sprintf( buff, "%ld", i );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR NAME( long i ) {					\
    char buff[256];							\
    sprintf( buff, "%ld", i );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR uInt ## NAME( unsigned long i ) {			\
    char buff[256];							\
    sprintf( buff, "%lu", i );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR NAME( unsigned long i ) {					\
    char buff[256];							\
    sprintf( buff, "%lu", i );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR long ## NAME( long long  l ) {					\
    char buff[256];							\
    sprintf( buff, "%lld", l );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR NAME( long long l ) {					\
    char buff[256];							\
    sprintf( buff, "%lld", l );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR double ## NAME( double d ) {			\
    char buff[256];							\
    sprintf( buff, "%f", d );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR NAME( double d ) {				\
    char buff[256];							\
    sprintf( buff, "%f", d );						\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR complex ## NAME( const std::complex<double> &c ) {\
    char buff[512];							\
    sprintf( buff, "(%f,%f)", c.real(), c.imag() );			\
    return NEW std::string( (const char *) buff );			\
}									\
inline std::string PTR NAME( const std::complex<double> &c ) {		\
    char buff[512];							\
    sprintf( buff, "(%f,%f)", c.real(), c.imag() );			\
    return NEW std::string( (const char *) buff );			\
}

TOSTRHELPERS(tostring,,)
TOSTRHELPERS(tostringptr,*,new)

inline std::string std_stringtostring(std::string s) { return s; }

#endif
