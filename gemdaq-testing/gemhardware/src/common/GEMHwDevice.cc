/*General structure taken blatantly from tcds::utils::HwDeviceTCA as we're using the same card*/

#include "toolbox/net/URN.h"

#include "gem/hw/GEMHwDevice.h"
#include "gem/base/utils/GEMInfoSpaceToolBox.h"

gem::hw::GEMHwDevice::GEMHwDevice(std::string const& deviceName,
                                  std::string const& connectionFile) :
  b_is_connected(false),
  m_gemLogger(log4cplus::Logger::getInstance(deviceName)),
  m_hwLock(toolbox::BSem::FULL, true)
{
  DEBUG("GEMHwDevice(std::string, std::string) ctor");
  setLogLevelTo(uhal::Error());
  p_gemConnectionManager = std::shared_ptr<uhal::ConnectionManager>(new uhal::ConnectionManager("file://${GEM_ADDRESS_TABLE_PATH}/"+connectionFile));
  try {
    p_gemHW = std::shared_ptr<uhal::HwInterface>(new uhal::HwInterface(p_gemConnectionManager->getDevice(deviceName)));
  } catch (uhal::exception::FileNotFound const& err) {
    std::string msg = toolbox::toString("Could not find uhal connection file '%s' ",
                                        connectionFile.c_str());
    ERROR("GEMHwDevice::" << msg);
  } catch (uhal::exception::exception const& err) {
    std::string msgBase = "Could not obtain the uhal device from the connection manager";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  } catch (std::exception const& err) {
    ERROR("GEMHwDevice::Unknown std::exception caught from uhal");
    std::string msgBase = "Could not connect to th e hardware";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  }
  // should have pointer to device by here
  setup(deviceName);
  DEBUG("GEMHwDevice::ctor done");
}
gem::hw::GEMHwDevice::GEMHwDevice(std::string const& deviceName,
                                  std::string const& connectionURI,
                                  std::string const& addressTable) :
  b_is_connected(false),
  m_gemLogger(log4cplus::Logger::getInstance(deviceName)),
  m_hwLock(toolbox::BSem::FULL, true)
{
  DEBUG("GEMHwDevice(std::string, std::string, std::string) ctor");
  setLogLevelTo(uhal::Error());
  try {
    p_gemHW = std::shared_ptr<uhal::HwInterface>(new uhal::HwInterface(uhal::ConnectionManager::getDevice(deviceName,
                                                                                                          connectionURI,
                                                                                                          addressTable)));
  } catch (uhal::exception::FileNotFound const& err) {
    std::string msg = toolbox::toString("Could not find uhal address table file '%s' "
                                        "(or one of its included address table modules).",
                                        addressTable.c_str());
    ERROR("GEMHwDevice::" << msg);
  } catch (uhal::exception::exception const& err) {
    std::string msgBase = "Could not obtain the uhal device from the connection manager";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  } catch (std::exception const& err) {
    ERROR("GEMHwDevice::Unknown std::exception caught from uhal");
    std::string msgBase = "Could not connect to th e hardware";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  }
  // should have pointer to device by here
  setup(deviceName);
  DEBUG("GEMHwDevice::ctor done");
}

gem::hw::GEMHwDevice::GEMHwDevice(std::string const& deviceName,
                                  uhal::HwInterface& uhalDevice) :
  b_is_connected(false),
  m_gemLogger(log4cplus::Logger::getInstance(deviceName)),
  m_hwLock(toolbox::BSem::FULL, true)
{
  DEBUG("GEMHwDevice(std::string, uhal::HwInterface) ctor");
  setLogLevelTo(uhal::Error());
  try {
    p_gemHW = std::shared_ptr<uhal::HwInterface>(new uhal::HwInterface(uhalDevice));
    // maybe get specific node, or pass this in as an argument?
    // p_gemHW = std::shared_ptr<uhal::HwInterface>(new uhal::HwInterface(uhalDevice->getNode("someNode")));
  } catch (uhal::exception::exception const& err) {
    std::string msgBase = "Could not obtain the uhal device from the passed device";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  } catch (std::exception const& err) {
    ERROR("GEMHwDevice::Unknown std::exception caught from uhal");
    std::string msgBase = "Could not connect to th e hardware";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  }
  // should have pointer to device by here
  setup(deviceName);
  DEBUG("GEMHwDevice::ctor done");
}

