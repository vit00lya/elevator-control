#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include "input_reader.h"
#include "elevator_control.h"
#include "json_reader.h"
#include "xserial.hpp" 
#include "magic_enum.hpp" 

using namespace std::literals;

int main(){

  elevator_control::ElevatorControl ec;
  input_reader::InputReader         ir;
  jsonreader::JsonReader            jr;
  
  jr.LoadSettings(ec);

  elevator_control::Settings settings;
  settings = ec.GetSettings();
    
  if(settings.scanner_enable){
    xserial::ComPort scanner((short)settings.scanner_num_com_port,
			     (long)settings.scanner_baud_rate,
			     magic_enum::enum_cast<xserial::ComPort::eParity>(settings.scanner_parity).value(),
			     (long)settings.scanner_data_bits,
   		             magic_enum::enum_cast<xserial::ComPort::eStopBit>(settings.scanner_stop_bits).value(),
			     0,
			     settings.scanner_linux_com_port); 
  }
  
  try{
    jr.FilligBarcodes(ec);
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
      if(barcode == "9999999999999"s){
	if(ec.EmptyBarcodesToSend()){
	  std::cout << "Список пуст, нечего отправлять."s <<  "\n";
	  continue;
	}
	try
	  {
           std::string name_pack = jr.SaveTransportPackage(ec);
	   std::cout << "Транспортный пакет записан. Имя пакета:"s << name_pack << "\n";
	  }
	catch(...){ 
	   std::cerr << "Ошибка при записи транспортного пакета в файл"s << std::endl;
	}
      }
      else{
	std::optional<std::string_view> name_product = ec.GetNameProduct(barcode);

	if(name_product.has_value()){
	  std::cout << name_product.value() << std::endl;
	}
	else{
	  std::cout << "Неопознанный штрихкод"s << std::endl;
	}
	 ec.AddBarcodeToSend(barcode);
      }
    }
      catch(const std::exception& e){
       std::cerr << e.what() << std::endl;
    }  
  }

  return 0;
}
