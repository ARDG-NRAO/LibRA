#ifndef __casac_variant_h__
#define __casac_variant_h__

#include <string>
#include <vector>
#include <complex>

namespace casac {

class record;

class variant {

    public:

	enum TYPE { RECORD, BOOL, INT, UINT, DOUBLE, COMPLEX, STRING, BOOLVEC, INTVEC, UINTVEC, DOUBLEVEC, COMPLEXVEC, STRINGVEC };

	static TYPE compatible_type( TYPE one, TYPE two );

	class error {
	    public:
		error( std::string msg ) : message_(msg) { }
		const std::string &message( ) const { return message_; }
	    private:
		std::string message_;
	};

	class overflow : public error {
	    public:
		overflow( std::string lbl ) : error(lbl + ": overflow error") { }
	};

        variant *clone() const { return new variant(*this); }
        int compare(const variant*) const;

	variant( );
	variant(const variant &);

	variant(bool arg) : typev(BOOL), shape_(1,1) { val.b = arg;  }
	variant(long arg) : typev(INT), shape_(1,1) { val.i = arg; }
	variant(unsigned long arg) : typev(UINT), shape_(1,1) { val.ui = arg; }
	variant(double arg) : typev(DOUBLE), shape_(1,1) { val.d = arg; }
	variant(std::complex<double> arg) : typev(COMPLEX) { val.c = new std::complex<double>(arg); }
	variant(const char *arg) : typev(STRING), shape_(1,1)
			{ val.s = new std::string(arg); }
	variant(const std::string &arg) : typev(STRING), shape_(1,1)
			{ val.s = new std::string(arg); }
//
	variant(const std::vector<bool> &arg) : typev(BOOLVEC), shape_(1,arg.size())
			{ val.bv = new std::vector<bool>(arg); }
	variant(const std::vector<bool> &arg, const std::vector<ssize_t> &theshape) : typev(BOOLVEC), shape_(theshape)
			{ val.bv = new std::vector<bool>(arg); }
	variant(std::vector<bool> *arg) : typev(BOOLVEC), shape_(1,arg->size())
                        { val.bv = arg; }
	variant(std::vector<bool> *arg, std::vector<ssize_t> &theshape) : typev(BOOLVEC), shape_(theshape)
                        { val.bv = arg; }
//
	variant(const std::vector<long> &arg) : typev(INTVEC), shape_(1,arg.size())
			{ val.iv = new std::vector<long>(arg); }
	variant(const std::vector<long> &arg, const std::vector<ssize_t> &theshape) : typev(INTVEC), shape_(theshape)
			{ val.iv = new std::vector<long>(arg); }
	variant(std::vector<long> *arg) : typev(INTVEC), shape_(1, arg->size())
                        { val.iv = arg; }
	variant(std::vector<long> *arg, std::vector<ssize_t> &theshape) : typev(INTVEC), shape_(theshape)
			{ val.iv = arg; }

        variant(const std::vector<unsigned long> &arg) : typev(UINTVEC), shape_(1,arg.size())
                        { val.uiv = new std::vector<unsigned long>(arg); }
        variant(const std::vector<unsigned long> &arg, const std::vector<ssize_t> &theshape) : typev(UINTVEC), shape_(theshape)
                        { val.uiv = new std::vector<unsigned long>(arg); }
        variant(std::vector<unsigned long> *arg) : typev(UINTVEC), shape_(1, arg->size())
                        { val.uiv = arg; }
        variant(std::vector<unsigned long> *arg, std::vector<ssize_t> &theshape) : typev(UINTVEC), shape_(theshape)
                        { val.uiv = arg; }

//
	variant(const std::vector<double> &arg) : typev(DOUBLEVEC), shape_(1,arg.size())
			{ val.dv = new std::vector<double>(arg); }
	variant(const std::vector<double> &arg, const std::vector<ssize_t> &theshape) : typev(DOUBLEVEC), shape_(theshape)
			{ val.dv = new std::vector<double>(arg); }
	variant(std::vector<double> *arg) : typev(DOUBLEVEC), shape_(1,arg->size())
			{ val.dv = arg; }
	variant(std::vector<double> *arg, std::vector<ssize_t> &theshape) : typev(DOUBLEVEC), shape_(theshape)
			{ val.dv = arg; }

	variant(const std::vector<std::complex<double> > &arg) : typev(COMPLEXVEC), shape_(1, arg.size())
			{ val.cv = new std::vector<std::complex<double> >(arg); }
	variant(const std::vector<std::complex<double> > &arg, const std::vector<ssize_t> &theshape) : typev(COMPLEXVEC), shape_(theshape)
			{ val.cv = new std::vector<std::complex<double> >(arg); }
	variant(std::vector<std::complex<double> > *arg) : typev(COMPLEXVEC), shape_(1,arg->size())
                        { val.cv = arg; }
	variant(std::vector<std::complex<double> > *arg, std::vector<ssize_t> &theshape) : typev(COMPLEXVEC), shape_(theshape)
			{ val.cv = arg; }
//
	variant(const std::vector<std::string> &arg, const std::vector<ssize_t> &theshape) : typev(STRINGVEC), shape_(theshape)
			{ val.sv = new std::vector<std::string>(arg); }
	variant(const std::vector<std::string> &arg) : typev(STRINGVEC), shape_(1,arg.size())
			{ val.sv = new std::vector<std::string>(arg); }
	variant(std::vector<std::string> *arg) : typev(STRINGVEC), shape_(1, arg->size())
			{ val.sv = arg; }
	variant(std::vector<std::string> *arg, std::vector<ssize_t> &theshape) : typev(STRINGVEC), shape_(theshape)
			{ val.sv = arg; }
//
	variant(const record &arg);
	variant(record *arg);

