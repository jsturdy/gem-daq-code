#ifndef gem_hw_vfat_HwVFAT2_h
#define gem_hw_vfat_HwVFAT2_h

#include "xdaq/Application.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/ActionListener.h"

#include "gem/hw/GEMHwDevice.h"

//#include "gem/hw/vfat/VFAT2Monitor.h"
#include "gem/hw/vfat/VFAT2Settings.h"
#include "gem/hw/vfat/VFAT2SettingsEnums.h"
#include "gem/hw/vfat/VFAT2Enums2Strings.h"
#include "gem/hw/vfat/VFAT2Strings2Enums.h"
//#include "gem/hw/vfat/VFAT2EnumStrings.h"

#include "gem/hw/vfat/exception/Exception.h"

#define N_VFAT2_CHANNELS 128

typedef uhal::exception::exception uhalException;

namespace uhal {
  class HwInterface;
}

namespace xdaq {
  class Application;;
}

namespace gem {
  namespace hw {
    namespace vfat {
      //class VFAT2Monitor;
      //class VFAT2ChannelData;
      
      //class VFAT2ChannelSettings;
      //class VFAT2Settings;

      class HwVFAT2: public gem::hw::GEMHwDevice
	{
	public:
	  //XDAQ_INSTANTIATOR();
	
	  HwVFAT2(xdaq::Application * vfat2App,
		  std::string const& vfatDevice="CMS_hybrid_J8");
	  //HwVFAT2(xdaq::Application * vfat2App);
	  //throw (xdaq::exception::Exception);

	  ~HwVFAT2();
	
	  //void connectDevice();
	  //void releaseDevice();
	  //void initDevice();
	  //void enableDevice();
	  virtual void configureDevice();
	  virtual void configureDevice(std::string const& xmlSettings);
	  //virtual void configureDevice(std::string const& dbConnectionString);
	  //void disableDevice();
	  //void pauseDevice();
	  //void startDevice();
	  //void stopDevice();
	  //void resumeDevice();
	  //void haltDevice();

	  //bool isHwVFATConnected();

	  //special implementation of the read/write for VFATs
	  uint32_t readReg( std::string const& regName);
	  uint32_t readReg( std::string const& regPrefix,
			    std::string const& regName) {
	    std::string name = regPrefix+"."+regName;
	    return readReg(name); };
	  //void     readRegs( std::vector<std::pair<std::string, uint32_t> > &regList);

	  uint8_t  readVFATReg( std::string const& regName) {
	    //std::cout << "readVFATReg(" << regName << ")" << std::endl;
	    return readReg(regName)&0x000000ff; };
	  void     readVFATRegs( std::vector<std::pair<std::string, uint8_t> > &regList) {
	    std::vector<std::pair<std::string,uint32_t> > fullRegList;
	    std::vector<std::pair<std::string,uint8_t> >::const_iterator curReg = regList.begin();
	    for (; curReg != regList.end(); ++curReg) 
	      fullRegList.push_back(std::make_pair(getDeviceBaseNode()+"."+curReg->first,static_cast<uint32_t>(curReg->second)));
	    readRegs(fullRegList);
	  };
	  //void     readVFAT2Counters(gem::hw::vfat::VFAT2ControlParams &params);
	  void     readVFAT2Counters();

	  void     writeReg(std::string const& regName,
			    uint32_t const writeVal) {
	    std::string name = getDeviceBaseNode()+"."+regName;
	    gem::hw::GEMHwDevice::writeReg(name,writeVal); };

	  void     writeVFATReg(std::string const& regName,
				uint8_t const writeVal) {
	    writeReg(regName, static_cast<uint32_t>(writeVal)); };

	  void     writeReg(std::string const& regPrefix,
			    std::string const& regName,
			    uint32_t const writeVal) {
	    std::string name = regPrefix+"."+regName;
	    gem::hw::GEMHwDevice::writeReg(name,writeVal); };

