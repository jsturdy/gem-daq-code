#ifndef gem_hw_GEMHwDevice_h
#define gem_hw_GEMHwDevice_h

//#include "xdata/InfoSpace.h"
#include "xdata/InfoSpaceFactory.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger32.h"
#include "toolbox/string.h"

#include <iomanip>

#include "gem/hw/exception/Exception.h"

#include "uhal/uhal.hpp"
#include "uhal/Utilities.hpp"

#include "gem/utils/GEMLogging.h"

/* would like to avoid rewriting this nice functionality,
   but the code isn't in the main xdaq release.
   have copied directly into GEM and figure out any compatibility later
*/
#include "gem/utils/Lock.h"
#include "gem/utils/LockGuard.h"

/* IPBus transactions still have some problems in the firmware
   so it helps to retry a few times in the case of a failure
   that is recognized
*/
#define MAX_IPBUS_RETRIES 25

typedef uhal::exception::exception uhalException;

typedef std::pair<std::string, uint32_t> register_pair;
typedef std::vector<register_pair>       register_pair_list;

typedef std::pair<std::string, uhal::ValWord<uint32_t> > register_value;
typedef std::vector<register_value>                      register_val_list;


namespace uhal {
  class HwInterface;
}

namespace gem {
  namespace hw {
    
    class GEMHwDevice
    {

    public:
      typedef struct OpticalLinkStatus {
        uint32_t Errors          ;
        uint32_t I2CReceived     ; 
        uint32_t I2CSent         ;
        uint32_t RegisterReceived;
        uint32_t RegisterSent    ;

      OpticalLinkStatus() : Errors(0),I2CReceived(0),I2CSent(0),RegisterReceived(0),RegisterSent(0) {};
        void reset()       {Errors=0; I2CReceived=0; I2CSent=0; RegisterReceived=0; RegisterSent=0; return; };
      } OpticalLinkStatus;
	
      typedef struct DeviceErrors {
        int BadHeader    ;
        int ReadError    ;
        int Timeout      ;
        int ControlHubErr;

      DeviceErrors() : BadHeader(0),ReadError(0),Timeout(0),ControlHubErr(0) {};
        void reset() { BadHeader=0; ReadError=0; Timeout=0; ControlHubErr=0; return; };
      } DeviceErrors;
	
      typedef std::pair<uint8_t, OpticalLinkStatus>  linkStatus;
      //typedef std::vector<linkStatus>                linkStatus;

      /** 
       * GEMHwDevice constructor 
       * @param deviceName string to put into the logger
       **/
      GEMHwDevice(std::string const& deviceName);

      GEMHwDevice(std::string const& deviceName,
                  std::string const& connectionFile);

      GEMHwDevice(std::string const& deviceName,
                  std::string const& connectionURI,
                  std::string const& addressTable);

      GEMHwDevice(std::string const& deviceName,
                  uhal::HwInterface& uhalDevice);

      /*
        GEMHwDevice(xdaq::Application* xdaqApp,
        std::string& addressFileName,
        std::string& ipAddress,
        std::string& controlHubAddress,
        );
      */

      virtual ~GEMHwDevice();
      /*
      virtual void connectDevice();
      virtual void releaseDevice();
      //virtual void initDevice();
      virtual void configureDevice()=0;
      virtual void enableDevice();
      //virtual void disableDevice();
      //virtual void startDevice();
      //virtual void stopDevice();
      //virtual void pauseDevice();
      //virtual void resumeDevice();
      //virtual void haltDevice();
      */
      virtual bool isHwConnected() { return p_gemHW != 0; };
	
      /**
       *Generic read/write functions or IPBus devices
       * operation will be the same for the GLIB, MP7, VFAT2/3, 
       * and AMC13 ( we should use the already defined AMC13, rather than write our own,
       * unless there are GEM specific functions we need to implement)
       */

      /** readReg(std::string const& regName)
       * @param regName name of the register to read 
       * @retval returns the 32 bit unsigned value in the register
       */
      uint32_t readReg( std::string const& regName);

