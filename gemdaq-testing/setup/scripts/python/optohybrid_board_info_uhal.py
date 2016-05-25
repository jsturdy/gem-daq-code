#!/bin/env python

import sys, re
import time, datetime, os

sys.path.append('${GEM_PYTHON_PATH}')

import uhal
from registers_uhal import *
from glib_user_functions_uhal import *
from optohybrid_user_functions_uhal import *
from rate_calculator import getErrorRate,errorRate

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-s", "--slot", type="int", dest="slot",
		  help="slot in uTCA crate", metavar="slot", default=10)
parser.add_option("-g", "--gtx", type="int", dest="gtx",
		  help="GTX on the GLIB", metavar="gtx", default=0)
parser.add_option("-r", "--reset", action="store_true", dest="resetCounters",
		  help="reset link error counters", metavar="resetCounters")
parser.add_option("-k", "--clkSrc", type="int", dest="clkSrc",
		  help="which reference clock to use on OH", metavar="clkSrc")
parser.add_option("-x", "--extTrig", type="int", dest="trgSrc",
		  help="change trigger source", metavar="trgSrc")
parser.add_option("-b", "--sbitout", type="int", dest="sbitSrc",
		  help="use s-bit from VFAT <num>", metavar="sbitSrc")
parser.add_option("-d", "--debug", action="store_true", dest="debug",
		  help="print extra debugging information", metavar="debug")
parser.add_option("-l", "--localT1", action="store_true", dest="localT1",
		  help="enable the localT1 controller", metavar="localT1")
parser.add_option("-e", "--errors", type="int", dest="errorRate", default=1,
		  help="calculate link error rates for N seconds", metavar="errorRate")
parser.add_option("--testbeam", action="store_true", dest="testbeam",
		  help="fixed IP address for testbeam", metavar="testbeam")
parser.add_option("--v2b", action="store_true", dest="v2b",
		  help="Specific functionality only in v2b", metavar="v2b")

(options, args) = parser.parse_args()

uhal.setLogLevelTo( uhal.LogLevel.FATAL )

uTCAslot = 10
if options.slot:
	uTCAslot = 160+options.slot
	print options.slot, uTCAslot
ipaddr        = '192.168.0.%d'%(uTCAslot)
if options.testbeam:
        ipaddr        = '137.138.115.185'
uri           = "chtcp-2.0://localhost:10203?target=%s:50001"%(ipaddr)

address_table = "file://${GEM_ADDRESS_TABLE_PATH}/glib_address_table.xml"
optohybrid    = uhal.getDevice( "optohybrid" , uri, address_table )

address_table = "file://${GEM_ADDRESS_TABLE_PATH}/glib_address_table.xml"
glib          = uhal.getDevice( "glib"       , uri, address_table )

SAMPLE_TIME = 1.

########################################
# IP address
########################################
print
print "-> -----------------"
print "-> OPTOHYBRID STATUS     "
print "-> -----------------"

fwver = getFirmwareVersion(optohybrid,options.gtx)
date = '%02x/%02x/%04x'%(fwver["d"],fwver["m"],fwver["y"])
print "-> oh fw date : %s%s%s"%(colors.YELLOW,date,colors.ENDC)
print
print "Connected VFATs mask: 0x%08x"%(getConnectedVFATsMask(glib,options.gtx,options.debug))
print "VFATs s-bit mask:     0x%08x"%(getVFATsBitMask(glib,options.gtx,options.debug))
setVFATsBitMask(glib,options.gtx,0x000000,options.debug)
print "VFATs s-bit mask:     0x%08x"%(getVFATsBitMask(glib,options.gtx,options.debug))
print


if options.clkSrc in [0,1,2]:
        setReferenceClock(optohybrid,options.gtx,options.clkSrc)
#print "-> OH VFATs accessible: 0x%x"%(readRegister(glib,"VFATs_TEST"))
if options.trgSrc in [0,1,2,3,4]:
        setTriggerSource(optohybrid,options.gtx,options.trgSrc)
	
if options.localT1:
        #configureLocalT1(optohybrid,options.gtx,0x0,0x0,0,25,100)
        sendL1ACalPulse(optohybrid,options.gtx,15)

if options.sbitSrc in [1,2,3,4,5,6]:
        setTriggerSBits(False,optohybrid,options.gtx,options.sbitSrc)

