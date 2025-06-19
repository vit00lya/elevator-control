#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include "input_reader.h"
#include "elevator_control.h"
#include "json_reader.h"

using namespace std::literals;

int main(){

  elevator_control::ElevatorControl ec;
  input_reader::InputReader ir;
  jsonreader::JsonReader jr;


  try 
    {
      jr.FilligBarcodes("barcode.json"s, ec);
    }

    catch(...){
       std::cerr << "Не возможно прочитать штрихкоды, возможно не верная кодировка файла barcode.json"s << std::endl;
       return 1;
    }
  
  std::string input_string;
  std::string barcode;
  
  while(true){

    std::cout << "Готов"s << "\n";
    std::cin >> input_string;

    try{
      barcode = ir.ParseLine(input_string);
      if(barcode == "0000000000000"){
	std::cout << "Транспортный пакет записан"s << "\n";
      }
      else{
	std::optional<std::string_view> name_product = ec.GetNameProduct(barcode);

	if(name_product.has_value()){
	  std::cout << name_product.value() << std::endl;
	  ec.AddBarcodeToSend(barcode);
	}
      }
    }
      catch(const std::exception& e){
       std::cerr << e.what() << std::endl;
    }  
  }

  return 0;
}
