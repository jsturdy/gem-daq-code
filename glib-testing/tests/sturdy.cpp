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

      /////////
      //ValWord< uint32_t > user_ipb_regs      = hw2.getNode("user_ipb_regs").read();
      //ValWord< uint32_t > user_ipb_stat_regs = hw2.getNode("user_ipb_stat_regs").read();
      //ValWord< uint32_t > user_ipb_ctrl_regs = hw2.getNode("user_ipb_ctrl_regs").read();
      //ValWord< uint32_t > user_wb_regs       = hw2.getNode("user_wb_regs").read();
      //hw2.dispatch();
      //
      //std::cout<<"user_ipb_regs:"<<user_ipb_regs.value()<<"  "
      //	       <<"user_ipb_stat_regs:"<<user_ipb_regs.value()<<"  "
      //	       <<"user_ipb_ctrl_regs:"<<user_ipb_regs.value()<<"  "
      //	       <<"user_wb_regs:"<<user_ipb_regs.value()<<"  "
      //	       <<std::endl;
      //
      ////hw2.getNode("user_ipb_regs").write(0x1);
      ////hw2.getNode("user_ipb_stat_regs:").write(0x2);
      ////hw2.getNode("user_ipb_ctrl_regs:").write(0x3);
      ////hw2.getNode("user_wb_regs:").write(0x4);
      ////hw2.dispatch();
      ////
      ////user_ipb_regs   = hw2.getNode("user_ipb_regs").read();
      ////user_ipb_stat_regs   = hw2.getNode("user_ipb_stat_regs").read();
      ////user_ipb_ctrl_regs   = hw2.getNode("user_ipb_ctrl_regs").read();
      ////user_wb_regs   = hw2.getNode("user_wb_regs").read();
      ////hw2.dispatch();
      ////
      ////std::cout<<"user_ipb_regs:"<<user_ipb_regs.value()<<"  "
      ////	       <<"user_ipb_stat_regs:"<<user_ipb_regs.value()<<"  "
      ////	       <<"user_ipb_ctrl_regs:"<<user_ipb_regs.value()<<"  "
      ////	       <<"user_wb_regs:"<<user_ipb_regs.value()<<"  "
      ////	       <<std::endl;
      ////

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


      /*****
      std::vector<std::string> ids = hw.getNodes();
      std::cout << "getNodes(): ";
      std::copy(ids.begin(),
		ids.end(),
		std::ostream_iterator<std::string>(std::cout,", "));
      
      std::cout << std::endl << std::endl;
      
      ids = hw.getNodes(".*MEM.*");
      std::cout << "getNodes(\".*MEM.*\").getNodes(): ";
      std::copy(ids.begin(),
		ids.end(),
		std::ostream_iterator<std::string>(std::cout,", "));
      
      std::cout << std::endl << std::endl;
      
      ids = hw.getNode("SUBSYSTEM1").getNodes();
      std::cout << "getNode("SUBSYSTEM1").getNodes(): ";
      std::copy(ids.begin(),
		ids.end(),
		std::ostream_iterator<std::string>(std::cout,", "));
      
      std::cout << std::endl;
      *****/
      //

      std::cout<<"testing bitshifting"<<std::endl;
      char num0 = 3;
      char num1 = 7;
      char num2 = 11;
      std::cout<<"num2 = "<<int(num2)<<"  "
	       <<"num1 = "<<int(num1)<<"  "
	       <<"num0 = "<<int(num0)<<"  "
	       <<std::endl;
      std::cout<<"num2 = "<<std::hex<<int(num2)<<std::dec<<"  "
	       <<"num1 = "<<std::hex<<int(num1)<<std::dec<<"  "
	       <<"num0 = "<<std::hex<<int(num0)<<std::dec<<"  "
	       <<std::endl;
      
      uint32_t num = 0x0000;
      std::cout<<"num = "<<num<<"("<<std::hex<<num<<std::dec<<")"<<std::endl;
      //num = (((num<<8)+num2)<<8+num1)<<8+num0;
      num = (num2<<16)+(num1<<8)+num0;
      std::cout<<"num = "<<num<<"("<<std::hex<<num<<std::dec<<")"<<std::endl;
      num = 0x0000;
      num = (num<<8)+num2;
      std::cout<<"num = "<<num<<"("<<std::hex<<num<<std::dec<<")"<<std::endl;
      num = (num<<8)+num1;
      std::cout<<"num = "<<num<<"("<<std::hex<<num<<std::dec<<")"<<std::endl;
      num = (num<<8)+num0;
      std::cout<<"num = "<<num<<"("<<std::hex<<num<<std::dec<<")"<<std::endl;
      std::cout<<"num = "<<num<<" or "<<((num2<<16) | (num1<<8) | num0)<<std::endl;
      std::cout<<"num = "<<std::hex<<num<<std::dec
	       <<" or "<<std::hex<<((num2<<16) | (num1<<8) | num0)
	       <<std::dec<<std::endl;
    }
  catch ( const std::exception& e )
    {
      std::cout << "Something went wrong: " << e.what() <<
	std::endl;
    }
  return 0;
}
