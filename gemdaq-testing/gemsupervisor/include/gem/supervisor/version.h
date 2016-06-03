#ifndef GEM_SUPERVISOR_VERSION_H
#define GEM_SUPERVISOR_VERSION_H

#ifndef DOXYGEN_IGNORE_THIS

#include "config/PackageInfo.h"

namespace gemsupervisor {

#define GEMSUPERVISOR_VERSION_MAJOR 0
#define GEMSUPERVISOR_VERSION_MINOR 2
#define GEMSUPERVISOR_VERSION_PATCH 0
#define GEMSUPERVISOR_PREVIOUS_VERSIONS "0.0.0,0.1.0"

#define GEMSUPERVISOR_VERSION_CODE PACKAGE_VERSION_CODE(GEMSUPERVISOR_VERSION_MAJOR, GEMSUPERVISOR_VERSION_MINOR, GEMSUPERVISOR_VERSION_PATCH)

#ifndef GEMSUPERVISOR_PREVIOUS_VERSIONS
#define GEMSUPERVISOR_FULL_VERSION_LIST PACKAGE_VERSION_STRING(GEMSUPERVISOR_VERSION_MAJOR, GEMSUPERVISOR_VERSION_MINOR, GEMSUPERVISOR_VERSION_PATCH)
#else
#define GEMSUPERVISOR_FULL_VERSION_LIST GEMSUPERVISOR_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(GEMSUPERVISOR_VERSION_MAJOR, GEMSUPERVISOR_VERSION_MINOR, GEMSUPERVISOR_VERSION_PATCH)
#endif
    
  const std::string package     = "gemsupervisor";
  const std::string versions    = GEMSUPERVISOR_FULL_VERSION_LIST;
  const std::string summary     = "GEM Supervisor";
  const std::string description = "";
  const std::string authors     = "GEM Online Systems Group";
  const std::string link        = "";
    
  config::PackageInfo getPackageInfo();
  void checkPackageDependencies() throw (config::PackageInfo::VersionException);
  std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif // DOXYGEN_IGNORE_THIS

#endif // GEM_SUPERVISOR_VERSION_H
