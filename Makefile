BUILD_HOME:=$(shell pwd)/../..

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=gem-daq-code
ifeq ($(Set),gemdaq)
Packages=base \
         core \
         supervisor \
         mangaer \
         vfat \
#         vfat/base \ 
#         vfat/core \ 
#         vfat/supervisor \ 
#         vfat/manager \ 
         glib \
#         glib/base \ 
#         glib/core \ 
#         glib/supervisor \ 
#         glib/manager \ 
endif

ifeq ($(Set),glib)
Packages=glib \
#         glib/base \ 
#         glib/core \ 
#         glib/supervisor \ 
#         glib/manager \ 
endif

ifeq ($(Set),vfat)
Packages=vfat \
#         vfat/base \ 
#         vfat/core \ 
#         vfat/supervisor \ 
#         vfat/manager \ 
endif

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
