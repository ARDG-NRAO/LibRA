/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * File ArrayTimeInterval.h
 */

#ifndef ArrayTimeInterval_CLASS
#define ArrayTimeInterval_CLASS

#include <stdint.h>
#include <alma/ASDM/LongWrapper.h>
#include <alma/ASDM/ArrayTime.h>
#include <alma/ASDM/Interval.h>

#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
#endif

namespace asdm {
	
  /**
   * The ArrayTimeInterval class aggregates an instance of ArrayTime
   * and an instance of Interval. Instances of ArrayTimeInterval are
   * used in many ASDM tables whose rows contain information relevant
   * during a certain laps of time and starting at a given date.
   */
  class ArrayTimeInterval {
    friend std::ostream & operator << ( std::ostream &, ArrayTimeInterval& );
    friend std::istream & operator >> ( std::istream &, ArrayTimeInterval& );
  private:
    
    ArrayTime start;
    Interval duration;
  
    static bool readStartTimeDurationInBin_;
    static bool readStartTimeDurationInXML_;
  
  public:


    // Constructors
    ArrayTimeInterval();
    ArrayTimeInterval(ArrayTime  start,
		      Interval  duration);
					  
    ArrayTimeInterval(double startInMJD,
		      double durationInDays);
			  
    ArrayTimeInterval(int64_t startInNanoSeconds,
		      int64_t durationInNanoSeconds);
					  
    ArrayTimeInterval(ArrayTime start);
    ArrayTimeInterval(double startInMJD);
    ArrayTimeInterval(int64_t startInNanoSeconds);
	
#ifndef WITHOUT_ACS
    ArrayTimeInterval (asdmIDLTypes::IDLArrayTimeInterval t); 
#endif	
    // Setters
    void setStart(ArrayTime start);
    void setStart(double start);
    void setStart(int64_t start);
	
    void setDuration(Interval duration);
    void setDuration(int64_t nanoSeconds);
    void setDuration(double days);
		
    // Getters	
    ArrayTime getStart() const ;
    ArrayTime getMidPoint() const ;
    double getStartInMJD() const ;
    int64_t getStartInNanoSeconds() const ;
	
    Interval getDuration() const ;
    int64_t getDurationInNanoSeconds() const ;
    double getDurationInDays() const ;
	
    // Checkers
    bool equals(ArrayTimeInterval ati);
    bool overlaps(ArrayTimeInterval ati);
    bool contains(ArrayTimeInterval ati);
    bool contains(ArrayTime at);

    // Operators
    bool operator == (ArrayTimeInterval&);
    bool operator != (ArrayTimeInterval&);
	 	
#ifndef WITHOUT_ACS
    // To IDL conversion
    const asdmIDLTypes::IDLArrayTimeInterval toIDLArrayTimeInterval() const; 
#endif
    /**
     * Defines how the representation of an ArrayTimeInterval found in subsequent reads of
     * a document containing table exported in binary  must be interpreted. The interpretation depends on the value of the argument b :
     * b == true means that it must be interpreted as (startTime, duration)
     * b == false means that it must be interpreted as (midPoint, duration)
     *
     * @param b a boolean value.
     */
    static void readStartTimeDurationInBin(bool b);

    /**
     * Returns a boolean value whose meaning is defined as follows:
     * true <=> the representation of ArrayTimeInterval object found in any binary table will be considered as (startTime, duration).
     * false <=> the representation of ArrayTimeInterval object found in any binary table will be considered as (midPoint, duration).
     *
     */
    static bool readStartTimeDurationInBin();

    /**
     * Defines how the representation of an ArrayTimeInterval found in subsequent reads of
     * a document containing table exported in XML  must be interpreted. The interpretation depends on the value of the argument b :
     * b == true means that it must be interpreted as (startTime, duration)
     * b == false means that it must be interpreted as (midPoint, duration)
     *
     * @param b a boolean value.
     */
    static void readStartTimeDurationInXML(bool b);

    /**
     * Returns a boolean value whose meaning is defined as follows:
     * true <=> the representation of ArrayTimeInterval object found in any binary table will be considered as (startTime, duration).
     * false <=> the representation of ArrayTimeInterval object found in any binary table will be considered as (midPoint, duration).
     *
     */
    static bool readStartTimeDurationInXML();

