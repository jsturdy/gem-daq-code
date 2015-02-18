/*
 * This class provides interface for the Crate properties.
 */
#ifndef gem_base_utils_gemCrateProperties_h
#define gem_base_utils_gemCrateProperties_h

#include <string>
#include <vector>
#include <map>

#include "gemDeviceProperties.h"
#include "gemGLIBProperties.h"

namespace gem {
    namespace base {
        namespace utils {
            class gemCrateProperties: public gemDeviceProperties {
                friend class gemXMLparser;
                gemCrateProperties();
                ~gemCrateProperties();
                const std::vector<gem::base::utils::gemGLIBProperties*>& getGLIBRefs() {return subDevicesRefs_;}

                private:
                std::vector <gem::base::utils::gemGLIBProperties*> subDevicesRefs_;
            };
        }
    }
}

#endif