	  void     writeVFATRegs(std::vector<std::pair<std::string, uint8_t> > const& regList) {
	    std::vector<std::pair<std::string,uint32_t> > fullRegList;
	    std::vector<std::pair<std::string,uint8_t> >::const_iterator curReg = regList.begin();
	    for (; curReg != regList.end(); ++curReg) 
	      fullRegList.push_back(std::make_pair(getDeviceBaseNode()+"."+curReg->first,static_cast<uint32_t>(curReg->second)));
	    writeRegs(fullRegList);
	  };
	  //write single value to a list of registers in a single transaction (one dispatch call) using the supplied vector regList
	  void     writeValueToVFATRegs(std::vector<std::string> const& regList, uint8_t const& regValue) {
	    std::vector<std::string > fullRegList;
	    std::vector<std::string>::const_iterator curReg = regList.begin();
	    for (; curReg != regList.end(); ++curReg) 
	      fullRegList.push_back(getDeviceBaseNode()+"."+*curReg);
	    writeValueToRegs(regList,static_cast<uint32_t>(regValue)); };
	  
	  //control functions
	  //void reset();
	  
	  //get read only registers
	  uint16_t getChipID() {
	    return ((readVFATReg("ChipID1"))<<8)|(readVFATReg("ChipID0")); };
	  uint32_t getHitCount() {
	    return (((readVFATReg("HitCount2"))<<16)|((readVFATReg("HitCount1")))<<8)|(readVFATReg("HitCount0")); };
	  uint8_t  getUpsetCount() { return readReg("UpsetReg");    };
	  
	  //Set control register settings
	  void setAllSettings(const gem::hw::vfat::VFAT2ControlParams &params);
	  
	  //Control register settings
	  // may not be currently implemented correctly...
	  /// have to figure out how to use the enum properly
	  /// also might be good to overload them to act on local variables
	  /// and do a single IPBus transaction...
	  
	  void setRunMode(VFAT2RunMode mode) {
	    uint8_t settings = readVFATReg("ContReg0");
	    writeReg("ContReg0",
		     (settings&~VFAT2ContRegBitMasks::RUNMODE)|
		     (mode<<VFAT2ContRegBitShifts::RUNMODE)); };
	  
	  void setRunMode(VFAT2RunMode mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::RUNMODE)|
	      (mode<<VFAT2ContRegBitShifts::RUNMODE); };
	  
	  void setRunMode(uint8_t mode) {
	    setRunMode(static_cast<VFAT2RunMode>(mode)); };
	  void setRunMode(uint8_t mode, uint8_t&  settings) {
	    setRunMode(static_cast<VFAT2RunMode>(mode), settings); };

	  void setTriggerMode(VFAT2TrigMode mode) {
	    uint8_t settings = readVFATReg("ContReg0");
	    writeReg("ContReg0",
		     (settings&~VFAT2ContRegBitMasks::TRIGMODE)|
		     (mode<<VFAT2ContRegBitShifts::TRIGMODE)); };

	  void setTriggerMode(VFAT2TrigMode mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::TRIGMODE)|
	      (mode<<VFAT2ContRegBitShifts::TRIGMODE); };

	  void setTriggerMode(uint8_t mode) {
	    setTriggerMode(static_cast<VFAT2TrigMode>(mode)); };
	  void setTriggerMode(uint8_t mode, uint8_t& settings) {
	    setTriggerMode(static_cast<VFAT2TrigMode>(mode), settings); };

	  void setCalibrationMode(VFAT2CalibMode mode) {
	    uint8_t settings = readVFATReg("ContReg0");
	    writeReg("ContReg0",
		     (settings&~VFAT2ContRegBitMasks::CALMODE)|
		     (mode<<VFAT2ContRegBitShifts::CALMODE)); };

	  void setCalibrationMode(VFAT2CalibMode mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::CALMODE)|
	      (mode<<VFAT2ContRegBitShifts::CALMODE);
	    /*std::cout << "settings were 0x"       << std::hex
		      << (unsigned)settings << std::dec << std::endl;
	    std::cout << "mask 0x" << std::hex
		      << (unsigned)(VFAT2ContRegBitMasks::CALMODE) << std::dec << std::endl;
	    std::cout << "antimask 0x" << std::hex
		      << (unsigned)~(VFAT2ContRegBitMasks::CALMODE) << std::dec << std::endl;
	    std::cout << "mode 0x"            << std::hex
		      << (unsigned)mode << std::dec << std::endl;
	    std::cout << "mode bitshifted 0x" << std::hex
		      << (unsigned)(mode<<VFAT2ContRegBitShifts::CALMODE) << std::dec << std::endl;
	    std::cout << "settings are 0x" << std::hex << (unsigned)settings << std::dec << std::endl;
	    */
	    };
	  
