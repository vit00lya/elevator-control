#include <iostream>
#include <string>
#include "input_reader.h"
#include "elevator_control.h"

using namespace std::literals;

int main(){

  elevator_control::ElevatorControl ec;
  input_reader::InputReader ir;
  std::string input_string;
  
  while(true){
    
    std::cin >> input_string;
    
    try{
      std::cout << ir.ParseLine(input_string) << std::endl;
    }
    catch(const std::exception& e){
       std::cerr << e.what() << std::endl;
    }
    
  }

  return 0;
}
