#include <alma/ASDM/EndianStream.h>
#define write5(x) write((const char*) &x, sizeof(x))

using namespace std;

namespace asdm {
	
  EndianOSStream::EndianOSStream():byteOrder_(asdm::ByteOrder::Machine_Endianity)  {;}
  

  EndianOSStream::EndianOSStream(const asdm::ByteOrder* byteOrder):byteOrder_(byteOrder) {;}

  
  EndianOSStream::~EndianOSStream() { ; }

  const asdm::ByteOrder* EndianOSStream::byteOrder() const {
    return this->byteOrder_;
  }

  void EndianOSStream::writeBoolean(bool b) {
    write((const char*)&b,sizeof(bool));	
  }

  void EndianOSStream::writeBool(bool b) {
    EndianOSStream::writeBoolean(b);	
  }

  void EndianOSStream::writeByte(char c) {
    write((const char*)&c,sizeof(c));	
  }

  void EndianOSStream::writeShort(short s) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)
      ByteSwap5(s);
	
    //	write((const char*)&vn,sizeof(vn));
    //	write((const char*)&s,sizeof(s));
    write5(s);
				
  }

  void EndianOSStream::writeUShort(unsigned short s) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)
      ByteSwap5(s);
    write5(s);				
  }


  void EndianOSStream::writeInt(int i) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)
      ByteSwap5(i);
    write5(i);			
  }

  void EndianOSStream::writeUInt(unsigned int ui) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)    
      ByteSwap5(ui);
    write5(ui);		
  }

  void EndianOSStream::writeLongLong(int64_t li) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)    
      ByteSwap5(li);
    write5(li);		
  }

  void EndianOSStream::writeLong(int64_t li) {
    writeLongLong(li);
  }

  void EndianOSStream::writeULongLong(uint64_t li) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)    
      ByteSwap5(li);
    write5(li);		
  }

  void EndianOSStream::writeFloat(float f) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)    
      ByteSwap5(f);
    write5(f);	
  }
 
  void EndianOSStream::writeDouble(double d) {
    if ( byteOrder_ != ByteOrder::Machine_Endianity)    
      ByteSwap5(d);
    write5(d);		
  }

  void EndianOSStream::writeString(const string& s) {
    writeInt(s.length());
    *this  << s;
  }


  EndianISStream::EndianISStream(const string & s):byteOrder_(asdm::ByteOrder::Machine_Endianity) {
    this->str(s);
  }


  EndianISStream::EndianISStream(const string & s, const asdm::ByteOrder* byteOrder):byteOrder_(byteOrder) {
    this->str(s);
  }


  EndianISStream::~EndianISStream() { ; }


  const asdm::ByteOrder*  EndianISStream::byteOrder() const {
    return this->byteOrder_;
  }


  bool EndianISStream::readBoolean() {
    bool result;
    read ((char *) &result, sizeof(bool));
    return result;
  }

  char EndianISStream::readByte() {
    char result;
    read(&result, 1);
    return result;
  }

  short int EndianISStream::readShort() {
    union u { short i; char c[sizeof(short)]; } un;


    read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {	
      union v { short i; char c[sizeof(short)]; } vn;	
      vn.c[1] = un.c[0];
      vn.c[0] = un.c[1];
      return vn.i;
    }
    else
      return un.i;
  }

  unsigned short int EndianISStream::readUShort() {
    union u { unsigned short i; char c[sizeof( unsigned short)]; } un;


    read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {		
      union v { unsigned short i; char c[sizeof( unsigned short)]; } vn;	
      vn.c[1] = un.c[0];
      vn.c[0] = un.c[1];
      return vn.i;
    }
    else
      return un.i;
  }

  int  EndianISStream::readInt() {
    union u { int i; char c[sizeof(int)]; } un;
	
    read((char *) & un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {	
      union v { int i; char c[sizeof(int)]; } vn;	
      vn.c[3] = un.c[0];
      vn.c[2] = un.c[1];
      vn.c[1] = un.c[2];
      vn.c[0] = un.c[3];	
      return vn.i;
    }
    else
      return un.i;
  }

  unsigned int EndianISStream::readUInt() {
    union u { unsigned int i; char c[sizeof(unsigned int)]; } un;
	
    read((char *) & un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {	
      union v { unsigned int i; char c[sizeof(unsigned int)]; } vn;	
      vn.c[3] = un.c[0];
      vn.c[2] = un.c[1];
      vn.c[1] = un.c[2];
      vn.c[0] = un.c[3];	
      return vn.i;
    }
    else
      return un.i;
  }

  int64_t EndianISStream::readLongLong() {
    union u {int64_t i; char c[sizeof(int64_t)]; } un;
	
    read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {
      union v {int64_t i; char c[sizeof(int64_t)]; } vn;
      vn.c[7] = un.c[0];
      vn.c[6] = un.c[1];
      vn.c[5] = un.c[2];
      vn.c[4] = un.c[3];
      vn.c[3] = un.c[4];
      vn.c[2] = un.c[5];
      vn.c[1] = un.c[6];
      vn.c[0] = un.c[7];	
      return vn.i;
    }
    else
      return un.i;
  }

  int64_t EndianISStream::readLong() {
    return readLongLong();
  }

  uint64_t EndianISStream::readULongLong() {
    union u {uint64_t i; char c[sizeof(uint64_t)]; } un;
	
    read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {		
      union v {uint64_t i; char c[sizeof(uint64_t)]; } vn;
      vn.c[7] = un.c[0];
      vn.c[6] = un.c[1];
      vn.c[5] = un.c[2];
      vn.c[4] = un.c[3];
      vn.c[3] = un.c[4];
      vn.c[2] = un.c[5];
      vn.c[1] = un.c[6];
      vn.c[0] = un.c[7];	
      return vn.i;
    }
    else
      return un.i;
  }

  float EndianISStream::readFloat() {
    union u {float  f; char c[sizeof(float)]; } un;
	
    read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {		
      union v {float f;  char c[sizeof(float)]; } vn;	
      vn.c[3] = un.c[0];
      vn.c[2] = un.c[1];
      vn.c[1] = un.c[2];
      vn.c[0] = un.c[3];
      return vn.f;
    }
    else
      return un.f;

  }

  double EndianISStream::readDouble() {
    union u {double  d; char c[sizeof(double)]; } un;
	
    read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {
      union v {double d; char c[sizeof(double)]; } vn;			
      vn.c[7] = un.c[0];
      vn.c[6] = un.c[1];
      vn.c[5] = un.c[2];
      vn.c[4] = un.c[3];
      vn.c[3] = un.c[4];
      vn.c[2] = un.c[5];
      vn.c[1] = un.c[6];
      vn.c[0] = un.c[7];	
      return vn.d;	
    }
    else
      return un.d;
	

  }

  string EndianISStream::readString() {
    int len = readInt();
    char c[len];
    read(c, len);
    return string(c, len);
  }

  EndianIFStream::EndianIFStream():ifs_p(NULL) {;}
  EndianIFStream::EndianIFStream(ifstream*  ifs_p):ifs_p(ifs_p) {;}
  EndianIFStream::EndianIFStream(ifstream*  ifs_p, const asdm::ByteOrder* byteOrder ):ifs_p(ifs_p), byteOrder_(byteOrder) {;}
  EndianIFStream::~EndianIFStream() {;}

  const asdm::ByteOrder*  EndianIFStream::byteOrder() const {
    return this->byteOrder_;
  }

  bool EndianIFStream::readBoolean() {
    bool result;
    ifs_p->read ((char *) &result, sizeof(bool));
    return result;
  }

  char EndianIFStream::readByte() {
    char result;
    ifs_p->read(&result, 1);
    return result;
  }

  short int EndianIFStream::readShort() {
    union u { short i; char c[sizeof(short)]; } un;


    ifs_p->read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {	
      union v { short i; char c[sizeof(short)]; } vn;	
      vn.c[1] = un.c[0];
      vn.c[0] = un.c[1];
      return vn.i;
    }
    else
      return un.i;
  }

  unsigned short int EndianIFStream::readUShort() {
    union u { unsigned short i; char c[sizeof( unsigned short)]; } un;


    ifs_p->read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {		
      union v { unsigned short i; char c[sizeof( unsigned short)]; } vn;	
      vn.c[1] = un.c[0];
      vn.c[0] = un.c[1];
      return vn.i;
    }
    else
      return un.i;
  }

  int  EndianIFStream::readInt() {
    union u { int i; char c[sizeof(int)]; } un;
	
    ifs_p->read((char *) & un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {	
      union v { int i; char c[sizeof(int)]; } vn;	
      vn.c[3] = un.c[0];
      vn.c[2] = un.c[1];
      vn.c[1] = un.c[2];
      vn.c[0] = un.c[3];	
      return vn.i;
    }
    else
      return un.i;
  }

  unsigned int EndianIFStream::readUInt() {
    union u { unsigned int i; char c[sizeof(unsigned int)]; } un;
	
    ifs_p->read((char *) & un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {	
      union v { unsigned int i; char c[sizeof(unsigned int)]; } vn;	
      vn.c[3] = un.c[0];
      vn.c[2] = un.c[1];
      vn.c[1] = un.c[2];
      vn.c[0] = un.c[3];	
      return vn.i;
    }
    else
      return un.i;
  }

  int64_t EndianIFStream::readLongLong() {
    union u {int64_t i; char c[sizeof(int64_t)]; } un;
	
    ifs_p->read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {
      union v {int64_t i; char c[sizeof(int64_t)]; } vn;
      vn.c[7] = un.c[0];
      vn.c[6] = un.c[1];
      vn.c[5] = un.c[2];
      vn.c[4] = un.c[3];
      vn.c[3] = un.c[4];
      vn.c[2] = un.c[5];
      vn.c[1] = un.c[6];
      vn.c[0] = un.c[7];	
      return vn.i;
    }
    else
      return un.i;
  }

  int64_t EndianIFStream::readLong() {
    return readLongLong();
  }

  uint64_t EndianIFStream::readULongLong() {
    union u {uint64_t i; char c[sizeof(uint64_t)]; } un;
	
    ifs_p->read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {		
      union v {uint64_t i; char c[sizeof(uint64_t)]; } vn;
      vn.c[7] = un.c[0];
      vn.c[6] = un.c[1];
      vn.c[5] = un.c[2];
      vn.c[4] = un.c[3];
      vn.c[3] = un.c[4];
      vn.c[2] = un.c[5];
      vn.c[1] = un.c[6];
      vn.c[0] = un.c[7];	
      return vn.i;
    }
    else
      return un.i;
  }

  float EndianIFStream::readFloat() {
    union u {float  f; char c[sizeof(float)]; } un;
	
    ifs_p->read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {		
      union v {float f;  char c[sizeof(float)]; } vn;	
      vn.c[3] = un.c[0];
      vn.c[2] = un.c[1];
      vn.c[1] = un.c[2];
      vn.c[0] = un.c[3];
      return vn.f;
    }
    else
      return un.f;

  }

  double EndianIFStream::readDouble() {
    union u {double  d; char c[sizeof(double)]; } un;
	
    ifs_p->read((char *) &un, sizeof(un));

    if ( byteOrder_ != ByteOrder::Machine_Endianity) {
      union v {double d; char c[sizeof(double)]; } vn;			
      vn.c[7] = un.c[0];
      vn.c[6] = un.c[1];
      vn.c[5] = un.c[2];
      vn.c[4] = un.c[3];
      vn.c[3] = un.c[4];
      vn.c[2] = un.c[5];
      vn.c[1] = un.c[6];
      vn.c[0] = un.c[7];	
      return vn.d;	
    }
    else
      return un.d;
	

  }

  string EndianIFStream::readString() {
    int len = readInt();
    char c[len];
    ifs_p->read(c, len);
    return string(c, len);
  }

  BinaryAttributeReaderFunctor::~BinaryAttributeReaderFunctor() {}

} // end namespace asdm


