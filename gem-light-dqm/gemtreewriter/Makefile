#
# Makefile for gemtreewriter package
#
BUILD_HOME:=$(shell pwd)/../..

CC=g++
CCFLAGS=-O2 -Wall -fPIC -pthread -m64
ADDFLAGS=-g -std=c++0x
RC=rootcint
LBL=-shared -O2 -m64

# ROOT Config
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS  =$(shell root-config --libs) 
ROOTGLIBS =$(shell root-config --glibs) 

#Project=gem
Project=gem-light-dqm
Package=gemtreewriter
LongPackage=gemtreewriter
ShortPackage=treewriter
PackageName=treewriter

IncludeDirs =$(BUILD_HOME)/$(Project)/$(Package)/include 
IncludeDirs+= /usr/include/root 
INC=$(IncludeDirs:%=-I%)

SRC=$(BUILD_HOME)/$(Project)/$(Package)/src/common
OBJ=$(BUILD_HOME)/$(Project)/$(Package)/src/obj
DIC=$(BUILD_HOME)/$(Project)/$(Package)/src/dic
LIB=$(BUILD_HOME)/$(Project)/$(Package)/lib
BIN=$(BUILD_HOME)/$(Project)/$(Package)/bin

Sources= Event.cxx
Headers= Event.h EventLinkDef.h
Objects= $(OBJ)/Event.o $(OBJ)/EventDict.o
#SRCDIC=$(SourceDic:%=$(BUILD_HOME)/$(Project)/$(Package)/include/%)

event:
	mkdir -p $(OBJ)
	$(CC) $(CCFLAGS) $(ROOTLIBS) $($ROOTGLIBS) $(INC) -c $(SRC)/Event.cxx -o $(OBJ)/Event.o
	mkdir -p $(DIC)
	$(RC) -f $(DIC)/EventDict.cxx -c $(INC) $(Headers)
	$(CC) $(CCFLAGS) $(ROOTLIBS) $($ROOTGLIBS) $(INC) -c $(DIC)/EventDict.cxx -o $(OBJ)/EventDict.o
	mkdir -p $(LIB)
	$(CC) $(LBL) $(Objects) -o $(LIB)/libEvent.so
all:
	mkdir -p $(OBJ)
	$(CC) $(CCFLAGS) $(ROOTLIBS) $($ROOTGLIBS) $(INC) -c $(SRC)/Event.cxx -o $(OBJ)/Event.o
	mkdir -p $(DIC)
	$(RC) -f $(DIC)/EventDict.cxx -c $(INC) $(Headers)
	$(CC) $(CCFLAGS) $(ROOTLIBS) $($ROOTGLIBS) $(INC) -c $(DIC)/EventDict.cxx -o $(OBJ)/EventDict.o
	mkdir -p $(LIB)
	$(CC) $(LBL) $(Objects) -o $(LIB)/libEvent.so
	mkdir -p $(BIN)
	$(CC) $(ADDFLAGS) $(ROOTLIBS) $($ROOTGLIBS) $(INC) $(SRC)/gemTreeComposer.cxx -L$(LIB) -lEvent -o $(BIN)/gtc
clean:
	rm -rf $(BIN) $(OBJ) $(DIC) $(LIB)
