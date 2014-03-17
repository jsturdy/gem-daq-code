# - Finds Vc installation ( the wrapper library to SIMD intrinsics )
# This module sets up Vc information 
# It defines:
# uHAL_FOUND          If the ROOT is found
# uHAL_INCLUDE_DIR    PATH to the include directory
# uHAL_LIBRARIES      Most common libraries
# uHAL_LIBRARY_DIR    PATH to the library directory 

# look if an environment variable uHALROOT exists
set(uHALROOT $ENV{uHALROOT})

find_library(uHAL_LIBRARIES  libcactus_uhal_uhal.so PATHS ${uHALROOT}/lib)
if (uHAL_LIBRARIES)
   set(uHAL_FOUND TRUE)
   #string(REPLACE "/lib/libnt2.a" "" uHALROOT  ${uHAL_LIBRARIES})
   set(uHAL_INCLUDE_DIR ${uHALROOT}/include)
   set(uHAL_LIBRARY_DIR ${uHALROOT}/lib)
   message(STATUS "Found uHAL library in ${uHAL_LIBRARIES}")
else()
   message(STATUS "uHAL library not found; try to set a uHALROOT environment variable to the base installation path or add -DuHALROOT= to the cmake command")
endif()
