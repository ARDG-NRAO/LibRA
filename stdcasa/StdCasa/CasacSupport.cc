#include <stdcasa/StdCasa/CasacSupport.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Exceptions/Error.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/ValueHolder.h>
#include <casa/Quanta/QuantumHolder.h>
#include <casa/Quanta/MVAngle.h>
#include <measures/Measures/MeasureHolder.h>
#include <measures/Measures/MeasTable.h>
#include <algorithm>

using namespace casacore;
namespace casa {

Vector<String> toVectorString(const std::vector<std::string> &theVec){
       Vector<String> b(theVec.size());
       for(unsigned int i=0; i<theVec.size(); i++)
          b[i] = String(theVec[i]);
       return(b);
}

String toCasaString(const casac::variant& var){

  String out;
  //Going around a bug of toString when its a string vector
  if(var.type()==::casac::variant::STRINGVEC){
    Vector<String> outvec=toVectorString(var.toStringVec());
    if(outvec.nelements() >0)
      out=outvec[0];
    for (uInt k =1; k < outvec.nelements(); ++k){
      out=out+String(",")+outvec[k];
    }
  }
  else{
    out=String(var.toString());
  }
  // [] protection
  if (out.length()>1 && out.firstchar()=='[' && out.lastchar()==']')
    out = out.substr(1,out.length()-2);

  return out;

}

std::vector<string> fromVectorString(const Vector<String> &theVec){
	std::vector<string> b(theVec.size());
       for(unsigned int i=0; i<theVec.size(); i++)
          b[i] = theVec[i].c_str();
       return(b);
}

casac::Quantity casacQuantity(const Quantity &quant){
	casac::Quantity cquant(std::vector<double>(1,0.0), "");
	cquant.value[0] = quant.getValue();
	cquant.units = quant.getUnit().c_str();
	return cquant;
}

Quantity casaQuantity(const casac::Quantity &cquant){
     if(cquant.value.size() < 1)
        throw(AipsError("Bad quantity conversion"));
     return Quantity(cquant.value[0], Unit(cquant.units));
}

Quantity casaQuantity(const casac::variant &theVar){
   casacore::QuantumHolder qh;
   String error;

   // Strange "defaults" like BOOLVECs can come in are expected to go out as
   // 0.0.  Therefore unhandled types should produce a default Quantity, not an
   // exception.
   Bool triedAndFailed = false;

   const ::casac::variant::TYPE theType = theVar.type();

   if(theType == ::casac::variant::STRING ||
      theType == ::casac::variant::STRINGVEC){
     triedAndFailed = !qh.fromString(error, theVar.toString());
   }
   else if(theType == ::casac::variant::RECORD){
     //NOW the record has to be compatible with QuantumHolder::toRecord
     ::casac::variant localvar(theVar); 	// Because theVar is const.
     std::unique_ptr<Record> ptrRec(toRecord(localvar.asRecord()));

     triedAndFailed = !qh.fromRecord(error, *ptrRec);
   }
   else if(::casac::variant::compatible_type(theType, ::casac::variant::DOUBLE)
	   == ::casac::variant::DOUBLE){
     const casacore::Unit unitless("_");	 		// Dimensionless

     //qh = casacore::QuantumHolder(casacore::Quantity(const_cast<Double &>(const_cast<casac::variant &>(theVar).asDouble()),
     //unitless));
     qh = casacore::QuantumHolder(casacore::Quantity(const_cast<Double &>(const_cast<casac::variant &>(theVar).asDouble())));

     triedAndFailed = false;
   }
   else if(::casac::variant::compatible_type(theType, ::casac::variant::COMPLEX)
	   == ::casac::variant::COMPLEX){
     const casacore::Unit unitless("_");	 		// Dimensionless
     const casacore::Complex casaVal(const_cast<casac::variant &>(theVar).asComplex());

     //qh = casacore::QuantumHolder(casacore::Quantum<casacore::Complex>(casaVal,
     //unitless));
     qh = casacore::QuantumHolder(casacore::Quantum<casacore::Complex>(casaVal));
     triedAndFailed = false;
   }

   if(triedAndFailed){
     ostringstream oss;

     oss << "Error " << error << " in converting quantity";
     throw(AipsError(oss.str()));
   }

   if(qh.isQuantum()){		// Remember casac::Quantity is a broader class
     return qh.asQuantity();	// than casacore::Quantity, so use qh.isQuantum().
   }
   else{			// Probably variant's type was not handled above.
     casacore::Quantity retval;	// Defaults to 0.0.

     return retval;
   }
}

Quantum<Vector<Double> > casaQuantumVector(const casac::variant& thevar){
  Quantum<Vector<Double> > retval(Vector<Double> (), Unit(""));
  //For now we know (at least till we have more time) how to deal with records only
  if(thevar.type() != ::casac::variant::RECORD){
    return retval;
  }
  ::casac::variant localvar(thevar); //cause its const
  std::unique_ptr<Record> ptrRec(toRecord(localvar.asRecord()));
  QuantumHolder qh;
  String error;
  if(qh.fromRecord(error, *ptrRec)){
    try {
      if(qh.isQuantumVectorDouble()){
	Quantum<Vector<Double> >retval1=qh.asQuantumVectorDouble();
	return retval1;
      }
    }
    catch(...){
      return retval;
    }
  }

  return retval;
}

Bool toCasaVectorQuantity(const ::casac::variant& theval, casacore::Vector<casacore::Quantity>& theQuants){

  casacore::Vector<casacore::String> lesStrings;
  if (theval.type()== ::casac::variant::STRING){
    sepCommaEmptyToVectorStrings(lesStrings, theval.toString());
  }
  else if(theval.type()== ::casac::variant::STRINGVEC){
    //Force resize as toStringVec sometimes give the wrong length (bug?)
    Vector<Int> leShape(theval.arrayshape());
    lesStrings=toVectorString(theval.toStringVec());
    lesStrings.resize(product(leShape), true);
  }

  casacore::QuantumHolder qh;
  String error;
  theQuants.resize(lesStrings.nelements());
  for (uInt k=0; k < lesStrings.nelements(); ++k){
    //Drat QuantumHolder does not handle pix
    if(lesStrings[k].contains("pix")){
      lesStrings[k]=lesStrings[k].before("pix");
      Double value=atof(lesStrings[k].chars());
      theQuants[k]=casacore::Quantity(value, "pix");
    }
    else{
      if(!qh.fromString(error, lesStrings[k])){
	ostringstream oss;
	oss << "Error " << error << " In converting quantity " << lesStrings[k];
	throw( AipsError(oss.str()));
      }
         theQuants[k]=qh.asQuantity();


    }
  }

  return true;

}

::casac::record* recordFromQuantity(const Quantity q)
{
  ::casac::record *r=0;
  try{
    String error;
    Record R;
    if(QuantumHolder(q).toRecord(error, R))
      r = fromRecord(R);
    else
      throw(AipsError("Could not convert quantity to record."));
  }
  catch(AipsError x){
    ostringstream oss;

    oss << "Exception Reported: " << x.getMesg();
    RETHROW(x);
  }
  return r;
}

::casac::record* recordFromQuantity(const Quantum<Vector<Double> >& q)
{
  ::casac::record *r=0;
  try {
    String error;
    casacore::Record R;
    if(QuantumHolder(q).toRecord(error, R))
      r = fromRecord(R);
    else
      throw(AipsError("Could not convert quantity to record."));
  }
  catch(AipsError x){
    ostringstream oss;

    oss << "Exception Reported: " << x.getMesg();
    RETHROW(x);
  }
  return r;
}

/*
 * Note to self, asArrayDouble doesn't cut it.  We'll have to do asType and convert element by element,
 * sigh.....
 */
::casac::record *fromRecord(const Record &theRec){
    ::casac::record *transcribedRec = new ::casac::record();
    for(uInt i=0; i<theRec.nfields(); i++){
	    // std::cerr << theRec.name(i) << " " << theRec.dataType(i) << std::endl;
       switch(theRec.dataType(i)){
          case TpBool :
               transcribedRec->insert(theRec.name(i).c_str(), theRec.asBool(i));
               break;
          case TpChar :
          case TpUChar :
               transcribedRec->insert(theRec.name(i).c_str(), long(theRec.asuChar(i)));
               break;
          case TpShort :
          case TpUShort :
               transcribedRec->insert(theRec.name(i).c_str(), long(theRec.asShort(i)));
               break;
          case TpUInt :
               transcribedRec->insert(theRec.name(i).c_str(), (unsigned long) theRec.asuInt(i));
               break;
          case TpInt :
               transcribedRec->insert(theRec.name(i).c_str(), long(theRec.asInt(i)));
               break;
          case TpInt64 :
	       {
               casac::variant val((long)theRec.asInt64(i));
               transcribedRec->insert(theRec.name(i).c_str(), val);
	       }
               break;
          case TpFloat :
               transcribedRec->insert(theRec.name(i).c_str(), double(theRec.asFloat(i)));
               break;
          case TpDouble :
               transcribedRec->insert(theRec.name(i).c_str(), theRec.asDouble(i));
               break;
          case TpComplex :
               transcribedRec->insert(theRec.name(i).c_str(), DComplex(theRec.asComplex(i)));
               break;
          case TpDComplex :
               transcribedRec->insert(theRec.name(i).c_str(), theRec.asDComplex(i));
               break;
          case TpString :
               transcribedRec->insert(theRec.name(i).c_str(), theRec.asString(i));
               break;
          case TpTable :
               transcribedRec->insert(theRec.name(i).c_str(), theRec.asString(i));
               break;
          case TpArrayBool :
               {
               Array<Bool> tmpArray = theRec.asArrayBool(i);
               //Vector<ssize_t> tmpShape = (tmpArray.shape()).asVector();
               Vector<ssize_t> tmpShape(tmpArray.shape( ).begin( ),tmpArray.shape( ).end( ));
	       //std::cerr << "Vector Shape " << tmpShape << std::endl;
               std::vector<ssize_t> vecShape;
               tmpShape.tovector(vecShape);
	       //std::cerr << "vector shape ";
	       //for(int i=0;i<vecShape.size();i++)
		       //std::cerr << vecShape[i] << " ";
	       //std::cerr << std::endl;
               std::vector<bool> tmpVec;
               tmpArray.tovector(tmpVec);
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(tmpVec, vecShape));
               }
               break;
          case TpArrayChar :
          case TpArrayUChar :
          case TpArrayShort :
          case TpArrayUShort :
          case TpArrayInt :
               {
               Array<int> tmpArray = theRec.asArrayInt(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
               std::vector<ssize_t> vecShape(tmpShape.begin(),tmpShape.end());
               std::vector<long> tmpVec;
               std::for_each( tmpArray.begin( ), tmpArray.end( ), [&](int v){tmpVec.push_back((long)v);} );
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(tmpVec, vecShape));
               }
               break;
          case TpArrayUInt :
               {
               Array<uInt> tmpArray = theRec.asArrayuInt(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
               std::vector<ssize_t> vecShape(tmpShape.begin( ),tmpShape.end( ));
               std::vector<unsigned long> tmpVec;
               std::for_each( tmpArray.begin( ), tmpArray.end( ), [&](uInt v){tmpVec.push_back((unsigned long)v);} );
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(tmpVec, vecShape));
               }
               break;
	  case TpArrayInt64 :
               {
               Array<Int64> tmpArray = theRec.asArrayInt64(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
               std::vector<ssize_t> vecShape(tmpShape.begin( ),tmpShape.end( ));
               std::vector<long> tmpVec(tmpArray.begin(),tmpArray.end());
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(tmpVec, vecShape));
               }
               break;
          case TpArrayFloat :
	       {
               Array<Float> tmpArray = theRec.asArrayFloat(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
	       //std::cerr << "Vector Shape " << tmpShape << std::endl;
               std::vector<ssize_t> vecShape(tmpShape.begin( ),tmpShape.end( ));
               std::vector<Float> tmpVec;
               tmpArray.tovector(tmpVec);
	       std::vector<Double> dtmpVec(tmpVec.size());
	       for(unsigned int j=0;j<tmpVec.size();j++)
	           dtmpVec[j] = tmpVec[j];
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(dtmpVec, vecShape));
	       }
	       break;
          case TpArrayDouble :
               {
               Array<Double> tmpArray = theRec.asArrayDouble(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
	       //std::cerr << "Vector Shape " << tmpShape << std::endl;
               std::vector<ssize_t> vecShape(tmpShape.begin( ),tmpShape.end( ));
               std::vector<Double> tmpVec;
               tmpArray.tovector(tmpVec);
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(tmpVec, vecShape));
               }
               break;
          case TpArrayComplex :
               {
               Array<casacore::Complex> tmpArray = theRec.asArrayComplex(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
               std::vector<ssize_t> vecShape(tmpShape.begin( ),tmpShape.end( ));
               std::vector<casacore::Complex> tmpVec;
               tmpArray.tovector(tmpVec);
	       std::vector<std::complex<double> > dtmpVec(tmpVec.size());
	       for(unsigned int j=0;j<tmpVec.size();j++)
	           dtmpVec[j] = tmpVec[j];
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(dtmpVec, vecShape));
               }
	       break;
          case TpArrayDComplex :
               {
               Array<DComplex> tmpArray = theRec.asArrayDComplex(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
               std::vector<ssize_t> vecShape(tmpShape.begin( ),tmpShape.end( ));
               std::vector<DComplex> tmpVec;
               tmpArray.tovector(tmpVec);
	       std::vector<std::complex <double> > dtmpVec(tmpVec.size());
	       for(unsigned int j=0;j<tmpVec.size();j++)
	           dtmpVec[j] = tmpVec[j];
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(dtmpVec, vecShape));
               }
               break;
          case TpArrayString :
               {
               Array<String> tmpArray = theRec.asArrayString(i);
               Vector<Int> tmpShape = (tmpArray.shape()).asVector();
               std::vector<ssize_t> vecShape(tmpShape.begin( ),tmpShape.end( ));
               std::vector<casacore::String> tmpVec;
               tmpArray.tovector(tmpVec);
	       std::vector<std::string> dtmpVec(tmpVec.size());
	       for(unsigned int j=0;j<tmpVec.size();j++)
	           dtmpVec[j] = tmpVec[j].c_str();
               transcribedRec->insert(theRec.name(i).c_str(), casac::variant(dtmpVec, vecShape));
               }
               break;
          case TpRecord :
	       {
	       //std::cerr << "fromRecord is Record" << std::endl;
	       casac::record *dummy = fromRecord(theRec.asRecord(i));
	       casac::variant dum2(dummy);
               transcribedRec->insert(theRec.name(i).c_str(), dum2);
	       }
               break;
          case TpOther :
	       std::cerr << "casac::fromRecord TpOther, oops" << std::endl;
               break;
          case TpQuantity :
	       std::cerr << "casac::fromRecord TpQuantity, oops" << std::endl;
               break;
          case TpArrayQuantity :
	       std::cerr << "casac::fromRecord TpArrayQuantity, oops" << std::endl;
               break;
          default :
	       std::cerr << "casac::fromRecord unknown type, oops" << std::endl;
               break;
       }
    }
    return transcribedRec;
}

