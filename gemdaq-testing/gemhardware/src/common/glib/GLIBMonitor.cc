/**
 * class: GLIBMonitor
 * description: Monitor application for GLIB cards
 *              structure borrowed from TCDS core, with nods to HCAL and EMU code
 * author: J. Sturdy
 * date:
 */

#include "gem/hw/glib/HwGLIB.h"

#include "gem/hw/glib/GLIBMonitor.h"
#include "gem/hw/glib/GLIBManager.h"
#include "gem/base/GEMApplication.h"
#include "gem/base/GEMFSMApplication.h"

typedef gem::base::utils::GEMInfoSpaceToolBox::UpdateType GEMUpdateType;

gem::hw::glib::GLIBMonitor::GLIBMonitor(std::shared_ptr<HwGLIB> glib, GLIBManager* glibManager, int const& index) :
  GEMMonitor(glibManager->getApplicationLogger(), static_cast<xdaq::Application*>(glibManager), index),
  p_glib(glib)
{
  // application info space is added in the base class constructor
  // addInfoSpace("Application", glibManager->getApplicationInfoSpace());
  // addInfoSpace("GLIB", p_glib->getHwInfoSpace());

  // if (!p_glib->getHwInfoSpace()->hasItem(monname))
  //   p_glib->getHwInfoSpace()->fireItemAvailable(monname, &monvar);

  updateMonitorables();
}


