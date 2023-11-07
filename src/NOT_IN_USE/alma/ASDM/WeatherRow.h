
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
 * File WeatherRow.h
 */
 
#ifndef WeatherRow_CLASS
#define WeatherRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Speed.h>
	

	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Temperature.h>
	

	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Humidity.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/Pressure.h>
	




	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Weather.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::WeatherTable;


// class asdm::StationRow;
class StationRow;
	

class WeatherRow;
typedef void (WeatherRow::*WeatherAttributeFromBin) (EndianIStream& eis);
typedef void (WeatherRow::*WeatherAttributeFromText) (const std::string& s);

/**
 * The WeatherRow class is a row of a WeatherTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class WeatherRow {
friend class asdm::WeatherTable;
friend class asdm::RowTransformer<WeatherRow>;
//friend class asdm::TableStreamReader<WeatherTable, WeatherRow>;

public:

	virtual ~WeatherRow();

	/**
	 * Return the table to which this row belongs.
	 */
	WeatherTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval getTimeInterval() const;
	
 
 	
 	
 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setTimeInterval (ArrayTimeInterval timeInterval);
  		
	
	
	


	
	// ===> Attribute pressure, which is optional
	
	
	
	/**
	 * The attribute pressure is optional. Return true if this attribute exists.
	 * @return true if and only if the pressure attribute exists. 
	 */
	bool isPressureExists() const;
	

	
 	/**
 	 * Get pressure, which is optional.
 	 * @return pressure as Pressure
 	 * @throws IllegalAccessException If pressure does not exist.
 	 */
 	Pressure getPressure() const;
	
 
 	
 	
 	/**
 	 * Set pressure with the specified Pressure.
 	 * @param pressure The Pressure value to which pressure is to be set.
 	 
 		
 	 */
 	void setPressure (Pressure pressure);
		
	
	
	
	/**
	 * Mark pressure, which is an optional field, as non-existent.
	 */
	void clearPressure ();
	


	
	// ===> Attribute relHumidity, which is optional
	
	
	
	/**
	 * The attribute relHumidity is optional. Return true if this attribute exists.
	 * @return true if and only if the relHumidity attribute exists. 
	 */
	bool isRelHumidityExists() const;
	

	
 	/**
 	 * Get relHumidity, which is optional.
 	 * @return relHumidity as Humidity
 	 * @throws IllegalAccessException If relHumidity does not exist.
 	 */
 	Humidity getRelHumidity() const;
	
 
 	
 	
 	/**
 	 * Set relHumidity with the specified Humidity.
 	 * @param relHumidity The Humidity value to which relHumidity is to be set.
 	 
 		
 	 */
 	void setRelHumidity (Humidity relHumidity);
		
	
	
	
	/**
	 * Mark relHumidity, which is an optional field, as non-existent.
	 */
	void clearRelHumidity ();
	


	
	// ===> Attribute temperature, which is optional
	
	
	
	/**
	 * The attribute temperature is optional. Return true if this attribute exists.
	 * @return true if and only if the temperature attribute exists. 
	 */
	bool isTemperatureExists() const;
	

	
 	/**
 	 * Get temperature, which is optional.
 	 * @return temperature as Temperature
 	 * @throws IllegalAccessException If temperature does not exist.
 	 */
 	Temperature getTemperature() const;
	
 
 	
 	
 	/**
 	 * Set temperature with the specified Temperature.
 	 * @param temperature The Temperature value to which temperature is to be set.
 	 
 		
 	 */
 	void setTemperature (Temperature temperature);
		
	
	
	
	/**
	 * Mark temperature, which is an optional field, as non-existent.
	 */
	void clearTemperature ();
	


	
	// ===> Attribute windDirection, which is optional
	
	
	
	/**
	 * The attribute windDirection is optional. Return true if this attribute exists.
	 * @return true if and only if the windDirection attribute exists. 
	 */
	bool isWindDirectionExists() const;
	

	
 	/**
 	 * Get windDirection, which is optional.
 	 * @return windDirection as Angle
 	 * @throws IllegalAccessException If windDirection does not exist.
 	 */
 	Angle getWindDirection() const;
	
 
 	
 	
 	/**
 	 * Set windDirection with the specified Angle.
 	 * @param windDirection The Angle value to which windDirection is to be set.
 	 
 		
 	 */
 	void setWindDirection (Angle windDirection);
		
	
	
	
	/**
	 * Mark windDirection, which is an optional field, as non-existent.
	 */
	void clearWindDirection ();
	


	
	// ===> Attribute windSpeed, which is optional
	
	
	
	/**
	 * The attribute windSpeed is optional. Return true if this attribute exists.
	 * @return true if and only if the windSpeed attribute exists. 
	 */
	bool isWindSpeedExists() const;
	

	
 	/**
 	 * Get windSpeed, which is optional.
 	 * @return windSpeed as Speed
 	 * @throws IllegalAccessException If windSpeed does not exist.
 	 */
 	Speed getWindSpeed() const;
	
 
 	
 	
 	/**
 	 * Set windSpeed with the specified Speed.
 	 * @param windSpeed The Speed value to which windSpeed is to be set.
 	 
 		
 	 */
 	void setWindSpeed (Speed windSpeed);
		
	
	
	
	/**
	 * Mark windSpeed, which is an optional field, as non-existent.
	 */
	void clearWindSpeed ();
	


	
	// ===> Attribute windMax, which is optional
	
	
	
	/**
	 * The attribute windMax is optional. Return true if this attribute exists.
	 * @return true if and only if the windMax attribute exists. 
	 */
	bool isWindMaxExists() const;
	

	
 	/**
 	 * Get windMax, which is optional.
 	 * @return windMax as Speed
 	 * @throws IllegalAccessException If windMax does not exist.
 	 */
 	Speed getWindMax() const;
	
 
 	
 	
 	/**
 	 * Set windMax with the specified Speed.
 	 * @param windMax The Speed value to which windMax is to be set.
 	 
 		
 	 */
 	void setWindMax (Speed windMax);
		
	
	
	
	/**
	 * Mark windMax, which is an optional field, as non-existent.
	 */
	void clearWindMax ();
	


	
	// ===> Attribute dewPoint, which is optional
	
	
	
	/**
	 * The attribute dewPoint is optional. Return true if this attribute exists.
	 * @return true if and only if the dewPoint attribute exists. 
	 */
	bool isDewPointExists() const;
	

	
 	/**
 	 * Get dewPoint, which is optional.
 	 * @return dewPoint as Temperature
 	 * @throws IllegalAccessException If dewPoint does not exist.
 	 */
 	Temperature getDewPoint() const;
	
 
 	
 	
 	/**
 	 * Set dewPoint with the specified Temperature.
 	 * @param dewPoint The Temperature value to which dewPoint is to be set.
 	 
 		
 	 */
 	void setDewPoint (Temperature dewPoint);
		
	
	
	
	/**
	 * Mark dewPoint, which is an optional field, as non-existent.
	 */
	void clearDewPoint ();
	


	
	// ===> Attribute numLayer, which is optional
	
	
	
	/**
	 * The attribute numLayer is optional. Return true if this attribute exists.
	 * @return true if and only if the numLayer attribute exists. 
	 */
	bool isNumLayerExists() const;
	

	
 	/**
 	 * Get numLayer, which is optional.
 	 * @return numLayer as int
 	 * @throws IllegalAccessException If numLayer does not exist.
 	 */
 	int getNumLayer() const;
	
 
 	
 	
 	/**
 	 * Set numLayer with the specified int.
 	 * @param numLayer The int value to which numLayer is to be set.
 	 
 		
 	 */
 	void setNumLayer (int numLayer);
		
	
	
	
	/**
	 * Mark numLayer, which is an optional field, as non-existent.
	 */
	void clearNumLayer ();
	


	
	// ===> Attribute layerHeight, which is optional
	
	
	
	/**
	 * The attribute layerHeight is optional. Return true if this attribute exists.
	 * @return true if and only if the layerHeight attribute exists. 
	 */
	bool isLayerHeightExists() const;
	

	
 	/**
 	 * Get layerHeight, which is optional.
 	 * @return layerHeight as std::vector<Length >
 	 * @throws IllegalAccessException If layerHeight does not exist.
 	 */
 	std::vector<Length > getLayerHeight() const;
	
 
 	
 	
 	/**
 	 * Set layerHeight with the specified std::vector<Length >.
 	 * @param layerHeight The std::vector<Length > value to which layerHeight is to be set.
 	 
 		
 	 */
 	void setLayerHeight (std::vector<Length > layerHeight);
		
	
	
	
	/**
	 * Mark layerHeight, which is an optional field, as non-existent.
	 */
	void clearLayerHeight ();
	


	
	// ===> Attribute temperatureProfile, which is optional
	
	
	
	/**
	 * The attribute temperatureProfile is optional. Return true if this attribute exists.
	 * @return true if and only if the temperatureProfile attribute exists. 
	 */
	bool isTemperatureProfileExists() const;
	

	
 	/**
 	 * Get temperatureProfile, which is optional.
 	 * @return temperatureProfile as std::vector<Temperature >
 	 * @throws IllegalAccessException If temperatureProfile does not exist.
 	 */
 	std::vector<Temperature > getTemperatureProfile() const;
	
 
 	
 	
 	/**
 	 * Set temperatureProfile with the specified std::vector<Temperature >.
 	 * @param temperatureProfile The std::vector<Temperature > value to which temperatureProfile is to be set.
 	 
 		
 	 */
 	void setTemperatureProfile (std::vector<Temperature > temperatureProfile);
		
	
	
	
	/**
	 * Mark temperatureProfile, which is an optional field, as non-existent.
	 */
	void clearTemperatureProfile ();
	


	
	// ===> Attribute cloudMonitor, which is optional
	
	
	
	/**
	 * The attribute cloudMonitor is optional. Return true if this attribute exists.
	 * @return true if and only if the cloudMonitor attribute exists. 
	 */
	bool isCloudMonitorExists() const;
	

	
 	/**
 	 * Get cloudMonitor, which is optional.
 	 * @return cloudMonitor as Temperature
 	 * @throws IllegalAccessException If cloudMonitor does not exist.
 	 */
 	Temperature getCloudMonitor() const;
	
 
 	
 	
 	/**
 	 * Set cloudMonitor with the specified Temperature.
 	 * @param cloudMonitor The Temperature value to which cloudMonitor is to be set.
 	 
 		
 	 */
 	void setCloudMonitor (Temperature cloudMonitor);
		
	
	
	
	/**
	 * Mark cloudMonitor, which is an optional field, as non-existent.
	 */
	void clearCloudMonitor ();
	


	
	// ===> Attribute numWVR, which is optional
	
	
	
	/**
	 * The attribute numWVR is optional. Return true if this attribute exists.
	 * @return true if and only if the numWVR attribute exists. 
	 */
	bool isNumWVRExists() const;
	

	
 	/**
 	 * Get numWVR, which is optional.
 	 * @return numWVR as int
 	 * @throws IllegalAccessException If numWVR does not exist.
 	 */
 	int getNumWVR() const;
	
 
 	
 	
 	/**
 	 * Set numWVR with the specified int.
 	 * @param numWVR The int value to which numWVR is to be set.
 	 
 		
 	 */
 	void setNumWVR (int numWVR);
		
	
	
	
	/**
	 * Mark numWVR, which is an optional field, as non-existent.
	 */
	void clearNumWVR ();
	


	
	// ===> Attribute wvrTemp, which is optional
	
	
	
	/**
	 * The attribute wvrTemp is optional. Return true if this attribute exists.
	 * @return true if and only if the wvrTemp attribute exists. 
	 */
	bool isWvrTempExists() const;
	

	
 	/**
 	 * Get wvrTemp, which is optional.
 	 * @return wvrTemp as std::vector<Temperature >
 	 * @throws IllegalAccessException If wvrTemp does not exist.
 	 */
 	std::vector<Temperature > getWvrTemp() const;
	
 
 	
 	
 	/**
 	 * Set wvrTemp with the specified std::vector<Temperature >.
 	 * @param wvrTemp The std::vector<Temperature > value to which wvrTemp is to be set.
 	 
 		
 	 */
 	void setWvrTemp (std::vector<Temperature > wvrTemp);
		
	
	
	
	/**
	 * Mark wvrTemp, which is an optional field, as non-existent.
	 */
	void clearWvrTemp ();
	


	
	// ===> Attribute water, which is optional
	
	
	
	/**
	 * The attribute water is optional. Return true if this attribute exists.
	 * @return true if and only if the water attribute exists. 
	 */
	bool isWaterExists() const;
	

	
 	/**
 	 * Get water, which is optional.
 	 * @return water as double
 	 * @throws IllegalAccessException If water does not exist.
 	 */
 	double getWater() const;
	
 
 	
 	
 	/**
 	 * Set water with the specified double.
 	 * @param water The double value to which water is to be set.
 	 
 		
 	 */
 	void setWater (double water);
		
	
	
	
	/**
	 * Mark water, which is an optional field, as non-existent.
	 */
	void clearWater ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute stationId
	
	
	

	
 	/**
 	 * Get stationId.
 	 * @return stationId as Tag
 	 */
 	Tag getStationId() const;
	
 
 	
 	
 	/**
 	 * Set stationId with the specified Tag.
 	 * @param stationId The Tag value to which stationId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setStationId (Tag stationId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * stationId pointer to the row in the Station table having Station.stationId == stationId
	 * @return a StationRow*
	 * 
	 
	 */
	 StationRow* getStationUsingStationId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this WeatherRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param stationId
	    
	 * @param timeInterval
	    
	 */ 
	bool compareNoAutoInc(Tag stationId, ArrayTimeInterval timeInterval);
	
	

		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the WeatherRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(WeatherRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a WeatherRowIDL struct.
	 */
	asdmIDL::WeatherRowIDL *toIDL() const;
	
	/**
	 * Define the content of a WeatherRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the WeatherRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::WeatherRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct WeatherRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::WeatherRowIDL x) ;
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

	std::map<std::string, WeatherAttributeFromBin> fromBinMethods;
void stationIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);

void pressureFromBin( EndianIStream& eis);
void relHumidityFromBin( EndianIStream& eis);
void temperatureFromBin( EndianIStream& eis);
void windDirectionFromBin( EndianIStream& eis);
void windSpeedFromBin( EndianIStream& eis);
void windMaxFromBin( EndianIStream& eis);
void dewPointFromBin( EndianIStream& eis);
void numLayerFromBin( EndianIStream& eis);
void layerHeightFromBin( EndianIStream& eis);
void temperatureProfileFromBin( EndianIStream& eis);
void cloudMonitorFromBin( EndianIStream& eis);
void numWVRFromBin( EndianIStream& eis);
void wvrTempFromBin( EndianIStream& eis);
void waterFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the WeatherTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static WeatherRow* fromBin(EndianIStream& eis, WeatherTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	WeatherTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a WeatherRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	WeatherRow (WeatherTable &table);

	/**
	 * Create a WeatherRow using a copy constructor mechanism.
	 * <p>
	 * Given a WeatherRow row and a WeatherTable table, the method creates a new
	 * WeatherRow owned by table. Each attribute of the created row is a copy (deep)
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
	 WeatherRow (WeatherTable &table, WeatherRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute pressure, which is optional
	
	
	bool pressureExists;
	

	Pressure pressure;

	
	
 	

	
	// ===> Attribute relHumidity, which is optional
	
	
	bool relHumidityExists;
	

	Humidity relHumidity;

	
	
 	

	
	// ===> Attribute temperature, which is optional
	
	
	bool temperatureExists;
	

	Temperature temperature;

	
	
 	

	
	// ===> Attribute windDirection, which is optional
	
	
	bool windDirectionExists;
	

	Angle windDirection;

	
	
 	

	
	// ===> Attribute windSpeed, which is optional
	
	
	bool windSpeedExists;
	

	Speed windSpeed;

	
	
 	

	
	// ===> Attribute windMax, which is optional
	
	
	bool windMaxExists;
	

	Speed windMax;

	
	
 	

	
	// ===> Attribute dewPoint, which is optional
	
	
	bool dewPointExists;
	

	Temperature dewPoint;

	
	
 	

	
	// ===> Attribute numLayer, which is optional
	
	
	bool numLayerExists;
	

	int numLayer;

	
	
 	

	
	// ===> Attribute layerHeight, which is optional
	
	
	bool layerHeightExists;
	

	std::vector<Length > layerHeight;

	
	
 	

	
	// ===> Attribute temperatureProfile, which is optional
	
	
	bool temperatureProfileExists;
	

	std::vector<Temperature > temperatureProfile;

	
	
 	

	
	// ===> Attribute cloudMonitor, which is optional
	
	
	bool cloudMonitorExists;
	

	Temperature cloudMonitor;

	
	
 	

	
	// ===> Attribute numWVR, which is optional
	
	
	bool numWVRExists;
	

	int numWVR;

	
	
 	

	
	// ===> Attribute wvrTemp, which is optional
	
	
	bool wvrTempExists;
	

	std::vector<Temperature > wvrTemp;

	
	
 	

	
	// ===> Attribute water, which is optional
	
	
	bool waterExists;
	

	double water;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute stationId
	
	

	Tag stationId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, WeatherAttributeFromBin> fromBinMethods;
void stationIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);

void pressureFromBin( EndianIStream& eis);
void relHumidityFromBin( EndianIStream& eis);
void temperatureFromBin( EndianIStream& eis);
void windDirectionFromBin( EndianIStream& eis);
void windSpeedFromBin( EndianIStream& eis);
void windMaxFromBin( EndianIStream& eis);
void dewPointFromBin( EndianIStream& eis);
void numLayerFromBin( EndianIStream& eis);
void layerHeightFromBin( EndianIStream& eis);
void temperatureProfileFromBin( EndianIStream& eis);
void cloudMonitorFromBin( EndianIStream& eis);
void numWVRFromBin( EndianIStream& eis);
void wvrTempFromBin( EndianIStream& eis);
void waterFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, WeatherAttributeFromText> fromTextMethods;
	
void stationIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	

	
void pressureFromText (const std::string & s);
	
	
void relHumidityFromText (const std::string & s);
	
	
void temperatureFromText (const std::string & s);
	
	
void windDirectionFromText (const std::string & s);
	
	
void windSpeedFromText (const std::string & s);
	
	
void windMaxFromText (const std::string & s);
	
	
void dewPointFromText (const std::string & s);
	
	
void numLayerFromText (const std::string & s);
	
	
void layerHeightFromText (const std::string & s);
	
	
void temperatureProfileFromText (const std::string & s);
	
	
void cloudMonitorFromText (const std::string & s);
	
	
void numWVRFromText (const std::string & s);
	
	
void wvrTempFromText (const std::string & s);
	
	
void waterFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the WeatherTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static WeatherRow* fromBin(EndianIStream& eis, WeatherTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Weather_CLASS */
