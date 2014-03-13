#include <cstdlib>
#include "glib.h"
#include <sstream>
#include <string>

GLIB::GLIB(){
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
}

GLIB::~GLIB(){
  delete manager;
}

/*
  GLIB::GLIB(char *connectionFile)
  {
  //    manager = new ConnectionManager(connectionFile);
  //    hw = manager->getDevice("dummy.udp.0");
  }
*/

std::string GLIB::GetBoardDetails()
{
  FetchBoardName();	
  return _boardName;
}


std::string GLIB::GetSystemDetails()
{
  FetchSystemName();
  return _systemName;
}

std::string GLIB::GetFirmwareDate()
{
  return _firmWareDate; 
}

std::string GLIB::GetFirmwareVersion()
{
  return _firmWareVersion;
}

std::string GLIB::GetFirmwareDetails()
{
  //    FetchFirmWare();
  // return _firmWare;
}

void GLIB::FetchFirmWare()
{
  HwInterface hw=manager->getDevice ( "dummy.udp.0" );
  char regName[12];
  for(int i=1 ; i<=6 ; i++)
    {
      sprintf(regName,"firmware_REG_%d",i);
      mem = hw.getNode ( regName ).read();
      hw.dispatch();
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
}

void GLIB::FetchBoardName()
{
  HwInterface hw=manager->getDevice ( "dummy.udp.0" );
  char regName[12];
  for(int i=1 ; i<=4 ; i++)
    {
      sprintf(regName,"board_REG_%d",i);
      mem = hw.getNode ( regName ).read();
      hw.dispatch();
      std::stringstream ss;
      std::string s;
      char c = char(mem.value());

      ss << c;
      ss >> s;
      _boardName.append(s);
    }
}

void GLIB::FetchSystemName()
{
  HwInterface hw=manager->getDevice ( "dummy.udp.0" );
  char regName[12];
  for(int i=1 ; i<=4 ; i++)
    {
      sprintf(regName,"system_REG_%d",i);
      mem = hw.getNode ( regName ).read();
      hw.dispatch();
      std::stringstream ss;
      std::string s;
      char c = char(mem.value());
      ss << c;
      ss >> s;
      _systemName.append(s);
    }
}

//Added later
std::string GLIB::ToHexString(unsigned int val)
{
  char hexString[12];
  sprintf(hexString,"0x%x",val);
  std::string result(hexString);
  return result;
}

std::string GLIB::GetAddress(unsigned int val)
{
  return ToHexString(val);
}

std::string GLIB::GetMask(unsigned int val)
{
  return ToHexString(val);
}


void GLIB::test()
{
  HwInterface hw=manager->getDevice ( "dummy.udp.0" );
  int i=0;

  std::ifstream myReadFile;
  myReadFile.open("registers.txt");

  char output[100];

  int numOfLines=3;
  int lineNum=0;
  int forloop=4;
  if (myReadFile.is_open()) {
    while (numOfLines) {
      numOfLines--;
      lineNum++;
      if(lineNum==3)
	forloop=6;
      myReadFile >> output;

      std::cout<<output<<":";

      for( i=1;i<=forloop;i++)
	{
	  char regName[6];
	  sprintf(regName,"%s_REG_%d",output,i);
	  mem = hw.getNode ( regName ).read();
	  hw.dispatch();

	  if(lineNum==3)
	    std::cout <<  int(mem.value())<<".";
	  else
	    std::cout <<  char(mem.value())<<".";
	}
      std::cout<<std::endl;
    }
  }
  myReadFile.close();
  std::cout<<std::endl;
  std::cout<<"End of Test function"<<std::endl;
}

