#include <cstdlib>
#include <iostream>
#include <cmath>
#include "uhal/uhal.hpp"
#include <boost/lexical_cast.hpp>

using namespace uhal;

#define NUMREGS 9

void readCDCERegisters(HwInterface hw) {
  ValWord< uint32_t > rxdata[NUMREGS];  
  for (int regID = 0; regID < NUMREGS; ++regID) {
    uint32_t writeWord = (regID<<4)+0xE;
    //std::cout<<"Word to be transmitted:0x"<<std::hex<<writeWord<<std::dec<<std::endl;
    hw.getNode("spi_txdata").write(writeWord) ;
    hw.dispatch();
    hw.getNode("spi_command").write(0x8FA38014) ;
    hw.dispatch();
    hw.getNode("spi_txdata").write(0xAAAAAAAA) ;//dummy write, why?
    hw.dispatch();
    hw.getNode("spi_command").write(0x8FA38014) ;
    hw.dispatch();
    rxdata[regID] = hw.getNode("spi_rxdata").read() ;
    hw.dispatch();
    
    //std::cout<<"spi_rxdata["<<regID<<"]:0x" 
    std::cout<<"-> register 0"<<regID<<" contents ="<<std::hex<<rxdata[regID].value()<<std::dec
	     <<std::endl;
    /*
    std::cout<<"  spi_rxdata["<<regID<<"]:"<<std::bitset<32>(rxdata[regID].value())
	     <<std::endl;
    */
  }
  return;
}

void readCDCERegister(HwInterface hw, int regID) {
  
  if (regID > NUMREGS - 1) {
    std::cout<<"Please enter a valid CDCE register number (0-7)"<<std::endl;
    return;
  }
  
  uint32_t writeWord = (regID<<4)+0xE;
  std::cout<<std::hex<<writeWord<<std::dec<<std::endl;

  hw.getNode("spi_txdata").write(writeWord) ;
  hw.dispatch();
  hw.getNode("spi_command").write(0x8FA38014) ;
  hw.dispatch();
  //hw.getNode("spi_txdata").write(0xAAAAAAAA) ;//dummy write, why?
  //does the dummy write fill the register?
  //hw.dispatch();
  //hw.getNode("spi_command").write(0x8FA38014) ;
  //hw.dispatch();
  ValWord< uint32_t > rxdata = hw.getNode("spi_rxdata").read() ;
  hw.dispatch();
  
  std::cout<<"spi_rxdata["<<regID<<"]:0x" <<std::hex<<rxdata.value()<<std::dec
	   <<std::endl;
  std::cout<<"  spi_rxdata["<<regID<<"]:"<<std::bitset<32>(rxdata.value())
	   <<std::endl;
  return;
}


int main()
{
  try
    {
      //setLogLevelTo(uhal::Debug());  // Maximise uHAL logging
      setLogLevelTo(uhal::Error());  // Minimise uHAL logging
      
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
      ConnectionManager connectionManager( connectionPath );
      
      HwInterface hw = connectionManager.getDevice("spiregs.udp");
      
      std::cout<<"initial values"<<std::endl;
      readCDCERegisters(hw);

      //spi_prescale = 0x014 //bits 0-11
      //reserved 0xFA38 //bits 12-27
      //unused //bits 28-30
      //spi_strobe = 1 //bit 31, set to true
      //spi_comm = 0x8FA38014;
      
      //there are 8 CDCE registers
      //cdce_readcommand = [0xE,0x1E,0x2E,0x3E,0x4E,0x5E,0x6E,0x7E,0x8E] 
      //RdBuffer = [0,0,0,0,0,0,0,0,0,0]

    }
  catch ( const std::exception& e )
    {
      std::cout << "Something went wrong: " << e.what() <<
	std::endl;
    }
  return 0;
}
