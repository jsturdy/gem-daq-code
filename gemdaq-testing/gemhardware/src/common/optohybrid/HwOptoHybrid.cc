#include <iomanip>

#include "gem/hw/optohybrid/HwOptoHybrid.h"

gem::hw::optohybrid::HwOptoHybrid::HwOptoHybrid() :
  gem::hw::GEMHwDevice::GEMHwDevice("HwOptoHybrid"),
  //monOptoHybrid_(0)
  b_links({false,false,false}),
  m_controlLink(-1)  
{
  setDeviceID("OptoHybridHw");
  setAddressTableFileName("optohybrid_address_table.xml");
  setDeviceBaseNode("OptoHybrid");
  //gem::hw::optohybrid::HwOptoHybrid::initDevice();
  //set up which links are active, so that the control can be done without specifying a link
  INFO("HwOptoHybrid ctor done " << isHwConnected());
}

gem::hw::optohybrid::HwOptoHybrid::HwOptoHybrid(std::string const& optohybridDevice,
                                                std::string const& connectionFile) :
  gem::hw::GEMHwDevice::GEMHwDevice(optohybridDevice, connectionFile),
  //monOptoHybrid_(0)
  b_links({false,false,false}),
  m_controlLink(-1)  
{
  setDeviceBaseNode("OptoHybrid");
  INFO("HwOptoHybrid ctor done " << isHwConnected());
}

gem::hw::optohybrid::HwOptoHybrid::HwOptoHybrid(std::string const& optohybridDevice,
                                                std::string const& connectionURI,
                                                std::string const& addressTable) :
  gem::hw::GEMHwDevice::GEMHwDevice(optohybridDevice, connectionURI, addressTable),
  //monOptoHybrid_(0)
  b_links({false,false,false}),
  m_controlLink(-1)  
{
  setDeviceBaseNode("OptoHybrid");
  INFO("HwOptoHybrid ctor done " << isHwConnected());
}

gem::hw::optohybrid::HwOptoHybrid::HwOptoHybrid(std::string const& optohybridDevice,
                                                uhal::HwInterface& uhalDevice) :
  gem::hw::GEMHwDevice::GEMHwDevice(optohybridDevice,uhalDevice),
  //monOptoHybrid_(0)
  b_links({false,false,false}),
  m_controlLink(-1)  
{
  setDeviceBaseNode("OptoHybrid");
  INFO("HwOptoHybrid ctor done " << isHwConnected());
}
/*
gem::hw::optohybrid::HwOptoHybrid::HwOptoHybrid(gem::hw::glib::HwGLIB const& glib,
                                                int const& slot) :
  gem::hw::GEMHwDevice::GEMHwDevice("HwOptoHybrid"),
  //monOptoHybrid_(0),
  b_links({false,false,false}),
  m_controlLink(-1),
  m_slot(slot)
{
  //use a connection file and connection manager?
  setDeviceID(toolbox::toString("%s.optohybrid%02d",glib.getDeviceID().c_str(),slot));
  //uhal::ConnectionManager manager ( "file://${GEM_ADDRESS_TABLE_PATH}/connections_ch.xml" );
  p_gemConnectionManager.reset(new uhal::ConnectionManager("file://${GEM_ADDRESS_TABLE_PATH}/connections_ch.xml"));
  p_gemHW.reset(new uhal::HwInterface(p_gemConnectionManager->getDevice(this->getDeviceID())));
  //p_gemConnectionManager = std::shared_ptr<uhal::ConnectionManager>(uhal::ConnectionManager("file://${GEM_ADDRESS_TABLE_PATH}/connections_ch.xml"));
  //p_gemHW = std::shared_ptr<uhal::HwInterface>(p_gemConnectionManager->getDevice(this->getDeviceID()));
  //setAddressTableFileName("optohybrid_address_table.xml");
  //setDeviceIPAddress(toolbox::toString("192.168.0.%d",160+slot));
  setDeviceBaseNode("OptoHybrid");
  //gem::hw::optohybrid::HwOptoHybrid::initDevice();
}
*/
gem::hw::optohybrid::HwOptoHybrid::~HwOptoHybrid()
{
  //releaseDevice();
}

//void gem::hw::optohybrid::HwOptoHybrid::configureDevice(std::string const& xmlSettings)
//{
//  //here load the xml file settings onto the board
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::configureDevice()
//{
//  //determine the manner in which to configure the device (XML or DB parameters)
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::releaseDevice()
//{
//
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::enableDevice()
//{
//
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::disableDevice()
//{
//
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::pauseDevice()
//{
//
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::startDevice()
//{
//
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::stopDevice()
//{
//
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::resumeDevice()
//{
//
//}
//
//void gem::hw::optohybrid::HwOptoHybrid::haltDevice()
//{
//
//}
//