gem::hw::GEMHwDevice::GEMHwDevice(std::string const& deviceName):
  b_is_connected(false),
  m_gemLogger(log4cplus::Logger::getInstance(deviceName)),
  m_hwLock(toolbox::BSem::FULL, true),
  m_controlHubIPAddress("localhost"),
  m_addressTable("allregsnonfram.xml"),
  m_ipBusProtocol("2.0"),
  m_deviceIPAddress("192.168.0.115"),
  m_controlHubPort(10203),
  m_ipBusPort(50001)
  // monGEMHw_(0)
{
  DEBUG("GEMHwDevice(std::string) ctor");
  setLogLevelTo(uhal::Error());
  
  toolbox::net::URN hwCfgURN("urn:gem:hw:"+deviceName);
  INFO("GEMHwDevice::Getting hwCfgInfoSpace with urn " << hwCfgURN.toString());
  p_hwCfgInfoSpace = xdata::getInfoSpaceFactory()->get(hwCfgURN.toString());
  
  setParametersFromInfoSpace();

  // time for these to come from a configuration setup
  std::string const addressTable      = gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "AddressTable");
  std::string const controlhubAddress = gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "ControlHubAddress");
  std::string const deviceIPAddress   = gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "DeviceIPAddress");
  std::string const ipBusProtocol     = gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "IPBusProtocol");
  uint32_t    const controlhubPort    = gem::base::utils::GEMInfoSpaceToolBox::getUInt32(p_hwCfgInfoSpace, "ControlHubPort");
  uint32_t    const ipBusPort         = gem::base::utils::GEMInfoSpaceToolBox::getUInt32(p_hwCfgInfoSpace, "IPBusPort");
  
  std::stringstream tmpUri;
  if (controlhubAddress.size() > 0) {
    DEBUG("GEMHwDevice::Using control hub at address '" << controlhubAddress
          << ", port number "              << controlhubPort << "'.");
    tmpUri << "chtcp-"<< ipBusProtocol << "://"
           << controlhubAddress << ":" << controlhubPort
           << "?target=" << deviceIPAddress << ":" << ipBusPort;
  } else {
    DEBUG("GEMHwDevice::No control hub address specified -> "
          "continuing with a direct connection.");
    tmpUri << "ipbusudp-" << ipBusProtocol << "://"
           << deviceIPAddress << ":" << ipBusPort;
  }
  std::string const uri = tmpUri.str();
  // std::string const addressTable = getAddressTableFileName();

  INFO("GEMHwDevice::uri, deviceName, address table : " << uri << " " << deviceName << " " << addressTable);
  try {
    p_gemHW = std::shared_ptr<uhal::HwInterface>(new uhal::HwInterface(uhal::ConnectionManager::getDevice(deviceName,
                                                                                                          uri,
                                                                                                          addressTable)));
  } catch (uhal::exception::FileNotFound const& err) {
    std::string msg = toolbox::toString("Could not find uhal address table file '%s' "
                                        "(or one of its included address table modules).",
                                        addressTable.c_str());
    ERROR("GEMHwDevice::" << msg);
  } catch (uhal::exception::exception const& err) {
    std::string msgBase = "Could not obtain the uhal device from the connection manager";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  } catch (std::exception const& err) {
    ERROR("GEMHwDevice::Unknown std::exception caught from uhal");
    std::string msgBase = "Could not connect to th e hardware";
    std::string msg = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
    ERROR("GEMHwDevice::" << msg);
  }
  //should have pointer to device by here
  setup(deviceName);
  DEBUG("GEMHwDevice::ctor done");
}

