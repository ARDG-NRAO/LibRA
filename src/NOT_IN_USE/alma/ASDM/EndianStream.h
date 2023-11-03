#ifndef EndianStream_CLASS
#define EndianStream_CLASS

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <alma/ASDM/Misc.h>

namespace asdm {
	
  class EndianOSStream : public std::ostringstream {
  protected :
    //ostringstream _oss(stringstream::out | stringstream::binary);
			
  public :
    EndianOSStream() ;
    EndianOSStream(const asdm::ByteOrder* byteOrder);
    ~EndianOSStream();
   
    const asdm::ByteOrder* byteOrder() const;
		
    void writeBoolean(bool b);
    void writeBool(bool b);
    void writeByte(char c);
    void writeShort(short int s);
    void writeUShort(unsigned short int s);
    void writeInt(int i);
    void writeUInt(unsigned int ui);
    void writeLongLong(int64_t li);
    void writeLong(int64_t li);
    void writeULongLong(uint64_t li);
    void writeFloat(float f);
    void writeDouble(double d);	
    void writeString(const std::string& s);

  private:
    const asdm::ByteOrder* byteOrder_;
			
  };	
	
  class EndianIStream {
  public:
    virtual const asdm::ByteOrder* byteOrder() const = 0;
    virtual bool readBoolean() = 0;
    virtual char readByte() = 0;
    virtual short int readShort() = 0;
    virtual unsigned short int readUShort() = 0;
    virtual int readInt() = 0 ;
    virtual unsigned int readUInt() = 0 ;
    virtual int64_t readLongLong() = 0 ;
    virtual int64_t readLong() = 0 ;
    virtual uint64_t readULongLong() = 0 ;
    virtual float readFloat() = 0 ;
    virtual double readDouble() = 0 ;
    virtual std::string readString() = 0 ;
    virtual ~EndianIStream() { ; }
  };

  class EndianISStream : public std::istringstream, EndianIStream {			
  public :
    EndianISStream(const std::string& s) ;
    EndianISStream(const std::string& s, const asdm::ByteOrder* byteOrder);
    ~EndianISStream();
    
    const asdm::ByteOrder* byteOrder() const ;
    bool readBoolean();
    char readByte();
    short int readShort();
    unsigned short int readUShort();
    int readInt();
    unsigned int readUInt();
    int64_t readLongLong();
    int64_t readLong();
    uint64_t readULongLong();
    float readFloat();
    double readDouble();
    std::string readString();
    
  private :
    const asdm::ByteOrder* byteOrder_;
  };

  class EndianIFStream : EndianIStream  {			
  public :
    EndianIFStream();
    EndianIFStream(std::ifstream*  ifs_p) ;
    EndianIFStream(std::ifstream* ifs_p, const asdm::ByteOrder* byteOrder);
    ~EndianIFStream();
    
    const asdm::ByteOrder* byteOrder() const ;
    bool readBoolean();
    char readByte();
    short int readShort();
    unsigned short int readUShort();
    int readInt();
    unsigned int readUInt();
    int64_t readLongLong();
    int64_t readLong();
    uint64_t readULongLong();
    float readFloat();
    double readDouble();
    std::string readString();
    
  private :
    std::ifstream* ifs_p;
    const asdm::ByteOrder* byteOrder_;
  };

  /**
   * A pure virtual class whose derived classes are expected to be functors
   * whose behaviours will be to read the differents types of data stored into an EndianIStream.
   *
   */
  class BinaryAttributeReaderFunctor {
  public :
    virtual void operator()(EndianIStream& eis) = 0;
    virtual ~BinaryAttributeReaderFunctor() = 0;
  };

}; // end namespace asdm

#endif  // EndianStream_CLASS