Record *toRecord(const ::casac::record &theRec){
    Record *transcribedRec = new Record();
    casac::rec_map::const_iterator rec_end = theRec.end();
    for(::casac::rec_map::const_iterator rec_it = theRec.begin(); rec_it != rec_end; ++rec_it){
	//std::cerr << (*rec_it).first << " type is: " << (*rec_it).second.type() << std::endl;
        switch((*rec_it).second.type()){
            case ::casac::variant::RECORD :
               {
               std::unique_ptr<Record> tmpRecord(toRecord((*rec_it).second.getRecord()));
               transcribedRec->defineRecord(RecordFieldId((*rec_it).first), *tmpRecord);
               }
               break;
            case ::casac::variant::BOOL :
              {transcribedRec->define(RecordFieldId((*rec_it).first), (*rec_it).second.toBool());}
               break;
            case ::casac::variant::INT :
              {transcribedRec->define(RecordFieldId((*rec_it).first), Int((*rec_it).second.getInt()));}
               break;
            case ::casac::variant::UINT :
              {transcribedRec->define(RecordFieldId((*rec_it).first), Int((*rec_it).second.getuInt()));}
               break;
            case ::casac::variant::DOUBLE :
              transcribedRec->define(RecordFieldId((*rec_it).first), (*rec_it).second.getDouble());
               break;
            case ::casac::variant::COMPLEX :
              transcribedRec->define(RecordFieldId((*rec_it).first), (*rec_it).second.getComplex());
               break;
            case ::casac::variant::STRING :
              transcribedRec->define(RecordFieldId((*rec_it).first), (*rec_it).second.getString());
               break;
            case ::casac::variant::BOOLVEC :
               {
	       Vector<Int> shapeVec((*rec_it).second.arrayshape());
               Vector<Bool>boolVec((*rec_it).second.getBoolVec());
               IPosition tshape(shapeVec);
               Array<Bool> boolArr(tshape);
               int i(0);
	       Array<Bool>::iterator boolArrend = boolArr.end();
               for(Array<Bool>::iterator iter = boolArr.begin();
                                         iter != boolArrend; ++iter)
                   *iter = boolVec[i++];
               transcribedRec->define(RecordFieldId((*rec_it).first), boolArr);
               }
               break;
            case ::casac::variant::INTVEC :
               {
               Vector<Int> shapeVec((*rec_it).second.arrayshape());
               Vector<Int> intVec((*rec_it).second.getIntVec());
               IPosition tshape(shapeVec);
               Array<Int> intArr(tshape);
               int i(0);
	       Array<Int>::iterator intArrend = intArr.end();
               for(Array<Int>::iterator iter = intArr.begin();
                                         iter != intArrend; ++iter)
                   *iter = intVec[i++];
               transcribedRec->define(RecordFieldId((*rec_it).first), intArr);
               }
               break;
            case ::casac::variant::UINTVEC :
               {
               Vector<Int> shapeVec((*rec_it).second.arrayshape());
               Vector<uInt> uintVec((*rec_it).second.getuIntVec());
               IPosition tshape(shapeVec);
               Array<uInt> intArr(tshape);
               int i(0);
               Array<uInt>::iterator intArrend = intArr.end();
               for(Array<uInt>::iterator iter = intArr.begin();
                                         iter != intArrend; ++iter)
                   *iter = uintVec[i++];
               transcribedRec->define(RecordFieldId((*rec_it).first), intArr);
               }
               break;
            case ::casac::variant::DOUBLEVEC :
               {
               Vector<Double> doubleVec((*rec_it).second.getDoubleVec());
               Vector<Int> shapeVec((*rec_it).second.arrayshape());
               IPosition tshape(shapeVec);
               Array<Double> doubleArr(tshape);
               int i(0);
	       Array<Double>::iterator doubleArrEnd = doubleArr.end();
               for(Array<Double>::iterator iter = doubleArr.begin();
                                         iter != doubleArrEnd; ++iter)
                   *iter = doubleVec[i++];
               transcribedRec->define(RecordFieldId((*rec_it).first), doubleArr);
               }
               break;
            case ::casac::variant::COMPLEXVEC :
               {
               Vector<DComplex> complexVec((*rec_it).second.getComplexVec());
               Vector<Int> shapeVec((*rec_it).second.arrayshape());
               IPosition tshape(shapeVec);
               Array<DComplex> complexArr(tshape);
	       Array<DComplex>::iterator complexArrEnd = complexArr.end();
               int i(0);
               for(Array<DComplex>::iterator iter = complexArr.begin();
                                         iter != complexArrEnd; ++iter)
                   *iter = complexVec[i++];
               transcribedRec->define(RecordFieldId((*rec_it).first), complexArr);
               }
               break;
            case ::casac::variant::STRINGVEC :
               {
               Vector<Int> shapeVec((*rec_it).second.arrayshape());
               std::vector<std::string> tmp = (*rec_it).second.getStringVec();
               const Array<String> &stringArr(toVectorString(tmp));
               //stringArr.reform(IPosition(shapeVec));
               transcribedRec->define(RecordFieldId((*rec_it).first), stringArr);
		 //transcribedRec->define((*rec_it).first, Vector<String>((*rec_it).second.getStringVec()));
               }
               break;
            default :
	       std::cerr << "Unknown type: " << (*rec_it).second.type() << std::endl;
               break;
        }
    }
    return transcribedRec;
}