void gem::hw::glib::GLIBMonitor::setupHwMonitoring()
{
  // create the values to be monitored in the info space
  addMonitorableSet("SYSTEM", "HWMonitoring");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("BOARD_ID", "GLIB_SYSTEM.SYSTEM.BOARD_ID"),
                 GEMUpdateType::NOUPDATE, "id");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("SYSTEM_ID", "GLIB_SYSTEM.SYSTEM.SYSTEM_ID"),
                 GEMUpdateType::NOUPDATE, "id");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("FIRMWARE_ID", "GLIB_SYSTEM.SYSTEM.FIRMWARE.ID"),
                 GEMUpdateType::NOUPDATE, "fwver");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("FIRMWARE_DATE", "GLIB_SYSTEM.SYSTEM.FIRMWARE.DATE"),
                 GEMUpdateType::NOUPDATE, "date");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("IP_ADDRESS", "GLIB_SYSTEM.SYSTEM.IP_INFO"),
                 GEMUpdateType::NOUPDATE, "ip");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("MAC_ADDRESS", "GLIB_SYSTEM.SYSTEM.MAC"),
                 GEMUpdateType::NOUPDATE, "mac");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("SFP1_STATUS", "GLIB_SYSTEM.SYSTEM.STATUS.SFP1.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("SFP2_STATUS", "GLIB_SYSTEM.SYSTEM.STATUS.SFP2.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("SFP3_STATUS", "GLIB_SYSTEM.SYSTEM.STATUS.SFP3.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("SFP4_STATUS", "GLIB_SYSTEM.SYSTEM.STATUS.SFP4.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("FMC1_STATUS", "GLIB_SYSTEM.SYSTEM.STATUS.FMC1_PRESENT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("FMC2_STATUS", "GLIB_SYSTEM.SYSTEM.STATUS.FMC2_PRESENT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("FPGA_RESET", "GLIB_SYSTEM.SYSTEM.STATUS.FPGA_RESET"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("GBE_INT",  "GLIB_SYSTEM.SYSTEM.STATUS.GBE_INT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("V6_CPLD",  "GLIB_SYSTEM.SYSTEM.STATUS.V6_CPLD"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("SYSTEM", "HWMonitoring",
                 std::make_pair("CPLD_LOCK", "GLIB_SYSTEM.SYSTEM.STATUS.CDCE_LOCK"),
                 GEMUpdateType::HW32, "hex");

  /*
  addMonitorableSet("IPBus", "HWMonitoring");
  addMonitorable("IPBus", "HWMonitoring",
                 std::make_pair("OptoHybrid_0", "GLIB.COUNTERS.IPBus"),
                 GEMUpdateType::I2CSTAT, "i2c/hex");
  addMonitorable("IPBus", "HWMonitoring",
                 std::make_pair("OptoHybrid_1", "GLIB.COUNTERS.IPBus"),
                 GEMUpdateType::I2CSTAT, "i2c/hex");
  addMonitorable("IPBus", "HWMonitoring",
                 std::make_pair("TRK_0", "GLIB.COUNTERS.IPBus"),
                 GEMUpdateType::I2CSTAT, "i2c/hex");
  addMonitorable("IPBus", "HWMonitoring",
                 std::make_pair("TRK_1", "GLIB.COUNTERS.IPBus"),
                 GEMUpdateType::I2CSTAT, "i2c/hex");
  addMonitorable("IPBus", "HWMonitoring",
                 std::make_pair("Counters", "GLIB.COUNTERS.IPBus"),
                 GEMUpdateType::I2CSTAT, "i2c/hex");

  addMonitorableSet("GTX_LINKS", "HWMonitoring");
  addMonitorable("GTX_LINKS", "HWMonitoring",
                 std::make_pair("GTX0_TRK_ERR", "GLIB.COUNTERS.GTX0.TRK_ERR"),
                 GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable("GTX_LINKS", "HWMonitoring",
                 std::make_pair("GTX0_TRG_ERR", "GLIB.COUNTERS.GTX0.TRG_ERR"),
                 GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable("GTX_LINKS", "HWMonitoring",
                 std::make_pair("GTX0_DATA_Packets", "GLIB.COUNTERS.GTX0.DATA_Packets"),
                 GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable("GTX_LINKS", "HWMonitoring",
                 std::make_pair("GTX1_TRK_ERR", "GLIB.COUNTERS.GTX1.TRK_ERR"),
                 GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable("GTX_LINKS", "HWMonitoring",
                 std::make_pair("GTX1_TRG_ERR", "GLIB.COUNTERS.GTX1.TRG_ERR"),
                 GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable("GTX_LINKS", "HWMonitoring",
                 std::make_pair("GTX1_DATA_Packets", "GLIB.COUNTERS.GTX1.DATA_Packets"),
                 GEMUpdateType::PROCESS, "raw/rate");
  */
  addMonitorableSet("COUNTERS", "HWMonitoring");
  addMonitorable("COUNTERS", "HWMonitoring",
                 std::make_pair("L1A", "TTC.CMD_COUNTERS.L1A"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("COUNTERS", "HWMonitoring",
                 std::make_pair("CalPulse", "TTC.CMD_COUNTERS.CALPULSE"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("COUNTERS", "HWMonitoring",
                 std::make_pair("Resync", "TTC.CMD_COUNTERS.RESYNC"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("COUNTERS", "HWMonitoring",
                 std::make_pair("BC0", "TTC.CMD_COUNTERS.BC0"),
                 GEMUpdateType::HW32, "hex");

  addMonitorableSet("DAQ", "HWMonitoring");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("CONTROL", "DAQ.CONTROL"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("STATUS", "DAQ.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("NOTINTABLE_ERR", "DAQ.EXT_STATUS.NOTINTABLE_ERR"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("DISPER_ERR", "DAQ.EXT_STATUS.DISPER_ERR"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("EVT_SENT", "DAQ.EXT_STATUS.EVT_SENT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("L1AID", "DAQ.EXT_STATUS.L1AID"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("MAX_DAV_TIMER", "DAQ.EXT_STATUS.MAX_DAV_TIMER"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("LAST_DAV_TIMER", "DAQ.EXT_STATUS.LAST_DAV_TIMER"),
                 GEMUpdateType::HW32, "hex");

  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("RUN_TYPE", "DAQ.EXT_CONTROL.RUN_TYPE"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("RUN_PARAMS", "DAQ.EXT_CONTROL.RUN_PARAMS"),
                 GEMUpdateType::HW32, "hex");

  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH0_STATUS", "DAQ.OH0.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH0_CORRUPT_VFAT_BLK_CNT", "DAQ.OH0.COUNTERS.CORRUPT_VFAT_BLK_CNT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH0_EVN", "DAQ.OH0.COUNTERS.EVN"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH0_EOE_TIMEOUT", "DAQ.OH0.CONTROL.EOE_TIMEOUT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH0_MAX_EOE_TIMER", "DAQ.OH0.COUNTERS.MAX_EOE_TIMER"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH0_LAST_EOE_TIMER", "DAQ.OH0.COUNTERS.LAST_EOE_TIMER"),
                 GEMUpdateType::HW32, "hex");

  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH1_STATUS", "DAQ.OH1.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH1_CORRUPT_VFAT_BLK_CNT", "DAQ.OH1.COUNTERS.CORRUPT_VFAT_BLK_CNT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH1_EVN", "DAQ.OH1.COUNTERS.EVN"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH1_EOE_TIMEOUT", "DAQ.OH1.CONTROL.EOE_TIMEOUT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH1_MAX_EOE_TIMER", "DAQ.OH1.COUNTERS.MAX_EOE_TIMER"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH1_LAST_EOE_TIMER", "DAQ.OH1.COUNTERS.LAST_EOE_TIMER"),
                 GEMUpdateType::HW32, "hex");

  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH2_STATUS", "DAQ.OH2.STATUS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH2_CORRUPT_VFAT_BLK_CNT", "DAQ.OH2.COUNTERS.CORRUPT_VFAT_BLK_CNT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH2_EVN", "DAQ.OH2.COUNTERS.EVN"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH2_EOE_TIMEOUT", "DAQ.OH2.CONTROL.EOE_TIMEOUT"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH2_MAX_EOE_TIMER", "DAQ.OH2.COUNTERS.MAX_EOE_TIMER"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("DAQ", "HWMonitoring",
                 std::make_pair("OH2_LAST_EOE_TIMER", "DAQ.OH2.COUNTERS.LAST_EOE_TIMER"),
                 GEMUpdateType::HW32, "hex");

  addMonitorableSet("TTC", "HWMonitoring");
  //addMonitorable("TTC", "HWMonitoring",
  //               std::make_pair("TTC_CONTROL", "GLIB.TTC.CONTROL"),
  //               GEMUpdateType::HW32, "hex");
  addMonitorable("TTC", "HWMonitoring",
                 std::make_pair("TTC_SPY", "TTC.TTC_SPY_BUFFER"),
                 GEMUpdateType::HW32, "hex");

  addMonitorableSet("Trigger", "HWMonitoring");
  for (int oh = 0; oh < 4; ++oh) {
    std::stringstream ohname;
    ohname << "OH" << oh;
    for (int cluster = 0; cluster < 8; ++cluster) {
      std::stringstream cluname;
      cluname << "CLUSTER_" << cluster;
      addMonitorable("Trigger", "HWMonitoring",
                     std::make_pair(ohname.str()+"_"+cluname.str(), "TRIGGER."+ohname.str()+".DEBUG_LAST_"+cluname.str()),
                     GEMUpdateType::HW32, "hex");
    }
  }
  updateMonitorables();
}

gem::hw::glib::GLIBMonitor::~GLIBMonitor()
{

}

void gem::hw::glib::GLIBMonitor::updateMonitorables()
{
  // define how to update the desired values
  // get SYSTEM monitorables
  // can this be split into two loops, one just to do a list read, the second to fill the InfoSpace with the returned values
  DEBUG("GLIBMonitor: Updating monitorables");
  for (auto monlist = m_monitorableSetsMap.begin(); monlist != m_monitorableSetsMap.end(); ++monlist) {
    DEBUG("GLIBMonitor: Updating monitorables in set " << monlist->first);
    for (auto monitem = monlist->second.begin(); monitem != monlist->second.end(); ++monitem) {
      DEBUG("GLIBMonitor: Updating monitorable " << monitem->first);
      std::stringstream regName;
      regName << p_glib->getDeviceBaseNode() << "." << monitem->second.regname;
      uint32_t address = p_glib->getGEMHwInterface().getNode(regName.str()).getAddress();
      uint32_t mask    = p_glib->getGEMHwInterface().getNode(regName.str()).getMask();
      if (monitem->second.updatetype == GEMUpdateType::HW8) {
        (monitem->second.infoSpace)->setUInt32(monitem->first,p_glib->readReg(address,mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW16) {
        (monitem->second.infoSpace)->setUInt32(monitem->first,p_glib->readReg(address,mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW24) {
        (monitem->second.infoSpace)->setUInt32(monitem->first,p_glib->readReg(address,mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW32) {
        (monitem->second.infoSpace)->setUInt32(monitem->first,p_glib->readReg(address,mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW64) {
        address = p_glib->getGEMHwInterface().getNode(regName.str()+".LOWER").getAddress();
        mask    = p_glib->getGEMHwInterface().getNode(regName.str()+".LOWER").getMask();
        uint32_t lower = p_glib->readReg(address,mask);
        address = p_glib->getGEMHwInterface().getNode(regName.str()+".UPPER").getAddress();
        mask    = p_glib->getGEMHwInterface().getNode(regName.str()+".UPPER").getMask();
        uint32_t upper = p_glib->readReg(address,mask);
        (monitem->second.infoSpace)->setUInt64(monitem->first, (((uint64_t)upper) << 32) + lower);
      } else if (monitem->second.updatetype == GEMUpdateType::I2CSTAT) {
        std::stringstream strobeReg;
        strobeReg << regName.str() << ".Strobe." << monitem->first;
        address = p_glib->getGEMHwInterface().getNode(strobeReg.str()).getAddress();
        mask    = p_glib->getGEMHwInterface().getNode(strobeReg.str()).getMask();
        uint32_t strobe = p_glib->readReg(address,mask);
        std::stringstream ackReg;
        ackReg << regName.str() << ".Ack." << monitem->first;
        address = p_glib->getGEMHwInterface().getNode(ackReg.str()).getAddress();
        mask    = p_glib->getGEMHwInterface().getNode(ackReg.str()).getMask();
        uint32_t ack = p_glib->readReg(address,mask);
        (monitem->second.infoSpace)->setUInt64(monitem->first, (((uint64_t)ack) << 32) + strobe);
      } else if (monitem->second.updatetype == GEMUpdateType::PROCESS) {
        (monitem->second.infoSpace)->setUInt32(monitem->first,p_glib->readReg(address,mask));
      } else if (monitem->second.updatetype == GEMUpdateType::TRACKER) {
        (monitem->second.infoSpace)->setUInt32(monitem->first,p_glib->readReg(address,mask));
      } else if (monitem->second.updatetype == GEMUpdateType::NOUPDATE) {
        continue;
      } else {
        ERROR("GLIBMonitor: Unknown update type encountered");
        continue;
      }
    } // end loop over items in list
  } // end loop over monitorableSets
}

void gem::hw::glib::GLIBMonitor::buildMonitorPage(xgi::Output* out)
{
  DEBUG("GLIBMonitor::buildMonitorPage");
  if (m_infoSpaceMonitorableSetMap.find("HWMonitoring") == m_infoSpaceMonitorableSetMap.end()) {
    WARN("Unable to find item set HWMonitoring in monitor");
    return;
  }

  auto monsets = m_infoSpaceMonitorableSetMap.find("HWMonitoring")->second;

  // loop over the list of monitor sets and grab the monitorables from each one
  // create a div tab for each set, and a table for each set of values
  *out << "<div class=\"xdaq-tab-wrapper\">" << std::endl;
  for (auto monset = monsets.begin(); monset != monsets.end(); ++monset) {
    *out << "<div class=\"xdaq-tab\" title=\""  << *monset << "\" >"  << std::endl;
    *out << "<table class=\"xdaq-table\" id=\"" << *monset << "_table\">" << std::endl
         << cgicc::thead() << std::endl
         << cgicc::tr()    << std::endl //open
         << cgicc::th()    << "Register name"    << cgicc::th() << std::endl
         << cgicc::th()    << "Value"            << cgicc::th() << std::endl
         << cgicc::th()    << "Register address" << cgicc::th() << std::endl
         << cgicc::th()    << "Description"      << cgicc::th() << std::endl
         << cgicc::tr()    << std::endl //close
         << cgicc::thead() << std::endl
         << "<tbody>" << std::endl;

    for (auto monitem = m_monitorableSetsMap.find(*monset)->second.begin();
         monitem != m_monitorableSetsMap.find(*monset)->second.end(); ++monitem) {
      *out << "<tr>"    << std::endl;

      *out << "<td>"    << std::endl
           << monitem->first
           << "</td>"   << std::endl;

      DEBUG("GLIBMonitor::" << monitem->first << " formatted to "
            << (monitem->second.infoSpace)->getFormattedItem(monitem->first,monitem->second.format));
      //this will be repeated for every GLIBMonitor in the GLIBManager..., need a better unique ID
      *out << "<td id=\"" << monitem->second.infoSpace->name() << "-" << monitem->first << "\">" << std::endl
           << (monitem->second.infoSpace)->getFormattedItem(monitem->first,monitem->second.format)
           << "</td>"   << std::endl;

      *out << "<td>"    << std::endl
           << monitem->second.regname
           << "</td>"   << std::endl;

      *out << "<td>"    << std::endl
           << "description"
           << "</td>"   << std::endl;

      *out << "</tr>"   << std::endl;
    }
    *out << "</tbody>"  << std::endl
         << "</table>"  << std::endl
         << "</div>"    << std::endl;
  }
  *out << "</div>"  << std::endl;

}

void gem::hw::glib::GLIBMonitor::reset()
{
  //have to get rid of the timer
  DEBUG("GEMMonitor::reset");
  for (auto infoSpace = m_infoSpaceMap.begin(); infoSpace != m_infoSpaceMap.end(); ++infoSpace) {
    DEBUG("GLIBMonitor::reset removing " << infoSpace->first << " from p_timer");
    try {
      p_timer->remove(infoSpace->first);
    } catch (toolbox::task::exception::Exception& te) {
      ERROR("GLIBMonitor::Caught exception while removing timer task " << infoSpace->first << " " << te.what());
    }
  }
  stopMonitoring();
  DEBUG("GEMMonitor::reset removing timer " << m_timerName << " from timerFactory");
  try {
    toolbox::task::getTimerFactory()->removeTimer(m_timerName);
  } catch (toolbox::task::exception::Exception& te) {
    ERROR("GLIBMonitor::Caught exception while removing timer " << m_timerName << " " << te.what());
  }

  DEBUG("GLIBMonitor::reset - clearing all maps");
  m_infoSpaceMap.clear();
  m_infoSpaceMonitorableSetMap.clear();
  m_monitorableSetInfoSpaceMap.clear();
  m_monitorableSetsMap.clear();
}