      /** readReg(std::string const& regPrefix, std::string const& regName)
       * @param regPrefix prefix in the address table, possibly root nodes
       * @param regName name of the register to read from the address table
       * @retval returns the 32 bit unsigned value
       */
      uint32_t readReg( const std::string &regPrefix,
                        const std::string &regName) {
        return readReg(regPrefix+"."+regName); };

      /** readRegs( register_pair_list &regList)
       * read list of registers in a single transaction (one dispatch call)
       * into the supplied vector regList
       * @param regList list of register name and uint32_t value to store the result
       */
      void     readRegs( register_pair_list &regList);

      /** writeReg(std::string const& regName, uint32_t const val)
       * @param regName name of the register to read 
       * @param val value to write to the register
       */
      void     writeReg( std::string const& regName, uint32_t const val);

      /** writeReg(std::string const& regPrefux, std::string const& regName, uint32_t const val)
       * @param regPrefix prefix in the address table to the register
       * @param regName name of the register to write to 
       * @param val value to write to the register
       */
      void     writeReg( const std::string &regPrefix,
                         const std::string &regName,
                         uint32_t const val) {
        return writeReg(regPrefix+"."+regName, val); };

      /** writeRegs(register_pair_list const& regList)
       * write list of registers in a single transaction (one dispatch call)
       * using the supplied vector regList
       * @param regList std::vector of a pairs of register names and values to write
       */
      void     writeRegs(register_pair_list const& regList);

      /** writeRegs(register_pair_list const& regList)
       * write single value to a list of registers in a single transaction
       * (one dispatch call) using the supplied vector regList
       * @param regList list of registers to write a value to
       * @param regValue uint32_t value to write to the list of registers
       */
      void     writeValueToRegs(std::vector<std::string> const& regList, uint32_t const& regValue);
	
      /** zeroReg(std::string const& regName)
       * write zero to a single register
       * @param regName register to zero
       */
      void     zeroReg(  std::string const& regName) { writeReg(regName,0); };

      /** zeroRegs(std::vector<std::string> const& regNames)
       * write zero to a list of registers in a single transaction (one dispatch call)
       * using the supplied vector regNames
       * @param regNames registers to zero
       */
      void     zeroRegs( std::vector<std::string> const& regNames);

      /** readBlock(std::string const& regName)
       * read from a memory block
       * @param regName fixed size memory block to read from
       */
      std::vector<uint32_t> readBlock( std::string const& regName);
      //size_t readBlock( std::string const& regName, size_t nWords, uint32_t* buffer); /*hcal style */

      /** readBlock(std::string const& regName, size_t const nWords)
       * read from a memory block
       * @param regName memory block to read from
       * @param nWords size of the memory block to read
       * @retval returns a vector of 32 bit unsigned value
       */
      std::vector<uint32_t> readBlock( std::string const& regName,
                                       size_t      const& nWords);

      /** writeBlock(std::string const& regName, std::vector<uint32_t> const values)
       * write to a memory block
       * @param regName memory block to write to
       * @param values list of 32-bit words to write into the memory block
       */
      void writeBlock(std::string           const& regName,
                      std::vector<uint32_t> const values);

      /** zeroBlock( std::string const& regName)
       * write zeros to a block of memory
       * @param regName block or memory to zero
       */
      void zeroBlock( std::string const& regName);


      // These methods provide access to the member variables
      // specifying the uhal address table name and the IPbus protocol
      // version.
      //getters
      const std::string getControlHubIPAddress()  const { return m_controlHubIPAddress;};
      const std::string getDeviceIPAddress()      const { return m_deviceIPAddress;    };
      const std::string getIPBusProtocolVersion() const { return m_ipBusProtocol;      };
      const std::string getAddressTableFileName() const { return m_addressTable;       };

      const std::string getDeviceBaseNode()       const { return m_deviceBaseNode; };
      const std::string getDeviceID()             const { return m_deviceID;       };

      const uint32_t getControlHubPort() const { return m_controlHubPort;};
      const uint32_t getIPBusPort()      const { return m_ipBusPort;     };

      //setters, should maybe be private/protected? defeats the purpose?
      void setControlHubIPAddress(std::string const& ipAddress) {
        m_controlHubIPAddress = ipAddress; };
      void setIPBusProtocolVersion(std::string const& version) {
        m_ipBusProtocol = version; };
      void setDeviceIPAddress(std::string const& deviceIPAddr) {
        m_deviceIPAddress = deviceIPAddr; };
      void setAddressTableFileName(std::string const& name) {
        m_addressTable = "file://${GEM_ADDRESS_TABLE_PATH}/"+name; };