gem::hw::GEMHwDevice::~GEMHwDevice()
{
  // if (p_gemHW)
  //   releaseDevice();
  // if (p_gemConnectionManager)
  //   delete p_gemConnectionManager;
  // p_gemConnectionManager = 0;
}

std::string gem::hw::GEMHwDevice::printErrorCounts() const {
  std::stringstream errstream;
  errstream << "errors while accessing registers:"               << std::endl 
            << "Bad header:  "       <<m_ipBusErrs.BadHeader     << std::endl
            << "Read errors: "       <<m_ipBusErrs.ReadError     << std::endl
            << "Timeouts:    "       <<m_ipBusErrs.Timeout       << std::endl
            << "Controlhub errors: " <<m_ipBusErrs.ControlHubErr << std::endl;
  DEBUG(errstream);
  return errstream.str();
}

void gem::hw::GEMHwDevice::setParametersFromInfoSpace()
{
  DEBUG("GEMHwDevice::setParametersFromInfoSpace");
  try {
    DEBUG("GEMHwDevice::trying to get parameters from the hwCfgInfoSpace");
    setControlHubIPAddress( gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "ControlHubIPAddress"));
    setIPBusProtocolVersion(gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "IPBusProtocol"));
    setDeviceIPAddress(     gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "DeviceIPAddress"));
    setAddressTableFileName(gem::base::utils::GEMInfoSpaceToolBox::getString(p_hwCfgInfoSpace, "AddressTable"));
    
    setControlHubPort(gem::base::utils::GEMInfoSpaceToolBox::getUInt32(p_hwCfgInfoSpace, "ControlHubPort"));
    setIPBusPort(     gem::base::utils::GEMInfoSpaceToolBox::getUInt32(p_hwCfgInfoSpace, "IPBusPort"));
    return;
  } catch (gem::base::utils::exception::InfoSpaceProblem const& err) {
    ERROR("GEMHwDevice::Could not set the device parameters from the InfoSpace " <<
          "(gem::utils::exception::InfoSpacePRoblem)::"
          << err.what());
  } catch (std::exception const& err) {
    ERROR("GEMHwDevice::Could not set the device parameters from the InfoSpace " << 
          "(std::exception)"
          << err.what());
  }
  // if we catch an exception, need to execute this, as successful operation will return in the try block
  DEBUG("GEMHwDevice::Setting default values as InfoSpace setting failed");
  setControlHubIPAddress("localhost");
  setAddressTableFileName("glib_address_table.xml");
  setIPBusProtocolVersion("2.0");
  setDeviceIPAddress("192.168.0.115");

  setControlHubPort(10203);
  setIPBusPort(50001);
  return;
}

void gem::hw::GEMHwDevice::setup(std::string const& deviceName)
{
  setDeviceBaseNode("");
  setDeviceID(deviceName);
  
  m_ipBusErrs.BadHeader     = 0;
  m_ipBusErrs.ReadError     = 0;
  m_ipBusErrs.Timeout       = 0;
  m_ipBusErrs.ControlHubErr = 0;
    
  setLogLevelTo(uhal::Error());
}

uhal::HwInterface& gem::hw::GEMHwDevice::getGEMHwInterface() const
{
  if (p_gemHW == NULL) {
    std::string msg = "Trying to access hardware before connecting!";
    ERROR("GEMHwDevice::" << msg);
    XCEPT_RAISE(gem::hw::exception::UninitializedDevice, msg);
  } else {
    uhal::HwInterface& hw = static_cast<uhal::HwInterface&>(*p_gemHW);
    return hw;
  }
  //have to fix the return value for failed access, better to return a pointer?
}