casac::variant *fromValueHolder(const ValueHolder &theVH){
	casac::variant *theV(0);
        if(!theVH.isNull()){
	  switch(theVH.dataType()){
            case TpBool :
	         theV = new casac::variant(theVH.asBool());
                 break;
            case TpUChar :
	         theV = new casac::variant((long)theVH.asuChar());
                 break;
            case TpShort :
	         theV = new casac::variant((long)theVH.asShort());
                 break;
            case TpUShort :
	         theV = new casac::variant((long)theVH.asuShort());
                 break;
            case TpInt :
	         theV = new casac::variant((long)theVH.asInt());
                 break;
            case TpUInt :
                 theV = new casac::variant((unsigned long)theVH.asuInt());
                 break;
            case TpInt64 :
                theV = new casac::variant((long)theVH.asInt64());
                 break;
            case TpFloat :
	         theV = new casac::variant(theVH.asFloat());
                 break;
            case TpDouble :
	         theV = new casac::variant(theVH.asDouble());
                 break;
            case TpComplex :
	         theV = new casac::variant(theVH.asComplex());
                 break;
            case TpDComplex :
	         theV = new casac::variant(theVH.asDComplex());
                 break;
            case TpString :
	         theV = new casac::variant(theVH.asString().c_str());
                 break;
            case TpArrayBool :
	         {
		    Array<Bool> tArr(theVH.asArrayBool());
		    Vector<Int> ts = tArr.shape().asVector();
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<bool> tData;
		    tArr.tovector(tData);
		    theV = new casac::variant(tData, tShape);
	         }
                 break;
            case TpArrayUChar :
	         {
		    Array<uChar> tArr(theVH.asArrayuChar());
		    Vector<Int> ts = tArr.shape().asVector();
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<uChar> tData;
		    tArr.tovector(tData);
		    std::vector<long> tD(tData.size());
		    for(unsigned int i=0;i<tData.size();i++)
			    tD[i] = tData[i];
		    theV = new casac::variant(tD, tShape);
	         }
                 break;
            case TpArrayShort :
	         {
		    Array<Short> tArr(theVH.asArrayShort());
		    Vector<Int> ts = tArr.shape().asVector();
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<Short> tData;
		    tArr.tovector(tData);
		    std::vector<long> tD(tData.size());
		    for(unsigned int i=0;i<tData.size();i++)
			    tD[i] = tD[i];
		    theV = new casac::variant(tD, tShape);
	         }
                 break;
            case TpArrayUShort :
	         {
		    Array<uShort> tArr(theVH.asArrayuShort());
		    Vector<Int> ts = tArr.shape().asVector();
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<uShort> tData;
		    tArr.tovector(tData);
		    std::vector<long> tD(tData.size());
		    for(unsigned int i=0;i<tData.size();i++)
			    tD[i] = tData[i];
		    theV = new casac::variant(tD, tShape);
	         }
                 break;
            case TpArrayInt :
	         {
		    Array<Int> tArr(theVH.asArrayInt());
		    Vector<Int> ts = tArr.shape().asVector();
	            //std::cerr << "Vector Shape " << ts << std::endl;
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<long> tData;
            std::for_each( tArr.begin( ), tArr.end( ), [&](Int v){tData.push_back((long)v);} );
		    theV = new casac::variant(tData, tShape);
	         }
                 break;
            case TpArrayUInt :
                 {
                    Array<uInt> tArr(theVH.asArrayuInt());
                    Vector<Int> ts = tArr.shape().asVector();
                    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
                    std::vector<unsigned long> tData;
                    std::for_each( tArr.begin( ), tArr.end( ), [&](Int v){tData.push_back((unsigned long)v);} );
                    theV = new casac::variant(tData, tShape);
                 }
                 break;
            case TpArrayFloat :
	         {
		    Array<Float> tArr(theVH.asArrayFloat());
		    Vector<Int> ts = tArr.shape().asVector();
		    //std::cerr << ts << std::endl;
	            //std::cerr << "Vector Shape " << ts << std::endl;
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<float> tData;
		    tArr.tovector(tData);
		    std::vector<double> tD(tData.size());
		    for(unsigned int i=0;i<tData.size();i++)
			    tD[i] = tData[i];
		    theV = new casac::variant(tD, tShape);
	         }
	         break;
            case TpArrayDouble :
	         {
		    Array<Double> tArr(theVH.asArrayDouble());
		    Vector<Int> ts = tArr.shape().asVector();
	            //std::cerr << "Vector Shape " << ts << std::endl;
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<double> tData;
		    tArr.tovector(tData);
		    theV = new casac::variant(tData, tShape);
	         }
                 break;
            case TpArrayComplex :
	         {
		    Array<Complex> tArr(theVH.asArrayComplex());
		    Vector<Int> ts = tArr.shape().asVector();
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<std::complex<float> > tData;
		    tArr.tovector(tData);
		    std::vector<std::complex<double> > tD(tData.size());
		    for(unsigned int i=0;i<tData.size();i++)
			    tD[i] = tData[i];
		    theV = new casac::variant(tD, tShape);
	         }
                 break;
            case TpArrayDComplex :
	         {
		    Array<DComplex> tArr(theVH.asArrayDComplex());
		    Vector<Int> ts = tArr.shape().asVector();
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<std::complex<double> > tData;
		    tArr.tovector(tData);
		    theV = new casac::variant(tData, tShape);
	         }
                 break;
            case TpArrayString :
	         {
		         // Truly ugly...
		    Array<String> tArr(theVH.asArrayString());
		    Vector<Int> ts = tArr.shape().asVector();
		    std::vector<ssize_t> tShape(ts.begin( ),ts.end( ));
		    std::vector<String> tData;
		    tArr.tovector(tData);
	            std::vector<string> tData2(tData.size());
                    for(unsigned int i=0; i<tData.size(); i++)
                        tData2[i] = tData[i].c_str();
		    theV = new casac::variant(tData2, tShape);
	         }
                 break;
            case TpRecord :
		    theV = new casac::variant(fromRecord(theVH.asRecord()));
                 break;
	    default :
                 throw(AipsError("Unknown casa DataType!"));
	         break;
	  }
        }
	return theV;
}

