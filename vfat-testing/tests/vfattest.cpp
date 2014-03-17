#include <iostream>
#include <cmath>
#include "uhal/uhal.hpp"
#include <boost/lexical_cast.hpp>
#include "vfat.h"
#include "classTest.h"
//#include "/home/sturdy/xdaqgem/glib_dev/gem-daq-code/vfat-testing/inc/vfat.h"

using namespace uhal;
using namespace gem::base;

int main()
{
  try
    {
      //setLogLevelTo(uhal::Debug());  // Maximise uHAL logging
      setLogLevelTo(uhal::Error());  // Minimise uHAL logging
      
      gem::base::vfat::configuration conf;
      
      conf.trigMode       = 8;
      conf.calibMode      = 3;
      conf.calibPol       = 1;
      conf.latency        = 1;
      conf.deviceID       = "vfattest";
      char * val;
      val = std::getenv( "GLIBTEST" );
      std::string dirVal = "";
      if (val != NULL) {
	dirVal = val;
      }
      else {
	std::cout<<"$GLIBTEST not set, exiting"<<std::endl;
	exit(1);
      }

      char connectionPath[128];
      sprintf(connectionPath,"file://%s/data/myconnections.xml;",dirVal.c_str());
      std::string tmpString(connectionPath);
      conf.connectionFile = tmpString;
      
      TEST::TEST myglib;
      //myglib.FetchFirmWare();

      gem::base::vfat testing(conf);
      testing.PrintRegisters();

      testing.SetLatency(24);
      testing.PrintRegisters();

      testing.SetDACMode(6);
      testing.PrintRegisters();
      
      testing.SetHitCounterMode(3);
      testing.PrintRegisters();

      testing.SendDFTestPattern();
      testing.PrintRegisters();
      
    }
  catch ( const std::exception& e )
    {
      std::cout << "Something went wrong: " << e.what() <<
	std::endl;
    }
  return 0;
}
