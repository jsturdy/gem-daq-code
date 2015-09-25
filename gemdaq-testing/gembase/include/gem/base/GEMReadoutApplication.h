#ifndef gem_base_GEMReadoutApplication_h
#define gem_base_GEMReadoutApplication_h

#include "gem/base/GEMApplication.h"
#include "gem/base/GEMFSMApplication.h"

namespace gem {
  namespace base {
    
    class GEMReadoutApplication : public gem::base::GEMApplication
      {
      public:
        //XDAQ_INSTANTIATOR();
	
        GEMReadoutApplication(xdaq::ApplicationStub *stub)
          throw (xdaq::exception::Exception);
	
      protected:

        //copy from HCAL readout application
        virtual int readout(unsigned int expected, unsigned int* eventNumbers, std::vector< ::toolbox::mem::Reference* >& data) = 0;

        virtual void init();

        log4cplus::Logger gemReadoutLogger_;

        virtual void Default(xgi::Input *in, xgi::Output *out)
          throw (xgi::exception::Exception);
	
        virtual void Expert(xgi::Input *in, xgi::Output *out)
          throw (xgi::exception::Exception);

        virtual void webRedirect(xgi::Input *in, xgi::Output *out)
          throw (xgi::exception::Exception);
      };
  } // namespace gem::base
} // namespace gem

#endif