// I have some doubts about this, we may need to avoid the const ness of the variant
// if we run into some speed issues.
//
ValueHolder *toValueHolder(const casac::variant &theV){
	ValueHolder *theVH(0);
	std::unique_ptr<Record> tmpRec;
	switch(theV.type()){
		case casac::variant::RECORD :
			tmpRec.reset(toRecord(theV.getRecord()));
			theVH = new ValueHolder(*tmpRec);
			break;
		case casac::variant::BOOL :
			theVH = new ValueHolder(theV.getBool());
			break;
		case casac::variant::INT :
			/*** need range check ***/
			theVH = new ValueHolder((int)theV.getInt());
			break;
		case casac::variant::DOUBLE :
			theVH = new ValueHolder(theV.getDouble());
			break;
		case casac::variant::COMPLEX :
			theVH = new ValueHolder(theV.getComplex());
			break;
		case casac::variant::STRING :
			theVH = new ValueHolder(theV.getString());
			break;
		case casac::variant::BOOLVEC :
			{
			Vector<Bool> boolVec(theV.getBoolVec());
			Vector<Int>shapeVec(theV.arrayshape());
                        IPosition tshape(shapeVec);
			Array<Bool> tVec(tshape);
                        int i(0);
			Array<Bool>::iterator tVecEnd = tVec.end();
                        for(Array<Bool>::iterator iter = tVec.begin();
                                         iter != tVecEnd; ++iter)
                            *iter = boolVec[i++];
			theVH = new ValueHolder(tVec);
			}
			break;
		case casac::variant::INTVEC :
			{
			Vector<Int> intVec(theV.getIntVec());
			Vector<Int>shapeVec(theV.arrayshape());
                        IPosition tshape(shapeVec);
			Array<Int> tVec(tshape);
                        int i(0);
			Array<Int>::iterator tVecEnd = tVec.end();
                        for(Array<Int>::iterator iter = tVec.begin();
                                         iter != tVecEnd; ++iter)
                            *iter = intVec[i++];
			theVH = new ValueHolder(tVec);
			}
			break;
		case casac::variant::DOUBLEVEC :
			{
			Vector<Double> doubleVec(theV.getDoubleVec());
			Vector<Int>shapeVec(theV.arrayshape());
                        IPosition tshape(shapeVec);
			Array<Double> tVec(tshape);
			Array<Double>::iterator tVecEnd = tVec.end();
                        int i(0);
                        for(Array<Double>::iterator iter = tVec.begin();
                                         iter != tVecEnd; ++iter)
                            *iter = doubleVec[i++];
			theVH = new ValueHolder(tVec);
			}
			break;
		case casac::variant::COMPLEXVEC :
			{
			Vector<DComplex> complexVec(theV.getComplexVec());
			Vector<Int>shapeVec(theV.arrayshape());
                        IPosition tshape(shapeVec);
			Array<DComplex> tVec(tshape);
			Array<DComplex>::iterator tVecEnd = tVec.end();
                        int i(0);
                        for(Array<DComplex>::iterator iter = tVec.begin();
                                         iter != tVecEnd; ++iter)
                            *iter = complexVec[i++];
			theVH = new ValueHolder(tVec);
			}
			break;
		case casac::variant::STRINGVEC :
			// Unclear whether this may leak
			{
                        std::vector<std::string> tmp = theV.getStringVec();
			Vector<String> stringArr(toVectorString(tmp));
			Vector<Int>shapeVec(theV.arrayshape());
                        IPosition tshape(shapeVec);
                        Array<String> tVec(tshape);
			int i(0);
			Array<String>::iterator tVecEnd = tVec.end();
                        for(Array<String>::iterator iter = tVec.begin();
                                         iter != tVecEnd; ++iter)
                            *iter = stringArr[i++];
			theVH = new ValueHolder(tVec);
			}
			break;
		default :
                        throw(AipsError("Unknown variant type!"));
			break;
	}
	return theVH;
}

