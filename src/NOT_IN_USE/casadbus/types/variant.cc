//# variant.cc: variant implementation
//#
//# Copyright (C) 2011
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <casadbus/types/variant.h>
#include <casadbus/types/record.h>

namespace casa {
    namespace dbus {

	typedef std::string std_string;

	unsigned int variant::record_id_count = 0;

	variant::variant( ) : typev(BOOLVEC), shape_(1,0) {
	    val.bv = new std::vector<bool>( );
	}

	const std::vector<int> &variant::shape() const {
	    if ( typev == RECORD && (shape_.size() != 1 || (int) val.recordv->size() != shape_[0]) )
		((variant*)this)->shape_ = std::vector<int>(1,val.recordv->size());
	    return shape_;
	}

	std::vector<int> &variant::shape() {
	    if ( typev == RECORD && (shape_.size() != 1 || (int) val.recordv->size() != shape_[0]) )
		shape_ = std::vector<int>(1,val.recordv->size());
	    return shape_;
	}

	int variant::compare(const variant*) const { return -1; }

	variant::TYPE variant::compatible_type( TYPE one_, TYPE two_ ) {
	    TYPE one = one_;
	    TYPE two = two_;

	    // VEC has priority
	    if ( one >= BOOLVEC || two >= BOOLVEC ) {
		if ( one < BOOLVEC ) {
		    switch (one) {
		    case BOOL:		one = BOOLVEC; break;
		    case INT:		one = INTVEC; break;
		    case DOUBLE:	one = DOUBLEVEC; break;
		    case COMPLEX:	one = COMPLEXVEC; break;
		    case STRING:
		    default:		one = STRINGVEC; break;
		    }
		} else if ( two < BOOLVEC ) {
		    switch (two) {
		    case BOOL:		two = BOOLVEC; break;
		    case INT:		two = INTVEC; break;
		    case DOUBLE:	two = DOUBLEVEC; break;
		    case COMPLEX:	two = COMPLEXVEC; break;
		    default:		two = STRINGVEC; break;
		    }
		}

		switch(one) {
		case BOOLVEC: return two;
		case INTVEC:  return two == BOOLVEC ? INTVEC : two;
		case DOUBLEVEC: return two == COMPLEXVEC || two == STRINGVEC ? two : DOUBLEVEC;
		case COMPLEXVEC: return two == STRINGVEC ? STRINGVEC : COMPLEXVEC;
		default: return STRINGVEC;
		}
	    } else {
		switch(one) {
		case BOOL: return two;
		case INT:  return two == BOOL ? INT : two;
		case DOUBLE: return two == COMPLEX || two == STRING ? two : DOUBLE;
		case COMPLEX: return two == STRING ? STRING : COMPLEX;
		default: return STRING;
		}
	    }
	}

#include "string_conversions"

	variant::variant(record &arg) : typev(RECORD), shape_(1,arg.size()) {
	    val.recordv = new record(arg);
	}

	variant::variant(record *arg) : typev(RECORD), shape_(1,arg->size()) {
	    val.recordv = arg;
	}

	variant::variant(const variant &other) : typev(other.typev), shape_(other.shape_) {
	    switch( typev ) {
	    case BOOL:
		val.b = other.val.b;
		break;
	    case INT:
		val.i = other.val.i;
		break;
	    case DOUBLE:
		val.d = other.val.d;
		break;
	    case COMPLEX:
		val.c = new std::complex<double>(*other.val.c);
		break;
	    case BOOLVEC:
		val.bv = new std::vector<bool>(*other.val.bv);
		break;
	    case INTVEC:
		val.iv = new std::vector<int>(*other.val.iv);
		break;
	    case DOUBLEVEC:
		val.dv = new std::vector<double>(*other.val.dv);
		break;
	    case COMPLEXVEC:
		val.cv = new std::vector<std::complex<double> >(*other.val.cv);
		break;
	    case STRING:
		val.s = new std::string(*other.val.s);
		break;
	    case STRINGVEC:
		val.sv = new std::vector<std::string>(*other.val.sv);
		break;
	    case RECORD:
		val.recordv = new record(*other.val.recordv);
		break;
	    default:
		typev = BOOL;
		val.b = false;
		break;
	    }
	}

	variant::~variant( ) {
	    switch( typev ) {
	    case BOOLVEC:
		delete val.bv;
		break;
	    case INTVEC:
		delete val.iv;
		break;
	    case DOUBLEVEC:
		delete val.dv;
		break;
	    case COMPLEX:
		delete val.c;
		break;
	    case COMPLEXVEC:
		delete val.cv;
		break;
	    case STRING:
		delete val.s;
		break;
	    case STRINGVEC:
		delete val.sv;
		break;
	    case RECORD:
		delete val.recordv;
		break;
	    default:
		break;
	    }
	}

	const std::string &variant::typeString( ) const {
	    static std::string bs("bool");
	    static std::string bvs("boolvec");
	    static std::string is("int");
	    static std::string ivs("intvec");
	    static std::string ds("double");
	    static std::string dvs("doublevec");
	    static std::string cs("complex");
	    static std::string cvs("complexvec");
	    static std::string ss("str");
	    static std::string svs("strvec");
	    static std::string rec("record");
	    static std::string unknown("unknown");

	    switch( typev ) {
	    case BOOL:		return bs;
	    case BOOLVEC:	return bvs;
	    case INT:		return is;
	    case INTVEC:	return ivs;
	    case DOUBLE:	return ds;
	    case DOUBLEVEC:	return dvs;
	    case COMPLEX:	return cs;
	    case COMPLEXVEC:	return cvs;
	    case STRING:	return ss;
	    case STRINGVEC:	return svs;
	    case RECORD:	return rec;
	    default:		return unknown;
	    }
	}

	const char *variant::sig( ) const {
	    static const char *bs = "b";
	    static const char *bvs = "ab";
	    static const char *is = "i";
	    static const char *ivs = "ai";
	    static const char *ds = "d";
	    static const char *dvs = "ad";
	    static const char *cs = "(dd)";
	    static const char *cvs = "a(dd)";
	    static const char *ss = "s";
	    static const char *svs = "as";
	    static const char *rec = "a{sv}";
	    static const char *unknown = "v";

	    switch( typev ) {
	    case BOOL:		return bs;
	    case BOOLVEC:	return bvs;
	    case INT:		return is;
	    case INTVEC:	return ivs;
	    case DOUBLE:	return ds;
	    case DOUBLEVEC:	return dvs;
	    case COMPLEX:	return cs;
	    case COMPLEXVEC:	return cvs;
	    case STRING:	return ss;
	    case STRINGVEC:	return svs;
	    case RECORD:	return rec;
	    default:		return unknown;
	    }
	}

	bool variant::toBool( ) const {
	    switch( typev ) {
	    case BOOL:
		return val.b;
	    case BOOLVEC:
		return (*val.bv).size() > 0 ? (*val.bv)[0] : false;
	    case INT:
		return val.i ? true : false;
	    case INTVEC:
		return (*val.iv).size() > 0 ? ((*val.iv)[0] ? true : false) : false;
	    case DOUBLE:
		return val.d != 0.0 ? true : false;
	    case DOUBLEVEC:
		return (*val.dv).size() > 0 ? ((*val.dv)[0] != 0.0 ? true : false) : false;
	    case COMPLEX:
		return ((*val.c).real() != 0.0 || (*val.c).imag() != 0.0) ? true : false;
	    case COMPLEXVEC:
		return (*val.cv).size() > 0 ? (((*val.cv)[0].real() != 0.0 || (*val.cv)[0].imag() != 0.0) ? true : false) : false;
	    case STRING:
		return stringtobool( *val.s );
	    case STRINGVEC:
		return (*val.sv).size() > 0 ? stringtobool((*val.sv)[0]) : false;
	    case RECORD:
	    default:
		return false;
	    }
	}