clocking = getClockingInfo(optohybrid,options.gtx)
#OH:  TrgSrc  SBitSrc  FPGA PLL    EXT PLL    CDCE     GTX  RefCLKSrc
#->:     0x0      0x0       0x0       0x0      0x1     0x1        0x1
if options.v2b:
        print "Sources:  %6s  %7s"%("TrgSrc","SBitSrc")
        print "             0x%x      0x%x"%(
                readRegister(optohybrid,"GLIB.OptoHybrid_%d.OptoHybrid.CONTROL.TRIGGER.SOURCE"%(options.gtx)),
                readRegister(optohybrid,"GLIB.OptoHybrid_%d.OptoHybrid.CONTROL.OUTPUT.SBits"%(options.gtx)))

        print "Lock status:  %10s  %13s"%("QPLL","QPLL FPGA PLL")
        print "                     0x%x            0x%x"%(
                clocking["qplllock"],
                clocking["qpllfpgaplllock"])
        errorCounts = {}
        errorCounts["QPLL"] = []
        errorCounts["FPGA"] = []
        for trial in range(options.errorRate):
                errorCounts["QPLL"].append(calculateLockErrors(optohybrid,options.gtx,"QPLL",SAMPLE_TIME))
                errorCounts["FPGA"].append(calculateLockErrors(optohybrid,options.gtx,"QPLL_FPGA_PLL",SAMPLE_TIME))
        
        qrates = getErrorRate(errorCounts["QPLL"],SAMPLE_TIME)
        frates = getErrorRate(errorCounts["FPGA"],SAMPLE_TIME)
        print "Unlock count: 0x%08x     0x%08x"%(qrates[0],frates[0])
        0x00000107
        print "Unlock rate:%10sHz   %10sHz"%("%2.2f%s"%(qrates[1],qrates[2]),"%2.2f%s"%(frates[1],frates[2]))
else:
        print "Sources:  %6s  %7s  %9s"%("TrgSrc","SBitSrc","RefCLKSrc")
        print "             0x%x      0x%x        0x%x"%(
                readRegister(optohybrid,"GLIB.OptoHybrid_%d.OptoHybrid.CONTROL.TRIGGER.SOURCE"%(options.gtx)),
                readRegister(optohybrid,"GLIB.OptoHybrid_%d.OptoHybrid.CONTROL.OUTPUT.SBits"%(options.gtx)),
                clocking["refclock"])

        print "Lock status:  %8s  %7s  %4s  %3s"%("FPGA PLL","EXT PLL","CDCE","GTX")
        print "                   0x%x      0x%x   0x%x  0x%x"%(
                clocking["fpgaplllock"],
                clocking["extplllock"],
                clocking["cdcelock"],
                clocking["gtxreclock"])
	
print 
#print "-> OH Clocking (src, bkp):     VFAT         CDCE"
#print "-> %22s%d       (0x%x  0x%x)   (0x%x  0x%x)"%("link",links[link],
#						     clocking["vfatsrc"],clocking["vfatbkp"],
#						     clocking["cdcesrc"],clocking["cdcebkp"])
#exit(1)

if (options.resetCounters):
        optohybridCounters(optohybrid,options.gtx,True)

print "-> Counters    %8s     %8s     %8s     %8s"%("L1A","Cal","Resync","BC0")
counters = optohybridCounters(optohybrid,options.gtx)
for key in counters["T1"]:
        print "   %8s  0x%08x   0x%08x   0x%08x   0x%08x"%(key,
                                                           counters["T1"][key]["L1A"],
                                                           counters["T1"][key]["CalPulse"],
                                                           counters["T1"][key]["Resync"],
                                                           counters["T1"][key]["BC0"])
	
print
print "--=======================================--"
print "-> OH: %10s  %12s"%("ErrCnt","(rate)")
errorCounts = []
for trial in range(options.errorRate):
        errorCounts.append(calculateLinkErrors(False,optohybrid,options.gtx,SAMPLE_TIME))
sys.stdout.flush()

rates = errorRate(errorCounts,SAMPLE_TIME)
#counters = optohybridCounters(optohybrid,options.gtx)
print "-> TRK: 0x%08x  (%6.2f%1sHz)"%(rates["TRK"][0],rates["TRK"][1],rates["TRK"][2])
print "-> TRG: 0x%08x  (%6.2f%1sHz)"%(rates["TRG"][0],rates["TRG"][1],rates["TRG"][2])

print
print "FIFO:  %8s  %7s  %10s"%("isEmpty",  "isFull", "depth")
fifoInfo = readFIFODepth(glib,options.gtx)
print "       %8s  %7s  %10s"%("0x%x"%(fifoInfo["isEMPTY"]),
                               "0x%x"%(fifoInfo["isFULL"]),
                               "0x%x"%(fifoInfo["Occupancy"]))

print
print "--=======================================--"