bool gem::hw::optohybrid::HwOptoHybrid::isHwConnected() 
{
  if ( b_is_connected ) {
    DEBUG("HwOptoHybrid connection good");
    return true;
  } else if (gem::hw::GEMHwDevice::isHwConnected()) {
    DEBUG("Checking hardware connection");

    //try {
    //try to read from each of the three links
    
    std::vector<linkStatus> tmp_activeLinks;
    tmp_activeLinks.reserve(3);
    for (unsigned int link = 0; link < 3; ++link) {
      //need to make sure that this works only for "valid" FW results
      // for the moment we can do a check to see that 2015 appears in the string
      //if (this->getFirmware(link)) {
      if ((this->getFirmwareDate(link)).rfind("15") != std::string::npos) {
        b_links[link] = true;
        INFO("link" << link << " present(0x" << std::hex << this->getFirmware(link) << std::dec << ")");
        tmp_activeLinks.push_back(std::make_pair(link,this->LinkStatus(link)));
      } else {
        b_links[link] = false;
        DEBUG("link" << link << " not reachable (unable to find 15 in the firmware string)");
      }
    }
    v_activeLinks = tmp_activeLinks;
    if (!v_activeLinks.empty()) {
      b_is_connected = true;
      m_controlLink = (v_activeLinks.begin())->first;
      INFO("connected - control link" << (int)m_controlLink);
      INFO("HwOptoHybrid connection good");
      return true;
    } else {
      b_is_connected = false;
      DEBUG("not connected - control link" << (int)m_controlLink);
      return false;
    }
  } else if (m_controlLink < 0)
    return false;
  else
    return false;
}


gem::hw::GEMHwDevice::OpticalLinkStatus gem::hw::optohybrid::HwOptoHybrid::LinkStatus(uint8_t const& link) {
  
  gem::hw::GEMHwDevice::OpticalLinkStatus linkStatus;

  //put these into a checkLink(link) function that will return bool, since they're used often
  if (link > 2) {
    std::string msg = toolbox::toString("Link status requested for link (%d): outside expectation (0-2)",link);
    ERROR(msg);
    //XCEPT_RAISE(gem::hw::optohybrid::exception::InvalidLink,msg);
  } else if (!b_links[link]) {
    std::string msg = toolbox::toString("Link status requested inactive link (%d)",link);
    ERROR(msg);
    //XCEPT_RAISE(gem::hw::optohybrid::exception::InvalidLink,msg);
  } else {
    std::stringstream regName;
    regName << "OptoHybrid_LINKS.LINK" << (int)link << ".OPTICAL_LINKS.Counter.";
    linkStatus.Errors           = readReg(getDeviceBaseNode(),regName.str()+"LinkErr"       );
    linkStatus.I2CReceived      = readReg(getDeviceBaseNode(),regName.str()+"RecI2CRequests");
    linkStatus.I2CSent          = readReg(getDeviceBaseNode(),regName.str()+"SntI2CRequests");
    linkStatus.RegisterReceived = readReg(getDeviceBaseNode(),regName.str()+"RecRegRequests");
    linkStatus.RegisterSent     = readReg(getDeviceBaseNode(),regName.str()+"SntRegRequests");
  }
  return linkStatus;
}

void gem::hw::optohybrid::HwOptoHybrid::LinkReset(uint8_t const& link, uint8_t const& resets) {
  if (link > 2) {
    std::string msg = toolbox::toString("Link status requested for link (%d): outside expectation (0-2)",link);
    ERROR(msg);
    //XCEPT_RAISE(gem::hw::optohybrid::exception::InvalidLink,msg);
    return;
  } else if (!b_links[link]) {
    std::string msg = toolbox::toString("Link status requested inactive link (%d)",link);
    ERROR(msg);
    //XCEPT_RAISE(gem::hw::optohybrid::exception::InvalidLink,msg);
    return;
  }
  
  std::stringstream regName;
  regName << "OptoHybrid_LINKS.LINK" << (int)link << ".OPTICAL_LINKS.Resets";
  if (resets&0x01)
    writeReg(getDeviceBaseNode(),regName.str()+"LinkErr",0x1);
  if (resets&0x02)
    writeReg(getDeviceBaseNode(),regName.str()+"RecI2CRequests",0x1);
  if (resets&0x04)
    writeReg(getDeviceBaseNode(),regName.str()+"SntI2CRequests",0x1);
  if (resets&0x08)
    writeReg(getDeviceBaseNode(),regName.str()+"RecRegRequests",0x1);
  if (resets&0x10)
    writeReg(getDeviceBaseNode(),regName.str()+"SntRegRequests",0x1);
}

//uint32_t gem::hw::optohybrid::HwOptoHybrid::readTriggerData() {
//  return uint32_t value;
//}
