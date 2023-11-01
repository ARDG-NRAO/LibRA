
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
 * File EphemerisTable.h
 */
 
#ifndef EphemerisTable_CLASS
#define EphemerisTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTimeInterval.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	




	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/UniquenessViolationException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/DuplicateKey.h>


#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif

#include <alma/ASDM/Representable.h>

#include <pthread.h>

namespace asdm {

//class asdm::ASDM;
//class asdm::EphemerisRow;

class ASDM;
class EphemerisRow;
/**
 * The EphemerisTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * 
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of Ephemeris </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> timeInterval </TD>
 		 
 * <TD> ArrayTimeInterval</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;interval of time during which the data are relevant.  </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> ephemerisId </TD>
 		 
 * <TD> int</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a collection of rows in the table.  </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> observerLocation </TD> 
 * <TD> std::vector<double > </TD>
 * <TD>  3 </TD> 
 * <TD> &nbsp;a triple of double precision values defining the observer location. This triple contains in that order the longitude, the latitude and the altitude of the observer :
<ul>
<li> the longitude is expressed in radian. An east (resp. west) longitude is denoted as a positive (resp. negative) quantity.</li>
<li> the latitude is expressed in radian. A north (resp. south) latitude is denoted as a positive (resp. negative) quantity. </li>
<li> the altitude is expressed in meter. It's the altitude above the reference ellipsoid. </li>
</ul>
A triple with all its elements equal to 0.0 will mean that a geocentric coordinate system is in use instead of a topocentric one. </TD>
 * </TR>
	
 * <TR>
 * <TD> equinoxEquator </TD> 
 * <TD> double </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;epoch at which equator and equinox were calculated for ephemeris. Expresses a year as a decimal value (J2000 would be represented as 2000.0). </TD>
 * </TR>
	
 * <TR>
 * <TD> numPolyDir (numPolyDir)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of coefficients of the polynomial stored in phaseDir. It has to be \f$ \ge 1 \f$.  </TD>
 * </TR>
	
 * <TR>
 * <TD> dir </TD> 
 * <TD> std::vector<std::vector<double > > </TD>
 * <TD>  numPolyDir, 2 </TD> 
 * <TD> &nbsp;the ephemeris direction expressed in radian. The nominal entry in the phaseDir, delayDir, or ReferenceDir in the Field table serves as additional offset to the direction described by "dir". The actual direction is obtained by composition, e.g. actual phase direction = [phasDir value from Field table] + [dir].

The direction described by dir  is the result of the sum

\f[ dir_{0,i} + dir_{1,i}*dt + dir_{2,i}*dt^2 + ... + dir_{numPolyDir-1,i}*dt^{numPolyDir-1}, \forall i \in \{0,1\} \f]

where

\f[ dt = t - timeOrigin \f] 
 </TD>
 * </TR>
	
 * <TR>
 * <TD> numPolyDist (numPolyDist)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of coefficients of the polynomial stored in distance. It has to be \f$ \ge 1 \f$.  </TD>
 * </TR>
	
 * <TR>
 * <TD> distance </TD> 
 * <TD> std::vector<double > </TD>
 * <TD>  numPolyDist </TD> 
 * <TD> &nbsp;the coefficiens of the polynomial used to calculate the distance, expressed in meter,  to the object from the position of the antenna along the given direction. This distance is the result of the sum :

\f[ distance_0 + distance_1*dt + distance_2*dt^2 + ... + distance_{numPolyDist-1}*dt^{numPolyDist-1} \f]

where

\f[ dt = t - timeOrigin \f].
 </TD>
 * </TR>
	
 * <TR>
 * <TD> timeOrigin </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the time origin used in the evaluation of the polynomial expressions.  </TD>
 * </TR>
	
 * <TR>
 * <TD> origin </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the origin of the ephemeris information. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> numPolyRadVel(numPolyRadVel)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the number of coefficients of the polynomial stored in radVel . It has to be \f$ \ge 1 \f$.  </TD>
 * </TR>
	
 * <TR>
 * <TD> radVel</TD> 
 * <TD> std::vector<double > </TD>
 * <TD>  numPolyRadVel  </TD>
 * <TD>&nbsp;  the coefficients of a polynomial expressing a radial velocity as a function of the time expressed in m/s. The time origin used to tabulate the polynomial is stored in timeOrigin.   </TD>
 * </TR>
	

 * </TABLE>
 */
class EphemerisTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~EphemerisTable();
	