uint32_t gem::hw::GEMHwDevice::readReg(std::string const& name)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();

  unsigned retryCount = 0;
  uint32_t res = 0x0;
  DEBUG("GEMHwDevice::gem::hw::GEMHwDevice::readReg " << name << std::endl);
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      uhal::ValWord<uint32_t> val = hw.getNode(name).read();
      hw.dispatch();
      res = val.value();
      DEBUG("GEMHwDevice::Successfully read register " << name.c_str() << " with value 0x" 
            << std::setfill('0') << std::setw(8) << std::hex << res << std::dec 
            << " retry count is " << retryCount << ". Should move on to next operation");
      return res;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read register '%s' (uHAL)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        if (retryCount > 4)
          DEBUG("GEMHwDevice::Failed to read register " << name <<
                ", retrying. retryCount("<<retryCount<<")"
                << std::endl);
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read register '%s' (std)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
  std::string msg = toolbox::toString("Maximum number of retries reached, unable to read register %s",name.c_str());
  ERROR("GEMHwDevice::" << msg);
  // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
  return res;
}

uint32_t gem::hw::GEMHwDevice::readReg(uint32_t const& address)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();

  unsigned retryCount = 0;
  uint32_t res = 0x0;
  DEBUG("GEMHwDevice::gem::hw::GEMHwDevice::readReg 0x" << std::setfill('0') << std::setw(8)
        << std::hex << address << std::dec << std::endl);
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      uhal::ValWord<uint32_t> val = hw.getClient().read(address);
      hw.dispatch();
      res = val.value();
      DEBUG("GEMHwDevice::Successfully read register 0x" << std::setfill('0') << std::setw(8)
            << std::hex << address << std::dec << " with value 0x" 
            << std::setfill('0') << std::setw(8) << std::hex << res << std::dec 
            << " retry count is " << retryCount << ". Should move on to next operation");
      return res;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read register '0x%08x' (uHAL)", address);
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        if (retryCount > 4)
          DEBUG("GEMHwDevice::Failed to read register 0x" << std::setfill('0') << std::setw(8)
                << std::hex << address << std::dec
                << ", retrying. retryCount("<<retryCount<<")"
                << std::endl);
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read register '0x%08x' (std)", address);
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
  std::string msg = toolbox::toString("Maximum number of retries reached, unable to read register 0x%08x",
                                      address);
  ERROR("GEMHwDevice::" << msg);
  // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
  return res;
}

uint32_t gem::hw::GEMHwDevice::readReg(uint32_t const& address, uint32_t const& mask)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();

  unsigned retryCount = 0;
  uint32_t res = 0x0;
  DEBUG("GEMHwDevice::gem::hw::GEMHwDevice::readReg 0x" << std::setfill('0') << std::setw(8)
        << std::hex << address << std::dec << std::endl);
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      uhal::ValWord<uint32_t> val = hw.getClient().read(address,mask);
      hw.dispatch();
      res = val.value();
      DEBUG("GEMHwDevice::Successfully read register 0x" << std::setfill('0') << std::setw(8)
            << std::hex << address << std::dec << " with mask " 
            << std::hex << mask << std::dec << " with value " 
            << std::setfill('0') << std::setw(8) << std::hex << res << std::dec 
            << " retry count is " << retryCount << ". Should move on to next operation");
      return res;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read register '0x%08x' (uHAL)", address);
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        if (retryCount > 4)
          DEBUG("GEMHwDevice::Failed to read register 0x" << std::setfill('0') << std::setw(8)
                << std::hex << address << std::dec << " with mask "
                << std::hex << address << std::dec
                << ", retrying. retryCount("<<retryCount<<")"
                << std::endl);
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read register '0x%08x' (std)", address);
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
  std::string msg = toolbox::toString("Maximum number of retries reached, unable to read register 0x%08x",
                                      address);
  ERROR("GEMHwDevice::" << msg);
  // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
  return res;
}

uint32_t gem::hw::GEMHwDevice::readMaskedAddress(std::string const& name)
{
  uint32_t address = getGEMHwInterface().getNode(name).getAddress();
  uint32_t mask    = getGEMHwInterface().getNode(name).getMask();
  return readReg(address,mask);
}