Bool casaMDirection(const ::casac::variant& theVar,
		    MDirection& theMeas){

  casacore::MeasureHolder mh;
  casacore::QuantumHolder qh;
  String error;
  if(theVar.type()== ::casac::variant::RECORD){
    ::casac::variant localvar(theVar); //cause its const
    std::unique_ptr<Record> ptrRec(toRecord(localvar.asRecord()));
    if(mh.fromRecord(error, *ptrRec)){
      theMeas=mh.asMDirection();
      return true;
    }
    else{
       ostringstream oss;
       oss << "Error " << error <<  " in converting Direction parameter";
       throw( AipsError(oss.str()));
       return false;
    }
  }
  if(theVar.type()== ::casac::variant::STRING ||
     theVar.type()== ::casac::variant::STRINGVEC){
    Vector<String> str;
    //In case of compound strings with commas or empty space
    if (theVar.type()== ::casac::variant::STRING){
      sepCommaEmptyToVectorStrings(str, theVar.toString());
    }
    else{
      str=toVectorString(theVar.toStringVec());
    }
    if(str.nelements()==3){
      qh.fromString(error, str[1]);
      casacore::Quantity val1=qh.asQuantity();
      qh.fromString(error, str[2]);
      casacore::Quantity val2=qh.asQuantity();
      if(val2.getFullUnit()==Unit("deg") && str[2].contains(":")){
	ostringstream oss;
	oss << "NOTE: you provided the Declination/Latitude value \""<< str[2]
	    << "\" which is understood to be in units of hours.\n"
	    << "      If you meant degrees, please replace \":\" by \".\".";
	cerr << oss.str() << endl;
      }
      MDirection::Types tp;
      if(!MDirection::getType(tp, str[0])){
	ostringstream oss;
	oss << "Could not understand Direction frame...defaulting to J2000 " ;
	cerr << oss.str() << endl;
	tp=MDirection::J2000;
      }
      theMeas=MDirection(val1,val2,  tp);
      return true;
    }
    else if(str.nelements()==2){
      qh.fromString(error, str[0]);
      casacore::Quantity val1=qh.asQuantity();
      qh.fromString(error, str[1]);
      casacore::Quantity val2=qh.asQuantity();
      if(val2.getFullUnit()==Unit("deg") && str[1].contains(":")){
	ostringstream oss;
	oss << "NOTE: you provided the Declination/Latitude value \""<< str[2]
	    << "\" which is understood to be in units of hours.\n"
	    << "      If you meant degrees, please replace \":\" by \".\".";
	cerr << oss.str() << endl;
      }
      theMeas=MDirection(val1, val2);
      return true;
    }
    else if(str.nelements()==1){
      //Must be a string like sun, moon, jupiter
      casacore::Quantity val1(0.0, "deg");
      casacore::Quantity val2(90.0, "deg");
      theMeas=MDirection(val1, val2);
      MDirection::Types ref;
      Int numAll;
      Int numExtra;
      const uInt *dum;
      const String *allTypes=MDirection::allMyTypes(numAll, numExtra, dum);
      //if it is SUN moon etc
      if(MDirection::getType(ref,str[0])){

	theMeas=MDirection(val1, val2, ref);
	return true;
      }
      if(MeasTable::Source(theMeas, str[0])){
	return true;
      }
      if(!MDirection::getType(ref, str[0])){
	Vector<String> all(numExtra);
        for(Int k =0; k < numExtra; ++k){
	  all[k]=*(allTypes+numAll-k-1);
	}
	ostringstream oss;
	oss << "Could not understand Direction string " <<str[0] << "\n"  ;
	oss << "Valid ones are " << all;
	cerr << oss.str() <<  " or one of the valid known sources in the data repos" << endl;
	theMeas=MDirection(val1, val2);
	return false;
      }

    }

  }
  ///If i am here i don't know how to interprete this


  return false;
}

