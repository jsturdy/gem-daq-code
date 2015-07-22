#
# Makefile for gemsupervisor package
#
BUILD_HOME:=$(shell pwd)/../..

Project=gem-light-dqm
Package=dqm-reader
LongPackage=dqm-reader 
ShortPackage=dqm-reader
PackageName=dqm-reader

# Compilator
CC=g++
CCFLAGS=-O2 -Wall -fPIC -pthread -m64
ADDFLAGS=-g -std=c++0x
LBL=-shared -O2 -m64
LS=ls -lartF

# ROOT Config
ROOTCFLAGS =$(shell root-config --cflags)
ROOTLIBS   =$(shell root-config --libs)
ROOTGLIBS  =$(shell root-config --glibs) 

Sources1 = gem-read-events.cxx
Sources2 = dqm-read-make-ttree.cxx 

IncludeDirs = /usr/include/root
IncludeDirs+= $(BUILD_HOME)/$(Project)/$(Package)/include
IncludeDirs+= ${BUILD_HOME}/gemdaq-testing/gemreadout/include
INC=$(IncludeDirs:%=-I%)

SRC=$(BUILD_HOME)/$(Project)/$(Package)/src/common
OBJ=$(BUILD_HOME)/$(Project)/$(Package)/src/$(XDAQ_OS)/$(XDAQ_PLATFORM) 
DIC=$(BUILD_HOME)/$(Project)/$(Package)/src/$(XDAQ_OS)/$(XDAQ_PLATFORM)
LIB=$(BUILD_HOME)/$(Project)/$(Package)/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)
BIN=$(BUILD_HOME)/$(Project)/$(Package)/bin/$(XDAQ_OS)/$(XDAQ_PLATFORM)

reader:
	mkdir -p $(OBJ) $(BIN)
	$(CC) $(ADDFLAGS) $(ROOTLIBS) $(INC) $(SRC)/$(Sources1) -o $(BIN)/myDQMlight
	$(LS) $(BIN)
reader-simple:
	mkdir -p $(OBJ) $(LIB) $(BIN)
	$(CC) $(ADDFLAGS) $(ROOTLIBS) $(INC) $(SRC)/$(Sources2) -o $(BIN)/mySimpleDQMlight
	$(LS) $(BIN)
all:
	$(MAKE) $(reader) $(reader-simple)
clean:
	rm -rf $(BIN) $(OBJ)

print-env:
	@echo BUILD_HOME    $(BUILD_HOME)
	@echo XDAQ_OS       $(XDAQ_OS)
	@echo XDAQ_PLATFORM $(XDAQ_PLATFORM)
	@echo INC           $(INC)
	@echo reader        $(Sources1)
	@echo reader-simple $(Sources2)
