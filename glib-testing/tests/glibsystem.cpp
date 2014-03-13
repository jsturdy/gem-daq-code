#include "glib.h"
#include "iostream"
int main()
{
  GLIB obj;
  //obj.test();
  //obj.FetchBoardName();
  std::cout<<obj.GetSystemDetails()<<std::endl;
  std::cout<<obj.GetBoardDetails()<<std::endl;
  
  obj.FetchFirmWare();
  std::cout<<obj.GetFirmwareVersion()<< " : "<<obj.GetFirmwareDate()<<std::endl;
}