//Bool ang_as_formatted_str(string& out, const casacore::Quantity& qang,
//                          const std::string& format)
//{
//  return ang_as_formatted_str(out,qang,format,precision=2);
//}

Bool ang_as_formatted_str(string& out, const casacore::Quantity& qang,
                          const std::string& format, const Int precision=2)
{
  Bool retval = true;

  try{
    //hms, dms, deg, rad, +deg.
    casacore::String form(format);
    form.downcase();

    Int ndig=6+precision;

    MVAngle ang(qang);
    if(form == "dms"){
      out = ang(-0.5).string(MVAngle::ANGLE, ndig+1).c_str();
    }
    else if(form == "hms"){
      out = ang.string(MVAngle::TIME, ndig).c_str();
    }
    else if(form == "deg"){
      ostringstream os;
      os << ang().degree();
      out = os.str();
    }
    else if(form == "rad"){
      ostringstream os;
      os << ang().radian();
      out = os.str();
    }
    else if(form == "+deg"){
      ostringstream os;
      os << ang(0.0).degree();
      out = os.str();
    }
    else{
      retval = false;  // Format not understood - return false instead of
                       // throwing an exception.
    }
  }
  catch(AipsError x){
    retval = false;
    RETHROW(x);
  }
  return retval;
}

Bool MDirection2str(const MDirection& in, std::string& out)
{
  Quantum<Vector<Double> > lonlat(in.getAngle());
  Vector<Double> lonlatval(lonlat.getValue());
  Unit           inunit(lonlat.getUnit());
  string refcode(in.getRefString());

  casacore::Quantity qlon(lonlatval[0], inunit);
  casacore::Quantity qlat(lonlatval[1], inunit);

  string lon("");
  string lat("");
  Bool success;
  if(refcode == "J2000" || refcode[0] == 'B'){
    success = ang_as_formatted_str(lon, qlon, "hms", 2);
    success = success && ang_as_formatted_str(lat, qlat, "dms", 2);
  }
  else{
    success = ang_as_formatted_str(lon, qlon, "deg", 2);
    success = success && ang_as_formatted_str(lat, qlat, "deg", 2);
  }

  if(success)
    out = refcode + " " + lon + " " + lat;
  return success;
}