    /**
     * Write the binary representation of this to a EndianOSStream.
     * It writes successively the midpoint , the duration.
     */		
    void toBin(EndianOSStream& eoss) const ;

    /**
     * Write the binary representation of a vector of ArrayTime to a EndianOSStream.
     * @param arrayTimeInterval the vector of ArrayTime to be written
     * @param eoss the EndianOSStream to be written to
     */
    static void toBin(const std::vector<ArrayTimeInterval>& arrayTimeInterval,  EndianOSStream& eoss);
	
    /**
     * Write the binary representation of a vector of vector of ArrayTimeInterval to a EndianOSStream.
     * @param arrayTimeInterval the vector of vector of ArrayTimeInterval to be written
     * @param eoss the EndianOSStream to be written to
     */	
    static void toBin(const std::vector<std::vector<ArrayTimeInterval> >& arrayTimeInterval,  EndianOSStream& eoss);
	
    /**
     * Write the binary representation of a vector of vector of vector of ArrayTimeInterval to a EndianOSStream.
     * @param arrayTimeInterval the vector of vector of vector of ArrayTimeInterval to be written
     * @param eoss the EndianOSStream to be written to
     */
    static void toBin(const std::vector<std::vector<std::vector<ArrayTimeInterval> > >& arrayTimeInterval,  EndianOSStream& eoss);

    /**
     * Read the binary representation of an ArrayTimeInterval from a EndianIStream
     * and use the read value to set an  ArrayTimeInterval.
     * @param eis the EndianStream to be read
     * @return an ArrayTimeInterval
     */
    static ArrayTimeInterval fromBin(EndianIStream& eis);
	
    /**
     * Read the binary representation of  a vector of  ArrayTimeInterval from an EndianIStream
     * and use the read value to set a vector of  ArrayTimeInterval.
     * @param dis the EndianIStream to be read
     * @return a vector of ArrayTimeInterval
     */	 
    static std::vector<ArrayTimeInterval> from1DBin(EndianIStream & eis);
	 
    /**
     * Read the binary representation of  a vector of vector of ArrayTimeInterval from an EndianIStream
     * and use the read value to set a vector of  vector of ArrayTimeInterval.
     * @param eiis the EndianIStream to be read
     * @return a vector of vector of ArrayTimeInterval
     */	 
    static std::vector<std::vector<ArrayTimeInterval> > from2DBin(EndianIStream & eis);
	 
    /**
     * Read the binary representation of  a vector of vector of vector of ArrayTimeInterval from an EndianIStream
     * and use the read value to set a vector of  vector of vector of ArrayTimeInterval.
     * @param eis the EndianIStream to be read
     * @return a vector of vector of vector of ArrayTimeInterval
     */	 
    static std::vector<std::vector<std::vector<ArrayTimeInterval> > > from3DBin(EndianIStream & eis);

    /**
     * Returns a string representation of this.
     */
    std::string toString() const ;
  };


  // inline constructors

  inline ArrayTimeInterval::ArrayTimeInterval(): start((int64_t)0), duration(0) {}
  inline ArrayTimeInterval::ArrayTimeInterval(ArrayTime start_, Interval duration_) {
    start = start_;
    duration = Interval(std::min(duration_.get(), Long::MAX_VALUE - start.get()));
  }
 
  inline ArrayTimeInterval::ArrayTimeInterval(double startInMJD, double durationInDays) :
    start(startInMJD), 
    duration((int64_t) (ArrayTime::unitsInADay * durationInDays)){}
 
  inline ArrayTimeInterval::ArrayTimeInterval(int64_t startInNanoSeconds,
					      int64_t durationInNanoSeconds){
    start = startInNanoSeconds;
    duration = std::min(durationInNanoSeconds, Long::MAX_VALUE - startInNanoSeconds);
  }				
 
  inline ArrayTimeInterval::ArrayTimeInterval(ArrayTime  start_):
    start(start_) {
    duration = Interval(Long::MAX_VALUE - start.get());
  }
 