	~variant( );

	variant & operator= (const variant &other);

	bool toBool( ) const;
	long toInt( ) const;
	unsigned long touInt( ) const;
	double toDouble( ) const;
	std::complex<double> toComplex( ) const;
	std::string toString( bool no_brackets=false ) const;
	std::vector<bool> toBoolVec( ) const;
	std::vector<long> toIntVec( ) const;
	std::vector<unsigned long> touIntVec( ) const;
	std::vector<double> toDoubleVec( ) const;
	std::vector<std::complex<double> > toComplexVec( ) const;
	std::vector<std::string> toStringVec( ) const;

	// Yet to be implemented

//      Modify
//      ---------------------------------------------------
	bool &asBool( );
	long &asInt( );
	unsigned long &asuInt( );
	double &asDouble( );
	std::complex<double> &asComplex( );
	std::string &asString( );
	std::vector<long> &asIntVec( ssize_t size=-1 );
	std::vector<unsigned long> &asuIntVec( ssize_t size=-1 );
	std::vector<bool> &asBoolVec( ssize_t size=-1 );
	std::vector<double> &asDoubleVec( ssize_t size=-1 );
	std::vector<std::complex<double> > &asComplexVec( ssize_t size=-1 );
	std::vector<std::string> &asStringVec( ssize_t size=-1 );
	casac::record &asRecord( );

	void as( TYPE t, ssize_t size=-1 );

//      Const
//      ---------------------------------------------------
	bool getBool( ) const;
	long getInt( ) const;
	unsigned long getuInt( ) const;
	double getDouble( ) const;
	const std::complex<double> &getComplex( ) const;
	const std::string &getString( ) const;
	const std::vector<long> &getIntVec( ) const;
	const std::vector<unsigned long> &getuIntVec( ) const;
	const std::vector<bool> &getBoolVec( ) const;
	const std::vector<double> &getDoubleVec( ) const;
	const std::vector<std::complex<double> > &getComplexVec( ) const;
	const std::vector<std::string> &getStringVec( ) const;
	const record &getRecord( ) const;
        const std::vector<ssize_t> &shape() const;
        const std::vector<ssize_t> &arrayshape() const {return shape();}

//	Modify
//      ---------------------------------------------------
	bool &getBoolMod( );
	long &getIntMod( );
	unsigned long &getuIntMod( );
	double &getDoubleMod( );
	std::complex<double> &getComplexMod( );
	std::string &getStringMod( );
	std::vector<long> &getIntVecMod( );
	std::vector<unsigned long> &getuIntVecMod( );
	std::vector<bool> &getBoolVecMod( );
	std::vector<double> &getDoubleVecMod( );
	std::vector<std::complex<double> > &getComplexVecMod( );
	std::vector<std::string> &getStringVecMod( );
	record &getRecordMod( );
        std::vector<ssize_t> &shape();
        std::vector<ssize_t> &arrayshape() {return shape();}

	const std::string &typeString( ) const;
	TYPE type( ) const { return typev; }

	void push(bool, bool conform = true);
	void push(long, bool conform = true);
	void push(unsigned long, bool conform = true);
	void push(long long, bool conform = true);
	void push(double, bool conform = true);
	void push(std::vector<long long>, bool conform = true);
	void push(std::complex<double>, bool conform = true);
	void push(const std::string&, bool conform = true);

	void place(bool, unsigned int index, bool conform = true);
	void place(long, unsigned int index, bool conform = true);
	void place(unsigned long, unsigned int index, bool conform = true);
	void place(long long, unsigned int index, bool conform = true);
	void place(double, unsigned int index, bool conform = true);
	void place(std::vector<long long>, unsigned int index, bool conform = true);
	void place(std::complex<double>, unsigned int index, bool conform = true);
	void place(const std::string&, unsigned int index, bool conform = true);

	ssize_t size( ) const { return typev >= BOOLVEC ? vec_size() : 1; }
	void resize( ssize_t size );

	// return true if empty string, empty record, or size 0 vector.
	// always returns false if object is a non-array bool or numerical type
	bool empty() const;

    private:

	void freeStorage ();

	// what size does the shape imply
	ssize_t shape_size( ) const;

	// 4294967295
	static unsigned int record_id_count;

	ssize_t vec_size( ) const;
	TYPE typev;
	union {
	  bool b;
	  std::vector<bool> *bv;
	  long i;
	  unsigned long ui;
	  std::vector<long> *iv;
	  std::vector<unsigned long> *uiv;
	  double d;
	  std::vector<double> *dv;
	  std::complex<double> *c;
	  std::vector<std::complex<double> > *cv;
	  std::string *s;
	  std::vector<std::string> *sv;
	  record *recordv;
	} val;
	std::vector<ssize_t> shape_;

	std::string create_message( const std::string s ) const;
};

variant initialize_variant( const std::string & );
}	// casac namespace

#endif
