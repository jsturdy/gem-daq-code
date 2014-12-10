#include <cstdlib>
#include <iostream>
#include <cmath>
#include "uhal/uhal.hpp"
#include <boost/lexical_cast.hpp>

using namespace uhal;

void readRegisters(HwInterface hw) {
  
  ValWord< uint32_t > settings = hw.getNode("i2c_settings").read() ;
  ValWord< uint32_t > command  = hw.getNode("i2c_command").read() ;
  ValWord< uint32_t > reply    = hw.getNode("i2c_reply").read() ;
  hw.dispatch();
  
  std::cout<<"i2c_settings:0x" <<std::hex<<settings.value()<<std::dec
	   <<"  i2c_command:0x"<<std::hex<<command.value()   <<std::dec
	   <<"  i2c_reply:0x"  <<std::hex<<reply.value()     <<std::dec
	   <<std::endl;
  std::cout<<"i2c_settings:" <<std::bitset<13>(settings.value())
	   <<"  i2c_command:"<<std::bitset<32>(command.value())
	   <<"  i2c_reply:"  <<std::bitset<28>(reply.value())
	   <<std::endl;
  return;
}

void readSettings(HwInterface hw) {
  ValWord< uint32_t > settings    = hw.getNode("i2c_settings").read() ;
  hw.dispatch();
  std::cout<<"i2c_settings:0x"<<std::hex<<settings.value()<<std::dec<<std::endl;
  std::cout<<"i2c_settings:"<<std::bitset<13>(settings.value())<<std::endl;
  return;
}

void readCommand(HwInterface hw) {
  ValWord< uint32_t > command    = hw.getNode("i2c_command").read() ;
  ValWord< uint32_t > autoclear  = hw.getNode("i2c_command.i2c_autoclear" ).read();
  ValWord< uint32_t > strobe     = hw.getNode("i2c_command.i2c_strobe"    ).read();
  ValWord< uint32_t > mode16     = hw.getNode("i2c_command.i2c_mode16"    ).read();
  ValWord< uint32_t > write      = hw.getNode("i2c_command.i2c_write"     ).read();
  ValWord< uint32_t > slvaddr_7b = hw.getNode("i2c_command.i2c_slvaddr_7b").read();
  ValWord< uint32_t > wrdata     = hw.getNode("i2c_command.i2c_wrdata"    ).read();
  hw.dispatch();
  std::cout<<"i2c_command:0x"<<std::hex<<command.value()<<std::dec<<std::endl;
  std::cout<<"i2c_command:"<<std::bitset<32>(command.value())<<std::endl;

  std::cout<<"i2c_autoclear :0x"<<std::hex<<autoclear .value()<<std::dec<<std::endl;
  std::cout<<"i2c_strobe    :0x"<<std::hex<<strobe    .value()<<std::dec<<std::endl;
  std::cout<<"i2c_mode16    :0x"<<std::hex<<mode16    .value()<<std::dec<<std::endl;
  std::cout<<"i2c_write     :0x"<<std::hex<<write     .value()<<std::dec<<std::endl;
  std::cout<<"i2c_slvaddr_7b:0x"<<std::hex<<slvaddr_7b.value()<<std::dec<<std::endl;
  std::cout<<"i2c_wrdata    :0x"<<std::hex<<wrdata    .value()<<std::dec<<std::endl;
  return;
}

//void readReply(HwInterface hw) {
//  ValWord< uint32_t > reply    = hw.getNode("i2c_reply").read() ;
//  hw.dispatch();
//  std::cout<<"i2c_reply:0x"<<std::hex<<reply.value()<<std::dec<<std::endl;
//  std::cout<<"i2c_reply:"<<std::bitset<28>(reply.value())<<std::endl;
//  return;
//}


