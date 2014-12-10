#include <cstdlib>
#include "classTest.h"
#include <sstream>
#include <string>

TEST::TEST(){
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
  manager = new ConnectionManager( connectionPath );
  //*hw=manager->getDevice ( "gemsupervisor.udp.0" );

}

TEST::~TEST(){
  FetchFirmWare();
  delete manager;
}

/*
  TEST::TEST(char *connectionFile)
  {
  //    manager = new ConnectionManager(connectionFile);
  //    hw = manager->getDevice("dummy.udp.0");
  }
*/

void TEST::FetchFirmWare()
{
  HwInterface hw2=manager->getDevice ( "dummy.udp.0" );
  //*hw=manager->getDevice ( "dummy.udp.0" );
  char regName[12];
  for(int i=1 ; i<=6 ; i++)
    {
      sprintf(regName,"firmware_REG_%d",i);
      mem = hw2.getNode ( regName ).read();
      hw2.dispatch();
      char *pEnd;
      char firmChar[1];
      sprintf(firmChar,"%d",int(mem.value()));
      long int li1;
      li1 = strtol (firmChar,&pEnd,16);
      if(i==1)
	_firmWare_Id_Ver_MAJOR=li1;

      if(i==2)
	_firmWare_Id_Ver_MINOR=li1;

      if(i==3)
	_firmWare_Id_Ver_BUILD=li1;

      if(i==4)
	_firmWare_Id_YY=li1;

      if(i==5)
	_firmWare_Id_MM=li1;

      if(i==6)
	_firmWare_Id_DD=li1;
    }

  char version[20]="";
  char frmDat[20]="";
  sprintf(frmDat,"%x/%x/%x",_firmWare_Id_DD,_firmWare_Id_MM,_firmWare_Id_YY);
  sprintf(version,"%x.%x.%x",_firmWare_Id_Ver_MAJOR,_firmWare_Id_Ver_MINOR,_firmWare_Id_Ver_BUILD);
  std::string strVer(version);
  std::string strDate(frmDat);
  _firmWareVersion.append(strVer);
  _firmWareDate.append(strDate);
  std::cout<<"FW::ver:"<<_firmWareVersion
	   <<"  date:"<<_firmWareDate
	   <<std::endl;
}

uint32_t TEST::getTestReg()
{
  HwInterface hw2=manager->getDevice ( "gemsupervisor.udp.0" );
  uint32_t testReg_;
  try {
    r_test = hw2.getNode ( "test" ).read();
    hw2.dispatch();
    testReg_ = r_test.value();
  }
  catch (const std::exception& e) {
    std::cout << "Something went wrong reading the test register: " << e.what() << std::endl;
  }
  return testReg_;
  
}

uint32_t TEST::setTestReg(uint32_t setVal)
{
  HwInterface hw2=manager->getDevice ( "gemsupervisor.udp.0" );
  uint32_t testReg_ = setVal;
  
  try {
    hw2.getNode ( "test" ).write(testReg_);
    hw2.dispatch();
  }
  catch (const std::exception& e) {
    std::cout << "Something went wrong writing the test register: " << e.what() << std::endl;
  }
  
  try {
    r_test = hw2.getNode ( "test" ).read();
    hw2.dispatch();
    testReg_ = r_test.value();
  }
  catch (const std::exception& e) {
    std::cout << "Something went wrong reading the test register: " << e.what() << std::endl;
  }
  return testReg_;

}
