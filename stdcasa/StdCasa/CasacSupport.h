#include <string>
#include <vector>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MRadialVelocity.h>
#include <measures/Measures/MEpoch.h>
#include <stdcasa/Quantity.h>
#include <stdcasa/record.h>

// Conversions utilities

namespace casacore{

class Record;
class ValueHolder;
}

namespace casa {


casacore::Vector<casacore::String>    toVectorString(const std::vector<std::string> &);
std::vector<std::string> fromVectorString(const casacore::Vector<casacore::String> &);
 casacore::String toCasaString(const casac::variant &);

casacore::Quantity          casaQuantity(const casac::Quantity &);

// Constructs and returns a casacore::Quantity from its input.  It will throw an
// exception if given a malformed string or string vector, but otherwise will
// generate a default casacore::Quantity (0.0) for unhandled variants.
casacore::Quantity          casaQuantity(const casac::variant &);

 casacore::Quantum<casacore::Vector<casacore::Double> > casaQuantumVector(const casac::variant &);

::casac::Quantity casacQuantity(const casacore::Quantity &);

// See note in implementation about possible memory leak.
::casac::record  *fromRecord(const casacore::Record &);
// makes a casacore::Vector of casacore::Quantity out of strings or vector of such or later records
 casacore::Bool toCasaVectorQuantity(const ::casac::variant& theval, 
			   casacore::Vector<casacore::Quantity>& theQuants);

::casac::record* recordFromQuantity(const casacore::Quantity q);
::casac::record* recordFromQuantity(const casacore::Quantum<casacore::Vector<casacore::Double> >& q);

casacore::Record           *toRecord(const casac::record &);
casacore::ValueHolder      *toValueHolder(const casac::variant&);
::casac::variant *fromValueHolder(const casacore::ValueHolder &);
bool             casaMDirection(const ::casac::variant& theVar,
				            casacore::MDirection& theMeas);
bool             ang_as_formatted_str(string& out, const casacore::Quantity& qang,
                                      const std::string& format, const casacore::Int precision);
bool             MDirection2str(const casacore::MDirection& in, std::string& out);
bool             casaMFrequency(const ::casac::variant& theVar,
                                casacore::MFrequency& theMeas);
bool             casaMPosition(const ::casac::variant& theVar,
                               casacore::MPosition& theMeas);
bool             casaMRadialVelocity(const ::casac::variant& theVar,
				            casacore::MRadialVelocity& theMeas);
bool             casaMEpoch(const ::casac::variant& theVar,
                            casacore::MEpoch& theMeas);
 //utility to split a single string to multiple if they are , or empty space 
 //seperated. if commaOnly = true, only seperate on commas, not empty space
 casacore::Int sepCommaEmptyToVectorStrings(casacore::Vector<casacore::String>& retStr, 
				  const std::string& str);

 casacore::Int sepCommaToVectorStrings(casacore::Vector<casacore::String>& retStr,
 				  const std::string& str);

 // allow variant types DOUBLE, DOUBLEVEC, INT, INVEC. An empty string or a BOOLVEC
 // will return a 0 element vector. Any other type will cause an exception to be thrown.
 std::vector<double> toVectorDouble(const ::casac::variant& v, const casacore::String& varName);

// Allow variant types STRING or STRINGVEC. BOOLVEC will return a 0 element vector.
 // all other types will cause an excpetion to be thrown.
 std::vector<string> toVectorString(const ::casac::variant& v, const casacore::String& varName);

 // Allow variant types INT or INTVEC. An empty string or a BOOLVEC
 // will return a 0 element vector. Any other type will cause an exception to be thrown.
 std::vector<int> toVectorInt(const ::casac::variant& v, const casacore::String& varName);

 	 // Allow variant types BOOL or BOOLVEC. An empty string
 	 // will return a 0 element vector. Any other type will cause an exception to be thrown.
 std::vector<bool> toVectorBool(const ::casac::variant& v, const casacore::String& varName);

 	 casacore::Vector<casacore::String> toCasaVectorString(const std::vector<string>& vs);

}