	std::vector<bool> variant::toBoolVec( ) const {
	    switch( typev ) {
	    case BOOL:
		return std::vector<bool>(1,val.b);
	    case INT:
		return std::vector<bool>(1, val.i != 0 ? true : false);
	    case DOUBLE:
		return std::vector<bool>(1, val.d != 0.0 ? true : false);
	    case COMPLEX:
		return std::vector<bool>(1, ((*val.c).real() != 0.0 || (*val.c).imag() != 0.0)  ? true : false);
	    case BOOLVEC:
		return std::vector<bool>(*val.bv);
	    case INTVEC:
		{
		std::vector<bool> result((*val.iv).size());
		std::vector<int>::const_iterator from = (*val.iv).begin();
		for (unsigned int i=0; from != (*val.iv).end(); ++i, ++from)
		    result[i] = *from != 0 ? true : false;
		return result;
		}
	    case DOUBLEVEC:
		{
		std::vector<bool> result((*val.dv).size());
		std::vector<double>::const_iterator from = (*val.dv).begin();
		for (unsigned int i=0; from != (*val.dv).end(); ++i, ++from)
		    result[i] = *from != 0.0 ? true : false;
		return result;
		}
	    case COMPLEXVEC:
		{
		std::vector<bool> result((*val.dv).size());
		std::vector<std::complex<double> >::const_iterator from = (*val.cv).begin();
		for (unsigned int i=0; from != (*val.cv).end(); ++i, ++from) {
		    const std::complex<double> &cpx = *from;
		    result[i] = (cpx.real() != 0.0 || cpx.imag() != 0.0) ? true : false;
		}
		return result;
		}
	    case STRING:
		return std::vector<bool>(1,stringtobool( *val.s ));
	    case STRINGVEC:
		{
		std::vector<bool> result((*val.sv).size());
		std::vector<std::string>::const_iterator from = (*val.sv).begin();
		for (unsigned int i=0; from != (*val.sv).end(); ++i, ++from)
		    result[i] = stringtobool(*from);
		return result;
		}
	    case RECORD:
	    default:
		return std::vector<bool>(0);
	    }
	}

#define VECTOSTRING_CONVERSION(TYPE,IDTYPE,OPEN,CLOSE,DIVIDE,INVAR,OUTVAR,EMPTY) \
    if (INVAR.size() > 1) {							\
	OUTVAR = OPEN;								\
	std::vector<TYPE>::const_iterator from = INVAR.begin();			\
	OUTVAR += IDTYPE ## tostring(*from++);					\
	while (from != INVAR.end())						\
	    OUTVAR += DIVIDE + IDTYPE ## tostring(*from++);			\
	OUTVAR += CLOSE;							\
    } else if (INVAR.size() == 1) {						\
	OUTVAR = OPEN + IDTYPE ## tostring(INVAR[0]) + CLOSE;			\
    } else {									\
	OUTVAR = EMPTY;								\
    }										\

#define VECTOSTRING(TYPE,IDTYPE,MEM,SUBMEM,OPEN,CLOSE,DIVIDE,EMPTY)		\
    {										\
    std::string result;								\
    VECTOSTRING_CONVERSION(TYPE,IDTYPE,OPEN,CLOSE,DIVIDE,(*val.MEM) SUBMEM,result,EMPTY) \
    return result;								\
    }
#define VECASSTRING(TYPE,IDTYPE,MEM,SUBMEM,OPEN,CLOSE,DIVIDE,EMPTY)		\
    {										\
    std::string *tmp = new std::string();					\
    VECTOSTRING_CONVERSION(TYPE,IDTYPE,OPEN,CLOSE,DIVIDE,(*val.MEM) SUBMEM,(*tmp),EMPTY) \
    delete val.MEM;								\
    val.s = tmp;								\
    break;									\
    }

#define VECTOSTRINGVEC(TYPE,IDTYPE,MEM,SUBMEM)					\
    {										\
    std::vector<std::string> result;						\
    std::vector<TYPE>::const_iterator from = (*val.MEM) SUBMEM .begin();	\
    for (unsigned int i=0; from != (*val.MEM) SUBMEM .end(); ++i, ++from)	\
	result[i] = IDTYPE ## tostring(*from);					\
    return result;								\
    }

#define VECASSTRINGVEC(TYPE,IDTYPE,MEM,SUBMEM,OPEN,CLOSE)			\
    {										\
    int current_size = (*val.MEM) SUBMEM .size();				\
    newsize = size > current_size ? size : current_size;			\
    std::vector<std::string> *tmp = new std::vector<std::string>(newsize);	\
    std::vector<TYPE>::const_iterator from = (*val.MEM) SUBMEM .begin();	\
    for (unsigned int i=0; from != (*val.MEM) SUBMEM .end(); ++i, ++from)	\
	(*tmp)[i] = OPEN IDTYPE ## tostring(*from) CLOSE;			\
    delete val.MEM;								\
    val.sv = tmp;								\
    break;									\
    }

	std::string variant::toString( bool no_brackets ) const {
	    switch( typev ) {
	    case BOOL:
		return booltostring(val.b);
	    case INT:
		return inttostring(val.i);
	    case DOUBLE:
		return doubletostring(val.d);
	    case COMPLEX:
		return complextostring(*val.c);
	    case BOOLVEC:
		VECTOSTRING(bool,bool,bv,,(no_brackets?"":"["),(no_brackets?"":"]"),",",(no_brackets?"":"[]"))
	    case INTVEC:
		VECTOSTRING(int,int,iv,,(no_brackets?"":"["),(no_brackets?"":"]"),",",(no_brackets?"":"[]"))
	    case DOUBLEVEC:
		VECTOSTRING(double,double,dv,,(no_brackets?"":"["),(no_brackets?"":"]"),",",(no_brackets?"":"[]"))
	    case COMPLEXVEC:
		VECTOSTRING(std::complex<double>,complex,cv,,(no_brackets?"":"["),(no_brackets?"":"]"),",",(no_brackets?"":"[]"))
	    case STRING:
		return std::string( *val.s );
	    case STRINGVEC:
		VECTOSTRING(std_string,std_string,sv,,(no_brackets?"\"":"[\""),(no_brackets?"\"":"\"]"),"\",\"",(no_brackets?"":"[]"))
	    case RECORD:
		return std::string("{...}");
	    default:
		return std::string("");
	    }
	}