Bool casaMFrequency(const ::casac::variant& theVar,
		    MFrequency& theMeas){
  casacore::MeasureHolder mh;
  casacore::QuantumHolder qh;
  String error;
  if(theVar.type()== ::casac::variant::RECORD){
    ::casac::variant localvar(theVar); //cause its const
    std::unique_ptr<Record> ptrRec(toRecord(localvar.asRecord()));
    if(mh.fromRecord(error, *ptrRec)){
      theMeas=mh.asMFrequency();
      return true;
    }
    else{//could be a quantity
      if(qh.fromRecord(error, *ptrRec)){
	theMeas=MFrequency(qh.asQuantity());
	return true;
      }
      else{
	ostringstream oss;
	oss << "Error " << error
            << "In converting Frequency parameter";
	throw( AipsError(oss.str()));
	return false;
      }
    }
  }
  if(theVar.type()== ::casac::variant::STRING ||
     theVar.type()== ::casac::variant::STRINGVEC){
    Vector<String> str;
    if (theVar.type()== ::casac::variant::STRING){
      sepCommaEmptyToVectorStrings(str, theVar.toString());
    }
    else{
      str=toVectorString(theVar.toStringVec());
    }
    if(str.nelements()==2){
      qh.fromString(error, str[1]);
      casacore::Quantity val=qh.asQuantity();
      MFrequency::Types tp;
      if(!MFrequency::getType(tp, str[0])){

	cerr << "Could not understand frequency frame..defaulting to LSRK "
	     << endl;
	tp=MFrequency::LSRK;
	qh.fromString(error, str[0]+str[1]);
	val=qh.asQuantity();
      }
      theMeas=MFrequency(val, tp);
      return true;
    }
    else if(str.nelements()==1){
      if(str[0]=="")
	return false;
      qh.fromString(error, str[0]);
      theMeas=MFrequency(qh.asQuantity());
      return true;
    }
  }
  ///If i am here then it can be an integer or double then using Hz
  theMeas=MFrequency(casacore::Quantity(theVar.toDouble(), "Hz"));


  return true;

}

Bool casaMPosition(const ::casac::variant& theVar,
		   MPosition& theMeas){
  casacore::MeasureHolder mh;
  casacore::QuantumHolder qh;
  String error;
  if(theVar.type()== ::casac::variant::RECORD){
    ::casac::variant localvar(theVar); //cause its const
    std::unique_ptr<Record> ptrRec(toRecord(localvar.asRecord()));
    if(mh.fromRecord(error, *ptrRec)){
      theMeas=mh.asMPosition();
      return true;
    }
    else{
      ostringstream oss;
      oss << "Error " << error << "In converting Position parameter";
      throw( AipsError(oss.str()));
      return false;
    }
  }
  if(theVar.type()== ::casac::variant::STRING ||
     theVar.type()== ::casac::variant::STRINGVEC){
    Vector<String> str;
    if (theVar.type()== ::casac::variant::STRING){
      sepCommaEmptyToVectorStrings(str, theVar.toString());
    }
    else{
      str=toVectorString(theVar.toStringVec());
    }
    if(str.nelements()==4){
      qh.fromString(error, str[1]);
      casacore::Quantity val1=qh.asQuantity();
      qh.fromString(error, str[2]);
      casacore::Quantity val2=qh.asQuantity();
      qh.fromString(error, str[3]);
      casacore::Quantity val3=qh.asQuantity();
      MPosition::Types tp;
      if(!MPosition::getType(tp, str[0])){
	cerr << "Could not understand Position frame...defaulting to ITRF"
	     << endl;
	tp=MPosition::ITRF;
      }
      theMeas=MPosition(val1, val2, val3, tp);
      return true;
    }
    else if(str.nelements()==3){
      qh.fromString(error, str[0]);
      casacore::Quantity val1=qh.asQuantity();
      qh.fromString(error, str[1]);
      casacore::Quantity val2=qh.asQuantity();
      qh.fromString(error, str[2]);
      casacore::Quantity val3=qh.asQuantity();
      theMeas=MPosition(val1, val2, val3);
      return true;
    }
    else if(str.nelements()==1){
      //must be an observatory string hopefully
      str[0].upcase();
      if(casacore::MeasTable::Observatory(theMeas, str[0])){
	return true;
      }
      else{
	throw( AipsError("Could not understand Position parameter"));
	return false;
      }
    }
  }
  ///If i am here i don't know how to interprete this


  return false;


}


Bool casaMRadialVelocity(const ::casac::variant& theVar,
			 MRadialVelocity& theMeas){
  casacore::MeasureHolder mh;
  casacore::QuantumHolder qh;
  String error;
  if(theVar.type()== ::casac::variant::RECORD){
    ::casac::variant localvar(theVar); //cause its const
    std::unique_ptr<Record> ptrRec(toRecord(localvar.asRecord()));
    if(mh.fromRecord(error, *ptrRec)){
      theMeas=mh.asMRadialVelocity();
      return true;
    }
    else{//could be a quantity
      if(qh.fromRecord(error, *ptrRec)){
	theMeas=MRadialVelocity(qh.asQuantity());
	return true;
      }
      else{
	ostringstream oss;
	oss << "Error " << error
	      << "In converting Radial velocity parameter";
	throw( AipsError(oss.str()));
	return false;
      }
    }
  }
  if(theVar.type()== ::casac::variant::STRING ||
     theVar.type()== ::casac::variant::STRINGVEC){
    Vector<String> str;
    if (theVar.type()== ::casac::variant::STRING){
      sepCommaEmptyToVectorStrings(str, theVar.toString());
    }
    else{
      str=toVectorString(theVar.toStringVec());
    }
    if(str.nelements()==2){
      qh.fromString(error, str[1]);
      casacore::Quantity val=qh.asQuantity();
      MRadialVelocity::Types tp;
      if(!MRadialVelocity::getType(tp, str[0])){
	cerr << "Could not understand velocity frame..defaulting to LSRK "
	     << endl;
	tp=MRadialVelocity::LSRK;
      }
      theMeas=MRadialVelocity(val, tp);
      return true;
    }
    else if(str.nelements()==1){
      if(str[0]=="")
	return false;
      qh.fromString(error, str[0]);
      theMeas=MRadialVelocity(qh.asQuantity());
      return true;
    }
  }
  ///If i am here then it can be an integer or double then using m/s
  theMeas=MRadialVelocity(casacore::Quantity(theVar.toDouble(), "m/s"));

  return true;
}

