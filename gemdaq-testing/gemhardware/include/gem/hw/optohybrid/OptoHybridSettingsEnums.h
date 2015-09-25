#ifndef gem_hw_optohybrid_OptoHybridSettingsEnums_h
#define gem_hw_optohybrid_OptoHybridSettingsEnums_h

namespace gem {
  namespace hw {
    namespace optohybrid {
      class OptoHybridLinkSettings
      {
      public:
        struct LinkBitMasks {
          enum ELinkBitMasks {
            ERROR  = 0x01,
            RECI2C = 0x02,
            SNTI2C = 0x04,
            RECREG = 0x08,
            SNTREG = 0x10,
          } LinkBitMasks;
        };

        struct LinkBitShifts {
          enum ELinkBitShifts {
            ERROR  = 0,
            RECI2C = 1,
            SNTI2C = 2,
            RECREG = 3,
            SNTREG = 4,
          } LinkBitShifts;
        };
      };

      class OptoHybridSettings {
      public:

        //what are these for the OptoHybrid??
        struct ContRegBitMasks {
          enum EContRegBitMasks {
            //Control Register 0
            RUNMODE       = 0x01,
            TRIGMODE      = 0x0e,
            MSPOL         = 0x10,
            CALPOL        = 0x20,
            CALMODE       = 0xc0,
          } ContRegBitMasks;
        };

        struct ContRegBitShifts {
          enum EContRegBitShifts {
            //Control Register 0
            RUNMODE       = 0,
            TRIGMODE      = 1,
            MSPOL         = 4,
            CALPOL        = 5,
            CALMODE       = 6,
          } ContRegBitShifts;
        };

        struct RunMode { //defines the run mode
          enum ERunMode { //defines the run mode
            SLEEP = 0x0, //sleep (default)
            RUN   = 0x1  //running
          } RunMode;
        };

        struct TriggerMode { //Trigger mode settings
          enum ETriggerMode { //Trigger mode settings
            GLIB     = 0x0, //Take the triggers coming from the GLIB
            EXTERNAL = 0x1, //Use the triggers coming from the LEMO connector
            BOTH     = 0x2, //Use all sources of triggers
          } TriggerMode;
        };
	
        struct L1ACountMode { //L1A mode count settings
          enum EL1ACountMode { //L1A mode count settings
            EXTERNAL = 0x0, //Count the triggers coming from the LEMO connector
            INTERNAL = 0x1, //Count the triggers coming from the GLIB
            DELAYED  = 0x2, //Count the delayed triggers
            TOTAL    = 0x3, //Count triggers from all sources
          } L1ACountMode;
        };
	
        struct CalPulseCountMode { //CalPulse mode settings
          enum ECalPulseCountMode { //CalPulse mode settings
            EXTERNAL = 0x0, //Take the triggers coming from the GLIB
            INTERNAL = 0x1, //Use the triggers coming from the LEMO connector
            DELAYED  = 0x2, //Use all sources of triggers
            TOTAL    = 0x3, //Use all sources of triggers
          } CalPulseCountMode;
        };
	
      }; //end class OptoHybridSettings
      
    }//end namespace gem::hw::optohybrid
    
  }//end namespace gem::hw
  
}//end namespace gem

//typedef the struct for access to the members via struct::VALUE
typedef gem::hw::optohybrid::OptoHybridLinkSettings::LinkBitMasks   OptoHybridLinkBitMasks;
typedef gem::hw::optohybrid::OptoHybridLinkSettings::LinkBitShifts  OptoHybridLinkBitShifts;

//typedef the enum for casting and access
typedef gem::hw::optohybrid::OptoHybridSettings::RunMode::ERunMode                 OptoHybridRunMode;
typedef gem::hw::optohybrid::OptoHybridSettings::TriggerMode::ETriggerMode         OptoHybridTrigMode;

#endif