      void setDeviceBaseNode(std::string const& deviceBase) {
        m_deviceBaseNode = deviceBase; };
      void setDeviceID(std::string const& deviceID) {
        m_deviceID = deviceID; };

      void setControlHubPort(uint32_t const& port) {
        m_controlHubPort = port; };
      void setIPBusPort(uint32_t const& port) {
        m_ipBusPort = port; };
	
      uhal::HwInterface& getGEMHwInterface() const;
	
      void updateErrorCounters(std::string const& errCode);
	
      virtual std::string printErrorCounts() const;
	
      std::string uint32ToString(uint32_t const val) const {
        std::stringstream res;
        res <<(char)((val & (0xff000000)) / 16777216);
        res <<(char)((val & (0x00ff0000)) / 65536);
        res <<(char)((val & (0x0000ff00)) / 256);
        res <<(char)((val & (0x000000ff)));
        return res.str(); };

      std::string uint32ToDottedQuad(uint32_t const val) const {
        std::stringstream res;
        res << (uint32_t)((val & (0xff000000)) / 16777216)<< std::dec << ".";
        res << (uint32_t)((val & (0x00ff0000)) / 65536)   << std::dec << ".";
        res << (uint32_t)((val & (0x0000ff00)) / 256)     << std::dec << ".";
        res << (uint32_t)((val & (0x000000ff)))           << std::dec;
        return res.str(); };
	
      std::string uint32ToGroupedHex(uint32_t const val1, uint32_t const val2) const {
        std::stringstream res;
        res << std::setfill('0') << std::setw(2) << std::hex
            <<(uint32_t)((val1 & (0x0000ff00)) / 256)     << std::dec << ":";
        res << std::setfill('0') << std::setw(2) << std::hex
            <<(uint32_t)((val1 & (0x000000ff)))           << std::dec << ":";
        res << std::setfill('0') << std::setw(2) << std::hex
            <<(uint32_t)((val2 & (0xff000000)) / 16777216)<< std::dec << ":";
        res << std::setfill('0') << std::setw(2) << std::hex
            <<(uint32_t)((val2 & (0x00ff0000)) / 65536)   << std::dec << ":";
        res << std::setfill('0') << std::setw(2) << std::hex
            <<(uint32_t)((val2 & (0x0000ff00)) / 256)     << std::dec << ":";
        res << std::setfill('0') << std::setw(2) << std::hex
            <<(uint32_t)((val2 & (0x000000ff)))           << std::dec;
        return res.str(); };
	
      DeviceErrors m_ipBusErrs;
      
      bool b_is_connected;

    protected:
      std::shared_ptr<uhal::ConnectionManager> p_gemConnectionManager;
      std::shared_ptr<uhal::HwInterface> p_gemHW;

      xdata::InfoSpace *p_hwCfgInfoSpace;       /* Infospace for configuration values */

      log4cplus::Logger m_gemLogger;
		
      mutable gem::utils::Lock m_hwLock;
      
      void setParametersFromInfoSpace();
      void setup(std::string const& deviceName);
      
    private:
      std::string m_controlHubIPAddress;
      std::string m_addressTable;
      std::string m_ipBusProtocol;
      std::string m_deviceIPAddress;

      std::string m_deviceBaseNode;
      std::string m_deviceID;

      uint32_t m_controlHubPort;
      uint32_t m_ipBusPort;
      
      //infospace im(ex)portables
      xdata::String xs_controlHubIPAddress;
      xdata::String xs_deviceIPAddress;
      xdata::String xs_ipBusProtocol;
      xdata::String xs_addressTable;

      xdata::UnsignedInteger32 xs_controlHubPort;
      xdata::UnsignedInteger32 xs_ipBusPort;

      bool knownErrorCode(std::string const& errCode) const;
	
      //std::string registerToChar(uint32_t value) const;	

    }; //end class GEMHwDevice

  } //end namespace gem::hw

} //end namespace gem
#endif
