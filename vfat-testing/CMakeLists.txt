cmake_minimum_required(VERSION 2.6)

set(OUTDIR ${PROJECT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTDIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTDIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTDIR}/bin)

set(CMAKE_BUILD_TYPE Debug)

include_directories(inc)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/modules)
find_package(uHAL REQUIRED)
include_directories(${uHAL_INCLUDE_DIR})

#Below line may be uncommented later if more libraries are required
#set(LIBS -lpthread -lboost_thread -lboost_filesystem -lboost_regex -lboost_system -lboost_thread -lboost_program_options -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal)

set(LIBS -lcactus_uhal_uhal)

add_library(vfat SHARED inc/vfat.h src/vfat.cpp)
target_link_libraries(vfat "-L ${uHAL_LIBRARY_DIR} ${LIBS}")

message(STATUS "=======================================================")
message(STATUS "Please set your LD_LIBRARY_PATH env variable as" )
message(STATUS "export LD_LIBRARY_PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}:$LD_LIBRARY_PATH")
message(STATUS "=======================================================")

include_directories(${PROJECT_SOURCE_DIR}/inc)
set(CMAKE_GLIB_LIBRARY /home/sturdy/xdaqgem/glib_dev/gem-daq-code/glib-testing/lib)
include_directories(/home/sturdy/xdaqgem/glib_dev/gem-daq-code/glib-testing/inc)
add_executable(vfattest tests/vfattest.cpp)
target_link_libraries(vfattest "-L ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -L ${CMAKE_GLIB_LIBRARY} -lvfat -lclassTest")