	/**
	 * Return the container to which this table belongs.
	 *
	 * @return the ASDM containing this table.
	 */
	ASDM &getContainer() const;
	
	/**
	 * Return the number of rows in the table.
	 *
	 * @return the number of rows in an unsigned int.
	 */
	unsigned int size() const;
	
	/**
	 * Return the name of this table.
	 *
	 * This is a instance method of the class.
	 *
	 * @return the name of this table in a string.
	 */
	std::string getName() const;
	
	/**
	 * Return the name of this table.
	 *
	 * This is a static method of the class.
	 *
	 * @return the name of this table in a string.
	 */
	static std::string name() ;	
	
	/**
	 * Return the version information about this table.
	 *
	 */
	 std::string getVersion() const ;
	
	/**
	 * Return the names of the attributes of this table.
	 *
	 * @return a vector of string
	 */
	 static const std::vector<std::string>& getAttributesNames();

	/**
	 * Return the default sorted list of attributes names in the binary representation of the table.
	 *
	 * @return a const reference to a vector of string
	 */
	 static const std::vector<std::string>& defaultAttributesNamesInBin();
	 
	/**
	 * Return this table's Entity.
	 */
	Entity getEntity() const;

	/**
	 * Set this table's Entity.
	 * @param e An entity. 
	 */
	void setEntity(Entity e);
		
	/**
	 * Produces an XML representation conform
	 * to the schema defined for Ephemeris (EphemerisTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a EphemerisTableIDL CORBA structure.
	 *
	 * @return a pointer to a EphemerisTableIDL
	 */
	asdmIDL::EphemerisTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::EphemerisTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::EphemerisTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a EphemerisTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::EphemerisTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a EphemerisRow
	 */
	EphemerisRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param timeInterval
	
 	 * @param ephemerisId
	
 	 * @param observerLocation
	
 	 * @param equinoxEquator
	
 	 * @param numPolyDir
	
 	 * @param dir
	
 	 * @param numPolyDist
	
 	 * @param distance
	
 	 * @param timeOrigin
	
 	 * @param origin
	
