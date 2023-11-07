
#ifndef TagType_CLASS
#define TagType_CLASS
#include <string>
#include <map>
#include <iostream>

namespace asdm {
/**
** A class to represent the type of Tag , i.e. the ASDM class a Tag refers to.
*/
class TagType {
 public:
 	/**
	* The string representation of a TagType.
	* @return the string representation of of TagType.
	*/
  virtual std::string toString() const;
  
  	/**
	  * Returns a TagType given a string.
	  * @return the TagType associated to the string given in as a parameter. E.g. getTagType("Holography")
	  * returns TagType.Holography. Returns 0 is no Tag is associated to the given string.
	  */
  static const TagType* getTagType(std::string name);
  
 	/**
	  * The TagType for a no typed Tag.
	  */ 
  static const TagType* NoType;
  

	

	
	/**
	  * The TagType for a AlmaRadiometer Tag.
	  */
	static const TagType*  AlmaRadiometer ;
	

	
	/**
	  * The TagType for a Annotation Tag.
	  */
	static const TagType*  Annotation ;
	

	
	/**
	  * The TagType for a Antenna Tag.
	  */
	static const TagType*  Antenna ;
	

	

	

	

	

	

	

	
	/**
	  * The TagType for a CalData Tag.
	  */
	static const TagType*  CalData ;
	

	

	

	

	

	

	

	

	

	

	

	

	

	
	/**
	  * The TagType for a CalReduction Tag.
	  */
	static const TagType*  CalReduction ;
	

	

	

	
	/**
	  * The TagType for a ConfigDescription Tag.
	  */
	static const TagType*  ConfigDescription ;
	

	
	/**
	  * The TagType for a CorrelatorMode Tag.
	  */
	static const TagType*  CorrelatorMode ;
	

	
	/**
	  * The TagType for a DataDescription Tag.
	  */
	static const TagType*  DataDescription ;
	

	

	
	/**
	  * The TagType for a DelayModelFixedParameters Tag.
	  */
	static const TagType*  DelayModelFixedParameters ;
	

	
	/**
	  * The TagType for a DelayModelVariableParameters Tag.
	  */
	static const TagType*  DelayModelVariableParameters ;
	

	
	/**
	  * The TagType for a Doppler Tag.
	  */
	static const TagType*  Doppler ;
	

	

	
	/**
	  * The TagType for a ExecBlock Tag.
	  */
	static const TagType*  ExecBlock ;
	

	
	/**
	  * The TagType for a Feed Tag.
	  */
	static const TagType*  Feed ;
	

	
	/**
	  * The TagType for a Field Tag.
	  */
	static const TagType*  Field ;
	

	
	/**
	  * The TagType for a Flag Tag.
	  */
	static const TagType*  Flag ;
	

	

	

	
	/**
	  * The TagType for a FocusModel Tag.
	  */
	static const TagType*  FocusModel ;
	

	

	

	

	
	/**
	  * The TagType for a Holography Tag.
	  */
	static const TagType*  Holography ;
	

	
	/**
	  * The TagType for a Observation Tag.
	  */
	static const TagType*  Observation ;
	

	

	
	/**
	  * The TagType for a PointingModel Tag.
	  */
	static const TagType*  PointingModel ;
	

	
	/**
	  * The TagType for a Polarization Tag.
	  */
	static const TagType*  Polarization ;
	

	
	/**
	  * The TagType for a Processor Tag.
	  */
	static const TagType*  Processor ;
	

	
	/**
	  * The TagType for a Pulsar Tag.
	  */
	static const TagType*  Pulsar ;
	

	
	/**
	  * The TagType for a Receiver Tag.
	  */
	static const TagType*  Receiver ;
	

	
	/**
	  * The TagType for a SBSummary Tag.
	  */
	static const TagType*  SBSummary ;
	

	
	/**
	  * The TagType for a Scale Tag.
	  */
	static const TagType*  Scale ;
	

	

	

	
	/**
	  * The TagType for a Source Tag.
	  */
	static const TagType*  Source ;
	

	
	/**
	  * The TagType for a SpectralWindow Tag.
	  */
	static const TagType*  SpectralWindow ;
	

	
	/**
	  * The TagType for a SquareLawDetector Tag.
	  */
	static const TagType*  SquareLawDetector ;
	

	
	/**
	  * The TagType for a State Tag.
	  */
	static const TagType*  State ;
	

	
	/**
	  * The TagType for a Station Tag.
	  */
	static const TagType*  Station ;
	

	

	
	/**
	  * The TagType for a SwitchCycle Tag.
	  */
	static const TagType*  SwitchCycle ;
	

	

	

	

	

	

	

 