	std::vector<std::string> variant::toStringVec( ) const {
	    switch( typev ) {
	    case BOOL:
		return std::vector<std::string>(1,booltostring(val.b));
	    case INT:
		return std::vector<std::string>(1,inttostring(val.i));
	    case DOUBLE:
		return std::vector<std::string>(1,doubletostring(val.d));
	    case COMPLEX:
		return std::vector<std::string>(1,complextostring(*val.c));
	    case BOOLVEC:
		VECTOSTRINGVEC(bool,bool,bv,)
	    case INTVEC:
		VECTOSTRINGVEC(int,int,iv,)
	    case DOUBLEVEC:
		VECTOSTRINGVEC(double,double,dv,)
	    case COMPLEXVEC:
		VECTOSTRINGVEC(std::complex<double>,complex,cv,)
	    case STRING:
		return std::vector<std::string>(1,*val.s);
	    case STRINGVEC:
		return std::vector<std::string>(*val.sv);
	    case RECORD:
		return std::vector<std::string>(1,"{...}");
	    default:
		return std::vector<std::string>(0);
	    }
	}


#define TONUMERIC( NAME, TYPE, IDTYPE )						\
TYPE variant::NAME( ) const {							\
    switch( typev ) {								\
	case BOOL:								\
	    return (TYPE) (val.b ? 1 : 0);					\
	case INT:								\
	    return (TYPE) val.i;						\
	case DOUBLE:								\
	    return (TYPE) val.d;						\
	case COMPLEX:								\
	    return (TYPE) (*val.c).real();					\
	case BOOLVEC:								\
	    return (TYPE) ((*val.bv).size() > 0 ? ((*val.bv)[0] ? 1 : 0) : 0);	\
	case INTVEC:								\
	    return (TYPE) ((*val.iv).size() > 0 ? (*val.iv)[0] : 0);		\
	case DOUBLEVEC:								\
	    return (TYPE) ((*val.dv).size() > 0 ?  (*val.dv)[0] : 0);		\
	case COMPLEXVEC:							\
	    return (TYPE) ((*val.cv).size() > 0 ?  (*val.cv)[0].real() : 0);	\
	case STRING:								\
	    return stringto ## IDTYPE( *val.s );				\
	case STRINGVEC:								\
	    return (*val.sv).size() > 0 ? stringto ## IDTYPE((*val.sv)[0]) : 0;	\
	case RECORD:								\
	default:								\
	  return (TYPE) 0;							\
    }										\
}

	TONUMERIC(toInt,int,int)
	TONUMERIC(toDouble,double,double)