Bool casaMEpoch(const ::casac::variant& theVar,
                MEpoch& theMeas){

  casacore::MeasureHolder mh;
  casacore::QuantumHolder qh;
  String error;
  if(theVar.type()== ::casac::variant::RECORD){
    ::casac::variant localvar(theVar); //cause its const
    std::unique_ptr<Record> ptrRec(toRecord(localvar.asRecord()));
    if(mh.fromRecord(error, *ptrRec)){
      theMeas=mh.asMEpoch();
      return true;
    }
    else{//could be a quantity
      if(qh.fromRecord(error, *ptrRec)){
	theMeas=MEpoch(qh.asQuantity());
	return true;
      }
      else{
	ostringstream oss;
	oss << "Error " << error
	      << "In converting Epoch parameter";
	throw( AipsError(oss.str()));
	return false;
      }
    }
  }
  if(theVar.type()== ::casac::variant::STRING ||
     theVar.type()== ::casac::variant::STRINGVEC){
    Vector<String> str;
    if (theVar.type()== ::casac::variant::STRING){
      sepCommaEmptyToVectorStrings(str, theVar.toString());
    }
    else{
      str=toVectorString(theVar.toStringVec());
    }
    if(str.nelements()==2){
      qh.fromString(error, str[1]);
      casacore::Quantity val=qh.asQuantity();
      MEpoch::Types tp;
      if(!MEpoch::getType(tp, str[0])){
	cerr << "Could not understand epoch frame...defaulting to UTC "
	     << endl;
	tp=MEpoch::UTC;
      }
      theMeas=MEpoch(val, tp);
      return true;
    }
    else if(str.nelements()==1){
      if(str[0]=="")
	return false;
      qh.fromString(error, str[0]);
      theMeas=MEpoch(qh.asQuantity());
      return true;
    }
  }
  ///If i am here then it can be an integer or double then using days
  theMeas=MEpoch(casacore::Quantity(theVar.toDouble(), "d"));


  return true;


}
Int sepCommaEmptyToVectorStrings(Vector<String>& lesStrings,
				 const std::string& str){

    casacore::String oneStr=String(str);
    Int nsep=0;
    // decide if its comma seperated or empty space seperated
    casacore::String sep;
    if((nsep=oneStr.freq(",")) > 0){
      sep=",";
    }
    else {
      nsep=oneStr.freq(" ");
      sep=" ";
    }
    if(nsep == 0){
      lesStrings.resize(1);
      lesStrings=oneStr;
      nsep=1;
    }
    else{
      String *splitstrings = new String[nsep+1];
      nsep=split(oneStr, splitstrings, nsep+1, sep);
      lesStrings.resize(nsep);
      Int index=0;
      for (Int k=0; k < nsep; ++k){
	if((String(splitstrings[k]) == String(""))
	   || (String(splitstrings[k]) == String(" "))){
	  lesStrings.resize(lesStrings.nelements()-1, true);
	}
	else{
	  lesStrings[index]=splitstrings[k];
	  ++index;
	}
      }
      delete [] splitstrings;
    }

    return nsep;

}

Int sepCommaToVectorStrings(Vector<String>& lesStrings,
				 const std::string& str){

    casacore::String oneStr=String(str);
    // decide if its comma seperated or empty space seperated
    String sep=",";
    Int nsep=oneStr.freq(sep);
    if(nsep == 0){
      lesStrings.resize(1);
      lesStrings=oneStr;
      nsep=1;
    }
    else{
      String *splitstrings = new String[nsep+1];
      nsep=split(oneStr, splitstrings, nsep+1, sep);
      lesStrings.resize(nsep);
      Int index=0;
      for (Int k=0; k < nsep; ++k){
	if((String(splitstrings[k]) == String(""))
	   || (String(splitstrings[k]) == String(" "))){
	  lesStrings.resize(lesStrings.nelements()-1, true);
	}
	else{
	  lesStrings[index]=splitstrings[k];
	  ++index;
	}
      }
      delete [] splitstrings;
    }

    return nsep;

}

vector<double> toVectorDouble(const casac::variant& v, const String& varName) {
	vector<double> ret(0);
	switch (v.type()) {
	case casac::variant::BOOLVEC:
		return ret;
	case casac::variant::INT:
	case casac::variant::DOUBLE:
		ret.push_back(v.toDouble());
		return ret;
	case casac::variant::INTVEC:
	case casac::variant::DOUBLEVEC:
		return v.toDoubleVec();
	case casac::variant::STRING:
		if (v.toString().size() > 0) {
			throw AipsError(varName + " cannot be a non-empty string");
		}
		else {
			return ret;
		}
	default:
		ostringstream os;
		os << "Illegal type for " << varName << ": " << v.type();
		throw AipsError(os.str());
	}
}

vector<string> toVectorString(const ::casac::variant& v, const String& varName) {
	vector<string> ret(0);
	switch (v.type()) {
	case casac::variant::BOOLVEC:
		return ret;
	case casac::variant::STRING:
		if (v.toString().size() > 0) {
			ret.push_back(v.toString());
		}
		return ret;
	case casac::variant::STRINGVEC:
		return v.toStringVec();
	default:
		ostringstream os;
		os << "Illegal type for " << varName;
		throw AipsError(os.str());
	}
}

vector<int> toVectorInt(const ::casac::variant& v, const String& varName) {
	vector<int> ret(0);
	switch (v.type()) {
	case casac::variant::BOOLVEC:
		return ret;
	case casac::variant::INT:
		ret.push_back(v.toInt());
		return ret;
	case casac::variant::INTVEC: {
        auto input = v.toIntVec( );
        std::vector<int> ret;
        std::for_each( input.begin( ), input.end( ), [&](long x){ret.push_back((int)x);} );
		return ret; }
	case casac::variant::STRING:
		if (v.toString().size() > 0) {
			throw AipsError(varName + " cannot be a non-empty string");
		}
		else {
			return ret;
		}
	default:
		ostringstream os;
		os << "Illegal type for " << varName << ": " << v.type();
		throw AipsError(os.str());
	}
}

vector<bool> toVectorBool(const ::casac::variant& v, const String& varName) {
	vector<bool> ret(0);
	switch (v.type()) {
	case casac::variant::BOOLVEC:
		return v.toBoolVec();
	case casac::variant::BOOL:
		ret.push_back(v.toBool());
		return ret;
	case casac::variant::STRING:
		if (v.toString().size() > 0) {
			throw AipsError(varName + " cannot be a non-empty string");
		}
		else {
			return ret;
		}
	default:
		ostringstream os;
		os << "Illegal type for " << varName << ": " << v.type();
		throw AipsError(os.str());
	}
}
Vector<String> toCasaVectorString(const vector<string>& vs) {
	Vector<String> res(vs.size());
	vector<string>::const_iterator vsiter = vs.begin();
	Vector<String>::iterator resiter = res.begin();
	vector<string>::const_iterator end = vs.end();
	while (vsiter != end) {
		*resiter = *vsiter;
		resiter++;
		vsiter++;
	}
	return res;
}



using namespace casacore;
}  // End namespace casa