 private:
  std::string name;
  static std::map<std::string,const TagType*> name2TagType;
  virtual ~TagType() {};
  TagType(std::string name);
  
  class TagTypeMgr {
  public:
  	~TagTypeMgr () {
  	if (TagType::NoType) {delete TagType::NoType; TagType::NoType=0;}

	

	
	if (TagType:: AlmaRadiometer) { delete TagType::AlmaRadiometer; TagType::AlmaRadiometer = 0; }
	

	
	if (TagType:: Annotation) { delete TagType::Annotation; TagType::Annotation = 0; }
	

	
	if (TagType:: Antenna) { delete TagType::Antenna; TagType::Antenna = 0; }
	

	

	

	

	

	

	

	
	if (TagType:: CalData) { delete TagType::CalData; TagType::CalData = 0; }
	

	

	

	

	

	

	

	

	

	

	

	

	

	
	if (TagType:: CalReduction) { delete TagType::CalReduction; TagType::CalReduction = 0; }
	

	

	

	
	if (TagType:: ConfigDescription) { delete TagType::ConfigDescription; TagType::ConfigDescription = 0; }
	

	
	if (TagType:: CorrelatorMode) { delete TagType::CorrelatorMode; TagType::CorrelatorMode = 0; }
	

	
	if (TagType:: DataDescription) { delete TagType::DataDescription; TagType::DataDescription = 0; }
	

	

	
	if (TagType:: DelayModelFixedParameters) { delete TagType::DelayModelFixedParameters; TagType::DelayModelFixedParameters = 0; }
	

	
	if (TagType:: DelayModelVariableParameters) { delete TagType::DelayModelVariableParameters; TagType::DelayModelVariableParameters = 0; }
	

	
	if (TagType:: Doppler) { delete TagType::Doppler; TagType::Doppler = 0; }
	

	

	
	if (TagType:: ExecBlock) { delete TagType::ExecBlock; TagType::ExecBlock = 0; }
	

	
	if (TagType:: Feed) { delete TagType::Feed; TagType::Feed = 0; }
	

	
	if (TagType:: Field) { delete TagType::Field; TagType::Field = 0; }
	

	
	if (TagType:: Flag) { delete TagType::Flag; TagType::Flag = 0; }
	

	

	

	
	if (TagType:: FocusModel) { delete TagType::FocusModel; TagType::FocusModel = 0; }
	

	

	

	

	
	if (TagType:: Holography) { delete TagType::Holography; TagType::Holography = 0; }
	

	
	if (TagType:: Observation) { delete TagType::Observation; TagType::Observation = 0; }
	

	

	
	if (TagType:: PointingModel) { delete TagType::PointingModel; TagType::PointingModel = 0; }
	

	
	if (TagType:: Polarization) { delete TagType::Polarization; TagType::Polarization = 0; }
	

	
	if (TagType:: Processor) { delete TagType::Processor; TagType::Processor = 0; }
	

	
	if (TagType:: Pulsar) { delete TagType::Pulsar; TagType::Pulsar = 0; }
	

	
	if (TagType:: Receiver) { delete TagType::Receiver; TagType::Receiver = 0; }
	

	
	if (TagType:: SBSummary) { delete TagType::SBSummary; TagType::SBSummary = 0; }
	

	
	if (TagType:: Scale) { delete TagType::Scale; TagType::Scale = 0; }
	

	

	

	
	if (TagType:: Source) { delete TagType::Source; TagType::Source = 0; }
	

	
	if (TagType:: SpectralWindow) { delete TagType::SpectralWindow; TagType::SpectralWindow = 0; }
	

	
	if (TagType:: SquareLawDetector) { delete TagType::SquareLawDetector; TagType::SquareLawDetector = 0; }
	

	
	if (TagType:: State) { delete TagType::State; TagType::State = 0; }
	

	
	if (TagType:: Station) { delete TagType::Station; TagType::Station = 0; }
	

	

	
	if (TagType:: SwitchCycle) { delete TagType::SwitchCycle; TagType::SwitchCycle = 0; }
	

	

	

	

	

	

	
	  	
  	};
  	
  	TagTypeMgr() {};
  	
  private :
  	TagTypeMgr(const TagTypeMgr&);
  	TagTypeMgr & operator=(const TagTypeMgr&);
  };
  static TagTypeMgr tagTypeMgr;
};
} // end namespace asdm
#endif /* TagType_CLASS */