	  void setCalibrationMode(uint8_t mode) {
	    setCalibrationMode(static_cast<VFAT2CalibMode>(mode)); };
	  void setCalibrationMode(uint8_t mode, uint8_t& settings) {
	    setCalibrationMode(static_cast<VFAT2CalibMode>(mode), settings); };

	  void setMSPolarity(VFAT2MSPol polarity) {
	    uint8_t settings = readVFATReg("ContReg0");
	    writeReg("ContReg0",
		     (settings&~VFAT2ContRegBitMasks::MSPOL)|
		     (polarity<<VFAT2ContRegBitShifts::MSPOL)); };
	  
	  void setMSPolarity(VFAT2MSPol polarity, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::MSPOL)|
	      (polarity<<VFAT2ContRegBitShifts::MSPOL); };
	  
	  void setMSPolarity(uint8_t mode) {
	    setMSPolarity(static_cast<VFAT2MSPol>(mode)); };
	  void setMSPolarity(uint8_t mode, uint8_t& settings) {
	    setMSPolarity(static_cast<VFAT2MSPol>(mode), settings); };

	  void setCalPolarity(VFAT2CalPol polarity) {
	    uint8_t settings = readVFATReg("ContReg0");
	    writeReg("ContReg0",
		     (settings&~VFAT2ContRegBitMasks::CALPOL)|
		     (polarity<<VFAT2ContRegBitShifts::CALPOL)); };
	  
	  void setCalPolarity(VFAT2CalPol polarity, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::CALPOL)|
	      (polarity<<VFAT2ContRegBitShifts::CALPOL); };

	  void setCalPolarity(uint8_t mode) {
	    setCalPolarity(static_cast<VFAT2CalPol>(mode)); };
	  void setCalPolarity(uint8_t mode, uint8_t& settings) {
	    setCalPolarity(static_cast<VFAT2CalPol>(mode), settings); };

	  void setProbeMode(VFAT2ProbeMode mode) {
	    uint8_t settings = readVFATReg("ContReg1");
	    writeReg("ContReg1",
		     (settings&~VFAT2ContRegBitMasks::PROBEMODE)|
		     (mode<<VFAT2ContRegBitShifts::PROBEMODE)); };

	  void setProbeMode(VFAT2ProbeMode mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::PROBEMODE)|
	      (mode<<VFAT2ContRegBitShifts::PROBEMODE); };

	  void setProbeMode(uint8_t mode) {
	    setProbeMode(static_cast<VFAT2ProbeMode>(mode)); };
	  void setProbeMode(uint8_t mode, uint8_t& settings) {
	    setProbeMode(static_cast<VFAT2ProbeMode>(mode), settings); };

	  void setLVDSMode(VFAT2LVDSMode mode) {
	    uint8_t settings = readVFATReg("ContReg1");
	    writeReg("ContReg1",
		     (settings&~VFAT2ContRegBitMasks::LVDSMODE)|
		     (mode<<VFAT2ContRegBitShifts::LVDSMODE)); };

	  void setLVDSMode(VFAT2LVDSMode mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::LVDSMODE)|
	      (mode<<VFAT2ContRegBitShifts::LVDSMODE); };

	  void setLVDSMode(uint8_t mode) {
	    setLVDSMode(static_cast<VFAT2LVDSMode>(mode)); };
	  void setLVDSMode(uint8_t mode, uint8_t& settings) {
	    setLVDSMode(static_cast<VFAT2LVDSMode>(mode), settings); };

	  void setDACMode(VFAT2DACMode mode) {
	    uint8_t settings = readVFATReg("ContReg1");
	    writeReg("ContReg1",
		     (settings&~VFAT2ContRegBitMasks::DACMODE)|
		     (mode<<VFAT2ContRegBitShifts::DACMODE)); };

	  void setDACMode(VFAT2DACMode mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::DACMODE)|
	      (mode<<VFAT2ContRegBitShifts::DACMODE); };

	  void setDACMode(uint8_t mode) {
	    setDACMode(static_cast<VFAT2DACMode>(mode)); };
	  void setDACMode(uint8_t mode, uint8_t& settings) {
	    setDACMode(static_cast<VFAT2DACMode>(mode), settings); };

	  void setHitCountCycleTime(VFAT2ReHitCT cycleTime) {
	    uint8_t settings = readVFATReg("ContReg1");
	    writeReg("ContReg1",
		     (settings&~VFAT2ContRegBitMasks::REHITCT)|
		     (cycleTime<<VFAT2ContRegBitShifts::REHITCT)); };

	  void setHitCountCycleTime(VFAT2ReHitCT cycleTime, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::REHITCT)|
	      (cycleTime<<VFAT2ContRegBitShifts::REHITCT); };

	  void setHitCountCycleTime(uint8_t mode) {
	    setHitCountCycleTime(static_cast<VFAT2ReHitCT>(mode)); };
	  void setHitCountCycleTime(uint8_t mode, uint8_t& settings) {
	    setHitCountCycleTime(static_cast<VFAT2ReHitCT>(mode), settings); };

	  void setHitCountMode(VFAT2HitCountMode mode) {
	    uint8_t settings = readVFATReg("ContReg2");
	    writeReg("ContReg2",
		     (settings&~VFAT2ContRegBitMasks::HITCOUNTMODE)|
		     (mode<<VFAT2ContRegBitShifts::HITCOUNTMODE)); };

	  void setHitCountMode(VFAT2HitCountMode mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::HITCOUNTMODE)|
	      (mode<<VFAT2ContRegBitShifts::HITCOUNTMODE); };

	  void setHitCountMode(uint8_t mode) {
	    setHitCountMode(static_cast<VFAT2HitCountMode>(mode)); };
	  void setHitCountMode(uint8_t mode, uint8_t& settings) {
	    setHitCountMode(static_cast<VFAT2HitCountMode>(mode), settings); };

	  void setMSPulseLength(VFAT2MSPulseLength length) {
	    uint8_t settings = readVFATReg("ContReg2");
	    writeReg("ContReg2",
		     (settings&~VFAT2ContRegBitMasks::MSPULSELENGTH)|
		     (length<<VFAT2ContRegBitShifts::MSPULSELENGTH)); };

	  void setMSPulseLength(VFAT2MSPulseLength length, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::MSPULSELENGTH)|
	      (length<<VFAT2ContRegBitShifts::MSPULSELENGTH); };

	  void setMSPulseLength(uint8_t mode) {
	    setMSPulseLength(static_cast<VFAT2MSPulseLength>(mode)); };
	  void setMSPulseLength(uint8_t mode, uint8_t& settings) {
	    setMSPulseLength(static_cast<VFAT2MSPulseLength>(mode), settings); };

	  void setInputPadMode(VFAT2DigInSel mode) {
	    uint8_t settings = readVFATReg("ContReg2");
	    writeReg("ContReg2",
		     (settings&~VFAT2ContRegBitMasks::DIGINSEL)|
		     (mode<<VFAT2ContRegBitShifts::DIGINSEL)); };

	  void setInputPadMode(VFAT2DigInSel mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::DIGINSEL)|
	      (mode<<VFAT2ContRegBitShifts::DIGINSEL); };

	  void setInputPadMode(uint8_t mode) {
	    setInputPadMode(static_cast<VFAT2DigInSel>(mode)); };
	  void setInputPadMode(uint8_t mode, uint8_t& settings) {
	    setInputPadMode(static_cast<VFAT2DigInSel>(mode), settings); };

	  void setTrimDACRange(VFAT2TrimDACRange range) {
	    uint8_t settings = readVFATReg("ContReg3");
	    writeReg("ContReg3",
		     (settings&~VFAT2ContRegBitMasks::TRIMDACRANGE)|
		     (range<<VFAT2ContRegBitShifts::TRIMDACRANGE)); };

	  void setTrimDACRange(VFAT2TrimDACRange range, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::TRIMDACRANGE)|
	      (range<<VFAT2ContRegBitShifts::TRIMDACRANGE); };

	  void setTrimDACRange(uint8_t mode) {
	    setTrimDACRange(static_cast<VFAT2TrimDACRange>(mode)); };
	  void setTrimDACRange(uint8_t mode, uint8_t& settings) {
	    setTrimDACRange(static_cast<VFAT2TrimDACRange>(mode), settings); };

	  void setBandgapPad(VFAT2PadBandgap mode) {
	    uint8_t settings = readVFATReg("ContReg3");
	    writeReg("ContReg3",
		     (settings&~VFAT2ContRegBitMasks::PADBANDGAP)|
		     (mode<<VFAT2ContRegBitShifts::PADBANDGAP)); };

	  void setBandgapPad(VFAT2PadBandgap mode, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::PADBANDGAP)|
	      (mode<<VFAT2ContRegBitShifts::PADBANDGAP); };

	  void setBandgapPad(uint8_t mode) {
	    setBandgapPad(static_cast<VFAT2PadBandgap>(mode)); };
	  void setBandgapPad(uint8_t mode, uint8_t& settings) {
	    setBandgapPad(static_cast<VFAT2PadBandgap>(mode), settings); };

	  void sendTestPattern(VFAT2DFTestPattern send) {
	    uint8_t settings = readVFATReg("ContReg3");
	    writeReg("ContReg3",
		     (settings&~VFAT2ContRegBitMasks::DFTESTMODE)|
		     (send<<VFAT2ContRegBitShifts::DFTESTMODE)); };

	  void sendTestPattern(VFAT2DFTestPattern send, uint8_t& settings) {
	    settings = (settings&~VFAT2ContRegBitMasks::DFTESTMODE)|
	      (send<<VFAT2ContRegBitShifts::DFTESTMODE); };

	  void sendTestPattern(uint8_t mode) {
	    sendTestPattern(static_cast<VFAT2DFTestPattern>(mode)); };
	  void sendTestPattern(uint8_t mode, uint8_t& settings) {
	    sendTestPattern(static_cast<VFAT2DFTestPattern>(mode), settings); };

	  //////////////////////////////
	  void setLatency(uint8_t latency) {writeReg("Latency",latency); };

	  void setIPreampIn(  uint8_t value) { writeReg("IPreampIn",  value); };
	  void setIPreampFeed(uint8_t value) { writeReg("IPreampFeed",value); };
	  void setIPreampOut( uint8_t value) { writeReg("IPreampOut", value); };
	  void setIShaper(    uint8_t value) { writeReg("IShaper",    value); };
	  void setIShaperFeed(uint8_t value) { writeReg("IShaperFeed",value); };
	  void setIComp(      uint8_t value) { writeReg("IComp",      value); };

	  void setVCal(       uint8_t value  ) { writeReg("VCal",        value); };
	  void setVThreshold1(uint8_t value  ) { writeReg("VThreshold1", value); };
	  void setVThreshold2(uint8_t value=0) { writeReg("VThreshold2", value); };
	  void setCalPhase(   uint8_t value  ) { writeReg("CalPhase",    value); };

	  /*** may want to be able to set these values to a human readable number
	       lookup done through a LUT
	  void setIPreampIn(  float value) { writeReg("IPreampIn",  value); }  ;
	  void setIPreampFeed(float value) { writeReg("IPreampFeed",value); }  ;
	  void setIPreampOut( float value) { writeReg("IPreampOut", value); }  ;
	  void setIShaper(    float value) { writeReg("IShaper",    value); }  ;
	  void setIShaperFeed(float value) { writeReg("IShaperFeed",value); }  ;
	  void setIComp(      float value) { writeReg("IComp",      value); }  ;

	  void setVCal(       float value  ) { writeReg("VCal",        value); }  ;
	  void setVThreshold1(float value  ) { writeReg("VThreshold1", value); }  ;
	  void setVThreshold2(float value=0) { writeReg("VThreshold2", value); }  ;
	  void setCalPhase(   float value  ) { writeReg("CalPhase",    value); }  ;
	  ***/
	  //////////////////////////////

	  //void getAllSettings(gem::hw::vfat::VFAT2ControlParams &params);
	  //void getAllSettings() {
	  //  return getAllSettings(vfatParams_); };
	  void getAllSettings();
	  
	  //Get control register settings
	  //CR0:<7:0::calMode<7:5>,calPol<4>.msPol<3>,trigMode<2:1>,runMode<0>>
	  uint8_t getCR0()     {
	    return (readVFATReg("ContReg0")); };
	  uint8_t getRunMode()     {
	    return (getCR0()&(VFAT2ContRegBitMasks::RUNMODE))>>VFAT2ContRegBitShifts::RUNMODE; };
	  uint8_t getRunMode(uint8_t regVal)     {
	    return (regVal&(VFAT2ContRegBitMasks::RUNMODE))>>VFAT2ContRegBitShifts::RUNMODE; };
	  uint8_t getTriggerMode() {
	    return (getCR0()&(VFAT2ContRegBitMasks::TRIGMODE))>>VFAT2ContRegBitShifts::TRIGMODE; };
	  uint8_t getTriggerMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::TRIGMODE))>>VFAT2ContRegBitShifts::TRIGMODE; };
	  uint8_t getMSPolarity() {
	    return (getCR0()&(VFAT2ContRegBitMasks::MSPOL))>>VFAT2ContRegBitShifts::MSPOL; };
	  uint8_t getMSPolarity(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::MSPOL))>>VFAT2ContRegBitShifts::MSPOL; };
	  uint8_t getCalPolarity() {
	    return (getCR0()&(VFAT2ContRegBitMasks::CALPOL))>>VFAT2ContRegBitShifts::CALPOL; };
	  uint8_t getCalPolarity(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::CALPOL))>>VFAT2ContRegBitShifts::CALPOL; };
	  uint8_t getCalibrationMode() {
	    return (getCR0()&(VFAT2ContRegBitMasks::CALMODE))>>VFAT2ContRegBitShifts::CALMODE; };
	  uint8_t getCalibrationMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::CALMODE))>>VFAT2ContRegBitShifts::CALMODE; };

	  uint8_t getCR1()     {
	    return (readVFATReg("ContReg1")); };
	  //CR1:<7:0::ReHitCT<7:5>,lvdsMode<4>,probeMode<3>,dacMode<2:0> >
	  uint8_t getDACMode() {
	    return (getCR1()&(VFAT2ContRegBitMasks::DACMODE))>>VFAT2ContRegBitShifts::DACMODE; };
	  uint8_t getDACMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::DACMODE))>>VFAT2ContRegBitShifts::DACMODE; };
	  uint8_t getProbeMode() {
	    return (getCR1()&(VFAT2ContRegBitMasks::PROBEMODE))>>VFAT2ContRegBitShifts::PROBEMODE; };
	  uint8_t getProbeMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::PROBEMODE))>>VFAT2ContRegBitShifts::PROBEMODE; };
	  uint8_t getLVDSMode() {
	    return (getCR1()&(VFAT2ContRegBitMasks::LVDSMODE))>>VFAT2ContRegBitShifts::LVDSMODE; };
	  uint8_t getLVDSMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::LVDSMODE))>>VFAT2ContRegBitShifts::LVDSMODE; };
	  uint8_t getHitCountCycleTime() {
	    return (getCR1()&(VFAT2ContRegBitMasks::REHITCT))>>VFAT2ContRegBitShifts::REHITCT; };
	  uint8_t getHitCountCycleTime(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::REHITCT))>>VFAT2ContRegBitShifts::REHITCT; };

	  uint8_t getCR2()     {
	    return (readVFATReg("ContReg2")); };
	  uint8_t getHitCountMode() {
	    return (getCR2()&(VFAT2ContRegBitMasks::HITCOUNTMODE))>>VFAT2ContRegBitShifts::HITCOUNTMODE; };
	  uint8_t getHitCountMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::HITCOUNTMODE))>>VFAT2ContRegBitShifts::HITCOUNTMODE; };
	  uint8_t getMSPulseLength() {
	    return (getCR2()&(VFAT2ContRegBitMasks::MSPULSELENGTH))>>VFAT2ContRegBitShifts::MSPULSELENGTH; };
	  uint8_t getMSPulseLength(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::MSPULSELENGTH))>>VFAT2ContRegBitShifts::MSPULSELENGTH; };
	  uint8_t getInputPadMode() {
	    return (getCR2()&(VFAT2ContRegBitMasks::DIGINSEL))>>VFAT2ContRegBitShifts::DIGINSEL; } ;
	  uint8_t getInputPadMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::DIGINSEL))>>VFAT2ContRegBitShifts::DIGINSEL; } ;

	  uint8_t getCR3()     {
	    return (readVFATReg("ContReg3")); }; //maybe mask by 0x1F to eliminate the unused 3 MSBs?
	  uint8_t getTrimDACRange() {
	    return (getCR3()&(VFAT2ContRegBitMasks::TRIMDACRANGE))>>VFAT2ContRegBitShifts::TRIMDACRANGE; };
	  uint8_t getTrimDACRange(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::TRIMDACRANGE))>>VFAT2ContRegBitShifts::TRIMDACRANGE; };
	  uint8_t getBandgapPad() {
	    return (getCR3()&(VFAT2ContRegBitMasks::PADBANDGAP))>>VFAT2ContRegBitShifts::PADBANDGAP; };
	  uint8_t getBandgapPad(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::PADBANDGAP))>>VFAT2ContRegBitShifts::PADBANDGAP; };
	  uint8_t getTestPatternMode() {
	    return (getCR3()&(VFAT2ContRegBitMasks::DFTESTMODE))>>VFAT2ContRegBitShifts::DFTESTMODE; };
	  uint8_t getTestPatternMode(uint8_t regVal) {
	    return (regVal&(VFAT2ContRegBitMasks::DFTESTMODE))>>VFAT2ContRegBitShifts::DFTESTMODE; };
	  
	  //////////////////////////////
	  uint8_t getLatency()     { return readVFATReg("Latency");     };

	  uint8_t getIPreampIn()   { return readVFATReg("IPreampIn");   };
	  uint8_t getIPreampFeed() { return readVFATReg("IPreampFeed"); };
	  uint8_t getIPreampOut()  { return readVFATReg("IPreampOut");  };
	  uint8_t getIShaper()     { return readVFATReg("IShaper");     };
	  uint8_t getIShaperFeed() { return readVFATReg("IShaperFeed"); };
	  uint8_t getIComp()       { return readVFATReg("IComp");       };

	  uint8_t getVCal()        { return readVFATReg("VCal");        };
	  uint8_t getVThreshold1() { return readVFATReg("VThreshold1"); };
	  uint8_t getVThreshold2() { return readVFATReg("VThreshold2"); };
	  uint8_t getCalPhase()    { return readVFATReg("CalPhase");    };

	  /*** may want to be able to get these values to a human readable number
	       lookup done through a LUT
	  float getIPreampIn()   { return readVFATReg("IPreampIn");   };
	  float getIPreampFeed() { return readVFATReg("IPreampFeed"); };
	  float getIPreampOut()  { return readVFATReg("IPreampOut");  };
	  float getIShaper()     { return readVFATReg("IShaper");     };
	  float getIShaperFeed() { return readVFATReg("IShaperFeed"); };
	  float getIComp()       { return readVFATReg("IComp");       };

	  float getVCal()        { return readVFATReg("VCal");        };
	  float getVThreshold1() { return readVFATReg("VThreshold1"); };
	  float getVThreshold2() { return readVFATReg("VThreshold2"); };
	  float getCalPhase()    { return readVFATReg("CalPhase");    };
	  ***/
	  //////////////////////////////

	  //channel specific settings
	  //void    readVFAT2Channel(gem::hw::vfat::VFAT2ControlParams &params, uint8_t channel);
	  void    readVFAT2Channel(uint8_t channel);
	  //void    readVFAT2Channels(gem::hw::vfat::VFAT2ControlParams &params);
	  void    readVFAT2Channels();
	  void    sendCalPulseToChannel(uint8_t channel, bool on=true);
	  void    maskChannel(uint8_t channel, bool on=true);
	  uint8_t getChannelSettings(uint8_t channel) {
	    return readVFATReg(toolbox::toString("VFATChannels.ChanReg%d",(unsigned)channel)); };
	  uint8_t getChannelTrimDAC(uint8_t channel);
	  void    setChannelTrimDAC(uint8_t channel, uint8_t trimDAC);
	  //void    setChannelTrimDAC(uint8_t channel, double trimDAC);

	  uhal::HwInterface& getVFAT2HwInterface() {
	    return gem::hw::GEMHwDevice::getGEMHwInterface();
	  };

	  gem::hw::vfat::VFAT2ControlParams getVFAT2Params() {
	    return vfatParams_; };
	  
	  void setActiveChannelWeb(uint8_t chan) {
	    vfatParams_.activeChannel = chan; };

	protected:
	  //uhal::ConnectionManager *manageVFATConnection;
	  //log4cplus::Logger logVFAT_;
	  //uhal::HwInterface *hwVFAT_;
	  
	  gem::hw::vfat::VFAT2ControlParams vfatParams_;
	  //uhal::HwInterface& getVFA2Hw();
	  
	  //VFATMonitor *monVFAT_;

	  /*
	  xdata::UnsignedInteger32 vfat2_ctrl0        ;
	  xdata::UnsignedInteger32 vfat2_ctrl1        ;
	  xdata::UnsignedInteger32 vfat2_impreampin   ;
	  xdata::UnsignedInteger32 vfat2_impreampfeed ;
	  xdata::UnsignedInteger32 vfat2_impreampout  ;
	  xdata::UnsignedInteger32 vfat2_ishaper      ;
	  xdata::UnsignedInteger32 vfat2_ishaperfeed  ;
	  xdata::UnsignedInteger32 vfat2_icomp        ;
	  xdata::UnsignedInteger32 vfat2_chipid0      ;
	  xdata::UnsignedInteger32 vfat2_chipid1      ;
	  xdata::UnsignedInteger32 vfat2_upsetreg     ;
	  xdata::UnsignedInteger32 vfat2_hitcounter0  ;
	  xdata::UnsignedInteger32 vfat2_hitcounter1  ;
	  xdata::UnsignedInteger32 vfat2_hitcounter2  ;
	  xdata::UnsignedInteger32 vfat2_extregpointer;
	  xdata::UnsignedInteger32 vfat2_extregdata   ;
	  xdata::UnsignedInteger32 vfat2_lat          ;
	  xdata::UnsignedInteger32 vfat2_vcal         ;
	  xdata::UnsignedInteger32 vfat2_vthreshold1  ;
	  xdata::UnsignedInteger32 vfat2_vthreshold2  ;
	  xdata::UnsignedInteger32 vfat2_calphase     ;
	  xdata::UnsignedInteger32 vfat2_ctrl2        ;
	  xdata::UnsignedInteger32 vfat2_ctrl3        ;
	  xdata::UnsignedInteger32 vfat2_spare        ;

	  xdata::UnsignedInteger32 vfat2_response     ;

	  std::vector<xdata::UnsignedInteger32> vfat2_channels;
	  */
	
	private:
	  /*
	  uhal::ValWord< uint8_t > r_vfat2_ctrl0        ;
	  uhal::ValWord< uint8_t > r_vfat2_ctrl1        ;
	  uhal::ValWord< uint8_t > r_vfat2_impreampin   ;
	  uhal::ValWord< uint8_t > r_vfat2_impreampfeed ;
	  uhal::ValWord< uint8_t > r_vfat2_impreampout  ;
	  uhal::ValWord< uint8_t > r_vfat2_ishaper      ;
	  uhal::ValWord< uint8_t > r_vfat2_ishaperfeed  ;
	  uhal::ValWord< uint8_t > r_vfat2_icomp        ;
	  uhal::ValWord< uint8_t > r_vfat2_chipid0      ;
	  uhal::ValWord< uint8_t > r_vfat2_chipid1      ;
	  uhal::ValWord< uint8_t > r_vfat2_upsetreg     ;
	  uhal::ValWord< uint8_t > r_vfat2_hitcounter0  ;
	  uhal::ValWord< uint8_t > r_vfat2_hitcounter1  ;
	  uhal::ValWord< uint8_t > r_vfat2_hitcounter2  ;
	  uhal::ValWord< uint8_t > r_vfat2_extregpointer;
	  uhal::ValWord< uint8_t > r_vfat2_extregdata   ;
	  uhal::ValWord< uint8_t > r_vfat2_lat          ;
	  uhal::ValWord< uint8_t > r_vfat2_vcal         ;
	  uhal::ValWord< uint8_t > r_vfat2_vthreshold1  ;
	  uhal::ValWord< uint8_t > r_vfat2_vthreshold2  ;
	  uhal::ValWord< uint8_t > r_vfat2_calphase     ;
	  uhal::ValWord< uint8_t > r_vfat2_ctrl2        ;
	  uhal::ValWord< uint8_t > r_vfat2_ctrl3        ;
	  uhal::ValWord< uint8_t > r_vfat2_spare        ;

	  uhal::ValWord< uint32_t > r_vfat2_response        ;
	  
	  std::vector<uhal::ValWord< uint8_t > > r_vfat2_channels;
	  */
	}; //end class HwVFAT2
      
    } //end namespace gem::hw::vfat
    
  } //end namespace gem::hw
  
} //end namespace gem
#endif
