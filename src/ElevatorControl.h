#include "ComPortTools.h"
#include <iostream>

int main(){
    
  ComPortTools com = ComPortTools();
  com.InitPort(0,9600,8,"no","one",5,"ttyUSB");
  std::cout << com.GetLine() << std::endl;
  return 0;
}
