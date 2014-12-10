#include "glib.h"
#include <sstream>
#include <string>

//Just to create single instance of ConnectionManager and HwInterface
//Require singleton implementation.
ConnectionManager *manager=new ConnectionManager("file://myconnections.xml");
HwInterface hw=manager->getDevice ( "dummy.udp.0" );

GLIB::GLIB(){
  //manager=new ConnectionManager("file://myconnections.xml");
  //hw=manager->getDevice ( "dummy.udp.0" );
}

GLIB::~GLIB(){
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
  //  FetchFirmWare();
  return _firmWareDate; 
}

std::string GLIB::GetFirmwareVersion()
{
  //    FetchFirmWare();
  return _firmWareVersion;
}

std::string GLIB::GetFirmwareDetails()
{
  //    FetchFirmWare();
  // return _firmWare;
}

void GLIB::FetchFirmWare()
{
  ValWord< uint32_t > mem;
  char regName[12];
  for(int i=1 ; i<=6 ; i++)
    {
      sprintf(regName,"firmware_REG_%d",i);
      mem = hw.getNode ( regName ).read();
      hw.dispatch();
      //std::cout<<mem.value()<<" . ";
      char *pEnd;
      char firmChar[1];
      sprintf(firmChar,"%d",int(mem.value()));
      long int li1;
      li1 = strtol (firmChar,&pEnd,16);
      //std::cout<<li1<<" . ";
      //int c = int(mem.value());
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

      //std::cout<<li1<<" . ";
    }
  //std::cout<<_firmWare_Id_Ver_MAJOR<<" . "<<_firmWare_Id_Ver_MINOR<<" . "<<_firmWare_Id_Ver_BUILD<<" . "<<_firmWare_Id_YY<<" . "<<_firmWare_Id_MM << " . "<<_firmWare_Id_DD<<std::endl;


  char version[20]="";
  char frmDat[20]="";
  //char *frmDat;
  sprintf(frmDat,"%x/%x/%x",_firmWare_Id_DD,_firmWare_Id_MM,_firmWare_Id_YY);
  sprintf(version,"%x.%x.%x",_firmWare_Id_Ver_MAJOR,_firmWare_Id_Ver_MINOR,_firmWare_Id_Ver_BUILD);
  std::string strVer(version);
  std::string strDate(frmDat);
  _firmWareVersion.append(strVer);
  _firmWareDate.append(strDate);
  //std::cout<<frmDat<<std::endl;
  //std::cout<<_firmWareVersion<<" : "<<_firmWareDate<<std::endl;
  //std::cout<<_firmWare_Id_Ver_MAJOR<<" . "<<_firmWare_Id_Ver_MINOR<<" . "<<_firmWare_Id_Ver_BUILD<<" . "<<_firmWare_Id_YY<<" . "<<_firmWare_Id_MM << " . "<<_firmWare_Id_DD<<std::endl;
  //std::cout<<std::endl;
}

void GLIB::FetchBoardName()
{
  //HwInterface hw=manager->getDevice ( "dummy.udp.0" );
  ValWord< uint32_t > mem;
  //std::stringstream ss;
  char regName[12];
  for(int i=1 ; i<=4 ; i++)
    {
      sprintf(regName,"board_REG_%d",i);
      mem = hw.getNode ( regName ).read();
      hw.dispatch();
      //std::strcat(_boardName,char(mem.value()));
      std::stringstream ss;
      std::string s;
      char c = char(mem.value());

      //std::cout<<c<<".";
      ss << c;
      ss >> s;
      //std::cout<<s;
      _boardName.append(s);
      //std::cout <<  char(mem.value());
    }
  //std::cout<<std::endl;

}

void GLIB::FetchSystemName()
{
  //HwInterface hw=manager->getDevice ( "dummy.udp.0" );
  ValWord< uint32_t > mem;
  //std::stringstream ss;
  char regName[12];
  for(int i=1 ; i<=4 ; i++)
    {
      sprintf(regName,"system_REG_%d",i);
      mem = hw.getNode ( regName ).read();
      hw.dispatch();
      //std::strcat(_boardName,char(mem.value()));
      std::stringstream ss;
      std::string s;
      char c = char(mem.value());
      //std::cout<<c;
      ss << c;
      ss >> s;
      //std::cout<<s;
      _systemName.append(s);
      //std::cout <<  char(mem.value());
    }
  //std::cout<<std::endl;

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
  //ConnectionManager manager ( "file://myconnections.xml" );
  //manager=new ConnectionManager("file://myconnections.xml");
  HwInterface hw=manager->getDevice ( "dummy.udp.0" );
  ValWord< uint32_t > mem;
  int i=0; //,j=0;

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
	  //std::cout<<regName<<std::endl;
	  mem = hw.getNode ( regName ).read();
	  hw.dispatch();

	  //std::cout << "REG = " << char(mem.value()) << std::endl;
	  if(lineNum==3)
	    std::cout <<  int(mem.value())<<".";// << std::endl;
	  else
	    std::cout <<  char(mem.value())<<".";// << std::endl;
	}
      std::cout<<std::endl;
    }
  }
  myReadFile.close();
  std::cout<<std::endl;
  std::cout<<"End of Test function"<<std::endl;
}

