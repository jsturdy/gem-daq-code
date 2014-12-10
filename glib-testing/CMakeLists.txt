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

#add_library(glib SHARED inc/glib.h src/glib.cpp)
#target_link_libraries(glib "-L ${uHAL_LIBRARY_DIR} ${LIBS}")

add_library(classTest SHARED inc/classTest.h src/classTest.cpp)
target_link_libraries(classTest "-L ${uHAL_LIBRARY_DIR} ${LIBS}")

message(STATUS "=======================================================")
message(STATUS "Please set your LD_LIBRARY_PATH env variable as" )
message(STATUS "export LD_LIBRARY_PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}:$LD_LIBRARY_PATH")
message(STATUS "=======================================================")

##testing###set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
##testing##
add_executable(simple tests/simple.cpp)
target_link_libraries(simple "-L ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -lclassTest")
##testing##add_executable(glibsystem tests/glibsystem.cpp)
##testing##target_link_libraries(glibsystem "-L ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -lglib")
##testing##
##testing##add_executable(sturdy tests/sturdy.cpp)
##testing##target_link_libraries(sturdy "-L ${uHAL_LIBRARY_DIR} ${LIBS}")
##testing##
##testing##add_executable(readtemps tests/readtemps.cpp)
##testing##target_link_libraries(readtemps "-L ${uHAL_LIBRARY_DIR} ${LIBS}")
##testing##
##testing##add_executable(spitests tests/spitests.cpp)
##testing##target_link_libraries(spitests "-L ${uHAL_LIBRARY_DIR} ${LIBS}")