void gem::hw::GEMHwDevice::readRegs(register_pair_list &regList)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();

  unsigned retryCount = 0;
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      std::vector<std::pair<std::string,uhal::ValWord<uint32_t> > > vals;
      // vals.reserve(regList.size());
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        vals.push_back(std::make_pair(curReg->first,hw.getNode(curReg->first).read()));
      hw.dispatch();

      // would like to have these local to the loop, how to do...?
      auto curVal = vals.begin();
      auto curReg = regList.begin();
      for ( ; curReg != regList.end(); ++curVal,++curReg) 
        curReg->second = (curVal->second).value();
      return;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = "Could not read from register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '%s'", curReg->first.c_str());
      std::string msg     = toolbox::toString("%s (uHAL): %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = "Could not read from register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '%s'", curReg->first.c_str());
      std::string msg = toolbox::toString("%s (std): %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
}

void gem::hw::GEMHwDevice::readRegs(addressed_register_pair_list &regList)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();

  unsigned retryCount = 0;
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      std::vector<std::pair<uint32_t, uhal::ValWord<uint32_t> > > vals;
      // vals.reserve(regList.size());
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        vals.push_back(std::make_pair(curReg->first,hw.getClient().read(curReg->first)));
      hw.dispatch();

      // would like to have these local to the loop, how to do...?
      auto curVal = vals.begin();
      auto curReg = regList.begin();
      for ( ; curReg != regList.end(); ++curVal,++curReg) 
        curReg->second = (curVal->second).value();
      return;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = "Could not read from register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '0x%08x mask 0x%08x'", curReg->first);
      std::string msg     = toolbox::toString("%s (uHAL): %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = "Could not read from register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '0x%08x mask 0x%08x'", curReg->first);
      std::string msg = toolbox::toString("%s (std): %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
}

void gem::hw::GEMHwDevice::readRegs(masked_register_pair_list &regList)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();

  unsigned retryCount = 0;
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      std::vector<std::pair<std::pair<uint32_t,uint32_t>,uhal::ValWord<uint32_t> > > vals;
      // vals.reserve(regList.size());
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        vals.push_back(std::make_pair(std::make_pair(curReg->first.first,curReg->first.second),
                                      hw.getClient().read(curReg->first.first,curReg->second)));
      hw.dispatch();

      // would like to have these local to the loop, how to do...?
      auto curVal = vals.begin();
      auto curReg = regList.begin();
      for ( ; curReg != regList.end(); ++curVal,++curReg) 
        curReg->second = (curVal->second).value();
      return;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = "Could not read from register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '0x%08x mask 0x%08x'", curReg->first.first, curReg->first.second);
      std::string msg     = toolbox::toString("%s (uHAL): %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = "Could not read from register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '0x%08x mask 0x%08x'", curReg->first.first, curReg->first.second);
      std::string msg = toolbox::toString("%s (std): %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
}

void gem::hw::GEMHwDevice::writeReg(std::string const& name, uint32_t const val)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();
  unsigned retryCount = 0;
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      hw.getNode(name).write(val);
      hw.dispatch();
      return;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = toolbox::toString("Could not write to register '%s' (uHAL)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        if (retryCount > 4)
          DEBUG("GEMHwDevice::Failed to write value 0x" << std::hex<< val << std::dec << " to register " << name <<
                ", retrying. retryCount("<<retryCount<<")"
                << std::endl);
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = toolbox::toString("Could not write to register '%s' (std)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
}

void gem::hw::GEMHwDevice::writeReg(uint32_t const& address, uint32_t const val)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();
  unsigned retryCount = 0;
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      hw.getClient().write(address, val);
      hw.dispatch();
      return;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = toolbox::toString("Could not write to register '0x%08x' (uHAL)", address);
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        if (retryCount > 4)
          DEBUG("GEMHwDevice::Failed to write value 0x" << std::hex<< val << std::dec << " to register 0x"
                << std::setfill('0') << std::setw(8) << std::hex << address << std::dec
                << ", retrying. retryCount("<<retryCount<<")"
                << std::endl);
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = toolbox::toString("Could not write to register '0x%08x' (std)", address);
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
}

void gem::hw::GEMHwDevice::writeRegs(register_pair_list const& regList)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();
  unsigned retryCount = 0;
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        hw.getNode(curReg->first).write(curReg->second);
      hw.dispatch();
      return;
      //break;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = "Could not write to register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '%s'", curReg->first.c_str());
      std::string msg     = toolbox::toString("%s (uHAL): %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = "Could not write to register in list:";
      for (auto curReg = regList.begin(); curReg != regList.end(); ++curReg) 
        msgBase += toolbox::toString(" '%s'", curReg->first.c_str());
      std::string msg = toolbox::toString("%s (std): %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
}

void gem::hw::GEMHwDevice::writeValueToRegs(std::vector<std::string> const& regNames, uint32_t const& regValue)
{
  register_pair_list regsToWrite;
  for (auto curReg = regNames.begin(); curReg != regNames.end(); ++curReg)
    regsToWrite.push_back(std::make_pair(*curReg,regValue));
  writeRegs(regsToWrite);
}

/*
  void gem::hw::GEMHwDevice::zeroReg(std::string const& name)
  {
  writeReg(name,0);
  }
*/

void gem::hw::GEMHwDevice::zeroRegs(std::vector<std::string> const& regNames)
{
  register_pair_list regsToZero;
  for (auto curReg = regNames.begin(); curReg != regNames.end(); ++curReg)
    regsToZero.push_back(std::make_pair(*curReg,0x0));
  writeRegs(regsToZero);
}

std::vector<uint32_t> gem::hw::GEMHwDevice::readBlock(std::string const& name)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();
  size_t numWords       = hw.getNode(name).getSize();
  DEBUG("GEMHwDevice::reading block " << name << " which has size "<<numWords);
  return readBlock(name, numWords);
}

std::vector<uint32_t> gem::hw::GEMHwDevice::readBlock(std::string const& name, size_t const& numWords)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();

  std::vector<uint32_t> res(numWords);

  unsigned retryCount = 0;
  if (numWords < 1) 
    return res;
  
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      uhal::ValVector<uint32_t> values = hw.getNode(name).readBlock(numWords);
      hw.dispatch();
      std::copy(values.begin(), values.end(), res.begin());
      return res;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read block '%s' (uHAL)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        if (retryCount > 4)
          DEBUG("GEMHwDevice::Failed to read block " << name << " with " << numWords << " words" <<
                ", retrying. retryCount("<<retryCount<<")" << std::endl
                << "error was " << errCode
                << std::endl);
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = toolbox::toString("Could not read block '%s' (std)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
  std::string msg = toolbox::toString("Maximum number of retries reached, unable to read block");
  ERROR("GEMHwDevice::" << msg);
  // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
  return res;
}

uint32_t gem::hw::GEMHwDevice::readBlock(std::string const& name, uint32_t* buffer,
                                         size_t const& numWords)
{
  // not yet implemented
  return 0;
}

uint32_t gem::hw::GEMHwDevice::readBlock(std::string const& name, std::vector<toolbox::mem::Reference*>& buffer,
                                         size_t const& numWords)
{
  // not yet implemented
  return 0;
}

void gem::hw::GEMHwDevice::writeBlock(std::string const& name, std::vector<uint32_t> const values)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  if (values.size() < 1) 
    return;
  
  uhal::HwInterface& hw = getGEMHwInterface();
  unsigned retryCount = 0;
  while (retryCount < MAX_IPBUS_RETRIES) {
    try {
      hw.getNode(name).writeBlock(values);
      hw.dispatch();
      return;
    } catch (uhal::exception::exception const& err) {
      std::string msgBase = toolbox::toString("Could not write to block '%s' (uHAL)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      std::string errCode = toolbox::toString("%s",err.what());
      if (knownErrorCode(errCode)) {
        ++retryCount;
        if (retryCount > 4)
          DEBUG("GEMHwDevice::Failed to write block " << name <<
                ", retrying. retryCount("<<retryCount<<")"
                << std::endl);
        updateErrorCounters(errCode);
        continue;
      } else {
        ERROR("GEMHwDevice::" << msg);
        // XCEPT_RAISE(gem::hw::exception::HardwareProblem, toolbox::toString("%s.", msgBase.c_str()));
      }
    } catch (std::exception const& err) {
      std::string msgBase = toolbox::toString("Could not write to block '%s' (std)", name.c_str());
      std::string msg     = toolbox::toString("%s: %s.", msgBase.c_str(), err.what());
      ERROR("GEMHwDevice::" << msg);
      // XCEPT_RAISE(gem::hw::exception::HardwareProblem, msg);
    }
  }
}

std::vector<uint32_t> gem::hw::GEMHwDevice::readFIFO(std::string const& name)
{
  return readBlock(name);
}

std::vector<uint32_t> gem::hw::GEMHwDevice::readFIFO(std::string const& name, size_t const& numWords)
{
  std::vector<uint32_t> result;
  for (size_t word = 0; word < numWords; ++word)
    result.push_back(readReg(name));

  return result;
}

void gem::hw::GEMHwDevice::writeFIFO(std::string const& name, std::vector<uint32_t> const values)
{
  for (auto word = values.begin(); word != values.end(); ++word)
    writeReg(name,*word);

  return;
}

void gem::hw::GEMHwDevice::zeroFIFO(std::string const& name)
{
  return writeReg(name+".FLUSH",0x0);
}

bool gem::hw::GEMHwDevice::knownErrorCode(std::string const& errCode) const {
  return ((errCode.find("amount of data")              != std::string::npos) ||
          (errCode.find("INFO CODE = 0x4L")            != std::string::npos) ||
          (errCode.find("INFO CODE = 0x6L")            != std::string::npos) ||
          (errCode.find("timed out")                   != std::string::npos) ||
          (errCode.find("had response field = 0x04")   != std::string::npos) ||
          (errCode.find("had response field = 0x06")   != std::string::npos) ||
          (errCode.find("ControlHub error code is: 4") != std::string::npos));
}


void gem::hw::GEMHwDevice::updateErrorCounters(std::string const& errCode) {
  if (errCode.find("amount of data")    != std::string::npos)
    ++m_ipBusErrs.BadHeader;
  if (errCode.find("INFO CODE = 0x4L")  != std::string::npos)
    ++m_ipBusErrs.ReadError;
  if ((errCode.find("INFO CODE = 0x6L") != std::string::npos) ||
      (errCode.find("timed out")        != std::string::npos))
    ++m_ipBusErrs.Timeout;
  if (errCode.find("ControlHub error code is: 4") != std::string::npos)
    ++m_ipBusErrs.ControlHubErr;
  if ((errCode.find("had response field = 0x04") != std::string::npos) ||
      (errCode.find("had response field = 0x06") != std::string::npos))
    ++m_ipBusErrs.ControlHubErr;
}

void gem::hw::GEMHwDevice::zeroBlock(std::string const& name)
{
  gem::utils::LockGuard<gem::utils::Lock> guardedLock(m_hwLock);
  uhal::HwInterface& hw = getGEMHwInterface();
  size_t numWords = hw.getNode(name).getSize();
  std::vector<uint32_t> zeros(numWords, 0);
  return writeBlock(name, zeros);
}

void gem::hw::GEMHwDevice::generalReset()
{
  return;
}

void gem::hw::GEMHwDevice::counterReset()
{
  return;
}

void gem::hw::GEMHwDevice::linkReset(uint8_t const& link)
{
  return;
}