	std::complex<double> variant::toComplex( ) const {
	    switch( typev ) {
	    case BOOL:
		return std::complex<double>(val.b ? 1.0 : 0.0, 0.0);
	    case INT:
		return std::complex<double>(val.i, 0.0);
	    case DOUBLE:
		return std::complex<double>(val.d, 0.0);
	    case COMPLEX:
		return std::complex<double>(*val.c);
	    case BOOLVEC:
		return std::complex<double>(((*val.bv).size() > 0 ? ((*val.bv)[0] ? 1.0 : 0.0) : 0.0),0.0);;
	    case INTVEC:
		return std::complex<double>(((*val.iv).size() > 0 ? (*val.iv)[0] : 0),0.0);
	    case DOUBLEVEC:
		return std::complex<double>(((*val.dv).size() > 0 ?  (*val.dv)[0] : 0),0);
	    case COMPLEXVEC:
		return (*val.cv).size() > 0 ?  std::complex<double>((*val.cv)[0]) : std::complex<double>(0.0,0.0);
	    case STRING:
		return stringtocomplex( *val.s );
	    case STRINGVEC:
		return (*val.sv).size() > 0 ? stringtocomplex((*val.sv)[0]) : 0;
	    case RECORD:
	    default:
		return std::complex<double>( 0.0, 0.0 );
	    }
	}


#define TONUMERICVEC( NAME, TYPE, IDTYPE, TYPET, TYPEM, ONE, ONET, ONEM )	\
std::vector<TYPE> variant::NAME( ) const {					\
    switch( typev ) {								\
	case BOOL:								\
	    return std::vector<TYPE>(1,(TYPE)(val.b ? 1 : 0));			\
	case INT:								\
	    return std::vector<TYPE>(1,(TYPE) val.i);				\
	case DOUBLE:								\
	    return std::vector<TYPE>(1,(TYPE) val.d);				\
	case COMPLEX:								\
	    return std::vector<TYPE>(1,(TYPE) (*val.c).real());			\
	case BOOLVEC:								\
	    {									\
	    std::vector<TYPE> result((*val.bv).size());				\
	    std::vector<bool>::const_iterator from = (*val.bv).begin();		\
	    for (unsigned int i=0; from != (*val.bv).end(); ++i, ++from)	\
		result[i] = (TYPE)(*from ? 1 : 0);				\
	    return result;							\
	    }									\
	case TYPET:								\
	    return std::vector<TYPE>(*val.TYPEM);				\
	case ONET:								\
	    {									\
	    std::vector<TYPE> result((*val.ONEM).size());			\
	    std::vector<ONE>::const_iterator from = (*val.ONEM).begin();	\
	    for (unsigned int i=0; from != (*val.ONEM).end(); ++i, ++from)	\
		result[i] = (TYPE)(*from);					\
	    return result;							\
	    }									\
	case COMPLEXVEC:							\
	    {									\
	    std::vector<TYPE> result((*val.cv).size());				\
	    std::vector<std::complex<double> >::const_iterator from = (*val.cv).begin();\
	    for (unsigned int i=0; from != (*val.cv).end(); ++i, ++from)	\
		result[i] = (TYPE)((*from).real());				\
	    return result;							\
	    }									\
	case STRING:								\
	    return std::vector<TYPE>(1,stringto ## IDTYPE( *val.s ));		\
	case STRINGVEC:								\
	    {									\
	    std::vector<TYPE> result((*val.sv).size());				\
	    std::vector<std::string>::const_iterator from = (*val.sv).begin();	\
	    for (unsigned int i=0; from != (*val.sv).end(); ++i, ++from)	\
		result[i] = stringto ## IDTYPE(*from);				\
	    return result;							\
	    }									\
	case RECORD:								\
	default:								\
	    return std::vector<TYPE>(0);					\
    }										\
}

	TONUMERICVEC(toIntVec,   int,   int,   INTVEC,   iv,double,DOUBLEVEC,dv)
	TONUMERICVEC(toDoubleVec,double,double,DOUBLEVEC,dv,int,   INTVEC,   iv)

	std::vector<std::complex<double> > variant::toComplexVec( ) const {
	    switch( typev ) {
	    case BOOL:
		return std::vector<std::complex<double> >(1,std::complex<double>((val.b ? 1 : 0),0.0));
	    case INT:
		return std::vector<std::complex<double> >(1,std::complex<double>(val.i,0.0));
	    case DOUBLE:
		return std::vector<std::complex<double> >(1,std::complex<double>(val.d,0.0));
	    case COMPLEX:
		return std::vector<std::complex<double> >(1,*val.c);
	    case BOOLVEC:
		{
		std::vector<std::complex<double> > result((*val.bv).size());
		std::vector<bool>::const_iterator from = (*val.bv).begin();
		for (unsigned int i=0; from != (*val.bv).end(); ++i, ++from)
		    result[i] = std::complex<double>((*from ? 1 : 0),0.0);
		return result;
		}
	    case COMPLEXVEC:
		return std::vector<std::complex<double> >(*val.cv);
	    case INTVEC:
		{
		std::vector<std::complex<double> > result((*val.iv).size());
		std::vector<int>::const_iterator from = (*val.iv).begin();
		for (unsigned int i=0; from != (*val.iv).end(); ++i, ++from)
		    result[i] = std::complex<double>(*from,0.0);
		return result;
		}
	    case DOUBLEVEC:
		{
		std::vector<std::complex<double> > result((*val.dv).size());
		std::vector<double>::const_iterator from = (*val.dv).begin();
		for (unsigned int i=0; from != (*val.dv).end(); ++i, ++from)
		    result[i] = std::complex<double>(*from,0.0);
		return result;
		}
	    case STRING:
		return std::vector<std::complex<double> >(1,stringtocomplex( *val.s ));
	    case STRINGVEC:
		{
		std::vector<std::complex<double> > result((*val.sv).size());
		std::vector<std::string>::const_iterator from = (*val.sv).begin();
		for(unsigned int i=0; from != (*val.sv).end(); ++i, ++from)
		    result[i] = stringtocomplex(*from);
		return result;
		}
	    case RECORD:
	    default:
		return std::vector<std::complex<double> >(0);
	    }
	}

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
#define ASNUMERIC( NAME, TYPE, IDTYPE, TYPETAG, VAL )				\
TYPE &variant::NAME( ) {							\
    if ( typev != TYPETAG ) {							\
        switch( typev ) {							\
	    case BOOL:								\
		val.VAL = (TYPE) (val.b ? 1 : 0);				\
		break;								\
	    case INT:								\
	        val.VAL = (TYPE) val.i;						\
		break;								\
	    case DOUBLE:							\
		val.VAL = (TYPE) val.d;						\
		break;								\
	    case COMPLEX:							\
		{								\
		std::complex<double> *tmp = val.c;				\
		val.VAL = (TYPE) (*tmp).real();					\
		delete tmp;							\
		break;								\
		}								\
	    case BOOLVEC:							\
		{								\
		std::vector<bool> *tmp = val.bv;				\
	        val.VAL = (TYPE) ((*tmp).size() > 0 ? ((*tmp)[0] ? 1 : 0) : 0);	\
		delete tmp;							\
		break;								\
		}								\
	    case INTVEC:							\
		{								\
		std::vector<int> *tmp = val.iv;					\
		val.VAL = (TYPE) ((*tmp).size() > 0 ? (*tmp)[0] : 0);		\
		delete tmp;							\
		break;								\
		}								\
	    case DOUBLEVEC:							\
		{								\
		std::vector<double> *tmp = val.dv;				\
		val.VAL = (TYPE) ((*tmp).size() > 0 ?  (*tmp)[0] : 0);		\
		delete tmp;							\
		break;								\
		}								\
	    case COMPLEXVEC:							\
		{								\
		std::vector<std::complex<double> > *tmp = val.cv;		\
		val.VAL = (TYPE) ((*tmp).size() > 0 ?  (*tmp)[0].real() : 0);	\
		delete tmp;							\
		break;								\
		}								\
	    case STRING:							\
		{								\
		std::string *tmp = val.s;					\
		val.VAL = stringto ## IDTYPE( *tmp );				\
		delete tmp;							\
		break;								\
		}								\
	    case STRINGVEC:							\
		{								\
		std::vector<std::string> *tmp = val.sv;				\
		val.VAL = (*tmp).size() > 0 ? stringto ## IDTYPE((*tmp)[0]) : 0;\
		delete tmp;							\
		break;								\
		}								\
	    case RECORD:							\
		delete val.recordv;						\
	    default:								\
		val.VAL = (TYPE) 0;						\
		break;								\
	}									\
										\
	typev = TYPETAG;							\
	shape_ = std::vector<int>(1,1);						\
										\
    }										\
										\
    return val.VAL;								\
}

	ASNUMERIC(asInt,int,int,INT,i)
	ASNUMERIC(asDouble,double,double,DOUBLE,d)


	std::complex<double> &variant::asComplex( ) {
	    if ( typev != COMPLEX ) {
		switch( typev ) {
		case BOOL:
		    val.c = new std::complex<double>(val.b ? 1 : 0,0.0);
		    break;
		case INT:
		    val.c = new std::complex<double>(val.i,0.0);
		    break;
		case DOUBLE:
		    val.c = new std::complex<double>(val.d,0.0);
		    break;
		case COMPLEX:
		    break;
		case BOOLVEC:
		    {
		    std::vector<bool> *tmp = val.bv;
		    val.c = new std::complex<double>((*tmp).size() > 0 ? ((*tmp)[0] ? 1 : 0) : 0,0.0);
		    delete tmp;
		    break;
		    }
		case INTVEC:
		    {
		    std::vector<int> *tmp = val.iv;
		    val.c = new std::complex<double>((*tmp).size() > 0 ? (*tmp)[0] : 0,0.0);
		    delete tmp;
		    break;
		    }
		case DOUBLEVEC:
		    {
		    std::vector<double> *tmp = val.dv;
		    val.c = new std::complex<double>((*tmp).size() > 0 ?  (*tmp)[0] : 0,0.0);
		    delete tmp;
		    break;
		    }
		case COMPLEXVEC:
		    {
		    std::vector<std::complex<double> > *tmp = val.cv;
		    val.c = (*tmp).size() > 0 ?  new std::complex<double>((*tmp)[0]) : new std::complex<double>(0.0,0.0);
		    delete tmp;
		    break;
		    }
		case STRING:
		    {
		    std::string *tmp = val.s;
		    val.c = new std::complex<double>(stringtocomplex( *tmp ));
		    delete tmp;
		    break;
		    }
		case STRINGVEC:
		    {
		    std::vector<std::string> *tmp = val.sv;
		    val.c = (*tmp).size() > 0 ? new std::complex<double>(stringtocomplex((*tmp)[0])) : new std::complex<double>(0.0,0.0);
		    delete tmp;
		    break;
		    }
		case RECORD:
		    delete val.recordv;
		default:
		    val.c = new std::complex<double>(0.0,0.0);
		    break;
		}

		typev = COMPLEX;
		shape_ = std::vector<int>(1,1);
	    }

	    return *val.c;
	}

#define ASNUMERICVEC( NAME, TYPE, IDTYPE, TYPETAG, VAL )			\
std::vector<TYPE> &variant::NAME( int size ) {					\
    int newsize = -1;								\
    if ( typev != TYPETAG ) {							\
        switch( typev ) {							\
	    case BOOL:								\
		newsize = size > 1 ? size : 1;					\
		val.VAL = new std::vector<TYPE>(newsize,(TYPE)(val.b ? 1 : 0));	\
		break;								\
	    case INT:								\
		newsize = size > 1 ? size : 1;					\
		val.VAL = new std::vector<TYPE>(newsize,(TYPE) val.i);		\
		break;								\
	    case DOUBLE:							\
		newsize = size > 1 ? size : 1;					\
		val.VAL = new std::vector<TYPE>(newsize,(TYPE) val.d);		\
		break;								\
	    case COMPLEX:							\
		{								\
		newsize = size > 1 ? size : 1;					\
		std::complex<double> *tmp = val.c;				\
		val.VAL = new std::vector<TYPE>(newsize,(TYPE) (*tmp).real());	\
		delete tmp;							\
		break;								\
		}								\
	    case BOOLVEC:							\
		{								\
		std::vector<bool> *tmp = val.bv;				\
		int current_size = (*tmp).size();				\
		newsize = size > current_size ? size : current_size;		\
		val.VAL = new std::vector<TYPE>(newsize);			\
		std::vector<bool>::const_iterator from = (*tmp).begin();	\
		for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)	\
		    (*val.VAL)[i] = *from ? 1 : 0;				\
		delete tmp;							\
		break;								\
		}								\
	    case INTVEC:							\
		{								\
		std::vector<int> *tmp = val.iv;					\
		int current_size = (*tmp).size();				\
		newsize = size > current_size ? size : current_size;		\
		val.VAL = new std::vector<TYPE>(newsize);			\
		std::vector<int>::const_iterator from = (*tmp).begin();		\
		for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)	\
		    (*val.VAL)[i] = (TYPE)(*from);				\
		delete tmp;							\
		break;								\
		}								\
	    case DOUBLEVEC:							\
		{								\
		std::vector<double> *tmp = val.dv;				\
		int current_size = (*tmp).size();				\
		newsize = size > current_size ? size : current_size;		\
		val.VAL = new std::vector<TYPE>(newsize);			\
		std::vector<double>::const_iterator from = (*tmp).begin();	\
		for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)	\
		    (*val.VAL)[i] = (TYPE)(*from);				\
		delete tmp;							\
		break;								\
		}								\
	    case COMPLEXVEC:							\
		{								\
		std::vector<std::complex<double> > *tmp = val.cv;		\
		int current_size = (*tmp).size();				\
		newsize = size > current_size ? size : current_size;		\
		val.VAL = new std::vector<TYPE>(newsize);			\
		std::vector<std::complex<double> >::const_iterator from = (*tmp).begin();\
		for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)	\
		    (*val.VAL)[i] = (TYPE)((*from).real());			\
		delete tmp;							\
		break;								\
		}								\
	    case STRING:							\
		{								\
		std::string *tmp = val.s;					\
		newsize = size > 1 ? size : 1;					\
		val.VAL = new std::vector<TYPE>(newsize,stringto ## IDTYPE( *tmp ));\
		delete tmp;							\
		break;								\
		}								\
	    case STRINGVEC:							\
		{								\
		std::vector<std::string> *tmp = val.sv;				\
		int current_size = (*tmp).size();				\
		newsize = size > current_size ? size : current_size;		\
		val.VAL = new std::vector<TYPE>(newsize); 			\
		std::vector<std::string>::const_iterator from = (*tmp).begin();	\
		for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)	\
		    (*val.VAL)[i] = stringto ## IDTYPE(*from);			\
		delete tmp;							\
		break;								\
		}								\
	    case RECORD:							\
		delete val.recordv;						\
	    default:								\
		newsize = 0;							\
		val.VAL = new std::vector<TYPE>(0);				\
		break;								\
	}									\
										\
	typev = TYPETAG;							\
	if ( shape_.size() == 1 )						\
	    shape_[0] = newsize;						\
	else if ( shape_size() != newsize )					\
	    shape_ = std::vector<int>(1,newsize);				\
										\
    } else if ( size > 0 && (unsigned int) size > (*val.VAL).size() ) {		\
	resize(size);								\
	if ( shape_.size() == 1 )						\
	    shape_[0] = size;							\
	else if ( shape_size() != size )					\
	    shape_ = std::vector<int>(1,size);					\
    }										\
    return *val.VAL;								\
}

	ASNUMERICVEC(asIntVec,int,int,INTVEC,iv)
	ASNUMERICVEC(asDoubleVec,double,double,DOUBLEVEC,dv)

	std::vector<std::complex<double> > &variant::asComplexVec( int size ) {
	    int newsize = -1;
	    if ( typev != COMPLEXVEC ) {
		switch( typev ) {
		case BOOL:
		    newsize = size > 1 ? size : 1;
		    val.cv = new std::vector<std::complex<double> >(newsize,std::complex<double>(val.b ? 1 : 0,0.0));
		    break;
		case INT:
		    newsize = size > 1 ? size : 1;
		    val.cv = new std::vector<std::complex<double> >(newsize,std::complex<double>(val.i,0.0));
		    break;
		case DOUBLE:
		    newsize = size > 1 ? size : 1;
		    val.cv = new std::vector<std::complex<double> >(newsize,std::complex<double>(val.d,0.0));
		    break;
		case COMPLEX:
		    {
		    newsize = size > 1 ? size : 1;
		    std::complex<double> *tmp = val.c;
		    val.cv = new std::vector<std::complex<double> >(newsize,*tmp);
		    delete tmp;
		    break;
		    }
		case BOOLVEC:
		    {
		    std::vector<bool> *tmp = val.bv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.cv = new std::vector<std::complex<double> >(newsize);
		    std::vector<bool>::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from) {
			(*val.cv)[i] = std::complex<double>(*from ? 1 : 0,0.0);
		    }
		    delete tmp;
		    break;
		    }
		case INTVEC:
		    {
		    std::vector<int> *tmp = val.iv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.cv = new std::vector<std::complex<double> >(newsize);
		    std::vector<int>::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)
			(*val.cv)[i] = std::complex<double>(*from,0.0);
		    delete tmp;
		    break;
		    }
		case DOUBLEVEC:
		    {
		    std::vector<double> *tmp = val.dv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.cv = new std::vector<std::complex<double> >(newsize);
		    std::vector<double>::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)
			(*val.cv)[i] = std::complex<double>(*from,0.0);
		    delete tmp;
		    break;
		    }
		case STRING:
		    {
		    std::string *tmp = val.s;
		    newsize = 1;
		    val.cv = new std::vector<std::complex<double> >(1,stringtocomplex( *tmp ));
		    delete tmp;
		    break;
		    }
		case STRINGVEC:
		    {
		    std::vector<std::string> *tmp = val.sv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.cv = new std::vector<std::complex<double> >(newsize);
		    std::vector<std::string>::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)
			(*val.cv)[i] = stringtocomplex(*from);
		    delete tmp;
		    break;
		    }
		case RECORD:
		    delete val.recordv;
		default:
		    newsize = 0;
		    val.cv = new std::vector<std::complex<double> >(0);
		    break;
		}

		typev = COMPLEXVEC;
		if ( shape_.size() == 1 )
		    shape_[0] = newsize;
		else if ( shape_size() != newsize )
		    shape_ = std::vector<int>(1,newsize);

	    } else if ( size > 0 && (unsigned int) size > (*val.cv).size() ) {

		resize(size);
		if ( shape_.size() == 1 )
		    shape_[0] = size;
		else if ( shape_size() != size )
		    shape_ = std::vector<int>(1,size);

	    }

	    return *val.cv;
	}

	std::vector<bool> &variant::asBoolVec( int size ) {
	    int newsize = -1;
	    if ( typev != BOOLVEC ) {
		switch( typev ) {
		case BOOL:
		    newsize = size > 1 ? size : 1;
		    val.bv = new std::vector<bool>(newsize);
		    break;
		case INT:
		    newsize = size > 1 ? size : 1;
		    val.bv = new std::vector<bool>(newsize, val.i != 0 ? true : false);
		    break;
		case DOUBLE:
		    newsize = size > 1 ? size : 1;
		    val.bv = new std::vector<bool>(newsize, val.d != 0.0 ? true : false);
		    break;
		case COMPLEX:
		    {
		    std::complex<double> *tmp = val.c;
		    newsize = size > 1 ? size : 1;
		    val.bv = new std::vector<bool>(newsize, ((*tmp).real() != 0.0 || (*tmp).imag() != 0.0) ? true : false);
		    delete tmp;
		    break;
		    }
		case BOOLVEC:
		    break;
		case INTVEC:
		    {
		    std::vector<int> *tmp = val.iv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.bv = new std::vector<bool>(newsize);
		    std::vector<int>::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)
			(*val.bv)[i] = *from != 0 ? true : false;
		    delete tmp;
		    break;
		    }
		case DOUBLEVEC:
		    {
		    std::vector<double> *tmp = val.dv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.bv = new std::vector<bool>(newsize);
		    std::vector<double>::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)
			(*val.bv)[i] = *from != 0.0 ? true : false;
		    delete tmp;
		    break;
		    }
		case COMPLEXVEC:
		    {
		    std::vector<std::complex<double> > *tmp = val.cv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.bv = new std::vector<bool>(newsize);
		    std::vector<std::complex<double> >::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from) {
			const std::complex<double> &cpx = *from;
			(*val.bv)[i] = (cpx.real() != 0.0 || cpx.imag() != 0.0) ? true : false;
		    }
		    delete tmp;
		    break;
		    }
		case STRING:
		    newsize = 1;
		    val.bv = new std::vector<bool>(1,stringtobool( *val.s ));
		    break;
		case STRINGVEC:
		    {
		    std::vector<std::string> *tmp = val.sv;
		    int current_size = (*tmp).size();
		    newsize = size > current_size ? size : current_size;
		    val.bv = new std::vector<bool>(newsize);
		    std::vector<std::string>::const_iterator from = (*tmp).begin();
		    for (unsigned int i=0; from != (*tmp).end(); ++i, ++from)
		  	(*val.bv)[i] = stringtobool(*from);
		    delete tmp;
		    break;
		    }
		case RECORD:
		    delete val.recordv;
		default:
		    newsize = 0;
		    val.bv = new std::vector<bool>(0);
		    break;
		}

		typev = BOOLVEC;
		if ( shape_.size() == 1 )
		    shape_[0] = newsize;
		else if ( shape_size() != newsize )
		    shape_ = std::vector<int>(1,newsize);

	    } else if ( size > 0 && (unsigned int) size > (*val.bv).size() ) {
		resize(size);
		if ( shape_.size() == 1 )
		    shape_[0] = size;
		else if ( shape_size() != size )
		    shape_ = std::vector<int>(1,size);
	    }

	    return *val.bv;
	}

	bool &variant::asBool( ) {
	    if (typev != BOOL) {
		switch( typev ) {
		case BOOL:
		    break;
		case BOOLVEC:
		    {
		    std::vector<bool> *tmp = val.bv;
		    val.b = (*val.bv).size() > 0 ? (*val.bv)[0] : false;
		    delete tmp;
		    break;
		    }
		case INT:
		    val.b = val.i ? true : false;
		    break;
		case INTVEC:
		    {
		    std::vector<int> *tmp = val.iv;
		    val.b = (*tmp).size() > 0 ? ((*tmp)[0] ? true : false) : false;
		    delete tmp;
		    break;
		    }
		case DOUBLE:
		    val.b = val.d != 0.0 ? true : false;
		    break;
		case DOUBLEVEC:
		    {
		    std::vector<double> *tmp = val.dv;
		    val.b = (*tmp).size() > 0 ? ((*tmp)[0] != 0.0 ? true : false) : false;
		    delete tmp;
		    break;
		    }
		case COMPLEX:
		    {
		    std::complex<double> *tmp = val.c;
		    val.b = ((*tmp).real() != 0.0 || (*tmp).imag() != 0.0) ? true : false;
		    delete tmp;
		    break;
		    }
		case COMPLEXVEC:
		    {
		    std::vector<std::complex<double> > *tmp = val.cv;
		    val.b = (*tmp).size() > 0 ? (((*tmp)[0].real() != 0.0 || (*tmp)[0].imag() != 0.0) ? true : false) : false;
		    delete tmp;
		    break;
		    }
		case STRING:
		    {
		    std::string *tmp = val.s;
		    val.b = stringtobool( *val.s );
		    delete tmp;
		    break;
		    }
		case STRINGVEC:
		    {
		    std::vector<std::string> *tmp = val.sv;
		    val.b = (*tmp).size() > 0 ? stringtobool((*tmp)[0]) : false;
		    delete tmp;
		    break;
		    }
		case RECORD:
		    delete val.recordv;
		default:
		    val.b = false;
		    break;
		}
		typev = BOOL;
	    }

	    if ( shape_.size() == 1 )
		shape_[0] = 1;
	    else
		shape_ = std::vector<int>(1,1);

	    return val.b;
	}

	std::string &variant::asString( ) {
	    if ( typev != STRING ) {
		switch( typev ) {
		case BOOL:
		    val.s = booltostringptr(val.b);
		    break;
		case INT:
		    val.s = inttostringptr(val.i);
		    break;
		case DOUBLE:
		    val.s = doubletostringptr(val.d);
		    break;
		case COMPLEX:
		    {
		    std::complex<double> *tmp = val.c;
		    val.s = complextostringptr(*tmp);
		    delete tmp;
		    break;
		    }
		case BOOLVEC:
		    VECASSTRING(bool,bool,bv,,"[","]",",","[]")
		case INTVEC:
		    VECASSTRING(int,int,iv,,"[","]",",","[]")
		case DOUBLEVEC:
		    VECASSTRING(double,double,dv,,"[","]",",","[]")
		case COMPLEXVEC:
		    VECASSTRING(std::complex<double>,complex,cv,,"[","]",",","[]")
		case STRING:
		    break;
		case STRINGVEC:
		    VECASSTRING(std_string,std_string,sv,,"[\"","\"]","\",\"","[]")
		case RECORD:
		    delete val.recordv;
		    val.s = new std::string("{...}");
		    break;
		default:
		    val.s = new std::string("");
		    break;
		}
		typev = STRING;
	    }

	    if ( shape_.size() == 1 )
		shape_[0] = 1;
	    else
	        shape_ = std::vector<int>(1,1);

	    return *val.s;
	}

	std::vector<std::string> &variant::asStringVec( int size ) {
	    int newsize = -1;
	    if ( typev != STRINGVEC ) {
		switch( typev ) {
		case BOOL:
		    newsize = size > 1 ? size : 1;
		    val.sv = new std::vector<std::string>(newsize,booltostring(val.b));
		    break;
		case INT:
		    newsize = size > 1 ? size : 1;
		    val.sv = new std::vector<std::string>(newsize,inttostring(val.i));
		    break;
		case DOUBLE:
		    newsize = size > 1 ? size : 1;
		    val.sv = new std::vector<std::string>(newsize,doubletostring(val.d));
		    break;
		case COMPLEX:
		    {
		    std::complex<double> *tmp = val.c;
		    newsize = size > 1 ? size : 1;
		    val.sv = new std::vector<std::string>(newsize,complextostring(*tmp));
		    delete tmp;
		    break;
		    }
		case BOOLVEC:
		    VECASSTRINGVEC(bool,bool,bv,,,)
		case INTVEC:
		    VECASSTRINGVEC(int,int,iv,,,)
		case DOUBLEVEC:
		    VECASSTRINGVEC(double,double,dv,,,)
		case COMPLEXVEC:
		    VECASSTRINGVEC(std::complex<double>,complex,cv,,,)
		case STRING:
		    {
		    newsize = 1;
		    std::string *tmp = val.s;
		    val.sv = new std::vector<std::string>(1,*tmp);
		    delete tmp;
		    break;
		    }
		case STRINGVEC:
		    break;
		case RECORD:
		    newsize = 1;
		    delete val.recordv;
		    val.sv = new std::vector<std::string>(1,"{...}");
		    break;
		default:
		    newsize = 0;
		    val.sv = new std::vector<std::string>(0);
		    break;
		}

		typev = STRINGVEC;
		if ( shape_.size() == 1 )
		    shape_[0] = newsize;
		else if ( shape_size() != newsize )
		    shape_ = std::vector<int>(1,newsize);

	    } else if ( size > 0 && (unsigned int) size > (*val.sv).size() ) {
		resize(size);
		if ( shape_.size() == 1 )
		    shape_[0] = size;
		else if ( shape_size() != size )
		    shape_ = std::vector<int>(1,size);
	    }

	    return *val.sv;
	}

	record &variant::asRecord( ) {
	    switch( typev ) {
	    case RECORD:
		return *val.recordv;
	    case INT:
	    case BOOL:
	    case DOUBLE:
		break;
	    case BOOLVEC:
		delete val.bv;
		break;
	    case INTVEC:
		delete val.iv;
		break;
	    case DOUBLEVEC:
		delete val.dv;
		break;
	    case COMPLEX:
		delete val.c;
		break;
	    case COMPLEXVEC:
		delete val.cv;
		break;
	    case STRING:
		delete val.s;
		break;
	    case STRINGVEC:
		delete val.sv;
		break;
	    }

	    typev = RECORD;
	    val.recordv = new record();
	    if ( shape_.size() == 1 )
		shape_[0] = 0;
	    else
		shape_ = std::vector<int>(1,0);

	    return *val.recordv;
	}

	void variant::as( TYPE t, int size ) {

	    if (typev == t) return;

	    switch ( t ) {
	    case BOOL:
		asBool();
		break;
	    case INT:
		asInt();
		break;
	    case DOUBLE:
		asDouble();
		break;
	    case COMPLEX:
		asComplex();
		break;
	    case STRING:
		asString();
		break;
	    case BOOLVEC:
		asBoolVec(size);
		break;
	    case INTVEC:
		asIntVec(size);
		break;
	    case DOUBLEVEC:
		asDoubleVec(size);
		break;
	    case STRINGVEC:
		asStringVec(size);
		break;
	    case COMPLEXVEC:
		asComplexVec(size);
		break;
	    case RECORD:
		asRecord( );
		break;
	    }
	}

