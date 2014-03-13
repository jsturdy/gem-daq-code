#include <cstdlib>
#include <iostream>
#include "uhal/uhal.hpp"
#include <boost/lexical_cast.hpp>

using namespace uhal;

int main()
{
  std::cout<<"starting test code sturdy"<<std::endl;
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
  std::cout<<"dirVal = "<<dirVal<<std::endl;
  try
    {
      //setLogLevelTo(uhal::Debug());  // Maximise uHAL logging
      setLogLevelTo(uhal::Error());  // Minimise uHAL logging
      
      char connectionPath[128];
      sprintf(connectionPath,"file://%s/data/myconnections.xml;",dirVal.c_str());
      std::cout<<"connectionPath = "<<connectionPath<<std::endl;
      ConnectionManager connectionManager( connectionPath );
      
      HwInterface hw = connectionManager.getDevice("dummy.udp.0");
      
      for (int ir = 1; ir < 5; ++ir) {
	ValWord< uint32_t > sysreg = hw.getNode ( "system_REG_"+boost::lexical_cast<std::string>(ir) ).read();
	hw.dispatch();
	std::cout << "system_REG_" << ir << " = " << sysreg.value() << std::endl;
      }

      for (int ir = 1; ir < 5; ++ir) {
	ValWord< uint32_t > boardreg = hw.getNode ( "board_REG_"+boost::lexical_cast<std::string>(ir) ).read();
	hw.dispatch();
	std::cout << "board_REG_" << ir << " = " << boardreg.value() << std::endl;
      }

      for (int ir = 1; ir < 7; ++ir) {
	ValWord< uint32_t > fwreg = hw.getNode ( "firmware_REG_"+boost::lexical_cast<std::string>(ir) ).read();
	hw.dispatch();
	std::cout << "firmware_REG_" << ir << " = " << fwreg.value() << std::endl;
      }

      //
      HwInterface hw2 = connectionManager.getDevice("dummy.udp.1");
      //
      ValWord< uint32_t > mac_ip_source = hw2.getNode("mac_ip_source").read() ;
      ValWord< uint32_t > mac_b5  = hw2.getNode("mac_b5").read();
      ValWord< uint32_t > mac_b4  = hw2.getNode("mac_b4").read();
      ValWord< uint32_t > mac_b3  = hw2.getNode("mac_b3").read();
      ValWord< uint32_t > mac_b2  = hw2.getNode("mac_b2").read();
      ValWord< uint32_t > mac_b1  = hw2.getNode("mac_b1").read();
      ValWord< uint32_t > mac_b0  = hw2.getNode("mac_b0").read();
      ValWord< uint32_t > ip_addr = hw2.getNode("ip_addr").read();
      ValWord< uint32_t > ip_b3   = hw2.getNode("ip_b3").read();
      ValWord< uint32_t > ip_b2   = hw2.getNode("ip_b2").read();
      ValWord< uint32_t > ip_b1   = hw2.getNode("ip_b1").read();
      ValWord< uint32_t > ip_b0   = hw2.getNode("ip_b0").read();
      hw2.dispatch();

      std::cout<<"macInfo::ip_source:"
	       <<mac_ip_source.value()
	       <<"  b5:"<<mac_b5.value()
	       <<"  b4:"<<mac_b4.value()
	       <<"  b3:"<<mac_b3.value()
	       <<"  b2:"<<mac_b2.value()
	       <<"  b1:"<<mac_b1.value()
	       <<"  b0:"<<mac_b0.value()
	       <<std::endl;

      char ipInfo[128];
      sprintf(ipInfo,"addr:%d  b3:%d  b2:%d  b1:%d  b0:%d",
	      ip_addr.value(),
	      ip_b3.value(),
	      ip_b2.value(),
	      ip_b1.value(),
	      ip_b0.value());

      //std::cout<<macInfo.c_str()<<std::endl;
      std::cout<< ipInfo<<std::endl;

      ValWord< uint32_t > test     = hw2.getNode("test").read() ;
      hw2.dispatch();
      std::cout<<"test:"<<test.value()<<std::endl;
      hw2.getNode("test").write(0x03);
      hw2.dispatch();
      test = hw2.getNode("test").read() ;
      hw2.dispatch();
      std::cout<<"test:"<<test.value()<<std::endl;

      ValWord< uint32_t > status_2 = hw2.getNode("status_2").read() ;
      hw2.dispatch();
      std::cout<<"status_2:"<<status_2.value()<<std::endl;

    }
  catch ( const std::exception& e )
    {
      std::cout << "Something went wrong: " << e.what() <<
	std::endl;
    }
  return 0;
}
