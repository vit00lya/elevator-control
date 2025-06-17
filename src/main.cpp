#include <iostream>
#include <string>
#include <string_view>
#include <optional>
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
      std::optional<std::string_view> name_product = ir.ParseLine(input_string);

      if(name_product.has_value()){
	std::cout << name_product.value() << std::endl;
      }

    }
      catch(const std::exception& e){
       std::cerr << e.what() << std::endl;
    }
  
    
  }

  return 0;
}
