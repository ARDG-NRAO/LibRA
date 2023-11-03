
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
 * /////////////////////////////////////////////////////////////////
 * // WARNING!  DO NOT MODIFY THIS FILE!                          //
 * //  ---------------------------------------------------------  //
 * // | This is generated code!  Do not modify this file.       | //
 * // | Any changes will be lost when the file is re-generated. | //
 * //  ---------------------------------------------------------  //
 * /////////////////////////////////////////////////////////////////
 *
 * File CEnumerationParser.h
 */
 #include <string>
 #include <alma/ASDM/ConversionException.h>
 
 
  
 #include <alma/Enumerations/CReceiverBand.h>
 
 #include <alma/Enumerations/CSBType.h>
 
 #include <alma/Enumerations/CDirectionReferenceCode.h>
 
 #include <alma/Enumerations/CCorrelationMode.h>
 
 #include <alma/Enumerations/CAtmPhaseCorrection.h>
 
 #include <alma/Enumerations/CProcessorType.h>
 
 #include <alma/Enumerations/CSpectralResolutionType.h>
 
 #include <alma/Enumerations/CCalibrationDevice.h>
 
 #include <alma/Enumerations/CAntennaMake.h>
 
 #include <alma/Enumerations/CAntennaType.h>
 
 #include <alma/Enumerations/CSourceModel.h>
 
 #include <alma/Enumerations/CFrequencyReferenceCode.h>
 
 #include <alma/Enumerations/CStokesParameter.h>
 
 #include <alma/Enumerations/CRadialVelocityReferenceCode.h>
 
 #include <alma/Enumerations/CDopplerReferenceCode.h>
 
 #include <alma/Enumerations/CPolarizationType.h>
 
 #include <alma/Enumerations/CBasebandName.h>
 
 #include <alma/Enumerations/CNetSideband.h>
 
 #include <alma/Enumerations/CSidebandProcessingMode.h>
 
 #include <alma/Enumerations/CWindowFunction.h>
 
 #include <alma/Enumerations/CCorrelationBit.h>
 
 #include <alma/Enumerations/CReceiverSideband.h>
 
 #include <alma/Enumerations/CProcessorSubType.h>
 
 #include <alma/Enumerations/CAccumMode.h>
 
 #include <alma/Enumerations/CAxisName.h>
 
 #include <alma/Enumerations/CFilterMode.h>
 
 #include <alma/Enumerations/CCorrelatorName.h>
 
 #include <alma/Enumerations/CWVRMethod.h>
 
 #include <alma/Enumerations/CScanIntent.h>
 
 #include <alma/Enumerations/CCalDataOrigin.h>
 
 #include <alma/Enumerations/CCalibrationFunction.h>
 
 #include <alma/Enumerations/CCalibrationSet.h>
 
 #include <alma/Enumerations/CAntennaMotionPattern.h>
 
 #include <alma/Enumerations/CSubscanIntent.h>
 
 #include <alma/Enumerations/CSwitchingMode.h>
 
 #include <alma/Enumerations/CCorrelatorCalibration.h>
 
 #include <alma/Enumerations/CTimeSampling.h>
 
 #include <alma/Enumerations/CCalType.h>
 
 #include <alma/Enumerations/CAssociatedCalNature.h>
 
 #include <alma/Enumerations/CInvalidatingCondition.h>
 
 #include <alma/Enumerations/CPositionMethod.h>
 
 #include <alma/Enumerations/CPointingModelMode.h>
 
 #include <alma/Enumerations/CPointingMethod.h>
 
 #include <alma/Enumerations/CSyscalMethod.h>
 
 #include <alma/Enumerations/CCalCurveType.h>
 
 #include <alma/Enumerations/CStationType.h>
 
 #include <alma/Enumerations/CDetectorBandType.h>
 
 #include <alma/Enumerations/CFocusMethod.h>
 
 #include <alma/Enumerations/CHolographyChannelType.h>
 
 #include <alma/Enumerations/CFluxCalibrationMethod.h>
 
 #include <alma/Enumerations/CPrimaryBeamDescription.h>
 
 #include <alma/Enumerations/CTimeScale.h>
 
 #include <alma/Enumerations/CDataScale.h>
 
 #include <alma/Enumerations/CWeightType.h>
 
 #include <alma/Enumerations/CDifferenceType.h>
 
 #include <alma/Enumerations/CCalibrationMode.h>
 
 #include <alma/Enumerations/CAssociatedFieldNature.h>
 
 #include <alma/Enumerations/CDataContent.h>
 
 #include <alma/Enumerations/CPrimitiveDataType.h>
 
 #include <alma/Enumerations/CSchedulerMode.h>
 
 #include <alma/Enumerations/CFieldCode.h>
 
 #include <alma/Enumerations/CACAPolarization.h>
 
 #include <alma/Enumerations/CPositionReferenceCode.h>
 
 #include <alma/Enumerations/CBaselineReferenceCode.h>
 
 #include <alma/Enumerations/CCorrelatorType.h>
 
 #include <alma/Enumerations/CDopplerTrackingMode.h>
 
 #include <alma/Enumerations/CSynthProf.h>
 
 
 namespace asdm {
 
 	class EnumerationParser {
 	
 	static std::string getField(const std::string &xml, const std::string &field);
 	
 	static std::string substring(const std::string &s, int a, int b);
 	
 	static std::string trim(const std::string &s);
	
	public:
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  ReceiverBandMod::ReceiverBand value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, ReceiverBandMod::ReceiverBand e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<ReceiverBandMod::ReceiverBand>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<ReceiverBandMod::ReceiverBand>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<ReceiverBandMod::ReceiverBand> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<ReceiverBandMod::ReceiverBand> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<ReceiverBandMod::ReceiverBand> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<ReceiverBandMod::ReceiverBand> > >& vvv_e); 

	/**
	 * Returns a ReceiverBandMod::ReceiverBand from a string.
	 * @param xml the string to be converted into a ReceiverBandMod::ReceiverBand
	 * @return a ReceiverBandMod::ReceiverBand.
	 */
	static ReceiverBandMod::ReceiverBand getReceiverBand(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<ReceiverBandMod::ReceiverBand> from a string.
	 * @param xml the string to be converted into a vector<ReceiverBandMod::ReceiverBand>
	 * @return a vector<ReceiverBandMod::ReceiverBand>.
	 */
	static std::vector<ReceiverBandMod::ReceiverBand> getReceiverBand1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<ReceiverBandMod::ReceiverBand> > from a string.
	 * @param xml the string to be converted into a vector<vector<ReceiverBandMod::ReceiverBand> >
	 * @return a vector<vector<ReceiverBandMod::ReceiverBand> >.
	 */
	static std::vector<std::vector<ReceiverBandMod::ReceiverBand> > getReceiverBand2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<ReceiverBandMod::ReceiverBand> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<ReceiverBandMod::ReceiverBand> > >
	 * @return a vector<vector<vector<ReceiverBandMod::ReceiverBand> > >.
	 */
	static std::vector<std::vector<std::vector<ReceiverBandMod::ReceiverBand> > > getReceiverBand3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SBTypeMod::SBType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SBTypeMod::SBType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SBTypeMod::SBType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SBTypeMod::SBType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SBTypeMod::SBType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SBTypeMod::SBType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SBTypeMod::SBType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SBTypeMod::SBType> > >& vvv_e); 

	/**
	 * Returns a SBTypeMod::SBType from a string.
	 * @param xml the string to be converted into a SBTypeMod::SBType
	 * @return a SBTypeMod::SBType.
	 */
	static SBTypeMod::SBType getSBType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SBTypeMod::SBType> from a string.
	 * @param xml the string to be converted into a vector<SBTypeMod::SBType>
	 * @return a vector<SBTypeMod::SBType>.
	 */
	static std::vector<SBTypeMod::SBType> getSBType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SBTypeMod::SBType> > from a string.
	 * @param xml the string to be converted into a vector<vector<SBTypeMod::SBType> >
	 * @return a vector<vector<SBTypeMod::SBType> >.
	 */
	static std::vector<std::vector<SBTypeMod::SBType> > getSBType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SBTypeMod::SBType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SBTypeMod::SBType> > >
	 * @return a vector<vector<vector<SBTypeMod::SBType> > >.
	 */
	static std::vector<std::vector<std::vector<SBTypeMod::SBType> > > getSBType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  DirectionReferenceCodeMod::DirectionReferenceCode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, DirectionReferenceCodeMod::DirectionReferenceCode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<DirectionReferenceCodeMod::DirectionReferenceCode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<DirectionReferenceCodeMod::DirectionReferenceCode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<DirectionReferenceCodeMod::DirectionReferenceCode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<DirectionReferenceCodeMod::DirectionReferenceCode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<DirectionReferenceCodeMod::DirectionReferenceCode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<DirectionReferenceCodeMod::DirectionReferenceCode> > >& vvv_e); 

	/**
	 * Returns a DirectionReferenceCodeMod::DirectionReferenceCode from a string.
	 * @param xml the string to be converted into a DirectionReferenceCodeMod::DirectionReferenceCode
	 * @return a DirectionReferenceCodeMod::DirectionReferenceCode.
	 */
	static DirectionReferenceCodeMod::DirectionReferenceCode getDirectionReferenceCode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<DirectionReferenceCodeMod::DirectionReferenceCode> from a string.
	 * @param xml the string to be converted into a vector<DirectionReferenceCodeMod::DirectionReferenceCode>
	 * @return a vector<DirectionReferenceCodeMod::DirectionReferenceCode>.
	 */
	static std::vector<DirectionReferenceCodeMod::DirectionReferenceCode> getDirectionReferenceCode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > from a string.
	 * @param xml the string to be converted into a vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> >
	 * @return a vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> >.
	 */
	static std::vector<std::vector<DirectionReferenceCodeMod::DirectionReferenceCode> > getDirectionReferenceCode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > >
	 * @return a vector<vector<vector<DirectionReferenceCodeMod::DirectionReferenceCode> > >.
	 */
	static std::vector<std::vector<std::vector<DirectionReferenceCodeMod::DirectionReferenceCode> > > getDirectionReferenceCode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CorrelationModeMod::CorrelationMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CorrelationModeMod::CorrelationMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CorrelationModeMod::CorrelationMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CorrelationModeMod::CorrelationMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CorrelationModeMod::CorrelationMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CorrelationModeMod::CorrelationMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CorrelationModeMod::CorrelationMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CorrelationModeMod::CorrelationMode> > >& vvv_e); 

	/**
	 * Returns a CorrelationModeMod::CorrelationMode from a string.
	 * @param xml the string to be converted into a CorrelationModeMod::CorrelationMode
	 * @return a CorrelationModeMod::CorrelationMode.
	 */
	static CorrelationModeMod::CorrelationMode getCorrelationMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CorrelationModeMod::CorrelationMode> from a string.
	 * @param xml the string to be converted into a vector<CorrelationModeMod::CorrelationMode>
	 * @return a vector<CorrelationModeMod::CorrelationMode>.
	 */
	static std::vector<CorrelationModeMod::CorrelationMode> getCorrelationMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CorrelationModeMod::CorrelationMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<CorrelationModeMod::CorrelationMode> >
	 * @return a vector<vector<CorrelationModeMod::CorrelationMode> >.
	 */
	static std::vector<std::vector<CorrelationModeMod::CorrelationMode> > getCorrelationMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CorrelationModeMod::CorrelationMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CorrelationModeMod::CorrelationMode> > >
	 * @return a vector<vector<vector<CorrelationModeMod::CorrelationMode> > >.
	 */
	static std::vector<std::vector<std::vector<CorrelationModeMod::CorrelationMode> > > getCorrelationMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AtmPhaseCorrectionMod::AtmPhaseCorrection value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AtmPhaseCorrectionMod::AtmPhaseCorrection e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > >& vvv_e); 

	/**
	 * Returns a AtmPhaseCorrectionMod::AtmPhaseCorrection from a string.
	 * @param xml the string to be converted into a AtmPhaseCorrectionMod::AtmPhaseCorrection
	 * @return a AtmPhaseCorrectionMod::AtmPhaseCorrection.
	 */
	static AtmPhaseCorrectionMod::AtmPhaseCorrection getAtmPhaseCorrection(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> from a string.
	 * @param xml the string to be converted into a vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>
	 * @return a vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>.
	 */
	static std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> getAtmPhaseCorrection1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > from a string.
	 * @param xml the string to be converted into a vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> >
	 * @return a vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> >.
	 */
	static std::vector<std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > getAtmPhaseCorrection2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > >
	 * @return a vector<vector<vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > >.
	 */
	static std::vector<std::vector<std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> > > getAtmPhaseCorrection3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  ProcessorTypeMod::ProcessorType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, ProcessorTypeMod::ProcessorType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<ProcessorTypeMod::ProcessorType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<ProcessorTypeMod::ProcessorType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<ProcessorTypeMod::ProcessorType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<ProcessorTypeMod::ProcessorType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<ProcessorTypeMod::ProcessorType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<ProcessorTypeMod::ProcessorType> > >& vvv_e); 

	/**
	 * Returns a ProcessorTypeMod::ProcessorType from a string.
	 * @param xml the string to be converted into a ProcessorTypeMod::ProcessorType
	 * @return a ProcessorTypeMod::ProcessorType.
	 */
	static ProcessorTypeMod::ProcessorType getProcessorType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<ProcessorTypeMod::ProcessorType> from a string.
	 * @param xml the string to be converted into a vector<ProcessorTypeMod::ProcessorType>
	 * @return a vector<ProcessorTypeMod::ProcessorType>.
	 */
	static std::vector<ProcessorTypeMod::ProcessorType> getProcessorType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<ProcessorTypeMod::ProcessorType> > from a string.
	 * @param xml the string to be converted into a vector<vector<ProcessorTypeMod::ProcessorType> >
	 * @return a vector<vector<ProcessorTypeMod::ProcessorType> >.
	 */
	static std::vector<std::vector<ProcessorTypeMod::ProcessorType> > getProcessorType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<ProcessorTypeMod::ProcessorType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<ProcessorTypeMod::ProcessorType> > >
	 * @return a vector<vector<vector<ProcessorTypeMod::ProcessorType> > >.
	 */
	static std::vector<std::vector<std::vector<ProcessorTypeMod::ProcessorType> > > getProcessorType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SpectralResolutionTypeMod::SpectralResolutionType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SpectralResolutionTypeMod::SpectralResolutionType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SpectralResolutionTypeMod::SpectralResolutionType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SpectralResolutionTypeMod::SpectralResolutionType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SpectralResolutionTypeMod::SpectralResolutionType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SpectralResolutionTypeMod::SpectralResolutionType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SpectralResolutionTypeMod::SpectralResolutionType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SpectralResolutionTypeMod::SpectralResolutionType> > >& vvv_e); 

	/**
	 * Returns a SpectralResolutionTypeMod::SpectralResolutionType from a string.
	 * @param xml the string to be converted into a SpectralResolutionTypeMod::SpectralResolutionType
	 * @return a SpectralResolutionTypeMod::SpectralResolutionType.
	 */
	static SpectralResolutionTypeMod::SpectralResolutionType getSpectralResolutionType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SpectralResolutionTypeMod::SpectralResolutionType> from a string.
	 * @param xml the string to be converted into a vector<SpectralResolutionTypeMod::SpectralResolutionType>
	 * @return a vector<SpectralResolutionTypeMod::SpectralResolutionType>.
	 */
	static std::vector<SpectralResolutionTypeMod::SpectralResolutionType> getSpectralResolutionType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > from a string.
	 * @param xml the string to be converted into a vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> >
	 * @return a vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> >.
	 */
	static std::vector<std::vector<SpectralResolutionTypeMod::SpectralResolutionType> > getSpectralResolutionType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > >
	 * @return a vector<vector<vector<SpectralResolutionTypeMod::SpectralResolutionType> > >.
	 */
	static std::vector<std::vector<std::vector<SpectralResolutionTypeMod::SpectralResolutionType> > > getSpectralResolutionType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CalibrationDeviceMod::CalibrationDevice value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CalibrationDeviceMod::CalibrationDevice e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CalibrationDeviceMod::CalibrationDevice>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CalibrationDeviceMod::CalibrationDevice>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CalibrationDeviceMod::CalibrationDevice> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CalibrationDeviceMod::CalibrationDevice> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CalibrationDeviceMod::CalibrationDevice> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CalibrationDeviceMod::CalibrationDevice> > >& vvv_e); 

	/**
	 * Returns a CalibrationDeviceMod::CalibrationDevice from a string.
	 * @param xml the string to be converted into a CalibrationDeviceMod::CalibrationDevice
	 * @return a CalibrationDeviceMod::CalibrationDevice.
	 */
	static CalibrationDeviceMod::CalibrationDevice getCalibrationDevice(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CalibrationDeviceMod::CalibrationDevice> from a string.
	 * @param xml the string to be converted into a vector<CalibrationDeviceMod::CalibrationDevice>
	 * @return a vector<CalibrationDeviceMod::CalibrationDevice>.
	 */
	static std::vector<CalibrationDeviceMod::CalibrationDevice> getCalibrationDevice1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CalibrationDeviceMod::CalibrationDevice> > from a string.
	 * @param xml the string to be converted into a vector<vector<CalibrationDeviceMod::CalibrationDevice> >
	 * @return a vector<vector<CalibrationDeviceMod::CalibrationDevice> >.
	 */
	static std::vector<std::vector<CalibrationDeviceMod::CalibrationDevice> > getCalibrationDevice2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CalibrationDeviceMod::CalibrationDevice> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CalibrationDeviceMod::CalibrationDevice> > >
	 * @return a vector<vector<vector<CalibrationDeviceMod::CalibrationDevice> > >.
	 */
	static std::vector<std::vector<std::vector<CalibrationDeviceMod::CalibrationDevice> > > getCalibrationDevice3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AntennaMakeMod::AntennaMake value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AntennaMakeMod::AntennaMake e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AntennaMakeMod::AntennaMake>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AntennaMakeMod::AntennaMake>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AntennaMakeMod::AntennaMake> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AntennaMakeMod::AntennaMake> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AntennaMakeMod::AntennaMake> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AntennaMakeMod::AntennaMake> > >& vvv_e); 

	/**
	 * Returns a AntennaMakeMod::AntennaMake from a string.
	 * @param xml the string to be converted into a AntennaMakeMod::AntennaMake
	 * @return a AntennaMakeMod::AntennaMake.
	 */
	static AntennaMakeMod::AntennaMake getAntennaMake(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AntennaMakeMod::AntennaMake> from a string.
	 * @param xml the string to be converted into a vector<AntennaMakeMod::AntennaMake>
	 * @return a vector<AntennaMakeMod::AntennaMake>.
	 */
	static std::vector<AntennaMakeMod::AntennaMake> getAntennaMake1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AntennaMakeMod::AntennaMake> > from a string.
	 * @param xml the string to be converted into a vector<vector<AntennaMakeMod::AntennaMake> >
	 * @return a vector<vector<AntennaMakeMod::AntennaMake> >.
	 */
	static std::vector<std::vector<AntennaMakeMod::AntennaMake> > getAntennaMake2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AntennaMakeMod::AntennaMake> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AntennaMakeMod::AntennaMake> > >
	 * @return a vector<vector<vector<AntennaMakeMod::AntennaMake> > >.
	 */
	static std::vector<std::vector<std::vector<AntennaMakeMod::AntennaMake> > > getAntennaMake3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AntennaTypeMod::AntennaType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AntennaTypeMod::AntennaType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AntennaTypeMod::AntennaType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AntennaTypeMod::AntennaType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AntennaTypeMod::AntennaType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AntennaTypeMod::AntennaType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AntennaTypeMod::AntennaType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AntennaTypeMod::AntennaType> > >& vvv_e); 

	/**
	 * Returns a AntennaTypeMod::AntennaType from a string.
	 * @param xml the string to be converted into a AntennaTypeMod::AntennaType
	 * @return a AntennaTypeMod::AntennaType.
	 */
	static AntennaTypeMod::AntennaType getAntennaType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AntennaTypeMod::AntennaType> from a string.
	 * @param xml the string to be converted into a vector<AntennaTypeMod::AntennaType>
	 * @return a vector<AntennaTypeMod::AntennaType>.
	 */
	static std::vector<AntennaTypeMod::AntennaType> getAntennaType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AntennaTypeMod::AntennaType> > from a string.
	 * @param xml the string to be converted into a vector<vector<AntennaTypeMod::AntennaType> >
	 * @return a vector<vector<AntennaTypeMod::AntennaType> >.
	 */
	static std::vector<std::vector<AntennaTypeMod::AntennaType> > getAntennaType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AntennaTypeMod::AntennaType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AntennaTypeMod::AntennaType> > >
	 * @return a vector<vector<vector<AntennaTypeMod::AntennaType> > >.
	 */
	static std::vector<std::vector<std::vector<AntennaTypeMod::AntennaType> > > getAntennaType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SourceModelMod::SourceModel value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SourceModelMod::SourceModel e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SourceModelMod::SourceModel>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SourceModelMod::SourceModel>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SourceModelMod::SourceModel> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SourceModelMod::SourceModel> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SourceModelMod::SourceModel> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SourceModelMod::SourceModel> > >& vvv_e); 

	/**
	 * Returns a SourceModelMod::SourceModel from a string.
	 * @param xml the string to be converted into a SourceModelMod::SourceModel
	 * @return a SourceModelMod::SourceModel.
	 */
	static SourceModelMod::SourceModel getSourceModel(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SourceModelMod::SourceModel> from a string.
	 * @param xml the string to be converted into a vector<SourceModelMod::SourceModel>
	 * @return a vector<SourceModelMod::SourceModel>.
	 */
	static std::vector<SourceModelMod::SourceModel> getSourceModel1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SourceModelMod::SourceModel> > from a string.
	 * @param xml the string to be converted into a vector<vector<SourceModelMod::SourceModel> >
	 * @return a vector<vector<SourceModelMod::SourceModel> >.
	 */
	static std::vector<std::vector<SourceModelMod::SourceModel> > getSourceModel2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SourceModelMod::SourceModel> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SourceModelMod::SourceModel> > >
	 * @return a vector<vector<vector<SourceModelMod::SourceModel> > >.
	 */
	static std::vector<std::vector<std::vector<SourceModelMod::SourceModel> > > getSourceModel3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  FrequencyReferenceCodeMod::FrequencyReferenceCode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, FrequencyReferenceCodeMod::FrequencyReferenceCode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<FrequencyReferenceCodeMod::FrequencyReferenceCode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > >& vvv_e); 

	/**
	 * Returns a FrequencyReferenceCodeMod::FrequencyReferenceCode from a string.
	 * @param xml the string to be converted into a FrequencyReferenceCodeMod::FrequencyReferenceCode
	 * @return a FrequencyReferenceCodeMod::FrequencyReferenceCode.
	 */
	static FrequencyReferenceCodeMod::FrequencyReferenceCode getFrequencyReferenceCode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> from a string.
	 * @param xml the string to be converted into a vector<FrequencyReferenceCodeMod::FrequencyReferenceCode>
	 * @return a vector<FrequencyReferenceCodeMod::FrequencyReferenceCode>.
	 */
	static std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> getFrequencyReferenceCode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > from a string.
	 * @param xml the string to be converted into a vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> >
	 * @return a vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> >.
	 */
	static std::vector<std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > getFrequencyReferenceCode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > >
	 * @return a vector<vector<vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > >.
	 */
	static std::vector<std::vector<std::vector<FrequencyReferenceCodeMod::FrequencyReferenceCode> > > getFrequencyReferenceCode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  StokesParameterMod::StokesParameter value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, StokesParameterMod::StokesParameter e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<StokesParameterMod::StokesParameter>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<StokesParameterMod::StokesParameter>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<StokesParameterMod::StokesParameter> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<StokesParameterMod::StokesParameter> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<StokesParameterMod::StokesParameter> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<StokesParameterMod::StokesParameter> > >& vvv_e); 

	/**
	 * Returns a StokesParameterMod::StokesParameter from a string.
	 * @param xml the string to be converted into a StokesParameterMod::StokesParameter
	 * @return a StokesParameterMod::StokesParameter.
	 */
	static StokesParameterMod::StokesParameter getStokesParameter(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<StokesParameterMod::StokesParameter> from a string.
	 * @param xml the string to be converted into a vector<StokesParameterMod::StokesParameter>
	 * @return a vector<StokesParameterMod::StokesParameter>.
	 */
	static std::vector<StokesParameterMod::StokesParameter> getStokesParameter1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<StokesParameterMod::StokesParameter> > from a string.
	 * @param xml the string to be converted into a vector<vector<StokesParameterMod::StokesParameter> >
	 * @return a vector<vector<StokesParameterMod::StokesParameter> >.
	 */
	static std::vector<std::vector<StokesParameterMod::StokesParameter> > getStokesParameter2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<StokesParameterMod::StokesParameter> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<StokesParameterMod::StokesParameter> > >
	 * @return a vector<vector<vector<StokesParameterMod::StokesParameter> > >.
	 */
	static std::vector<std::vector<std::vector<StokesParameterMod::StokesParameter> > > getStokesParameter3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > >& vvv_e); 

	/**
	 * Returns a RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode from a string.
	 * @param xml the string to be converted into a RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode
	 * @return a RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode.
	 */
	static RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode getRadialVelocityReferenceCode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> from a string.
	 * @param xml the string to be converted into a vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode>
	 * @return a vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode>.
	 */
	static std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> getRadialVelocityReferenceCode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > from a string.
	 * @param xml the string to be converted into a vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> >
	 * @return a vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> >.
	 */
	static std::vector<std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > getRadialVelocityReferenceCode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > >
	 * @return a vector<vector<vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > >.
	 */
	static std::vector<std::vector<std::vector<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> > > getRadialVelocityReferenceCode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  DopplerReferenceCodeMod::DopplerReferenceCode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, DopplerReferenceCodeMod::DopplerReferenceCode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<DopplerReferenceCodeMod::DopplerReferenceCode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<DopplerReferenceCodeMod::DopplerReferenceCode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<DopplerReferenceCodeMod::DopplerReferenceCode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<DopplerReferenceCodeMod::DopplerReferenceCode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<DopplerReferenceCodeMod::DopplerReferenceCode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<DopplerReferenceCodeMod::DopplerReferenceCode> > >& vvv_e); 

	/**
	 * Returns a DopplerReferenceCodeMod::DopplerReferenceCode from a string.
	 * @param xml the string to be converted into a DopplerReferenceCodeMod::DopplerReferenceCode
	 * @return a DopplerReferenceCodeMod::DopplerReferenceCode.
	 */
	static DopplerReferenceCodeMod::DopplerReferenceCode getDopplerReferenceCode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<DopplerReferenceCodeMod::DopplerReferenceCode> from a string.
	 * @param xml the string to be converted into a vector<DopplerReferenceCodeMod::DopplerReferenceCode>
	 * @return a vector<DopplerReferenceCodeMod::DopplerReferenceCode>.
	 */
	static std::vector<DopplerReferenceCodeMod::DopplerReferenceCode> getDopplerReferenceCode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > from a string.
	 * @param xml the string to be converted into a vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> >
	 * @return a vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> >.
	 */
	static std::vector<std::vector<DopplerReferenceCodeMod::DopplerReferenceCode> > getDopplerReferenceCode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > >
	 * @return a vector<vector<vector<DopplerReferenceCodeMod::DopplerReferenceCode> > >.
	 */
	static std::vector<std::vector<std::vector<DopplerReferenceCodeMod::DopplerReferenceCode> > > getDopplerReferenceCode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  PolarizationTypeMod::PolarizationType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, PolarizationTypeMod::PolarizationType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<PolarizationTypeMod::PolarizationType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<PolarizationTypeMod::PolarizationType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<PolarizationTypeMod::PolarizationType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<PolarizationTypeMod::PolarizationType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<PolarizationTypeMod::PolarizationType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<PolarizationTypeMod::PolarizationType> > >& vvv_e); 

	/**
	 * Returns a PolarizationTypeMod::PolarizationType from a string.
	 * @param xml the string to be converted into a PolarizationTypeMod::PolarizationType
	 * @return a PolarizationTypeMod::PolarizationType.
	 */
	static PolarizationTypeMod::PolarizationType getPolarizationType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<PolarizationTypeMod::PolarizationType> from a string.
	 * @param xml the string to be converted into a vector<PolarizationTypeMod::PolarizationType>
	 * @return a vector<PolarizationTypeMod::PolarizationType>.
	 */
	static std::vector<PolarizationTypeMod::PolarizationType> getPolarizationType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<PolarizationTypeMod::PolarizationType> > from a string.
	 * @param xml the string to be converted into a vector<vector<PolarizationTypeMod::PolarizationType> >
	 * @return a vector<vector<PolarizationTypeMod::PolarizationType> >.
	 */
	static std::vector<std::vector<PolarizationTypeMod::PolarizationType> > getPolarizationType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<PolarizationTypeMod::PolarizationType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<PolarizationTypeMod::PolarizationType> > >
	 * @return a vector<vector<vector<PolarizationTypeMod::PolarizationType> > >.
	 */
	static std::vector<std::vector<std::vector<PolarizationTypeMod::PolarizationType> > > getPolarizationType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  BasebandNameMod::BasebandName value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, BasebandNameMod::BasebandName e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<BasebandNameMod::BasebandName>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<BasebandNameMod::BasebandName>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<BasebandNameMod::BasebandName> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<BasebandNameMod::BasebandName> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<BasebandNameMod::BasebandName> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<BasebandNameMod::BasebandName> > >& vvv_e); 

	/**
	 * Returns a BasebandNameMod::BasebandName from a string.
	 * @param xml the string to be converted into a BasebandNameMod::BasebandName
	 * @return a BasebandNameMod::BasebandName.
	 */
	static BasebandNameMod::BasebandName getBasebandName(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<BasebandNameMod::BasebandName> from a string.
	 * @param xml the string to be converted into a vector<BasebandNameMod::BasebandName>
	 * @return a vector<BasebandNameMod::BasebandName>.
	 */
	static std::vector<BasebandNameMod::BasebandName> getBasebandName1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<BasebandNameMod::BasebandName> > from a string.
	 * @param xml the string to be converted into a vector<vector<BasebandNameMod::BasebandName> >
	 * @return a vector<vector<BasebandNameMod::BasebandName> >.
	 */
	static std::vector<std::vector<BasebandNameMod::BasebandName> > getBasebandName2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<BasebandNameMod::BasebandName> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<BasebandNameMod::BasebandName> > >
	 * @return a vector<vector<vector<BasebandNameMod::BasebandName> > >.
	 */
	static std::vector<std::vector<std::vector<BasebandNameMod::BasebandName> > > getBasebandName3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  NetSidebandMod::NetSideband value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, NetSidebandMod::NetSideband e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<NetSidebandMod::NetSideband>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<NetSidebandMod::NetSideband>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<NetSidebandMod::NetSideband> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<NetSidebandMod::NetSideband> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<NetSidebandMod::NetSideband> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<NetSidebandMod::NetSideband> > >& vvv_e); 

	/**
	 * Returns a NetSidebandMod::NetSideband from a string.
	 * @param xml the string to be converted into a NetSidebandMod::NetSideband
	 * @return a NetSidebandMod::NetSideband.
	 */
	static NetSidebandMod::NetSideband getNetSideband(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<NetSidebandMod::NetSideband> from a string.
	 * @param xml the string to be converted into a vector<NetSidebandMod::NetSideband>
	 * @return a vector<NetSidebandMod::NetSideband>.
	 */
	static std::vector<NetSidebandMod::NetSideband> getNetSideband1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<NetSidebandMod::NetSideband> > from a string.
	 * @param xml the string to be converted into a vector<vector<NetSidebandMod::NetSideband> >
	 * @return a vector<vector<NetSidebandMod::NetSideband> >.
	 */
	static std::vector<std::vector<NetSidebandMod::NetSideband> > getNetSideband2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<NetSidebandMod::NetSideband> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<NetSidebandMod::NetSideband> > >
	 * @return a vector<vector<vector<NetSidebandMod::NetSideband> > >.
	 */
	static std::vector<std::vector<std::vector<NetSidebandMod::NetSideband> > > getNetSideband3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SidebandProcessingModeMod::SidebandProcessingMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SidebandProcessingModeMod::SidebandProcessingMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SidebandProcessingModeMod::SidebandProcessingMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SidebandProcessingModeMod::SidebandProcessingMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SidebandProcessingModeMod::SidebandProcessingMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SidebandProcessingModeMod::SidebandProcessingMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SidebandProcessingModeMod::SidebandProcessingMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SidebandProcessingModeMod::SidebandProcessingMode> > >& vvv_e); 

	/**
	 * Returns a SidebandProcessingModeMod::SidebandProcessingMode from a string.
	 * @param xml the string to be converted into a SidebandProcessingModeMod::SidebandProcessingMode
	 * @return a SidebandProcessingModeMod::SidebandProcessingMode.
	 */
	static SidebandProcessingModeMod::SidebandProcessingMode getSidebandProcessingMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SidebandProcessingModeMod::SidebandProcessingMode> from a string.
	 * @param xml the string to be converted into a vector<SidebandProcessingModeMod::SidebandProcessingMode>
	 * @return a vector<SidebandProcessingModeMod::SidebandProcessingMode>.
	 */
	static std::vector<SidebandProcessingModeMod::SidebandProcessingMode> getSidebandProcessingMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> >
	 * @return a vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> >.
	 */
	static std::vector<std::vector<SidebandProcessingModeMod::SidebandProcessingMode> > getSidebandProcessingMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > >
	 * @return a vector<vector<vector<SidebandProcessingModeMod::SidebandProcessingMode> > >.
	 */
	static std::vector<std::vector<std::vector<SidebandProcessingModeMod::SidebandProcessingMode> > > getSidebandProcessingMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  WindowFunctionMod::WindowFunction value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, WindowFunctionMod::WindowFunction e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<WindowFunctionMod::WindowFunction>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<WindowFunctionMod::WindowFunction>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<WindowFunctionMod::WindowFunction> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<WindowFunctionMod::WindowFunction> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<WindowFunctionMod::WindowFunction> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<WindowFunctionMod::WindowFunction> > >& vvv_e); 

	/**
	 * Returns a WindowFunctionMod::WindowFunction from a string.
	 * @param xml the string to be converted into a WindowFunctionMod::WindowFunction
	 * @return a WindowFunctionMod::WindowFunction.
	 */
	static WindowFunctionMod::WindowFunction getWindowFunction(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<WindowFunctionMod::WindowFunction> from a string.
	 * @param xml the string to be converted into a vector<WindowFunctionMod::WindowFunction>
	 * @return a vector<WindowFunctionMod::WindowFunction>.
	 */
	static std::vector<WindowFunctionMod::WindowFunction> getWindowFunction1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<WindowFunctionMod::WindowFunction> > from a string.
	 * @param xml the string to be converted into a vector<vector<WindowFunctionMod::WindowFunction> >
	 * @return a vector<vector<WindowFunctionMod::WindowFunction> >.
	 */
	static std::vector<std::vector<WindowFunctionMod::WindowFunction> > getWindowFunction2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<WindowFunctionMod::WindowFunction> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<WindowFunctionMod::WindowFunction> > >
	 * @return a vector<vector<vector<WindowFunctionMod::WindowFunction> > >.
	 */
	static std::vector<std::vector<std::vector<WindowFunctionMod::WindowFunction> > > getWindowFunction3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CorrelationBitMod::CorrelationBit value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CorrelationBitMod::CorrelationBit e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CorrelationBitMod::CorrelationBit>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CorrelationBitMod::CorrelationBit>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CorrelationBitMod::CorrelationBit> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CorrelationBitMod::CorrelationBit> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CorrelationBitMod::CorrelationBit> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CorrelationBitMod::CorrelationBit> > >& vvv_e); 

	/**
	 * Returns a CorrelationBitMod::CorrelationBit from a string.
	 * @param xml the string to be converted into a CorrelationBitMod::CorrelationBit
	 * @return a CorrelationBitMod::CorrelationBit.
	 */
	static CorrelationBitMod::CorrelationBit getCorrelationBit(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CorrelationBitMod::CorrelationBit> from a string.
	 * @param xml the string to be converted into a vector<CorrelationBitMod::CorrelationBit>
	 * @return a vector<CorrelationBitMod::CorrelationBit>.
	 */
	static std::vector<CorrelationBitMod::CorrelationBit> getCorrelationBit1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CorrelationBitMod::CorrelationBit> > from a string.
	 * @param xml the string to be converted into a vector<vector<CorrelationBitMod::CorrelationBit> >
	 * @return a vector<vector<CorrelationBitMod::CorrelationBit> >.
	 */
	static std::vector<std::vector<CorrelationBitMod::CorrelationBit> > getCorrelationBit2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CorrelationBitMod::CorrelationBit> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CorrelationBitMod::CorrelationBit> > >
	 * @return a vector<vector<vector<CorrelationBitMod::CorrelationBit> > >.
	 */
	static std::vector<std::vector<std::vector<CorrelationBitMod::CorrelationBit> > > getCorrelationBit3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  ReceiverSidebandMod::ReceiverSideband value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, ReceiverSidebandMod::ReceiverSideband e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<ReceiverSidebandMod::ReceiverSideband>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<ReceiverSidebandMod::ReceiverSideband>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<ReceiverSidebandMod::ReceiverSideband> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<ReceiverSidebandMod::ReceiverSideband> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<ReceiverSidebandMod::ReceiverSideband> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<ReceiverSidebandMod::ReceiverSideband> > >& vvv_e); 

	/**
	 * Returns a ReceiverSidebandMod::ReceiverSideband from a string.
	 * @param xml the string to be converted into a ReceiverSidebandMod::ReceiverSideband
	 * @return a ReceiverSidebandMod::ReceiverSideband.
	 */
	static ReceiverSidebandMod::ReceiverSideband getReceiverSideband(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<ReceiverSidebandMod::ReceiverSideband> from a string.
	 * @param xml the string to be converted into a vector<ReceiverSidebandMod::ReceiverSideband>
	 * @return a vector<ReceiverSidebandMod::ReceiverSideband>.
	 */
	static std::vector<ReceiverSidebandMod::ReceiverSideband> getReceiverSideband1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<ReceiverSidebandMod::ReceiverSideband> > from a string.
	 * @param xml the string to be converted into a vector<vector<ReceiverSidebandMod::ReceiverSideband> >
	 * @return a vector<vector<ReceiverSidebandMod::ReceiverSideband> >.
	 */
	static std::vector<std::vector<ReceiverSidebandMod::ReceiverSideband> > getReceiverSideband2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<ReceiverSidebandMod::ReceiverSideband> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<ReceiverSidebandMod::ReceiverSideband> > >
	 * @return a vector<vector<vector<ReceiverSidebandMod::ReceiverSideband> > >.
	 */
	static std::vector<std::vector<std::vector<ReceiverSidebandMod::ReceiverSideband> > > getReceiverSideband3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  ProcessorSubTypeMod::ProcessorSubType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, ProcessorSubTypeMod::ProcessorSubType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<ProcessorSubTypeMod::ProcessorSubType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<ProcessorSubTypeMod::ProcessorSubType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<ProcessorSubTypeMod::ProcessorSubType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<ProcessorSubTypeMod::ProcessorSubType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<ProcessorSubTypeMod::ProcessorSubType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<ProcessorSubTypeMod::ProcessorSubType> > >& vvv_e); 

	/**
	 * Returns a ProcessorSubTypeMod::ProcessorSubType from a string.
	 * @param xml the string to be converted into a ProcessorSubTypeMod::ProcessorSubType
	 * @return a ProcessorSubTypeMod::ProcessorSubType.
	 */
	static ProcessorSubTypeMod::ProcessorSubType getProcessorSubType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<ProcessorSubTypeMod::ProcessorSubType> from a string.
	 * @param xml the string to be converted into a vector<ProcessorSubTypeMod::ProcessorSubType>
	 * @return a vector<ProcessorSubTypeMod::ProcessorSubType>.
	 */
	static std::vector<ProcessorSubTypeMod::ProcessorSubType> getProcessorSubType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<ProcessorSubTypeMod::ProcessorSubType> > from a string.
	 * @param xml the string to be converted into a vector<vector<ProcessorSubTypeMod::ProcessorSubType> >
	 * @return a vector<vector<ProcessorSubTypeMod::ProcessorSubType> >.
	 */
	static std::vector<std::vector<ProcessorSubTypeMod::ProcessorSubType> > getProcessorSubType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<ProcessorSubTypeMod::ProcessorSubType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<ProcessorSubTypeMod::ProcessorSubType> > >
	 * @return a vector<vector<vector<ProcessorSubTypeMod::ProcessorSubType> > >.
	 */
	static std::vector<std::vector<std::vector<ProcessorSubTypeMod::ProcessorSubType> > > getProcessorSubType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AccumModeMod::AccumMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AccumModeMod::AccumMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AccumModeMod::AccumMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AccumModeMod::AccumMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AccumModeMod::AccumMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AccumModeMod::AccumMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AccumModeMod::AccumMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AccumModeMod::AccumMode> > >& vvv_e); 

	/**
	 * Returns a AccumModeMod::AccumMode from a string.
	 * @param xml the string to be converted into a AccumModeMod::AccumMode
	 * @return a AccumModeMod::AccumMode.
	 */
	static AccumModeMod::AccumMode getAccumMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AccumModeMod::AccumMode> from a string.
	 * @param xml the string to be converted into a vector<AccumModeMod::AccumMode>
	 * @return a vector<AccumModeMod::AccumMode>.
	 */
	static std::vector<AccumModeMod::AccumMode> getAccumMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AccumModeMod::AccumMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<AccumModeMod::AccumMode> >
	 * @return a vector<vector<AccumModeMod::AccumMode> >.
	 */
	static std::vector<std::vector<AccumModeMod::AccumMode> > getAccumMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AccumModeMod::AccumMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AccumModeMod::AccumMode> > >
	 * @return a vector<vector<vector<AccumModeMod::AccumMode> > >.
	 */
	static std::vector<std::vector<std::vector<AccumModeMod::AccumMode> > > getAccumMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AxisNameMod::AxisName value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AxisNameMod::AxisName e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AxisNameMod::AxisName>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AxisNameMod::AxisName>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AxisNameMod::AxisName> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AxisNameMod::AxisName> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AxisNameMod::AxisName> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AxisNameMod::AxisName> > >& vvv_e); 

	/**
	 * Returns a AxisNameMod::AxisName from a string.
	 * @param xml the string to be converted into a AxisNameMod::AxisName
	 * @return a AxisNameMod::AxisName.
	 */
	static AxisNameMod::AxisName getAxisName(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AxisNameMod::AxisName> from a string.
	 * @param xml the string to be converted into a vector<AxisNameMod::AxisName>
	 * @return a vector<AxisNameMod::AxisName>.
	 */
	static std::vector<AxisNameMod::AxisName> getAxisName1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AxisNameMod::AxisName> > from a string.
	 * @param xml the string to be converted into a vector<vector<AxisNameMod::AxisName> >
	 * @return a vector<vector<AxisNameMod::AxisName> >.
	 */
	static std::vector<std::vector<AxisNameMod::AxisName> > getAxisName2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AxisNameMod::AxisName> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AxisNameMod::AxisName> > >
	 * @return a vector<vector<vector<AxisNameMod::AxisName> > >.
	 */
	static std::vector<std::vector<std::vector<AxisNameMod::AxisName> > > getAxisName3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  FilterModeMod::FilterMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, FilterModeMod::FilterMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<FilterModeMod::FilterMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<FilterModeMod::FilterMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<FilterModeMod::FilterMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<FilterModeMod::FilterMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<FilterModeMod::FilterMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<FilterModeMod::FilterMode> > >& vvv_e); 

	/**
	 * Returns a FilterModeMod::FilterMode from a string.
	 * @param xml the string to be converted into a FilterModeMod::FilterMode
	 * @return a FilterModeMod::FilterMode.
	 */
	static FilterModeMod::FilterMode getFilterMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<FilterModeMod::FilterMode> from a string.
	 * @param xml the string to be converted into a vector<FilterModeMod::FilterMode>
	 * @return a vector<FilterModeMod::FilterMode>.
	 */
	static std::vector<FilterModeMod::FilterMode> getFilterMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<FilterModeMod::FilterMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<FilterModeMod::FilterMode> >
	 * @return a vector<vector<FilterModeMod::FilterMode> >.
	 */
	static std::vector<std::vector<FilterModeMod::FilterMode> > getFilterMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<FilterModeMod::FilterMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<FilterModeMod::FilterMode> > >
	 * @return a vector<vector<vector<FilterModeMod::FilterMode> > >.
	 */
	static std::vector<std::vector<std::vector<FilterModeMod::FilterMode> > > getFilterMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CorrelatorNameMod::CorrelatorName value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CorrelatorNameMod::CorrelatorName e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CorrelatorNameMod::CorrelatorName>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CorrelatorNameMod::CorrelatorName>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CorrelatorNameMod::CorrelatorName> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CorrelatorNameMod::CorrelatorName> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CorrelatorNameMod::CorrelatorName> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CorrelatorNameMod::CorrelatorName> > >& vvv_e); 

	/**
	 * Returns a CorrelatorNameMod::CorrelatorName from a string.
	 * @param xml the string to be converted into a CorrelatorNameMod::CorrelatorName
	 * @return a CorrelatorNameMod::CorrelatorName.
	 */
	static CorrelatorNameMod::CorrelatorName getCorrelatorName(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CorrelatorNameMod::CorrelatorName> from a string.
	 * @param xml the string to be converted into a vector<CorrelatorNameMod::CorrelatorName>
	 * @return a vector<CorrelatorNameMod::CorrelatorName>.
	 */
	static std::vector<CorrelatorNameMod::CorrelatorName> getCorrelatorName1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CorrelatorNameMod::CorrelatorName> > from a string.
	 * @param xml the string to be converted into a vector<vector<CorrelatorNameMod::CorrelatorName> >
	 * @return a vector<vector<CorrelatorNameMod::CorrelatorName> >.
	 */
	static std::vector<std::vector<CorrelatorNameMod::CorrelatorName> > getCorrelatorName2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CorrelatorNameMod::CorrelatorName> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CorrelatorNameMod::CorrelatorName> > >
	 * @return a vector<vector<vector<CorrelatorNameMod::CorrelatorName> > >.
	 */
	static std::vector<std::vector<std::vector<CorrelatorNameMod::CorrelatorName> > > getCorrelatorName3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  WVRMethodMod::WVRMethod value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, WVRMethodMod::WVRMethod e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<WVRMethodMod::WVRMethod>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<WVRMethodMod::WVRMethod>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<WVRMethodMod::WVRMethod> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<WVRMethodMod::WVRMethod> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<WVRMethodMod::WVRMethod> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<WVRMethodMod::WVRMethod> > >& vvv_e); 

	/**
	 * Returns a WVRMethodMod::WVRMethod from a string.
	 * @param xml the string to be converted into a WVRMethodMod::WVRMethod
	 * @return a WVRMethodMod::WVRMethod.
	 */
	static WVRMethodMod::WVRMethod getWVRMethod(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<WVRMethodMod::WVRMethod> from a string.
	 * @param xml the string to be converted into a vector<WVRMethodMod::WVRMethod>
	 * @return a vector<WVRMethodMod::WVRMethod>.
	 */
	static std::vector<WVRMethodMod::WVRMethod> getWVRMethod1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<WVRMethodMod::WVRMethod> > from a string.
	 * @param xml the string to be converted into a vector<vector<WVRMethodMod::WVRMethod> >
	 * @return a vector<vector<WVRMethodMod::WVRMethod> >.
	 */
	static std::vector<std::vector<WVRMethodMod::WVRMethod> > getWVRMethod2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<WVRMethodMod::WVRMethod> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<WVRMethodMod::WVRMethod> > >
	 * @return a vector<vector<vector<WVRMethodMod::WVRMethod> > >.
	 */
	static std::vector<std::vector<std::vector<WVRMethodMod::WVRMethod> > > getWVRMethod3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  ScanIntentMod::ScanIntent value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, ScanIntentMod::ScanIntent e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<ScanIntentMod::ScanIntent>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<ScanIntentMod::ScanIntent>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<ScanIntentMod::ScanIntent> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<ScanIntentMod::ScanIntent> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<ScanIntentMod::ScanIntent> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<ScanIntentMod::ScanIntent> > >& vvv_e); 

	/**
	 * Returns a ScanIntentMod::ScanIntent from a string.
	 * @param xml the string to be converted into a ScanIntentMod::ScanIntent
	 * @return a ScanIntentMod::ScanIntent.
	 */
	static ScanIntentMod::ScanIntent getScanIntent(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<ScanIntentMod::ScanIntent> from a string.
	 * @param xml the string to be converted into a vector<ScanIntentMod::ScanIntent>
	 * @return a vector<ScanIntentMod::ScanIntent>.
	 */
	static std::vector<ScanIntentMod::ScanIntent> getScanIntent1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<ScanIntentMod::ScanIntent> > from a string.
	 * @param xml the string to be converted into a vector<vector<ScanIntentMod::ScanIntent> >
	 * @return a vector<vector<ScanIntentMod::ScanIntent> >.
	 */
	static std::vector<std::vector<ScanIntentMod::ScanIntent> > getScanIntent2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<ScanIntentMod::ScanIntent> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<ScanIntentMod::ScanIntent> > >
	 * @return a vector<vector<vector<ScanIntentMod::ScanIntent> > >.
	 */
	static std::vector<std::vector<std::vector<ScanIntentMod::ScanIntent> > > getScanIntent3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CalDataOriginMod::CalDataOrigin value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CalDataOriginMod::CalDataOrigin e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CalDataOriginMod::CalDataOrigin>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CalDataOriginMod::CalDataOrigin>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CalDataOriginMod::CalDataOrigin> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CalDataOriginMod::CalDataOrigin> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CalDataOriginMod::CalDataOrigin> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CalDataOriginMod::CalDataOrigin> > >& vvv_e); 

	/**
	 * Returns a CalDataOriginMod::CalDataOrigin from a string.
	 * @param xml the string to be converted into a CalDataOriginMod::CalDataOrigin
	 * @return a CalDataOriginMod::CalDataOrigin.
	 */
	static CalDataOriginMod::CalDataOrigin getCalDataOrigin(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CalDataOriginMod::CalDataOrigin> from a string.
	 * @param xml the string to be converted into a vector<CalDataOriginMod::CalDataOrigin>
	 * @return a vector<CalDataOriginMod::CalDataOrigin>.
	 */
	static std::vector<CalDataOriginMod::CalDataOrigin> getCalDataOrigin1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CalDataOriginMod::CalDataOrigin> > from a string.
	 * @param xml the string to be converted into a vector<vector<CalDataOriginMod::CalDataOrigin> >
	 * @return a vector<vector<CalDataOriginMod::CalDataOrigin> >.
	 */
	static std::vector<std::vector<CalDataOriginMod::CalDataOrigin> > getCalDataOrigin2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CalDataOriginMod::CalDataOrigin> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CalDataOriginMod::CalDataOrigin> > >
	 * @return a vector<vector<vector<CalDataOriginMod::CalDataOrigin> > >.
	 */
	static std::vector<std::vector<std::vector<CalDataOriginMod::CalDataOrigin> > > getCalDataOrigin3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CalibrationFunctionMod::CalibrationFunction value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CalibrationFunctionMod::CalibrationFunction e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CalibrationFunctionMod::CalibrationFunction>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CalibrationFunctionMod::CalibrationFunction>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CalibrationFunctionMod::CalibrationFunction> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CalibrationFunctionMod::CalibrationFunction> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CalibrationFunctionMod::CalibrationFunction> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CalibrationFunctionMod::CalibrationFunction> > >& vvv_e); 

	/**
	 * Returns a CalibrationFunctionMod::CalibrationFunction from a string.
	 * @param xml the string to be converted into a CalibrationFunctionMod::CalibrationFunction
	 * @return a CalibrationFunctionMod::CalibrationFunction.
	 */
	static CalibrationFunctionMod::CalibrationFunction getCalibrationFunction(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CalibrationFunctionMod::CalibrationFunction> from a string.
	 * @param xml the string to be converted into a vector<CalibrationFunctionMod::CalibrationFunction>
	 * @return a vector<CalibrationFunctionMod::CalibrationFunction>.
	 */
	static std::vector<CalibrationFunctionMod::CalibrationFunction> getCalibrationFunction1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CalibrationFunctionMod::CalibrationFunction> > from a string.
	 * @param xml the string to be converted into a vector<vector<CalibrationFunctionMod::CalibrationFunction> >
	 * @return a vector<vector<CalibrationFunctionMod::CalibrationFunction> >.
	 */
	static std::vector<std::vector<CalibrationFunctionMod::CalibrationFunction> > getCalibrationFunction2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CalibrationFunctionMod::CalibrationFunction> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CalibrationFunctionMod::CalibrationFunction> > >
	 * @return a vector<vector<vector<CalibrationFunctionMod::CalibrationFunction> > >.
	 */
	static std::vector<std::vector<std::vector<CalibrationFunctionMod::CalibrationFunction> > > getCalibrationFunction3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CalibrationSetMod::CalibrationSet value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CalibrationSetMod::CalibrationSet e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CalibrationSetMod::CalibrationSet>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CalibrationSetMod::CalibrationSet>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CalibrationSetMod::CalibrationSet> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CalibrationSetMod::CalibrationSet> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CalibrationSetMod::CalibrationSet> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CalibrationSetMod::CalibrationSet> > >& vvv_e); 

	/**
	 * Returns a CalibrationSetMod::CalibrationSet from a string.
	 * @param xml the string to be converted into a CalibrationSetMod::CalibrationSet
	 * @return a CalibrationSetMod::CalibrationSet.
	 */
	static CalibrationSetMod::CalibrationSet getCalibrationSet(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CalibrationSetMod::CalibrationSet> from a string.
	 * @param xml the string to be converted into a vector<CalibrationSetMod::CalibrationSet>
	 * @return a vector<CalibrationSetMod::CalibrationSet>.
	 */
	static std::vector<CalibrationSetMod::CalibrationSet> getCalibrationSet1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CalibrationSetMod::CalibrationSet> > from a string.
	 * @param xml the string to be converted into a vector<vector<CalibrationSetMod::CalibrationSet> >
	 * @return a vector<vector<CalibrationSetMod::CalibrationSet> >.
	 */
	static std::vector<std::vector<CalibrationSetMod::CalibrationSet> > getCalibrationSet2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CalibrationSetMod::CalibrationSet> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CalibrationSetMod::CalibrationSet> > >
	 * @return a vector<vector<vector<CalibrationSetMod::CalibrationSet> > >.
	 */
	static std::vector<std::vector<std::vector<CalibrationSetMod::CalibrationSet> > > getCalibrationSet3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AntennaMotionPatternMod::AntennaMotionPattern value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AntennaMotionPatternMod::AntennaMotionPattern e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AntennaMotionPatternMod::AntennaMotionPattern>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AntennaMotionPatternMod::AntennaMotionPattern>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AntennaMotionPatternMod::AntennaMotionPattern> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AntennaMotionPatternMod::AntennaMotionPattern> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AntennaMotionPatternMod::AntennaMotionPattern> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AntennaMotionPatternMod::AntennaMotionPattern> > >& vvv_e); 

	/**
	 * Returns a AntennaMotionPatternMod::AntennaMotionPattern from a string.
	 * @param xml the string to be converted into a AntennaMotionPatternMod::AntennaMotionPattern
	 * @return a AntennaMotionPatternMod::AntennaMotionPattern.
	 */
	static AntennaMotionPatternMod::AntennaMotionPattern getAntennaMotionPattern(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AntennaMotionPatternMod::AntennaMotionPattern> from a string.
	 * @param xml the string to be converted into a vector<AntennaMotionPatternMod::AntennaMotionPattern>
	 * @return a vector<AntennaMotionPatternMod::AntennaMotionPattern>.
	 */
	static std::vector<AntennaMotionPatternMod::AntennaMotionPattern> getAntennaMotionPattern1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > from a string.
	 * @param xml the string to be converted into a vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> >
	 * @return a vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> >.
	 */
	static std::vector<std::vector<AntennaMotionPatternMod::AntennaMotionPattern> > getAntennaMotionPattern2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > >
	 * @return a vector<vector<vector<AntennaMotionPatternMod::AntennaMotionPattern> > >.
	 */
	static std::vector<std::vector<std::vector<AntennaMotionPatternMod::AntennaMotionPattern> > > getAntennaMotionPattern3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SubscanIntentMod::SubscanIntent value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SubscanIntentMod::SubscanIntent e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SubscanIntentMod::SubscanIntent>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SubscanIntentMod::SubscanIntent>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SubscanIntentMod::SubscanIntent> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SubscanIntentMod::SubscanIntent> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SubscanIntentMod::SubscanIntent> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SubscanIntentMod::SubscanIntent> > >& vvv_e); 

	/**
	 * Returns a SubscanIntentMod::SubscanIntent from a string.
	 * @param xml the string to be converted into a SubscanIntentMod::SubscanIntent
	 * @return a SubscanIntentMod::SubscanIntent.
	 */
	static SubscanIntentMod::SubscanIntent getSubscanIntent(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SubscanIntentMod::SubscanIntent> from a string.
	 * @param xml the string to be converted into a vector<SubscanIntentMod::SubscanIntent>
	 * @return a vector<SubscanIntentMod::SubscanIntent>.
	 */
	static std::vector<SubscanIntentMod::SubscanIntent> getSubscanIntent1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SubscanIntentMod::SubscanIntent> > from a string.
	 * @param xml the string to be converted into a vector<vector<SubscanIntentMod::SubscanIntent> >
	 * @return a vector<vector<SubscanIntentMod::SubscanIntent> >.
	 */
	static std::vector<std::vector<SubscanIntentMod::SubscanIntent> > getSubscanIntent2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SubscanIntentMod::SubscanIntent> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SubscanIntentMod::SubscanIntent> > >
	 * @return a vector<vector<vector<SubscanIntentMod::SubscanIntent> > >.
	 */
	static std::vector<std::vector<std::vector<SubscanIntentMod::SubscanIntent> > > getSubscanIntent3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SwitchingModeMod::SwitchingMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SwitchingModeMod::SwitchingMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SwitchingModeMod::SwitchingMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SwitchingModeMod::SwitchingMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SwitchingModeMod::SwitchingMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SwitchingModeMod::SwitchingMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SwitchingModeMod::SwitchingMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SwitchingModeMod::SwitchingMode> > >& vvv_e); 

	/**
	 * Returns a SwitchingModeMod::SwitchingMode from a string.
	 * @param xml the string to be converted into a SwitchingModeMod::SwitchingMode
	 * @return a SwitchingModeMod::SwitchingMode.
	 */
	static SwitchingModeMod::SwitchingMode getSwitchingMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SwitchingModeMod::SwitchingMode> from a string.
	 * @param xml the string to be converted into a vector<SwitchingModeMod::SwitchingMode>
	 * @return a vector<SwitchingModeMod::SwitchingMode>.
	 */
	static std::vector<SwitchingModeMod::SwitchingMode> getSwitchingMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SwitchingModeMod::SwitchingMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<SwitchingModeMod::SwitchingMode> >
	 * @return a vector<vector<SwitchingModeMod::SwitchingMode> >.
	 */
	static std::vector<std::vector<SwitchingModeMod::SwitchingMode> > getSwitchingMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SwitchingModeMod::SwitchingMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SwitchingModeMod::SwitchingMode> > >
	 * @return a vector<vector<vector<SwitchingModeMod::SwitchingMode> > >.
	 */
	static std::vector<std::vector<std::vector<SwitchingModeMod::SwitchingMode> > > getSwitchingMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CorrelatorCalibrationMod::CorrelatorCalibration value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CorrelatorCalibrationMod::CorrelatorCalibration e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CorrelatorCalibrationMod::CorrelatorCalibration>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CorrelatorCalibrationMod::CorrelatorCalibration>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CorrelatorCalibrationMod::CorrelatorCalibration> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CorrelatorCalibrationMod::CorrelatorCalibration> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CorrelatorCalibrationMod::CorrelatorCalibration> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CorrelatorCalibrationMod::CorrelatorCalibration> > >& vvv_e); 

	/**
	 * Returns a CorrelatorCalibrationMod::CorrelatorCalibration from a string.
	 * @param xml the string to be converted into a CorrelatorCalibrationMod::CorrelatorCalibration
	 * @return a CorrelatorCalibrationMod::CorrelatorCalibration.
	 */
	static CorrelatorCalibrationMod::CorrelatorCalibration getCorrelatorCalibration(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CorrelatorCalibrationMod::CorrelatorCalibration> from a string.
	 * @param xml the string to be converted into a vector<CorrelatorCalibrationMod::CorrelatorCalibration>
	 * @return a vector<CorrelatorCalibrationMod::CorrelatorCalibration>.
	 */
	static std::vector<CorrelatorCalibrationMod::CorrelatorCalibration> getCorrelatorCalibration1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > from a string.
	 * @param xml the string to be converted into a vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> >
	 * @return a vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> >.
	 */
	static std::vector<std::vector<CorrelatorCalibrationMod::CorrelatorCalibration> > getCorrelatorCalibration2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > >
	 * @return a vector<vector<vector<CorrelatorCalibrationMod::CorrelatorCalibration> > >.
	 */
	static std::vector<std::vector<std::vector<CorrelatorCalibrationMod::CorrelatorCalibration> > > getCorrelatorCalibration3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  TimeSamplingMod::TimeSampling value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, TimeSamplingMod::TimeSampling e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<TimeSamplingMod::TimeSampling>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<TimeSamplingMod::TimeSampling>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<TimeSamplingMod::TimeSampling> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<TimeSamplingMod::TimeSampling> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<TimeSamplingMod::TimeSampling> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<TimeSamplingMod::TimeSampling> > >& vvv_e); 

	/**
	 * Returns a TimeSamplingMod::TimeSampling from a string.
	 * @param xml the string to be converted into a TimeSamplingMod::TimeSampling
	 * @return a TimeSamplingMod::TimeSampling.
	 */
	static TimeSamplingMod::TimeSampling getTimeSampling(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<TimeSamplingMod::TimeSampling> from a string.
	 * @param xml the string to be converted into a vector<TimeSamplingMod::TimeSampling>
	 * @return a vector<TimeSamplingMod::TimeSampling>.
	 */
	static std::vector<TimeSamplingMod::TimeSampling> getTimeSampling1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<TimeSamplingMod::TimeSampling> > from a string.
	 * @param xml the string to be converted into a vector<vector<TimeSamplingMod::TimeSampling> >
	 * @return a vector<vector<TimeSamplingMod::TimeSampling> >.
	 */
	static std::vector<std::vector<TimeSamplingMod::TimeSampling> > getTimeSampling2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<TimeSamplingMod::TimeSampling> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<TimeSamplingMod::TimeSampling> > >
	 * @return a vector<vector<vector<TimeSamplingMod::TimeSampling> > >.
	 */
	static std::vector<std::vector<std::vector<TimeSamplingMod::TimeSampling> > > getTimeSampling3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CalTypeMod::CalType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CalTypeMod::CalType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CalTypeMod::CalType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CalTypeMod::CalType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CalTypeMod::CalType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CalTypeMod::CalType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CalTypeMod::CalType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CalTypeMod::CalType> > >& vvv_e); 

	/**
	 * Returns a CalTypeMod::CalType from a string.
	 * @param xml the string to be converted into a CalTypeMod::CalType
	 * @return a CalTypeMod::CalType.
	 */
	static CalTypeMod::CalType getCalType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CalTypeMod::CalType> from a string.
	 * @param xml the string to be converted into a vector<CalTypeMod::CalType>
	 * @return a vector<CalTypeMod::CalType>.
	 */
	static std::vector<CalTypeMod::CalType> getCalType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CalTypeMod::CalType> > from a string.
	 * @param xml the string to be converted into a vector<vector<CalTypeMod::CalType> >
	 * @return a vector<vector<CalTypeMod::CalType> >.
	 */
	static std::vector<std::vector<CalTypeMod::CalType> > getCalType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CalTypeMod::CalType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CalTypeMod::CalType> > >
	 * @return a vector<vector<vector<CalTypeMod::CalType> > >.
	 */
	static std::vector<std::vector<std::vector<CalTypeMod::CalType> > > getCalType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AssociatedCalNatureMod::AssociatedCalNature value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AssociatedCalNatureMod::AssociatedCalNature e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AssociatedCalNatureMod::AssociatedCalNature>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AssociatedCalNatureMod::AssociatedCalNature>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AssociatedCalNatureMod::AssociatedCalNature> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AssociatedCalNatureMod::AssociatedCalNature> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AssociatedCalNatureMod::AssociatedCalNature> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AssociatedCalNatureMod::AssociatedCalNature> > >& vvv_e); 

	/**
	 * Returns a AssociatedCalNatureMod::AssociatedCalNature from a string.
	 * @param xml the string to be converted into a AssociatedCalNatureMod::AssociatedCalNature
	 * @return a AssociatedCalNatureMod::AssociatedCalNature.
	 */
	static AssociatedCalNatureMod::AssociatedCalNature getAssociatedCalNature(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AssociatedCalNatureMod::AssociatedCalNature> from a string.
	 * @param xml the string to be converted into a vector<AssociatedCalNatureMod::AssociatedCalNature>
	 * @return a vector<AssociatedCalNatureMod::AssociatedCalNature>.
	 */
	static std::vector<AssociatedCalNatureMod::AssociatedCalNature> getAssociatedCalNature1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > from a string.
	 * @param xml the string to be converted into a vector<vector<AssociatedCalNatureMod::AssociatedCalNature> >
	 * @return a vector<vector<AssociatedCalNatureMod::AssociatedCalNature> >.
	 */
	static std::vector<std::vector<AssociatedCalNatureMod::AssociatedCalNature> > getAssociatedCalNature2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > >
	 * @return a vector<vector<vector<AssociatedCalNatureMod::AssociatedCalNature> > >.
	 */
	static std::vector<std::vector<std::vector<AssociatedCalNatureMod::AssociatedCalNature> > > getAssociatedCalNature3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  InvalidatingConditionMod::InvalidatingCondition value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, InvalidatingConditionMod::InvalidatingCondition e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<InvalidatingConditionMod::InvalidatingCondition>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<InvalidatingConditionMod::InvalidatingCondition>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<InvalidatingConditionMod::InvalidatingCondition> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<InvalidatingConditionMod::InvalidatingCondition> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<InvalidatingConditionMod::InvalidatingCondition> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<InvalidatingConditionMod::InvalidatingCondition> > >& vvv_e); 

	/**
	 * Returns a InvalidatingConditionMod::InvalidatingCondition from a string.
	 * @param xml the string to be converted into a InvalidatingConditionMod::InvalidatingCondition
	 * @return a InvalidatingConditionMod::InvalidatingCondition.
	 */
	static InvalidatingConditionMod::InvalidatingCondition getInvalidatingCondition(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<InvalidatingConditionMod::InvalidatingCondition> from a string.
	 * @param xml the string to be converted into a vector<InvalidatingConditionMod::InvalidatingCondition>
	 * @return a vector<InvalidatingConditionMod::InvalidatingCondition>.
	 */
	static std::vector<InvalidatingConditionMod::InvalidatingCondition> getInvalidatingCondition1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<InvalidatingConditionMod::InvalidatingCondition> > from a string.
	 * @param xml the string to be converted into a vector<vector<InvalidatingConditionMod::InvalidatingCondition> >
	 * @return a vector<vector<InvalidatingConditionMod::InvalidatingCondition> >.
	 */
	static std::vector<std::vector<InvalidatingConditionMod::InvalidatingCondition> > getInvalidatingCondition2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<InvalidatingConditionMod::InvalidatingCondition> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<InvalidatingConditionMod::InvalidatingCondition> > >
	 * @return a vector<vector<vector<InvalidatingConditionMod::InvalidatingCondition> > >.
	 */
	static std::vector<std::vector<std::vector<InvalidatingConditionMod::InvalidatingCondition> > > getInvalidatingCondition3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  PositionMethodMod::PositionMethod value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, PositionMethodMod::PositionMethod e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<PositionMethodMod::PositionMethod>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<PositionMethodMod::PositionMethod>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<PositionMethodMod::PositionMethod> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<PositionMethodMod::PositionMethod> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<PositionMethodMod::PositionMethod> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<PositionMethodMod::PositionMethod> > >& vvv_e); 

	/**
	 * Returns a PositionMethodMod::PositionMethod from a string.
	 * @param xml the string to be converted into a PositionMethodMod::PositionMethod
	 * @return a PositionMethodMod::PositionMethod.
	 */
	static PositionMethodMod::PositionMethod getPositionMethod(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<PositionMethodMod::PositionMethod> from a string.
	 * @param xml the string to be converted into a vector<PositionMethodMod::PositionMethod>
	 * @return a vector<PositionMethodMod::PositionMethod>.
	 */
	static std::vector<PositionMethodMod::PositionMethod> getPositionMethod1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<PositionMethodMod::PositionMethod> > from a string.
	 * @param xml the string to be converted into a vector<vector<PositionMethodMod::PositionMethod> >
	 * @return a vector<vector<PositionMethodMod::PositionMethod> >.
	 */
	static std::vector<std::vector<PositionMethodMod::PositionMethod> > getPositionMethod2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<PositionMethodMod::PositionMethod> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<PositionMethodMod::PositionMethod> > >
	 * @return a vector<vector<vector<PositionMethodMod::PositionMethod> > >.
	 */
	static std::vector<std::vector<std::vector<PositionMethodMod::PositionMethod> > > getPositionMethod3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  PointingModelModeMod::PointingModelMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, PointingModelModeMod::PointingModelMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<PointingModelModeMod::PointingModelMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<PointingModelModeMod::PointingModelMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<PointingModelModeMod::PointingModelMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<PointingModelModeMod::PointingModelMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<PointingModelModeMod::PointingModelMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<PointingModelModeMod::PointingModelMode> > >& vvv_e); 

	/**
	 * Returns a PointingModelModeMod::PointingModelMode from a string.
	 * @param xml the string to be converted into a PointingModelModeMod::PointingModelMode
	 * @return a PointingModelModeMod::PointingModelMode.
	 */
	static PointingModelModeMod::PointingModelMode getPointingModelMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<PointingModelModeMod::PointingModelMode> from a string.
	 * @param xml the string to be converted into a vector<PointingModelModeMod::PointingModelMode>
	 * @return a vector<PointingModelModeMod::PointingModelMode>.
	 */
	static std::vector<PointingModelModeMod::PointingModelMode> getPointingModelMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<PointingModelModeMod::PointingModelMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<PointingModelModeMod::PointingModelMode> >
	 * @return a vector<vector<PointingModelModeMod::PointingModelMode> >.
	 */
	static std::vector<std::vector<PointingModelModeMod::PointingModelMode> > getPointingModelMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<PointingModelModeMod::PointingModelMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<PointingModelModeMod::PointingModelMode> > >
	 * @return a vector<vector<vector<PointingModelModeMod::PointingModelMode> > >.
	 */
	static std::vector<std::vector<std::vector<PointingModelModeMod::PointingModelMode> > > getPointingModelMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  PointingMethodMod::PointingMethod value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, PointingMethodMod::PointingMethod e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<PointingMethodMod::PointingMethod>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<PointingMethodMod::PointingMethod>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<PointingMethodMod::PointingMethod> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<PointingMethodMod::PointingMethod> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<PointingMethodMod::PointingMethod> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<PointingMethodMod::PointingMethod> > >& vvv_e); 

	/**
	 * Returns a PointingMethodMod::PointingMethod from a string.
	 * @param xml the string to be converted into a PointingMethodMod::PointingMethod
	 * @return a PointingMethodMod::PointingMethod.
	 */
	static PointingMethodMod::PointingMethod getPointingMethod(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<PointingMethodMod::PointingMethod> from a string.
	 * @param xml the string to be converted into a vector<PointingMethodMod::PointingMethod>
	 * @return a vector<PointingMethodMod::PointingMethod>.
	 */
	static std::vector<PointingMethodMod::PointingMethod> getPointingMethod1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<PointingMethodMod::PointingMethod> > from a string.
	 * @param xml the string to be converted into a vector<vector<PointingMethodMod::PointingMethod> >
	 * @return a vector<vector<PointingMethodMod::PointingMethod> >.
	 */
	static std::vector<std::vector<PointingMethodMod::PointingMethod> > getPointingMethod2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<PointingMethodMod::PointingMethod> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<PointingMethodMod::PointingMethod> > >
	 * @return a vector<vector<vector<PointingMethodMod::PointingMethod> > >.
	 */
	static std::vector<std::vector<std::vector<PointingMethodMod::PointingMethod> > > getPointingMethod3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SyscalMethodMod::SyscalMethod value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SyscalMethodMod::SyscalMethod e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SyscalMethodMod::SyscalMethod>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SyscalMethodMod::SyscalMethod>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SyscalMethodMod::SyscalMethod> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SyscalMethodMod::SyscalMethod> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SyscalMethodMod::SyscalMethod> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SyscalMethodMod::SyscalMethod> > >& vvv_e); 

	/**
	 * Returns a SyscalMethodMod::SyscalMethod from a string.
	 * @param xml the string to be converted into a SyscalMethodMod::SyscalMethod
	 * @return a SyscalMethodMod::SyscalMethod.
	 */
	static SyscalMethodMod::SyscalMethod getSyscalMethod(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SyscalMethodMod::SyscalMethod> from a string.
	 * @param xml the string to be converted into a vector<SyscalMethodMod::SyscalMethod>
	 * @return a vector<SyscalMethodMod::SyscalMethod>.
	 */
	static std::vector<SyscalMethodMod::SyscalMethod> getSyscalMethod1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SyscalMethodMod::SyscalMethod> > from a string.
	 * @param xml the string to be converted into a vector<vector<SyscalMethodMod::SyscalMethod> >
	 * @return a vector<vector<SyscalMethodMod::SyscalMethod> >.
	 */
	static std::vector<std::vector<SyscalMethodMod::SyscalMethod> > getSyscalMethod2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SyscalMethodMod::SyscalMethod> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SyscalMethodMod::SyscalMethod> > >
	 * @return a vector<vector<vector<SyscalMethodMod::SyscalMethod> > >.
	 */
	static std::vector<std::vector<std::vector<SyscalMethodMod::SyscalMethod> > > getSyscalMethod3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CalCurveTypeMod::CalCurveType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CalCurveTypeMod::CalCurveType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CalCurveTypeMod::CalCurveType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CalCurveTypeMod::CalCurveType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CalCurveTypeMod::CalCurveType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CalCurveTypeMod::CalCurveType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CalCurveTypeMod::CalCurveType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CalCurveTypeMod::CalCurveType> > >& vvv_e); 

	/**
	 * Returns a CalCurveTypeMod::CalCurveType from a string.
	 * @param xml the string to be converted into a CalCurveTypeMod::CalCurveType
	 * @return a CalCurveTypeMod::CalCurveType.
	 */
	static CalCurveTypeMod::CalCurveType getCalCurveType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CalCurveTypeMod::CalCurveType> from a string.
	 * @param xml the string to be converted into a vector<CalCurveTypeMod::CalCurveType>
	 * @return a vector<CalCurveTypeMod::CalCurveType>.
	 */
	static std::vector<CalCurveTypeMod::CalCurveType> getCalCurveType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CalCurveTypeMod::CalCurveType> > from a string.
	 * @param xml the string to be converted into a vector<vector<CalCurveTypeMod::CalCurveType> >
	 * @return a vector<vector<CalCurveTypeMod::CalCurveType> >.
	 */
	static std::vector<std::vector<CalCurveTypeMod::CalCurveType> > getCalCurveType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CalCurveTypeMod::CalCurveType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CalCurveTypeMod::CalCurveType> > >
	 * @return a vector<vector<vector<CalCurveTypeMod::CalCurveType> > >.
	 */
	static std::vector<std::vector<std::vector<CalCurveTypeMod::CalCurveType> > > getCalCurveType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  StationTypeMod::StationType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, StationTypeMod::StationType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<StationTypeMod::StationType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<StationTypeMod::StationType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<StationTypeMod::StationType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<StationTypeMod::StationType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<StationTypeMod::StationType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<StationTypeMod::StationType> > >& vvv_e); 

	/**
	 * Returns a StationTypeMod::StationType from a string.
	 * @param xml the string to be converted into a StationTypeMod::StationType
	 * @return a StationTypeMod::StationType.
	 */
	static StationTypeMod::StationType getStationType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<StationTypeMod::StationType> from a string.
	 * @param xml the string to be converted into a vector<StationTypeMod::StationType>
	 * @return a vector<StationTypeMod::StationType>.
	 */
	static std::vector<StationTypeMod::StationType> getStationType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<StationTypeMod::StationType> > from a string.
	 * @param xml the string to be converted into a vector<vector<StationTypeMod::StationType> >
	 * @return a vector<vector<StationTypeMod::StationType> >.
	 */
	static std::vector<std::vector<StationTypeMod::StationType> > getStationType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<StationTypeMod::StationType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<StationTypeMod::StationType> > >
	 * @return a vector<vector<vector<StationTypeMod::StationType> > >.
	 */
	static std::vector<std::vector<std::vector<StationTypeMod::StationType> > > getStationType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  DetectorBandTypeMod::DetectorBandType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, DetectorBandTypeMod::DetectorBandType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<DetectorBandTypeMod::DetectorBandType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<DetectorBandTypeMod::DetectorBandType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<DetectorBandTypeMod::DetectorBandType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<DetectorBandTypeMod::DetectorBandType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<DetectorBandTypeMod::DetectorBandType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<DetectorBandTypeMod::DetectorBandType> > >& vvv_e); 

	/**
	 * Returns a DetectorBandTypeMod::DetectorBandType from a string.
	 * @param xml the string to be converted into a DetectorBandTypeMod::DetectorBandType
	 * @return a DetectorBandTypeMod::DetectorBandType.
	 */
	static DetectorBandTypeMod::DetectorBandType getDetectorBandType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<DetectorBandTypeMod::DetectorBandType> from a string.
	 * @param xml the string to be converted into a vector<DetectorBandTypeMod::DetectorBandType>
	 * @return a vector<DetectorBandTypeMod::DetectorBandType>.
	 */
	static std::vector<DetectorBandTypeMod::DetectorBandType> getDetectorBandType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<DetectorBandTypeMod::DetectorBandType> > from a string.
	 * @param xml the string to be converted into a vector<vector<DetectorBandTypeMod::DetectorBandType> >
	 * @return a vector<vector<DetectorBandTypeMod::DetectorBandType> >.
	 */
	static std::vector<std::vector<DetectorBandTypeMod::DetectorBandType> > getDetectorBandType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<DetectorBandTypeMod::DetectorBandType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<DetectorBandTypeMod::DetectorBandType> > >
	 * @return a vector<vector<vector<DetectorBandTypeMod::DetectorBandType> > >.
	 */
	static std::vector<std::vector<std::vector<DetectorBandTypeMod::DetectorBandType> > > getDetectorBandType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  FocusMethodMod::FocusMethod value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, FocusMethodMod::FocusMethod e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<FocusMethodMod::FocusMethod>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<FocusMethodMod::FocusMethod>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<FocusMethodMod::FocusMethod> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<FocusMethodMod::FocusMethod> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<FocusMethodMod::FocusMethod> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<FocusMethodMod::FocusMethod> > >& vvv_e); 

	/**
	 * Returns a FocusMethodMod::FocusMethod from a string.
	 * @param xml the string to be converted into a FocusMethodMod::FocusMethod
	 * @return a FocusMethodMod::FocusMethod.
	 */
	static FocusMethodMod::FocusMethod getFocusMethod(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<FocusMethodMod::FocusMethod> from a string.
	 * @param xml the string to be converted into a vector<FocusMethodMod::FocusMethod>
	 * @return a vector<FocusMethodMod::FocusMethod>.
	 */
	static std::vector<FocusMethodMod::FocusMethod> getFocusMethod1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<FocusMethodMod::FocusMethod> > from a string.
	 * @param xml the string to be converted into a vector<vector<FocusMethodMod::FocusMethod> >
	 * @return a vector<vector<FocusMethodMod::FocusMethod> >.
	 */
	static std::vector<std::vector<FocusMethodMod::FocusMethod> > getFocusMethod2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<FocusMethodMod::FocusMethod> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<FocusMethodMod::FocusMethod> > >
	 * @return a vector<vector<vector<FocusMethodMod::FocusMethod> > >.
	 */
	static std::vector<std::vector<std::vector<FocusMethodMod::FocusMethod> > > getFocusMethod3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  HolographyChannelTypeMod::HolographyChannelType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, HolographyChannelTypeMod::HolographyChannelType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<HolographyChannelTypeMod::HolographyChannelType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<HolographyChannelTypeMod::HolographyChannelType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<HolographyChannelTypeMod::HolographyChannelType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<HolographyChannelTypeMod::HolographyChannelType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<HolographyChannelTypeMod::HolographyChannelType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<HolographyChannelTypeMod::HolographyChannelType> > >& vvv_e); 

	/**
	 * Returns a HolographyChannelTypeMod::HolographyChannelType from a string.
	 * @param xml the string to be converted into a HolographyChannelTypeMod::HolographyChannelType
	 * @return a HolographyChannelTypeMod::HolographyChannelType.
	 */
	static HolographyChannelTypeMod::HolographyChannelType getHolographyChannelType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<HolographyChannelTypeMod::HolographyChannelType> from a string.
	 * @param xml the string to be converted into a vector<HolographyChannelTypeMod::HolographyChannelType>
	 * @return a vector<HolographyChannelTypeMod::HolographyChannelType>.
	 */
	static std::vector<HolographyChannelTypeMod::HolographyChannelType> getHolographyChannelType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<HolographyChannelTypeMod::HolographyChannelType> > from a string.
	 * @param xml the string to be converted into a vector<vector<HolographyChannelTypeMod::HolographyChannelType> >
	 * @return a vector<vector<HolographyChannelTypeMod::HolographyChannelType> >.
	 */
	static std::vector<std::vector<HolographyChannelTypeMod::HolographyChannelType> > getHolographyChannelType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<HolographyChannelTypeMod::HolographyChannelType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<HolographyChannelTypeMod::HolographyChannelType> > >
	 * @return a vector<vector<vector<HolographyChannelTypeMod::HolographyChannelType> > >.
	 */
	static std::vector<std::vector<std::vector<HolographyChannelTypeMod::HolographyChannelType> > > getHolographyChannelType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  FluxCalibrationMethodMod::FluxCalibrationMethod value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, FluxCalibrationMethodMod::FluxCalibrationMethod e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<FluxCalibrationMethodMod::FluxCalibrationMethod>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > >& vvv_e); 

	/**
	 * Returns a FluxCalibrationMethodMod::FluxCalibrationMethod from a string.
	 * @param xml the string to be converted into a FluxCalibrationMethodMod::FluxCalibrationMethod
	 * @return a FluxCalibrationMethodMod::FluxCalibrationMethod.
	 */
	static FluxCalibrationMethodMod::FluxCalibrationMethod getFluxCalibrationMethod(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<FluxCalibrationMethodMod::FluxCalibrationMethod> from a string.
	 * @param xml the string to be converted into a vector<FluxCalibrationMethodMod::FluxCalibrationMethod>
	 * @return a vector<FluxCalibrationMethodMod::FluxCalibrationMethod>.
	 */
	static std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod> getFluxCalibrationMethod1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > from a string.
	 * @param xml the string to be converted into a vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> >
	 * @return a vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> >.
	 */
	static std::vector<std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > getFluxCalibrationMethod2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > >
	 * @return a vector<vector<vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > >.
	 */
	static std::vector<std::vector<std::vector<FluxCalibrationMethodMod::FluxCalibrationMethod> > > getFluxCalibrationMethod3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  PrimaryBeamDescriptionMod::PrimaryBeamDescription value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, PrimaryBeamDescriptionMod::PrimaryBeamDescription e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > >& vvv_e); 

	/**
	 * Returns a PrimaryBeamDescriptionMod::PrimaryBeamDescription from a string.
	 * @param xml the string to be converted into a PrimaryBeamDescriptionMod::PrimaryBeamDescription
	 * @return a PrimaryBeamDescriptionMod::PrimaryBeamDescription.
	 */
	static PrimaryBeamDescriptionMod::PrimaryBeamDescription getPrimaryBeamDescription(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> from a string.
	 * @param xml the string to be converted into a vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription>
	 * @return a vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription>.
	 */
	static std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> getPrimaryBeamDescription1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > from a string.
	 * @param xml the string to be converted into a vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> >
	 * @return a vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> >.
	 */
	static std::vector<std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > getPrimaryBeamDescription2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > >
	 * @return a vector<vector<vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > >.
	 */
	static std::vector<std::vector<std::vector<PrimaryBeamDescriptionMod::PrimaryBeamDescription> > > getPrimaryBeamDescription3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  TimeScaleMod::TimeScale value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, TimeScaleMod::TimeScale e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<TimeScaleMod::TimeScale>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<TimeScaleMod::TimeScale>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<TimeScaleMod::TimeScale> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<TimeScaleMod::TimeScale> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<TimeScaleMod::TimeScale> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<TimeScaleMod::TimeScale> > >& vvv_e); 

	/**
	 * Returns a TimeScaleMod::TimeScale from a string.
	 * @param xml the string to be converted into a TimeScaleMod::TimeScale
	 * @return a TimeScaleMod::TimeScale.
	 */
	static TimeScaleMod::TimeScale getTimeScale(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<TimeScaleMod::TimeScale> from a string.
	 * @param xml the string to be converted into a vector<TimeScaleMod::TimeScale>
	 * @return a vector<TimeScaleMod::TimeScale>.
	 */
	static std::vector<TimeScaleMod::TimeScale> getTimeScale1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<TimeScaleMod::TimeScale> > from a string.
	 * @param xml the string to be converted into a vector<vector<TimeScaleMod::TimeScale> >
	 * @return a vector<vector<TimeScaleMod::TimeScale> >.
	 */
	static std::vector<std::vector<TimeScaleMod::TimeScale> > getTimeScale2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<TimeScaleMod::TimeScale> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<TimeScaleMod::TimeScale> > >
	 * @return a vector<vector<vector<TimeScaleMod::TimeScale> > >.
	 */
	static std::vector<std::vector<std::vector<TimeScaleMod::TimeScale> > > getTimeScale3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  DataScaleMod::DataScale value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, DataScaleMod::DataScale e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<DataScaleMod::DataScale>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<DataScaleMod::DataScale>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<DataScaleMod::DataScale> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<DataScaleMod::DataScale> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<DataScaleMod::DataScale> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<DataScaleMod::DataScale> > >& vvv_e); 

	/**
	 * Returns a DataScaleMod::DataScale from a string.
	 * @param xml the string to be converted into a DataScaleMod::DataScale
	 * @return a DataScaleMod::DataScale.
	 */
	static DataScaleMod::DataScale getDataScale(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<DataScaleMod::DataScale> from a string.
	 * @param xml the string to be converted into a vector<DataScaleMod::DataScale>
	 * @return a vector<DataScaleMod::DataScale>.
	 */
	static std::vector<DataScaleMod::DataScale> getDataScale1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<DataScaleMod::DataScale> > from a string.
	 * @param xml the string to be converted into a vector<vector<DataScaleMod::DataScale> >
	 * @return a vector<vector<DataScaleMod::DataScale> >.
	 */
	static std::vector<std::vector<DataScaleMod::DataScale> > getDataScale2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<DataScaleMod::DataScale> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<DataScaleMod::DataScale> > >
	 * @return a vector<vector<vector<DataScaleMod::DataScale> > >.
	 */
	static std::vector<std::vector<std::vector<DataScaleMod::DataScale> > > getDataScale3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  WeightTypeMod::WeightType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, WeightTypeMod::WeightType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<WeightTypeMod::WeightType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<WeightTypeMod::WeightType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<WeightTypeMod::WeightType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<WeightTypeMod::WeightType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<WeightTypeMod::WeightType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<WeightTypeMod::WeightType> > >& vvv_e); 

	/**
	 * Returns a WeightTypeMod::WeightType from a string.
	 * @param xml the string to be converted into a WeightTypeMod::WeightType
	 * @return a WeightTypeMod::WeightType.
	 */
	static WeightTypeMod::WeightType getWeightType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<WeightTypeMod::WeightType> from a string.
	 * @param xml the string to be converted into a vector<WeightTypeMod::WeightType>
	 * @return a vector<WeightTypeMod::WeightType>.
	 */
	static std::vector<WeightTypeMod::WeightType> getWeightType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<WeightTypeMod::WeightType> > from a string.
	 * @param xml the string to be converted into a vector<vector<WeightTypeMod::WeightType> >
	 * @return a vector<vector<WeightTypeMod::WeightType> >.
	 */
	static std::vector<std::vector<WeightTypeMod::WeightType> > getWeightType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<WeightTypeMod::WeightType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<WeightTypeMod::WeightType> > >
	 * @return a vector<vector<vector<WeightTypeMod::WeightType> > >.
	 */
	static std::vector<std::vector<std::vector<WeightTypeMod::WeightType> > > getWeightType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  DifferenceTypeMod::DifferenceType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, DifferenceTypeMod::DifferenceType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<DifferenceTypeMod::DifferenceType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<DifferenceTypeMod::DifferenceType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<DifferenceTypeMod::DifferenceType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<DifferenceTypeMod::DifferenceType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<DifferenceTypeMod::DifferenceType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<DifferenceTypeMod::DifferenceType> > >& vvv_e); 

	/**
	 * Returns a DifferenceTypeMod::DifferenceType from a string.
	 * @param xml the string to be converted into a DifferenceTypeMod::DifferenceType
	 * @return a DifferenceTypeMod::DifferenceType.
	 */
	static DifferenceTypeMod::DifferenceType getDifferenceType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<DifferenceTypeMod::DifferenceType> from a string.
	 * @param xml the string to be converted into a vector<DifferenceTypeMod::DifferenceType>
	 * @return a vector<DifferenceTypeMod::DifferenceType>.
	 */
	static std::vector<DifferenceTypeMod::DifferenceType> getDifferenceType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<DifferenceTypeMod::DifferenceType> > from a string.
	 * @param xml the string to be converted into a vector<vector<DifferenceTypeMod::DifferenceType> >
	 * @return a vector<vector<DifferenceTypeMod::DifferenceType> >.
	 */
	static std::vector<std::vector<DifferenceTypeMod::DifferenceType> > getDifferenceType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<DifferenceTypeMod::DifferenceType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<DifferenceTypeMod::DifferenceType> > >
	 * @return a vector<vector<vector<DifferenceTypeMod::DifferenceType> > >.
	 */
	static std::vector<std::vector<std::vector<DifferenceTypeMod::DifferenceType> > > getDifferenceType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CalibrationModeMod::CalibrationMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CalibrationModeMod::CalibrationMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CalibrationModeMod::CalibrationMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CalibrationModeMod::CalibrationMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CalibrationModeMod::CalibrationMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CalibrationModeMod::CalibrationMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CalibrationModeMod::CalibrationMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CalibrationModeMod::CalibrationMode> > >& vvv_e); 

	/**
	 * Returns a CalibrationModeMod::CalibrationMode from a string.
	 * @param xml the string to be converted into a CalibrationModeMod::CalibrationMode
	 * @return a CalibrationModeMod::CalibrationMode.
	 */
	static CalibrationModeMod::CalibrationMode getCalibrationMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CalibrationModeMod::CalibrationMode> from a string.
	 * @param xml the string to be converted into a vector<CalibrationModeMod::CalibrationMode>
	 * @return a vector<CalibrationModeMod::CalibrationMode>.
	 */
	static std::vector<CalibrationModeMod::CalibrationMode> getCalibrationMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CalibrationModeMod::CalibrationMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<CalibrationModeMod::CalibrationMode> >
	 * @return a vector<vector<CalibrationModeMod::CalibrationMode> >.
	 */
	static std::vector<std::vector<CalibrationModeMod::CalibrationMode> > getCalibrationMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CalibrationModeMod::CalibrationMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CalibrationModeMod::CalibrationMode> > >
	 * @return a vector<vector<vector<CalibrationModeMod::CalibrationMode> > >.
	 */
	static std::vector<std::vector<std::vector<CalibrationModeMod::CalibrationMode> > > getCalibrationMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  AssociatedFieldNatureMod::AssociatedFieldNature value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, AssociatedFieldNatureMod::AssociatedFieldNature e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<AssociatedFieldNatureMod::AssociatedFieldNature>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<AssociatedFieldNatureMod::AssociatedFieldNature>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<AssociatedFieldNatureMod::AssociatedFieldNature> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<AssociatedFieldNatureMod::AssociatedFieldNature> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<AssociatedFieldNatureMod::AssociatedFieldNature> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<AssociatedFieldNatureMod::AssociatedFieldNature> > >& vvv_e); 

	/**
	 * Returns a AssociatedFieldNatureMod::AssociatedFieldNature from a string.
	 * @param xml the string to be converted into a AssociatedFieldNatureMod::AssociatedFieldNature
	 * @return a AssociatedFieldNatureMod::AssociatedFieldNature.
	 */
	static AssociatedFieldNatureMod::AssociatedFieldNature getAssociatedFieldNature(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<AssociatedFieldNatureMod::AssociatedFieldNature> from a string.
	 * @param xml the string to be converted into a vector<AssociatedFieldNatureMod::AssociatedFieldNature>
	 * @return a vector<AssociatedFieldNatureMod::AssociatedFieldNature>.
	 */
	static std::vector<AssociatedFieldNatureMod::AssociatedFieldNature> getAssociatedFieldNature1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > from a string.
	 * @param xml the string to be converted into a vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> >
	 * @return a vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> >.
	 */
	static std::vector<std::vector<AssociatedFieldNatureMod::AssociatedFieldNature> > getAssociatedFieldNature2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > >
	 * @return a vector<vector<vector<AssociatedFieldNatureMod::AssociatedFieldNature> > >.
	 */
	static std::vector<std::vector<std::vector<AssociatedFieldNatureMod::AssociatedFieldNature> > > getAssociatedFieldNature3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  DataContentMod::DataContent value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, DataContentMod::DataContent e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<DataContentMod::DataContent>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<DataContentMod::DataContent>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<DataContentMod::DataContent> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<DataContentMod::DataContent> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<DataContentMod::DataContent> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<DataContentMod::DataContent> > >& vvv_e); 

	/**
	 * Returns a DataContentMod::DataContent from a string.
	 * @param xml the string to be converted into a DataContentMod::DataContent
	 * @return a DataContentMod::DataContent.
	 */
	static DataContentMod::DataContent getDataContent(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<DataContentMod::DataContent> from a string.
	 * @param xml the string to be converted into a vector<DataContentMod::DataContent>
	 * @return a vector<DataContentMod::DataContent>.
	 */
	static std::vector<DataContentMod::DataContent> getDataContent1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<DataContentMod::DataContent> > from a string.
	 * @param xml the string to be converted into a vector<vector<DataContentMod::DataContent> >
	 * @return a vector<vector<DataContentMod::DataContent> >.
	 */
	static std::vector<std::vector<DataContentMod::DataContent> > getDataContent2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<DataContentMod::DataContent> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<DataContentMod::DataContent> > >
	 * @return a vector<vector<vector<DataContentMod::DataContent> > >.
	 */
	static std::vector<std::vector<std::vector<DataContentMod::DataContent> > > getDataContent3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  PrimitiveDataTypeMod::PrimitiveDataType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, PrimitiveDataTypeMod::PrimitiveDataType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<PrimitiveDataTypeMod::PrimitiveDataType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<PrimitiveDataTypeMod::PrimitiveDataType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<PrimitiveDataTypeMod::PrimitiveDataType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<PrimitiveDataTypeMod::PrimitiveDataType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<PrimitiveDataTypeMod::PrimitiveDataType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<PrimitiveDataTypeMod::PrimitiveDataType> > >& vvv_e); 

	/**
	 * Returns a PrimitiveDataTypeMod::PrimitiveDataType from a string.
	 * @param xml the string to be converted into a PrimitiveDataTypeMod::PrimitiveDataType
	 * @return a PrimitiveDataTypeMod::PrimitiveDataType.
	 */
	static PrimitiveDataTypeMod::PrimitiveDataType getPrimitiveDataType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<PrimitiveDataTypeMod::PrimitiveDataType> from a string.
	 * @param xml the string to be converted into a vector<PrimitiveDataTypeMod::PrimitiveDataType>
	 * @return a vector<PrimitiveDataTypeMod::PrimitiveDataType>.
	 */
	static std::vector<PrimitiveDataTypeMod::PrimitiveDataType> getPrimitiveDataType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > from a string.
	 * @param xml the string to be converted into a vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> >
	 * @return a vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> >.
	 */
	static std::vector<std::vector<PrimitiveDataTypeMod::PrimitiveDataType> > getPrimitiveDataType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > >
	 * @return a vector<vector<vector<PrimitiveDataTypeMod::PrimitiveDataType> > >.
	 */
	static std::vector<std::vector<std::vector<PrimitiveDataTypeMod::PrimitiveDataType> > > getPrimitiveDataType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SchedulerModeMod::SchedulerMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SchedulerModeMod::SchedulerMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SchedulerModeMod::SchedulerMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SchedulerModeMod::SchedulerMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SchedulerModeMod::SchedulerMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SchedulerModeMod::SchedulerMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SchedulerModeMod::SchedulerMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SchedulerModeMod::SchedulerMode> > >& vvv_e); 

	/**
	 * Returns a SchedulerModeMod::SchedulerMode from a string.
	 * @param xml the string to be converted into a SchedulerModeMod::SchedulerMode
	 * @return a SchedulerModeMod::SchedulerMode.
	 */
	static SchedulerModeMod::SchedulerMode getSchedulerMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SchedulerModeMod::SchedulerMode> from a string.
	 * @param xml the string to be converted into a vector<SchedulerModeMod::SchedulerMode>
	 * @return a vector<SchedulerModeMod::SchedulerMode>.
	 */
	static std::vector<SchedulerModeMod::SchedulerMode> getSchedulerMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SchedulerModeMod::SchedulerMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<SchedulerModeMod::SchedulerMode> >
	 * @return a vector<vector<SchedulerModeMod::SchedulerMode> >.
	 */
	static std::vector<std::vector<SchedulerModeMod::SchedulerMode> > getSchedulerMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SchedulerModeMod::SchedulerMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SchedulerModeMod::SchedulerMode> > >
	 * @return a vector<vector<vector<SchedulerModeMod::SchedulerMode> > >.
	 */
	static std::vector<std::vector<std::vector<SchedulerModeMod::SchedulerMode> > > getSchedulerMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  FieldCodeMod::FieldCode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, FieldCodeMod::FieldCode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<FieldCodeMod::FieldCode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<FieldCodeMod::FieldCode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<FieldCodeMod::FieldCode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<FieldCodeMod::FieldCode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<FieldCodeMod::FieldCode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<FieldCodeMod::FieldCode> > >& vvv_e); 

	/**
	 * Returns a FieldCodeMod::FieldCode from a string.
	 * @param xml the string to be converted into a FieldCodeMod::FieldCode
	 * @return a FieldCodeMod::FieldCode.
	 */
	static FieldCodeMod::FieldCode getFieldCode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<FieldCodeMod::FieldCode> from a string.
	 * @param xml the string to be converted into a vector<FieldCodeMod::FieldCode>
	 * @return a vector<FieldCodeMod::FieldCode>.
	 */
	static std::vector<FieldCodeMod::FieldCode> getFieldCode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<FieldCodeMod::FieldCode> > from a string.
	 * @param xml the string to be converted into a vector<vector<FieldCodeMod::FieldCode> >
	 * @return a vector<vector<FieldCodeMod::FieldCode> >.
	 */
	static std::vector<std::vector<FieldCodeMod::FieldCode> > getFieldCode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<FieldCodeMod::FieldCode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<FieldCodeMod::FieldCode> > >
	 * @return a vector<vector<vector<FieldCodeMod::FieldCode> > >.
	 */
	static std::vector<std::vector<std::vector<FieldCodeMod::FieldCode> > > getFieldCode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  ACAPolarizationMod::ACAPolarization value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, ACAPolarizationMod::ACAPolarization e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<ACAPolarizationMod::ACAPolarization>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<ACAPolarizationMod::ACAPolarization>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<ACAPolarizationMod::ACAPolarization> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<ACAPolarizationMod::ACAPolarization> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<ACAPolarizationMod::ACAPolarization> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<ACAPolarizationMod::ACAPolarization> > >& vvv_e); 

	/**
	 * Returns a ACAPolarizationMod::ACAPolarization from a string.
	 * @param xml the string to be converted into a ACAPolarizationMod::ACAPolarization
	 * @return a ACAPolarizationMod::ACAPolarization.
	 */
	static ACAPolarizationMod::ACAPolarization getACAPolarization(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<ACAPolarizationMod::ACAPolarization> from a string.
	 * @param xml the string to be converted into a vector<ACAPolarizationMod::ACAPolarization>
	 * @return a vector<ACAPolarizationMod::ACAPolarization>.
	 */
	static std::vector<ACAPolarizationMod::ACAPolarization> getACAPolarization1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<ACAPolarizationMod::ACAPolarization> > from a string.
	 * @param xml the string to be converted into a vector<vector<ACAPolarizationMod::ACAPolarization> >
	 * @return a vector<vector<ACAPolarizationMod::ACAPolarization> >.
	 */
	static std::vector<std::vector<ACAPolarizationMod::ACAPolarization> > getACAPolarization2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<ACAPolarizationMod::ACAPolarization> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<ACAPolarizationMod::ACAPolarization> > >
	 * @return a vector<vector<vector<ACAPolarizationMod::ACAPolarization> > >.
	 */
	static std::vector<std::vector<std::vector<ACAPolarizationMod::ACAPolarization> > > getACAPolarization3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  PositionReferenceCodeMod::PositionReferenceCode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, PositionReferenceCodeMod::PositionReferenceCode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<PositionReferenceCodeMod::PositionReferenceCode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<PositionReferenceCodeMod::PositionReferenceCode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<PositionReferenceCodeMod::PositionReferenceCode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<PositionReferenceCodeMod::PositionReferenceCode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<PositionReferenceCodeMod::PositionReferenceCode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<PositionReferenceCodeMod::PositionReferenceCode> > >& vvv_e); 

	/**
	 * Returns a PositionReferenceCodeMod::PositionReferenceCode from a string.
	 * @param xml the string to be converted into a PositionReferenceCodeMod::PositionReferenceCode
	 * @return a PositionReferenceCodeMod::PositionReferenceCode.
	 */
	static PositionReferenceCodeMod::PositionReferenceCode getPositionReferenceCode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<PositionReferenceCodeMod::PositionReferenceCode> from a string.
	 * @param xml the string to be converted into a vector<PositionReferenceCodeMod::PositionReferenceCode>
	 * @return a vector<PositionReferenceCodeMod::PositionReferenceCode>.
	 */
	static std::vector<PositionReferenceCodeMod::PositionReferenceCode> getPositionReferenceCode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > from a string.
	 * @param xml the string to be converted into a vector<vector<PositionReferenceCodeMod::PositionReferenceCode> >
	 * @return a vector<vector<PositionReferenceCodeMod::PositionReferenceCode> >.
	 */
	static std::vector<std::vector<PositionReferenceCodeMod::PositionReferenceCode> > getPositionReferenceCode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > >
	 * @return a vector<vector<vector<PositionReferenceCodeMod::PositionReferenceCode> > >.
	 */
	static std::vector<std::vector<std::vector<PositionReferenceCodeMod::PositionReferenceCode> > > getPositionReferenceCode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  BaselineReferenceCodeMod::BaselineReferenceCode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, BaselineReferenceCodeMod::BaselineReferenceCode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<BaselineReferenceCodeMod::BaselineReferenceCode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<BaselineReferenceCodeMod::BaselineReferenceCode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<BaselineReferenceCodeMod::BaselineReferenceCode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<BaselineReferenceCodeMod::BaselineReferenceCode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<BaselineReferenceCodeMod::BaselineReferenceCode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<BaselineReferenceCodeMod::BaselineReferenceCode> > >& vvv_e); 

	/**
	 * Returns a BaselineReferenceCodeMod::BaselineReferenceCode from a string.
	 * @param xml the string to be converted into a BaselineReferenceCodeMod::BaselineReferenceCode
	 * @return a BaselineReferenceCodeMod::BaselineReferenceCode.
	 */
	static BaselineReferenceCodeMod::BaselineReferenceCode getBaselineReferenceCode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<BaselineReferenceCodeMod::BaselineReferenceCode> from a string.
	 * @param xml the string to be converted into a vector<BaselineReferenceCodeMod::BaselineReferenceCode>
	 * @return a vector<BaselineReferenceCodeMod::BaselineReferenceCode>.
	 */
	static std::vector<BaselineReferenceCodeMod::BaselineReferenceCode> getBaselineReferenceCode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > from a string.
	 * @param xml the string to be converted into a vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> >
	 * @return a vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> >.
	 */
	static std::vector<std::vector<BaselineReferenceCodeMod::BaselineReferenceCode> > getBaselineReferenceCode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > >
	 * @return a vector<vector<vector<BaselineReferenceCodeMod::BaselineReferenceCode> > >.
	 */
	static std::vector<std::vector<std::vector<BaselineReferenceCodeMod::BaselineReferenceCode> > > getBaselineReferenceCode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  CorrelatorTypeMod::CorrelatorType value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, CorrelatorTypeMod::CorrelatorType e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<CorrelatorTypeMod::CorrelatorType>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<CorrelatorTypeMod::CorrelatorType>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<CorrelatorTypeMod::CorrelatorType> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<CorrelatorTypeMod::CorrelatorType> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<CorrelatorTypeMod::CorrelatorType> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<CorrelatorTypeMod::CorrelatorType> > >& vvv_e); 

	/**
	 * Returns a CorrelatorTypeMod::CorrelatorType from a string.
	 * @param xml the string to be converted into a CorrelatorTypeMod::CorrelatorType
	 * @return a CorrelatorTypeMod::CorrelatorType.
	 */
	static CorrelatorTypeMod::CorrelatorType getCorrelatorType(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<CorrelatorTypeMod::CorrelatorType> from a string.
	 * @param xml the string to be converted into a vector<CorrelatorTypeMod::CorrelatorType>
	 * @return a vector<CorrelatorTypeMod::CorrelatorType>.
	 */
	static std::vector<CorrelatorTypeMod::CorrelatorType> getCorrelatorType1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<CorrelatorTypeMod::CorrelatorType> > from a string.
	 * @param xml the string to be converted into a vector<vector<CorrelatorTypeMod::CorrelatorType> >
	 * @return a vector<vector<CorrelatorTypeMod::CorrelatorType> >.
	 */
	static std::vector<std::vector<CorrelatorTypeMod::CorrelatorType> > getCorrelatorType2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<CorrelatorTypeMod::CorrelatorType> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<CorrelatorTypeMod::CorrelatorType> > >
	 * @return a vector<vector<vector<CorrelatorTypeMod::CorrelatorType> > >.
	 */
	static std::vector<std::vector<std::vector<CorrelatorTypeMod::CorrelatorType> > > getCorrelatorType3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  DopplerTrackingModeMod::DopplerTrackingMode value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, DopplerTrackingModeMod::DopplerTrackingMode e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<DopplerTrackingModeMod::DopplerTrackingMode>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<DopplerTrackingModeMod::DopplerTrackingMode>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<DopplerTrackingModeMod::DopplerTrackingMode> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<DopplerTrackingModeMod::DopplerTrackingMode> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<DopplerTrackingModeMod::DopplerTrackingMode> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<DopplerTrackingModeMod::DopplerTrackingMode> > >& vvv_e); 

	/**
	 * Returns a DopplerTrackingModeMod::DopplerTrackingMode from a string.
	 * @param xml the string to be converted into a DopplerTrackingModeMod::DopplerTrackingMode
	 * @return a DopplerTrackingModeMod::DopplerTrackingMode.
	 */
	static DopplerTrackingModeMod::DopplerTrackingMode getDopplerTrackingMode(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<DopplerTrackingModeMod::DopplerTrackingMode> from a string.
	 * @param xml the string to be converted into a vector<DopplerTrackingModeMod::DopplerTrackingMode>
	 * @return a vector<DopplerTrackingModeMod::DopplerTrackingMode>.
	 */
	static std::vector<DopplerTrackingModeMod::DopplerTrackingMode> getDopplerTrackingMode1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > from a string.
	 * @param xml the string to be converted into a vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> >
	 * @return a vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> >.
	 */
	static std::vector<std::vector<DopplerTrackingModeMod::DopplerTrackingMode> > getDopplerTrackingMode2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > >
	 * @return a vector<vector<vector<DopplerTrackingModeMod::DopplerTrackingMode> > >.
	 */
	static std::vector<std::vector<std::vector<DopplerTrackingModeMod::DopplerTrackingMode> > > getDopplerTrackingMode3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and a content equal to 
	 * the string associated to 'e'
	 * @param elementName a string.
	 * @param e  SynthProfMod::SynthProf value.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, SynthProfMod::SynthProf e);
	 
	 
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'v_e' followed by sequence of strings associated to each element of 'v_e'
	 * @param elementName a string.
	 * @param v_e  a const reference to a vector<SynthProfMod::SynthProf>.
	 * @return a string.
	 */
	 static std::string toXML(const std::string& elementName, const std::vector<SynthProfMod::SynthProf>& v_e);
	
	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vv_e' followed by sequence of strings associated to each element of 'vv_e'.
	 * @param elementName a string.
	 * @param vv_e  a const reference to a std::vector<std::vector<SynthProfMod::SynthProf> >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<SynthProfMod::SynthProf> >& vv_e); 
	 


	/**
	 * Returns a string which represents a XML element 
	 * with name 'elementName' and of content equal to 
	 * the number of elements of 'vvv_e' followed by sequence of strings associated to each element of 'vvv_e'.
	 * @param elementName a string.
	 * @param vvv_e  a const reference to a std::vector<std::vector<std::vector<SynthProfMod::SynthProf> > >.
	 * @return a string.
	 */	
	 static std::string toXML(const std::string& elementName, const std::vector<std::vector<std::vector<SynthProfMod::SynthProf> > >& vvv_e); 

	/**
	 * Returns a SynthProfMod::SynthProf from a string.
	 * @param xml the string to be converted into a SynthProfMod::SynthProf
	 * @return a SynthProfMod::SynthProf.
	 */
	static SynthProfMod::SynthProf getSynthProf(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<SynthProfMod::SynthProf> from a string.
	 * @param xml the string to be converted into a vector<SynthProfMod::SynthProf>
	 * @return a vector<SynthProfMod::SynthProf>.
	 */
	static std::vector<SynthProfMod::SynthProf> getSynthProf1D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<SynthProfMod::SynthProf> > from a string.
	 * @param xml the string to be converted into a vector<vector<SynthProfMod::SynthProf> >
	 * @return a vector<vector<SynthProfMod::SynthProf> >.
	 */
	static std::vector<std::vector<SynthProfMod::SynthProf> > getSynthProf2D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	
	/**
	 * Returns a vector<vector<vector<SynthProfMod::SynthProf> > > from a string.
	 * @param xml the string to be converted into a vector<vector<vector<SynthProfMod::SynthProf> > >
	 * @return a vector<vector<vector<SynthProfMod::SynthProf> > >.
	 */
	static std::vector<std::vector<std::vector<SynthProfMod::SynthProf> > > getSynthProf3D(const std::string &name, const std::string &tableName, const std::string &xmlDoc);								

	
	};
	
} // namespace asdm.