  inline ArrayTimeInterval::ArrayTimeInterval(double startInMJD):
    start(startInMJD) {
    this->duration = Interval(Long::MAX_VALUE - start.get());
  }
 
  inline	ArrayTimeInterval::ArrayTimeInterval(int64_t startInNanoSeconds):
    start(startInNanoSeconds) {
    this->duration = Interval(Long::MAX_VALUE - start.get());
  }
 
  // Inline setters
  inline void ArrayTimeInterval::setStart(ArrayTime start) {
    this->start = ArrayTime(start);
  }
 
  inline void ArrayTimeInterval::setStart(double start) {
    this->start = ArrayTime(start);
  }
 
  inline void ArrayTimeInterval::setStart(int64_t start) {
    this->start = ArrayTime(start);
  }
 
  inline void ArrayTimeInterval::setDuration(Interval duration) {
    this->duration = Interval(duration);	
  }
 
 
  inline void ArrayTimeInterval::setDuration(double duration) {
    this->duration = Interval((int64_t) (ArrayTime::unitsInADay * duration));
  }
 
  inline void ArrayTimeInterval::setDuration(int64_t duration) {
    this->duration = Interval(duration);	
  }
 
  // inline getters
  inline ArrayTime ArrayTimeInterval::getStart() const {
    return start;	
  }
 
  inline ArrayTime ArrayTimeInterval::getMidPoint() const {
    return ArrayTime(start.get() + duration.get() / 2);
  }

  inline double ArrayTimeInterval::getStartInMJD() const {
    return start.getMJD();	
  }
 
  inline int64_t ArrayTimeInterval::getStartInNanoSeconds() const  {
    return start.get();	
  }
 
  inline Interval ArrayTimeInterval::getDuration() const {
    return duration;	
  }
 
  inline double ArrayTimeInterval::getDurationInDays() const {
    return (((double) duration.get()) / ArrayTime::unitsInADay);	
  }
 
  inline int64_t ArrayTimeInterval::getDurationInNanoSeconds() const {
    return duration.get();	
  }
 
  // inline checkers
  inline bool ArrayTimeInterval::equals(ArrayTimeInterval ati) {
    return ((start.get() == ati.getStart().get()) && 
	    (duration.get() == ati.getDuration().get()));
  }
 
  inline bool ArrayTimeInterval::overlaps(ArrayTimeInterval ati) {
    int64_t start1 = start.get();
    int64_t end1 = start1 + duration.get();
   
   
    int64_t start2 = ati.getStart().get();
    int64_t end2   = start2 + ati.getDuration().get();
   
    return (start2 <= start1 && end2 >= start1) ||
      (start2 >= start1 && start2 <= end1);
  }
 
  inline bool ArrayTimeInterval::contains(ArrayTimeInterval ati) {
    int64_t start1 = start.get();;
    int64_t end1 = start1 + duration.get();
   
    int64_t start2 = ati.getStart().get();
    int64_t end2   = start2 + ati.getDuration().get();
   
    return (start2>=start1 && end2<=end1);
  }
 
  inline bool ArrayTimeInterval::contains(ArrayTime ati) {
    int64_t start1 = start.get();
    int64_t end1 = start1 + duration.get();
   
    int64_t time = ati.get();
    return (time >= start1 && time < end1);
  }
 
  inline bool ArrayTimeInterval::operator == (ArrayTimeInterval &ati){
    return (start == ati.start) && (duration == ati.duration);
  }
 
  inline bool ArrayTimeInterval::operator != (ArrayTimeInterval &ati){
    return (start != ati.start) || (duration != ati.duration);
  }
 
  inline std::ostream & operator << ( std::ostream &o, ArrayTimeInterval &ati ) {
    o << "(start=" << ati.getStart().get() << ",duration=" << ati.getDuration().get() << ")";
    return o;	
  } 

  inline std::istream & operator >> ( std::istream &i, ArrayTimeInterval &ati) {
    i >> ati.start;
    i >> ati.duration;
    return i;
  }
} // End namespace asdm
#endif // ArrayTimeInterval_CLASS
