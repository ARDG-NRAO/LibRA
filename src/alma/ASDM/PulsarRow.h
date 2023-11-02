
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
 * Warning!
 *  -------------------------------------------------------------------- 
 * | This is generated code!  Do not modify this file.                  |
 * | If you do, all changes will be lost when the file is re-generated. |
 *  --------------------------------------------------------------------
 *
 * File PulsarRow.h
 */
 
#ifndef PulsarRow_CLASS
#define PulsarRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/Interval.h>
	




	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Pulsar.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::PulsarTable;

	

class PulsarRow;
typedef void (PulsarRow::*PulsarAttributeFromBin) (EndianIStream& eis);
typedef void (PulsarRow::*PulsarAttributeFromText) (const std::string& s);

/**
 * The PulsarRow class is a row of a PulsarTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class PulsarRow {
friend class asdm::PulsarTable;
friend class asdm::RowTransformer<PulsarRow>;
//friend class asdm::TableStreamReader<PulsarTable, PulsarRow>;

public:

	virtual ~PulsarRow();

	/**
	 * Return the table to which this row belongs.
	 */
	PulsarTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute pulsarId
	
	
	

	
 	/**
 	 * Get pulsarId.
 	 * @return pulsarId as Tag
 	 */
 	Tag getPulsarId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute refTime
	
	
	

	
 	/**
 	 * Get refTime.
 	 * @return refTime as ArrayTime
 	 */
 	ArrayTime getRefTime() const;
	
 
 	
 	
 	/**
 	 * Set refTime with the specified ArrayTime.
 	 * @param refTime The ArrayTime value to which refTime is to be set.
 	 
 		
 			
 	 */
 	void setRefTime (ArrayTime refTime);
  		
	
	
	


	
	// ===> Attribute refPulseFreq
	
	
	

	
 	/**
 	 * Get refPulseFreq.
 	 * @return refPulseFreq as Frequency
 	 */
 	Frequency getRefPulseFreq() const;
	
 
 	
 	
 	/**
 	 * Set refPulseFreq with the specified Frequency.
 	 * @param refPulseFreq The Frequency value to which refPulseFreq is to be set.
 	 
 		
 			
 	 */
 	void setRefPulseFreq (Frequency refPulseFreq);
  		
	
	
	


	
	// ===> Attribute refPhase
	
	
	

	
 	/**
 	 * Get refPhase.
 	 * @return refPhase as double
 	 */
 	double getRefPhase() const;
	
 
 	
 	
 	/**
 	 * Set refPhase with the specified double.
 	 * @param refPhase The double value to which refPhase is to be set.
 	 
 		
 			
 	 */
 	void setRefPhase (double refPhase);
  		
	
	
	


	
	// ===> Attribute numBin
	
	
	

	
 	/**
 	 * Get numBin.
 	 * @return numBin as int
 	 */
 	int getNumBin() const;
	
 
 	
 	
 	/**
 	 * Set numBin with the specified int.
 	 * @param numBin The int value to which numBin is to be set.
 	 
 		
 			
 	 */
 	void setNumBin (int numBin);
  		
	
	
	


	
	// ===> Attribute numPoly, which is optional
	
	
	
	/**
	 * The attribute numPoly is optional. Return true if this attribute exists.
	 * @return true if and only if the numPoly attribute exists. 
	 */
	bool isNumPolyExists() const;
	

	
 	/**
 	 * Get numPoly, which is optional.
 	 * @return numPoly as int
 	 * @throws IllegalAccessException If numPoly does not exist.
 	 */
 	int getNumPoly() const;
	
 
 	
 	
 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 		
 	 */
 	void setNumPoly (int numPoly);
		
	
	
	
	/**
	 * Mark numPoly, which is an optional field, as non-existent.
	 */
	void clearNumPoly ();
	


	
	// ===> Attribute phasePoly, which is optional
	
	
	
	/**
	 * The attribute phasePoly is optional. Return true if this attribute exists.
	 * @return true if and only if the phasePoly attribute exists. 
	 */
	bool isPhasePolyExists() const;
	

	
 	/**
 	 * Get phasePoly, which is optional.
 	 * @return phasePoly as std::vector<double >
 	 * @throws IllegalAccessException If phasePoly does not exist.
 	 */
 	std::vector<double > getPhasePoly() const;
	
 
 	
 	
 	/**
 	 * Set phasePoly with the specified std::vector<double >.
 	 * @param phasePoly The std::vector<double > value to which phasePoly is to be set.
 	 
 		
 	 */
 	void setPhasePoly (std::vector<double > phasePoly);
		
	
	
	
	/**
	 * Mark phasePoly, which is an optional field, as non-existent.
	 */
	void clearPhasePoly ();
	


	
	// ===> Attribute timeSpan, which is optional
	
	
	
	/**
	 * The attribute timeSpan is optional. Return true if this attribute exists.
	 * @return true if and only if the timeSpan attribute exists. 
	 */
	bool isTimeSpanExists() const;
	

	
 	/**
 	 * Get timeSpan, which is optional.
 	 * @return timeSpan as Interval
 	 * @throws IllegalAccessException If timeSpan does not exist.
 	 */
 	Interval getTimeSpan() const;
	
 
 	
 	
 	/**
 	 * Set timeSpan with the specified Interval.
 	 * @param timeSpan The Interval value to which timeSpan is to be set.
 	 
 		
 	 */
 	void setTimeSpan (Interval timeSpan);
		
	
	
	
	/**
	 * Mark timeSpan, which is an optional field, as non-existent.
	 */
	void clearTimeSpan ();
	


	
	// ===> Attribute startPhaseBin, which is optional
	
	
	
	/**
	 * The attribute startPhaseBin is optional. Return true if this attribute exists.
	 * @return true if and only if the startPhaseBin attribute exists. 
	 */
	bool isStartPhaseBinExists() const;
	

	
 	/**
 	 * Get startPhaseBin, which is optional.
 	 * @return startPhaseBin as std::vector<float >
 	 * @throws IllegalAccessException If startPhaseBin does not exist.
 	 */
 	std::vector<float > getStartPhaseBin() const;
	
 
 	
 	
 	/**
 	 * Set startPhaseBin with the specified std::vector<float >.
 	 * @param startPhaseBin The std::vector<float > value to which startPhaseBin is to be set.
 	 
 		
 	 */
 	void setStartPhaseBin (std::vector<float > startPhaseBin);
		
	
	
	
	/**
	 * Mark startPhaseBin, which is an optional field, as non-existent.
	 */
	void clearStartPhaseBin ();
	


	
	// ===> Attribute endPhaseBin, which is optional
	
	
	
	/**
	 * The attribute endPhaseBin is optional. Return true if this attribute exists.
	 * @return true if and only if the endPhaseBin attribute exists. 
	 */
	bool isEndPhaseBinExists() const;
	

	
 	/**
 	 * Get endPhaseBin, which is optional.
 	 * @return endPhaseBin as std::vector<float >
 	 * @throws IllegalAccessException If endPhaseBin does not exist.
 	 */
 	std::vector<float > getEndPhaseBin() const;
	
 
 	
 	
 	/**
 	 * Set endPhaseBin with the specified std::vector<float >.
 	 * @param endPhaseBin The std::vector<float > value to which endPhaseBin is to be set.
 	 
 		
 	 */
 	void setEndPhaseBin (std::vector<float > endPhaseBin);
		
	
	
	
	/**
	 * Mark endPhaseBin, which is an optional field, as non-existent.
	 */
	void clearEndPhaseBin ();
	


	
	// ===> Attribute dispersionMeasure, which is optional
	
	
	
	/**
	 * The attribute dispersionMeasure is optional. Return true if this attribute exists.
	 * @return true if and only if the dispersionMeasure attribute exists. 
	 */
	bool isDispersionMeasureExists() const;
	

	
 	/**
 	 * Get dispersionMeasure, which is optional.
 	 * @return dispersionMeasure as double
 	 * @throws IllegalAccessException If dispersionMeasure does not exist.
 	 */
 	double getDispersionMeasure() const;
	
 
 	
 	
 	/**
 	 * Set dispersionMeasure with the specified double.
 	 * @param dispersionMeasure The double value to which dispersionMeasure is to be set.
 	 
 		
 	 */
 	void setDispersionMeasure (double dispersionMeasure);
		
	
	
	
	/**
	 * Mark dispersionMeasure, which is an optional field, as non-existent.
	 */
	void clearDispersionMeasure ();
	


	
	// ===> Attribute refFrequency, which is optional
	
	
	
	/**
	 * The attribute refFrequency is optional. Return true if this attribute exists.
	 * @return true if and only if the refFrequency attribute exists. 
	 */
	bool isRefFrequencyExists() const;
	

	
 	/**
 	 * Get refFrequency, which is optional.
 	 * @return refFrequency as Frequency
 	 * @throws IllegalAccessException If refFrequency does not exist.
 	 */
 	Frequency getRefFrequency() const;
	
 
 	
 	
 	/**
 	 * Set refFrequency with the specified Frequency.
 	 * @param refFrequency The Frequency value to which refFrequency is to be set.
 	 
 		
 	 */
 	void setRefFrequency (Frequency refFrequency);
		
	
	
	
	/**
	 * Mark refFrequency, which is an optional field, as non-existent.
	 */
	void clearRefFrequency ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this PulsarRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param refTime
	    
	 * @param refPulseFreq
	    
	 * @param refPhase
	    
	 * @param numBin
	    
	 */ 
	bool compareNoAutoInc(ArrayTime refTime, Frequency refPulseFreq, double refPhase, int numBin);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param refTime
	    
	 * @param refPulseFreq
	    
	 * @param refPhase
	    
	 * @param numBin
	    
	 */ 
	bool compareRequiredValue(ArrayTime refTime, Frequency refPulseFreq, double refPhase, int numBin); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the PulsarRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(PulsarRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a PulsarRowIDL struct.
	 */
	asdmIDL::PulsarRowIDL *toIDL() const;
	
	/**
	 * Define the content of a PulsarRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the PulsarRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::PulsarRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct PulsarRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::PulsarRowIDL x) ;
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	std::string toXML() const;

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param rowDoc the XML string being used to set the values of this row.
	 * @throws ConversionException
	 */
	void setFromXML (std::string rowDoc) ;

	/// @cond DISPLAY_PRIVATE	
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////

	std::map<std::string, PulsarAttributeFromBin> fromBinMethods;
void pulsarIdFromBin( EndianIStream& eis);
void refTimeFromBin( EndianIStream& eis);
void refPulseFreqFromBin( EndianIStream& eis);
void refPhaseFromBin( EndianIStream& eis);
void numBinFromBin( EndianIStream& eis);

void numPolyFromBin( EndianIStream& eis);
void phasePolyFromBin( EndianIStream& eis);
void timeSpanFromBin( EndianIStream& eis);
void startPhaseBinFromBin( EndianIStream& eis);
void endPhaseBinFromBin( EndianIStream& eis);
void dispersionMeasureFromBin( EndianIStream& eis);
void refFrequencyFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the PulsarTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static PulsarRow* fromBin(EndianIStream& eis, PulsarTable& table, const std::vector<std::string>& attributesSeq);	 
 
 	 /**
 	  * Parses a string t and assign the result of the parsing to the attribute of name attributeName.
 	  *
 	  * @param attributeName the name of the attribute whose value is going to be defined.
 	  * @param t the string to be parsed into a value given to the attribute of name attributeName.
 	  */
 	 void fromText(const std::string& attributeName, const std::string&  t);
     /// @endcond			

private:
	/**
	 * The table to which this row belongs.
	 */
	PulsarTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a PulsarRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	PulsarRow (PulsarTable &table);

	/**
	 * Create a PulsarRow using a copy constructor mechanism.
	 * <p>
	 * Given a PulsarRow row and a PulsarTable table, the method creates a new
	 * PulsarRow owned by table. Each attribute of the created row is a copy (deep)
	 * of the corresponding attribute of row. The method does not add the created
	 * row to its table, its simply parents it to table, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a row with default values for its attributes. 
	 *
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 * @param row  The row which is to be copied.
	 */
	 PulsarRow (PulsarTable &table, PulsarRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute pulsarId
	
	

	Tag pulsarId;

	
	
 	
 	/**
 	 * Set pulsarId with the specified Tag value.
 	 * @param pulsarId The Tag value to which pulsarId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setPulsarId (Tag pulsarId);
  		
	

	
	// ===> Attribute refTime
	
	

	ArrayTime refTime;

	
	
 	

	
	// ===> Attribute refPulseFreq
	
	

	Frequency refPulseFreq;

	
	
 	

	
	// ===> Attribute refPhase
	
	

	double refPhase;

	
	
 	

	
	// ===> Attribute numBin
	
	

	int numBin;

	
	
 	

	
	// ===> Attribute numPoly, which is optional
	
	
	bool numPolyExists;
	

	int numPoly;

	
	
 	

	
	// ===> Attribute phasePoly, which is optional
	
	
	bool phasePolyExists;
	

	std::vector<double > phasePoly;

	
	
 	

	
	// ===> Attribute timeSpan, which is optional
	
	
	bool timeSpanExists;
	

	Interval timeSpan;

	
	
 	

	
	// ===> Attribute startPhaseBin, which is optional
	
	
	bool startPhaseBinExists;
	

	std::vector<float > startPhaseBin;

	
	
 	

	
	// ===> Attribute endPhaseBin, which is optional
	
	
	bool endPhaseBinExists;
	

	std::vector<float > endPhaseBin;

	
	
 	

	
	// ===> Attribute dispersionMeasure, which is optional
	
	
	bool dispersionMeasureExists;
	

	double dispersionMeasure;

	
	
 	

	
	// ===> Attribute refFrequency, which is optional
	
	
	bool refFrequencyExists;
	

	Frequency refFrequency;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, PulsarAttributeFromBin> fromBinMethods;
void pulsarIdFromBin( EndianIStream& eis);
void refTimeFromBin( EndianIStream& eis);
void refPulseFreqFromBin( EndianIStream& eis);
void refPhaseFromBin( EndianIStream& eis);
void numBinFromBin( EndianIStream& eis);

void numPolyFromBin( EndianIStream& eis);
void phasePolyFromBin( EndianIStream& eis);
void timeSpanFromBin( EndianIStream& eis);
void startPhaseBinFromBin( EndianIStream& eis);
void endPhaseBinFromBin( EndianIStream& eis);
void dispersionMeasureFromBin( EndianIStream& eis);
void refFrequencyFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, PulsarAttributeFromText> fromTextMethods;
	
void pulsarIdFromText (const std::string & s);
	
	
void refTimeFromText (const std::string & s);
	
	
void refPulseFreqFromText (const std::string & s);
	
	
void refPhaseFromText (const std::string & s);
	
	
void numBinFromText (const std::string & s);
	

	
void numPolyFromText (const std::string & s);
	
	
void phasePolyFromText (const std::string & s);
	
	
void timeSpanFromText (const std::string & s);
	
	
void startPhaseBinFromText (const std::string & s);
	
	
void endPhaseBinFromText (const std::string & s);
	
	
void dispersionMeasureFromText (const std::string & s);
	
	
void refFrequencyFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the PulsarTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static PulsarRow* fromBin(EndianIStream& eis, PulsarTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Pulsar_CLASS */