int readReply(HwInterface hw) {
  ValWord< uint32_t > reply;
  bool pending = true;
  //while (pending){
  reply = hw.getNode("i2c_reply").read() ;
  hw.dispatch();
  int rddata_lo = (reply.value())&0x000000ff;
  int rddata_hi = (reply.value())&0x0000ffff;
  int rddata_status = ((reply.value())&0x0c000000)/(pow(2,26));
  std::cout<<"i2c_reply:0x"<<std::hex<<reply.value()<<std::dec
	   <<"  rddata_lo::"<<rddata_lo
	   <<"  rddata_hi::"<<rddata_hi
	   <<"  rddata_status::"<<rddata_status
	   <<std::endl;
  if (rddata_status == 3 || rddata_status == 1) {
    pending = false;
    if (rddata_status == 3)
      std::cout<<"failed!";
    else
      std::cout<<"success!";
  }
  else
    std::cout<<"pending...";
  std::cout<<std::endl;
  //}
  return rddata_status;
}
void runTest(HwInterface hw) {
  ValWord< uint32_t > test    = hw.getNode("test").read() ;
  hw.dispatch();
  std::cout<<"test:"<<test.value()<<std::endl;

  hw.getNode("test").write(0xdeadbeef) ;
  hw.dispatch();
  test    = hw.getNode("test").read() ;
  hw.dispatch();
  std::cout<<"test:"<<test.value()<<std::endl;
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
      
      HwInterface hw = connectionManager.getDevice("i2cregs.udp");
      ////test register
      //runTest(hw);
      
      std::cout<<"initial values"<<std::endl;
      readRegisters(hw);
      readCommand(hw);
      //7 bit slave: address="0000000e"  mask="007f0000"

      //<!--<node id="i2c_command"       address="0000000e"  mask="ffffffff"  permission="rw"/>
      //<!--<node id="i2c_autoclear"   address="0000000e"  mask="f0000000"  permission="rw"/>
      
      //settings
      //xxx0 0000 0000 0000
      //prescale   = 1000 = 0x3E8 //bits 0-9
      //bus select = 0x0 //bit 10
      //i2c enable = 0x1 //bit 11
      //
      //    0x       0x0B      0xE8
      //---- ---- ---0 1011 1110 1000
      //    0x       0x09      0xF4
      //---- ---- ---0 1001 1111 0100

      //i2c_wrdata;      mask=000000ff;//bits 0-7
      ////reserved bits 8-15
      //i2c_slvaddr_7b;  mask=007f0000;//bits 16-22
      //i2c_write;       mask=00800000;//bit 23
      ////reserved bit 24
      //i2c_mode16;      mask=02000000;//bit 25
      ////space bits 26-30?
      //i2c_strobe;      mask=80000000;//bit 31
      ////FPGA die temperature command
      ////    0x80    0xAA       0x00       0x01
      ////1000 0000 1010 1010 0000 0000 0000 0001
      ////Read the FPGA die (register 0x01) temperature 7 bit slave 00101010 =0x2A

      std::cout<<"enabling i2c controller"<<std::endl;
      hw.getNode("i2c_settings").write(0x00000BE8);
      hw.dispatch();
      hw.getNode("i2c_command").write(0x80AA0001);
      hw.dispatch();
      std::cout<<"\nafter first setting values"<<std::endl;
      readRegisters(hw);
      readCommand(hw);
      std::cout<<"\nfirst write::status check"<<std::endl;
      int retries = 0;
      while (readReply(hw) == 3) {
	hw.getNode("i2c_command").write(0x80AA0001);
	hw.dispatch();
	++retries;
	if (retries > 10)
	  break;
      }
      
      hw.getNode("i2c_command").write(0x802A0000);
      hw.dispatch();
      std::cout<<"\nafter second setting values"<<std::endl;
      readRegisters(hw);
      readCommand(hw);
      std::cout<<"\nfirst write::FPGA die temperature"<<std::endl;
      retries = 0;
      while (readReply(hw) == 3) {
	hw.getNode("i2c_command").write(0x802A0000);
	hw.dispatch();
	++retries;
	if (retries > 10)
	  break;
      }
      hw.getNode("i2c_settings").write(0x00000000);
      hw.dispatch();
      std::cout<<"\nafter disabling i2c controller"<<std::endl;
      readRegisters(hw);
      readCommand(hw);
      std::cout<<std::endl<<std::endl;
      
      /*
      ////    0x80    0x9A       0x00       0x01
      ////1000 0000 1001 1010 0000 0000 0000 0001
      ////Read the PCB front (register 0x00) temperature 7 bit slave 00011010 =0x1A
      hw.getNode("i2c_settings").write(0x00000BE8);
      hw.dispatch();
      hw.getNode("i2c_command").write(0x809A0001);
      hw.dispatch();
      std::cout<<"second write::status check"<<std::endl;
      readReply(hw);
      hw.getNode("i2c_command").write(0x801A0000);
      std::cout<<"second write::PCB front temperature"<<std::endl;
      readReply(hw);
      hw.getNode("i2c_settings").write(0x00000000);
      hw.dispatch();
      readRegisters(hw);
      std::cout<<std::endl<<std::endl;


      ////    0x80    0xCA       0x00       0x01
      ////1000 0000 1100 1110 0000 0000 0000 0001
      ////Read the PCB rear (register 0x00) temperature 7 bit slave 01001110 = 0x4E
      hw.getNode("i2c_settings").write(0x00000BE8);
      hw.dispatch();
      hw.getNode("i2c_command").write(0x80CA0001);
      hw.dispatch();
      std::cout<<"third write::status check"<<std::endl;
      readReply(hw);
      hw.getNode("i2c_command").write(0x804E0000);
      hw.dispatch();
      std::cout<<"third write::PCB rear temperature"<<std::endl;
      readReply(hw);
      hw.getNode("i2c_settings").write(0x00000000);
      hw.dispatch();
      readRegisters(hw);
      std::cout<<std::endl<<std::endl;
      */
    }
  catch ( const std::exception& e )
    {
      std::cout << "Something went wrong: " << e.what() <<
	std::endl;
    }
  return 0;
}