     */
	EphemerisRow *newRow(ArrayTimeInterval timeInterval, int ephemerisId, std::vector<double > observerLocation, double equinoxEquator, int numPolyDir, std::vector<std::vector<double > > dir, int numPolyDist, std::vector<double > distance, ArrayTime timeOrigin, std::string origin);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new EphemerisRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new EphemerisRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 EphemerisRow *newRow(EphemerisRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the EphemerisRow to be added.
	 *
	 * @return a pointer to a EphemerisRow. If the table contains a EphemerisRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that EphemerisRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a EphemerisRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 * @note The row is inserted in the table in such a way that all the rows having the same value of
	 * ( ephemerisId ) are stored by ascending time.
	 * @see method getByContext.
	
	 */
	EphemerisRow* add(EphemerisRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of EphemerisRow. The elements of this vector are stored in the order 
	 * in which they have been added to the EphemerisTable.
	 */
	std::vector<EphemerisRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of EphemerisRow. The elements of this vector are stored in the order 
	 * in which they have been added to the EphemerisTable.
	 *
	 */
	 const std::vector<EphemerisRow *>& get() const ;
	

	/**
	 * Returns all the rows sorted by ascending startTime for a given context. 
	 * The context is defined by a value of ( ephemerisId ).
	 *
	 * @return a pointer on a vector<EphemerisRow *>. A null returned value means that the table contains
	 * no EphemerisRow for the given ( ephemerisId ).
	 *
	 * @throws IllegalAccessException when a call is done to this method when it's called while the dataset has been imported with the 
	 * option checkRowUniqueness set to false.
	 */
	 std::vector <EphemerisRow*> *getByContext(int ephemerisId);
	 


 
	
	/**
 	 * Returns a EphemerisRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param timeInterval
	
	 * @param ephemerisId
	
 	 *
	 */
 	EphemerisRow* getRowByKey(ArrayTimeInterval timeInterval, int ephemerisId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param timeInterval
 	 		
 	 * @param ephemerisId
 	 		
 	 * @param observerLocation
 	 		
 	 * @param equinoxEquator
 	 		
 	 * @param numPolyDir
 	 		
 	 * @param dir
 	 		
 	 * @param numPolyDist
 	 		
 	 * @param distance
 	 		
 	 * @param timeOrigin
 	 		
 	 * @param origin
 	 		 
 	 */
	EphemerisRow* lookup(ArrayTimeInterval timeInterval, int ephemerisId, std::vector<double > observerLocation, double equinoxEquator, int numPolyDir, std::vector<std::vector<double > > dir, int numPolyDist, std::vector<double > distance, ArrayTime timeOrigin, std::string origin); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a EphemerisTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	EphemerisTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	
	


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 */
	EphemerisRow* checkAndAdd(EphemerisRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an EphemerisRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param EphemerisRow* x a pointer onto the EphemerisRow to be appended.
	 */
	 void append(EphemerisRow* x) ;
	 
	/**
	 * Brutally append an EphemerisRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param EphemerisRow* x a pointer onto the EphemerisRow to be appended.
	 */
	 void addWithoutCheckingUnique(EphemerisRow* x) ;
	 
	 


	
	
	/**
	 * Insert a EphemerisRow* in a vector of EphemerisRow* so that it's ordered by ascending time.
	 *
	 * @param EphemerisRow* x . The pointer to be inserted.
	 * @param vector <EphemerisRow*>& row . A reference to the vector where to insert x.
	 *
	 */
	 EphemerisRow * insertByStartTime(EphemerisRow* x, std::vector<EphemerisRow* >& row);
	  


// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of EphemerisRow s.
   std::vector<EphemerisRow * > privateRows;
   

	

	
	
		
				
	typedef std::vector <EphemerisRow* > TIME_ROWS;
	std::map<std::string, TIME_ROWS > context;
		
	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 std::string Key(int ephemerisId) ;
		 
		
	
	
	/**
	 * Fills the vector vout (passed by reference) with pointers on elements of vin 
	 * whose attributes are equal to the corresponding parameters of the method.
	 *
	 */
	void getByKeyNoAutoIncNoTime(std::vector <EphemerisRow*>& vin, std::vector <EphemerisRow*>& vout,  int ephemerisId);
	

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a Ephemeris (EphemerisTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a Ephemeris table.
	  */
	void setFromMIMEFile(const std::string& directory);
	/*
	void openMIMEFile(const std::string& directory);
	*/
	void setFromXMLFile(const std::string& directory);
	
		 /**
	 * Serialize this into a stream of bytes and encapsulates that stream into a MIME message.
	 * @returns a string containing the MIME message.
	 *
	 * @param byteOrder a const pointer to a static instance of the class ByteOrder.
	 * 
	 */
	std::string toMIME(const asdm::ByteOrder* byteOrder=asdm::ByteOrder::Machine_Endianity);
  
	
   /** 
     * Extracts the binary part of a MIME message and deserialize its content
	 * to fill this with the result of the deserialization. 
	 * @param mimeMsg the string containing the MIME message.
	 * @throws ConversionException
	 */
	 void setFromMIME(const std::string & mimeMsg);
	
	/**
	  * Private methods involved during the export of this table into disk file(s).
	  */
	std::string MIMEXMLPart(const asdm::ByteOrder* byteOrder=asdm::ByteOrder::Machine_Endianity);
	
	/**
	  * Stores a representation (binary or XML) of this table into a file.
	  *
	  * Depending on the boolean value of its private field fileAsBin a binary serialization  of this (fileAsBin==true)  
	  * will be saved in a file "Ephemeris.bin" or an XML representation (fileAsBin==false) will be saved in a file "Ephemeris.xml".
	  * The file is always written in a directory whose name is passed as a parameter.
	 * @param directory The name of directory  where the file containing the table's representation will be saved.
	  * 
	  */
	  void toFile(std::string directory);
	  
	  /**
	   * Load the table in memory if necessary.
	   */
	  bool loadInProgress;
	  void checkPresenceInMemory() {
		if (!presentInMemory && !loadInProgress) {
			loadInProgress = true;
			setFromFile(getContainer().getDirectory());
			presentInMemory = true;
			loadInProgress = false;
	  	}
	  }
	/**
	 * Reads and parses a file containing a representation of a EphemerisTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* EphemerisTable_CLASS */
