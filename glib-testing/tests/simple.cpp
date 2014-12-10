#include "classTest.h"
#include "iostream"

int main()
{
  std::cout<<"starting test code sturdy"<<std::endl;
  TEST obj;
  std::cout<<"current value of test " << obj.getTestReg() << std::endl;
  std::cout<<"setting value of test to 5 (" << obj.setTestReg(5) << ")" << std::endl;
}

