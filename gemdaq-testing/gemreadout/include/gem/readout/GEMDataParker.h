#ifndef GEM_READOUT_GEMDATAPARKER_H
#define GEM_READOUT_GEMDATAPARKER_H

#include <string>
#include <queue>

#include "i2o/i2o.h"
#include "toolbox/Task.h"
#include "toolbox/mem/Pool.h"
#include "toolbox/SyncQueue.h"

#include "xoap/MessageReference.h"
#include "xoap/Method.h"

#include "xdata/String.h"

#include "gem/utils/GEMLogging.h"
#include "gem/utils/Lock.h"
#include "gem/utils/LockGuard.h"

#include "gem/readout/GEMDataAMCformat.h"

namespace gem {
  namespace hw {
    namespace glib {
      class HwGLIB;
    }
  }
  namespace readout {
    struct GEMDataAMCformat;
  }
  namespace readout {

    enum GEMRunType {
      DATA      = 0x0,
      THRESHOLD = 0x1,
      LATENCY   = 0x2,
      SCURVE    = 0x3,
      HVSCAN    = 0x4
    };

    class GEMDataParker
    {
    public:
      static const int I2O_READOUT_NOTIFY;
      static const int I2O_READOUT_CONFIRM;

      static const uint32_t kUPDATE;
      static const uint32_t kUPDATE7;

      GEMDataParker        (gem::hw::glib::HwGLIB& glibDevice,
                            std::string const& outFileName,
                            std::string const& errFileName,
                            std::string const& outputType,
                            std::string const& slotFileName="slot_table.csv",
                            GEMRunType  const& runType=DATA
                            );
      ~GEMDataParker() {};

      uint32_t* dumpData   ( uint8_t const& mask );
      uint32_t* selectData ( uint32_t counter[5]
                           );
      uint32_t* getGLIBData( uint8_t const& link,
                             uint32_t counter[5]
                           );
      uint32_t* GEMEventMaker( uint32_t counter[5]
                             );
      void GEMevSelector   ( const  uint32_t& ES
                           );
      void GEMfillHeaders  ( uint32_t const& BC,
                             uint32_t const& BX,
                             gem::readout::GEMDataAMCformat::GEMData& gem,
                             gem::readout::GEMDataAMCformat::GEBData& geb
                           );
      bool VFATfillData    ( int const& islot,
                             gem::readout::GEMDataAMCformat::GEBData& geb
                           );
      void GEMfillTrailers ( gem::readout::GEMDataAMCformat::GEMData& gem,
                             gem::readout::GEMDataAMCformat::GEBData& geb
                           );
      void writeGEMevent   ( std::string outFile,
                             bool const& OKprint,
                             std::string const& TypeDataFlag,
                             gem::readout::GEMDataAMCformat::GEMData& gem,
                             gem::readout::GEMDataAMCformat::GEBData& geb,
                             gem::readout::GEMDataAMCformat::VFATData& vfat
                           );
      int queueDepth       () {return m_dataque.size();}


      void ScanRoutines(uint8_t latency, uint8_t VT1, uint8_t VT2);

      uint64_t Runtype() {
	uint64_t RunType = 1; // :4
	return (RunType << 24)|(m_latency << 16)|(m_VT1 << 8)|(m_VT2);
      }

      // SOAP interface, updates the header used for calibration runs
      xoap::MessageReference updateScanParameters(xoap::MessageReference message)
        throw (xoap::exception::Exception);


    private:
      // moved from globals...
      uint32_t m_ESexp;
      bool     m_isFirst;
      //uint64_t m_ZSFlag;
      uint32_t m_contvfats;

      void readVFATblock(std::queue<uint32_t>& dataque);

      uint32_t dat10,dat11, dat20,dat21, dat30,dat31, dat40,dat41;
      uint32_t BX;
      uint16_t bcn, evn, chipid, vfatcrc;
      uint16_t b1010, b1100, b1110;
      uint8_t  flags;

      uint8_t m_latency, m_VT1, m_VT2;

      static const int MaxVFATS = 24; // was 32 ???
      static const int MaxERRS  = 4095; // should this also be 24? Or we can accomodate full GLIB FIFO of bad blocks belonging to the same event?

      std::unique_ptr<GEMslotContents> slotInfo;

      log4cplus::Logger m_gemLogger;
      gem::hw::glib::HwGLIB* p_glibDevice;
      std::string m_outFileName;
      std::string m_slotFileName;
      std::string m_errFileName;
      std::string m_outputType;

      // queue safety
      mutable gem::utils::Lock m_queueLock;
      // The main data flow
      std::queue<uint32_t> m_dataque;

      //type of run
      GEMRunType m_runType;

      /*
       * Counter all in one
       *   [0] VFAT's Blocks counter
       *   [1] Events counter
       *   [2] VFATs counter per last event
       *   [3] Good Events counter
       *   [4] Bad Events counter
       */
      uint32_t m_counter[5];

      // VFAT's blocks counter
      uint64_t m_vfat;

      // Events counter
      uint64_t m_event;

      // VFATs counter per event
      int m_sumVFAT;

      int16_t m_scanParam;


    };
  }  // namespace gem::readout
}  // namespace gem

#endif  // GEM_READOUT_GEMDATAPARKER_H
