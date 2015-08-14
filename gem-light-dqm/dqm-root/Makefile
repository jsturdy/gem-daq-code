#
# Makefile for gemtreewriter package
#
BUILD_HOME:=$(shell pwd)/../..

CC=g++
CCFLAGS=-O2 -Wall -fPIC -pthread -m64
ADDFLAGS=-g -std=c++0x
RC=rootcint
LBL=-shared -O2 -m64
LS=ls -lartF

# ROOT Config
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS  =$(shell root-config --libs) 
ROOTGLIBS =$(shell root-config --glibs) 

#Project=gem
Project=gem-light-dqm
Package=dqm-root
LongPackage=dqm-root
ShortPackage=dqm-root
PackageName=dqm-root

IncludeDirs = /usr/include/root
IncludeDirs+= $(BUILD_HOME)/$(Project)/$(Package)/include
IncludeDirs+= $(BUILD_HOME)/$(Project)/gemtreewriter/include
IncludeDirs+= ${BUILD_HOME}/gemdaq-testing/gemreadout/include
INC=$(IncludeDirs:%=-I%)

DependentLibraryDirs =$(XDAQ_ROOT)/lib 
DependentLibraryDirs+=$(BUILD_HOME)/gemdaq-testing/gemutils/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 
DependentLibraryDirs+=$(BUILD_HOME)/gemdaq-testing/gembase/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 
DependentLibraryDirs+=$(BUILD_HOME)/gemdaq-testing/gemhardware/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 
DependentLibraryDirs+=$(BUILD_HOME)/gemdaq-testing/gemreadout/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) 
DependentLibraryDirs+=$(uHALROOT)/lib 
DependentLibraryDirs+=$(BUILD_HOME)/$(Project)/gemtreewriter/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)
LIB=$(DependentLibraryDirs:%=-L%)

SRC=$(BUILD_HOME)/$(Project)/$(Package)/src/common
OBJ=$(BUILD_HOME)/$(Project)/$(Package)/src/obj
DIC=$(BUILD_HOME)/$(Project)/$(Package)/src/dic
BIN=$(BUILD_HOME)/$(Project)/$(Package)/bin/$(XDAQ_OS)/$(XDAQ_PLATFORM)

#DependentLibraries = log4cplus config xcept boost_system cactus_uhal_uhal xerces-c numa toolbox xdaq2rc asyncresolv uuid gem_hw gem_utils gem_base gem_readout Event cgicc xdaq xdata xoap logudpappender logxmlappender xgi ptfifo pthttp pttcp ptutcp ptatcp ptfrl ptibv ptudapl mimetic
DependentLibraries_1 := $(wildcard $(XDAQ_ROOT)/lib/*.so)
DependentLibraries_2 := $(notdir $(DependentLibraries_1))
DependentLibraries_3 := $(basename $(DependentLibraries_2))
DependentLibraries_4 := $(DependentLibraries_3:lib%=%)
rmlibs := fedstreamer ptudapl psxsapi psx xslp xplore
DependentLibraries := $(filter-out $(rmlibs), $(DependentLibraries_4))
DependentLibraries += log4cplus config boost_system cactus_uhal_uhal numa toolbox uuid ncurses Event cgicc gem_hw gem_utils gem_base gem_readout
dlibs=$(DependentLibraries:%=-l%)

all:
	mkdir -p $(BIN)
	$(CC) $(ADDFLAGS) $(ROOTLIBS) $($ROOTGLIBS) $(LIB) $(dlibs) $(INC) $(SRC)/treeReaderExample.cxx -o $(BIN)/reader
	$(LS) $(BIN)
clean:
	rm -rf $(BIN)
print-env:
	@echo BUILD_HOME    $(BUILD_HOME)
	@echo XDAQ_OS       $(XDAQ_OS)
	@echo XDAQ_PLATFORM $(XDAQ_PLATFORM)
	@echo INC           $(INC)