#define GETIT(CONST,CONST2,RET_TYPE,NAME,TYPE,VAL,DEREF)		\
CONST RET_TYPE variant::NAME( ) CONST2 {		\
    if ( typev != TYPE )					\
	throw( create_message( #NAME " called for type") );	\
    return DEREF val.VAL;					\
}

	GETIT(, const, int,getInt,INT,i,)
	GETIT(, const, bool,getBool,BOOL,b,)
	GETIT(, const, double,getDouble,DOUBLE,d,)
	GETIT(const, const, std::complex<double>&,getComplex,COMPLEX,c,*)
	GETIT(const, const, std::string&,getString,STRING,s,*)
	GETIT(const, const, std::vector<int>&,getIntVec,INTVEC,iv,*)
	GETIT(const, const, std::vector<bool>&,getBoolVec,BOOLVEC,bv,*)
	GETIT(const, const, std::vector<double>&,getDoubleVec,DOUBLEVEC,dv,*)
	GETIT(const, const, std::vector<std::complex<double> >&,getComplexVec,COMPLEXVEC,cv,*)
	GETIT(const, const, std::vector<std::string>&,getStringVec,STRINGVEC,sv,*)
	GETIT(const, const, record&,getRecord,RECORD,recordv,*)

	GETIT(,,int&,getIntMod,INT,i,)
	GETIT(,,bool&,getBoolMod,BOOL,b,)
	GETIT(,,double&,getDoubleMod,DOUBLE,d,)
	GETIT(,,std::complex<double>&,getComplexMod,COMPLEX,c,*)
	GETIT(,,std::string&,getStringMod,STRING,s,*)
	GETIT(,,std::vector<int>&,getIntVecMod,INTVEC,iv,*)
	GETIT(,,std::vector<bool>&,getBoolVecMod,BOOLVEC,bv,*)
	GETIT(,,std::vector<double>&,getDoubleVecMod,DOUBLEVEC,dv,*)
	GETIT(,,std::vector<std::complex<double> >&,getComplexVecMod,COMPLEXVEC,cv,*)
	GETIT(,,std::vector<std::string>&,getStringVecMod,STRINGVEC,sv,*)
	GETIT(,,record&,getRecordMod,RECORD,recordv,*)

#define PUSHIMPL(TYPEX,TYPETAG,TYPETOSTRING,NUMTWEAK,BOOLTWEAK,BOOLCPX,STRBOOL, STRINT,STRDBL,STRCPX) \
void variant::push(TYPEX v, bool conform ) {					\
										\
    if ( conform == true ) {							\
	TYPE new_type = variant::compatible_type(TYPETAG,typev);		\
	if (new_type != typev) as(new_type);					\
    }										\
										\
    switch (typev) {								\
	case BOOL:								\
	    asBoolVec().push_back((bool) STRBOOL(v NUMTWEAK));       		\
	    break;								\
	case INT:								\
	    asIntVec().push_back((int) STRINT(v BOOLTWEAK));			\
	    break;								\
	case DOUBLE:								\
	    asDoubleVec().push_back((double) STRDBL(v BOOLTWEAK));		\
	    break;								\
	case COMPLEX:								\
	    asComplexVec().push_back((std::complex<double>) STRCPX(v BOOLCPX));	\
	    break;								\
	case STRING:								\
	    asStringVec().push_back( TYPETOSTRING(v));				\
	    break;								\
	case BOOLVEC:								\
	    (*val.bv).push_back((bool) STRBOOL(v NUMTWEAK));			\
	    break;								\
	case INTVEC:								\
	    (*val.iv).push_back((int) STRINT(v BOOLTWEAK));			\
	    break;								\
	case DOUBLEVEC:								\
	    (*val.dv).push_back((double) STRDBL(v BOOLTWEAK));       		\
	    break;								\
	case COMPLEXVEC:							\
	    (*val.cv).push_back((std::complex<double>) STRCPX(v BOOLCPX)); 	\
	    break;								\
	case STRINGVEC:								\
	    (*val.sv).push_back( TYPETOSTRING(v));				\
	    break;								\
	case RECORD:								\
	    {									\
	    char buf[512];							\
	    sprintf(buf,"key*%010u",++record_id_count);				\
	    while ( (*val.recordv).find(buf) != (*val.recordv).end() )		\
		sprintf(buf,"key*%010u",++record_id_count);			\
	    (*val.recordv).insert(buf,variant(v));				\
	    }									\
	    break;								\
    }										\
										\
    if ( shape_.size() == 1 )							\
	shape_[0] += 1;								\
    else if ( shape_size() != size() )						\
	shape_ = std::vector<int>(1,size());					\
}

	PUSHIMPL(bool                ,BOOL    ,tostring ,                                             ,== true ? 1 : 0      ,== true ? 1 : 0, , , , )
	PUSHIMPL(std::complex<double>,COMPLEX ,tostring ,.real() == 0 && v.imag() == 0 ? false : true ,.real()              ,               , , , , )
	PUSHIMPL(int                ,INT     ,tostring ,== 0 ? false : true                          ,                     ,               , , , , )
	PUSHIMPL(double              ,DOUBLE  ,tostring ,== 0 ? false : true                          ,                     ,               , , , , )
	PUSHIMPL(const std::string&  ,STRING  ,         , , , ,stringtobool ,stringtoint,stringtodouble ,stringtocomplex )

#define PLACEIMPL(TYPEX,TYPETAG,TYPETOSTRING,NUMTWEAK,BOOLTWEAK,BOOLCPX,STRBOOL, STRINT,STRDBL,STRCPX) \
void variant::place(TYPEX v, unsigned int index, bool conform ) {			\
										\
    if ( conform == true ) {							\
	TYPE new_type = variant::compatible_type(TYPETAG,typev);		\
	if (new_type != typev) as(new_type);					\
    }										\
										\
    switch (typev) {								\
	case BOOL:								\
	    if ( index > 0 )							\
		asBoolVec(index+1)[index] = (bool) (STRBOOL(v NUMTWEAK));       \
	    else								\
		val.b = (bool) (STRBOOL(v NUMTWEAK));				\
	    break;								\
	case INT:								\
	    if ( index > 0 )							\
		asIntVec(index+1)[index] = (int) (STRINT(v BOOLTWEAK));		\
	    else								\
		val.i = (int) (STRINT(v BOOLTWEAK));				\
	    break;								\
	case DOUBLE:								\
	    if ( index > 0 )							\
		asDoubleVec(index+1).push_back((double) STRDBL(v BOOLTWEAK));	\
	    break;								\
	case COMPLEX:								\
	    asComplexVec(index+1).push_back((std::complex<double>) STRCPX(v BOOLCPX)); \
	    break;								\
	case STRING:								\
	    asStringVec(index+1).push_back( TYPETOSTRING(v));			\
	    break;								\
	case BOOLVEC:								\
	    if ( index+1 > (*val.bv).size() )					\
		(*val.bv).resize(index+1);					\
	    (*val.bv)[index] = (bool) (STRBOOL(v NUMTWEAK));			\
	    break;								\
	case INTVEC:								\
	    if ( index+1 > (*val.iv).size() )					\
		(*val.iv).resize(index+1);					\
	    (*val.iv)[index] = (int) (STRINT(v BOOLTWEAK));			\
	    break;								\
	case DOUBLEVEC:								\
	    if ( index+1 > (*val.dv).size() )					\
		(*val.dv).resize(index+1);					\
	    (*val.dv)[index] = (double) (STRDBL(v BOOLTWEAK));			\
	    break;								\
	case COMPLEXVEC:							\
	    if ( index+1 > (*val.cv).size() )					\
		(*val.cv).resize(index+1);					\
	    (*val.cv)[index] = (std::complex<double>) (STRCPX(v BOOLCPX));	\
	    break;								\
	case STRINGVEC:								\
	    if ( index+1 > (*val.sv).size() )					\
		(*val.sv).resize(index+1);					\
	    (*val.sv)[index] = (TYPETOSTRING(v));				\
	    break;								\
	case RECORD:								\
	    {									\
	    char buf[512];							\
	    sprintf(buf,"idx*%010u",index);					\
	    if ((*val.recordv).find(buf) == (*val.recordv).end())		\
		(*val.recordv).insert(buf,variant(v));				\
	    else {								\
		sprintf(buf,"key*%010u",++record_id_count);			\
		while ( (*val.recordv).find(buf) != (*val.recordv).end() )	\
		    sprintf(buf,"key*%010u",++record_id_count);			\
		(*val.recordv).insert(buf,variant(v));				\
	    }									\
	    }									\
	    break;								\
    }										\
}

	PLACEIMPL(bool                ,BOOL    ,tostring ,                                             ,== true ? 1 : 0      ,== true ? 1 : 0, , , , )
	PLACEIMPL(std::complex<double>,COMPLEX ,tostring ,.real() == 0.0 && v.imag() == 0.0 ? false : true ,.real()              ,               , , , , )
	PLACEIMPL(int                ,INT     ,tostring ,== 0 ? false : true                          ,                     ,               , , , , )
	PLACEIMPL(double              ,DOUBLE  ,tostring ,== 0 ? false : true                          ,                     ,               , , , , )
	PLACEIMPL(const std::string&  ,STRING  ,         , , , ,stringtobool ,stringtoint,stringtodouble ,stringtocomplex )


	std::string variant::create_message( const std::string s ) const {
	    std::string type = (typev == BOOL ? "bool" :
				typev == INT ? "int" :
				typev == DOUBLE ? "double" :
				typev == STRING ? "string" :
				typev == BOOLVEC ? "boolvec" :
				typev == INTVEC ? "intvec" :
				typev == DOUBLEVEC ? "doublevec" :
				typev == STRINGVEC ? "stringvec" : "ErRoR");
	    return s + " " + type + " variant";
	}

	int variant::shape_size( ) const {
	    int result = 1;
	    for ( std::vector<int>::const_iterator iter = shape_.begin( );
		  iter != shape_.end( ); ++iter ) result *= *iter;
	    return result;
	}

	int variant::vec_size( ) const {
	    switch (typev) {
	    case BOOLVEC: return (*val.bv).size();
	    case INTVEC: return (*val.iv).size();
	    case DOUBLEVEC: return (*val.dv).size();
	    case COMPLEXVEC: return (*val.cv).size();
	    case STRINGVEC: return (*val.sv).size();
	    default: break;
	    }
	    return 1;
	}

	void variant::resize( int size ) {

	    if ( size < 0 ) return;

	    if ( size > 1 ) {
		switch (typev) {
		case BOOL:
		    asBoolVec(size);
		    break;
		case INT:
		    asIntVec(size);
		    break;
		case DOUBLE:
		    asDoubleVec(size);
		    break;
		case COMPLEX:
		    asComplexVec(size);
		    break;
		case STRING:
		    asStringVec(size);
		    break;
		case BOOLVEC:
		    (*val.bv).resize(size);
		    break;
		case INTVEC:
		    (*val.iv).resize(size);
		    break;
		case DOUBLEVEC:
		    (*val.dv).resize(size);
		    break;
		case COMPLEXVEC:
		    (*val.cv).resize(size);
		    break;
		case STRINGVEC:
		    (*val.sv).resize(size);
		    break;
		case RECORD:
		    break; // case not handled
		}
	    } else {
		switch (typev) {
		case BOOL:
		case INT:
		case DOUBLE:
		case COMPLEX:
		case STRING:
		case RECORD:
		    break;
		case BOOLVEC:
		    (*val.bv).resize(size);
		    break;
		case INTVEC:
		    (*val.iv).resize(size);
		    break;
		case DOUBLEVEC:
		    (*val.dv).resize(size);
		    break;
		case COMPLEXVEC:
		    (*val.cv).resize(size);
		    break;
		case STRINGVEC:
		    (*val.sv).resize(size);
		    break;
		}
	    }
	}

    }	// dbus namespace
}	// casa namespace
