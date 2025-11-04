#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include "input_reader.h"
#include "elevator_control.h"
#include "json_reader.h"
#include "xserial.hpp" 
#include "magic_enum.hpp"

using namespace std::literals;

#if EXTERNAL_DISPLAY
  #include "display.h"
#endif

int main(){

  elevator_control::ElevatorControl ec;
  input_reader::InputReader         ir;
  jsonreader::JsonReader            jr;
  xserial::ComPort                 scanner; 
  
  jr.LoadSettings(ec);
  
  elevator_control::Settings settings;
  settings = ec.GetSettings();

  #if EXTERNAL_DISPLAY
    InitDisplay(settings);
    signal(SIGINT, ReleaseWiringRP);
  #endif
  
  if(settings.scanner_enable){
    if (!scanner.open((short)settings.scanner_num_com_port,
			     (long)settings.scanner_baud_rate,
			     magic_enum::enum_cast<xserial::ComPort::eParity>(settings.scanner_parity).value(),
			     (long)settings.scanner_data_bits,
   		             magic_enum::enum_cast<xserial::ComPort::eStopBit>(settings.scanner_stop_bits).value(),
			     0,
		      settings.scanner_linux_com_port)){
       return 1;
    }
  }
  
  try{
    jr.FilligBarcodes(ec);
  }

  catch(...){
    #if EXTERNAL_DISPLAY
       PrintDisplayText(L"Не возможно прочитать штрихкоды, возможно не верная кодировка файла barcode.json", false);
    #endif
     std::cerr << "Не возможно прочитать штрихкоды, возможно не верная кодировка файла barcode.json"s << std::endl;
     return 1;
  }
  
  std::string input_string;
  std::string barcode;
  
  while(true){

    #if EXTERNAL_DISPLAY
       PrintDisplayText(L"Введите штрихкод", false);
    #endif
    std::cout << "Введите штрихкод"s << "\n";
    input_string = ""s;
    if(settings.scanner_enable){
      input_string = scanner.getLine();
      scanner.flushRxAndTx();
    }
    else
    {
      std::cin >> input_string;
    }

    try{
      barcode = ir.ParseLine(input_string);
      if(barcode == "9999999999999"sv){
	if(ec.EmptyBarcodesToSend()){
	    #if EXTERNAL_DISPLAY
	     PrintDisplayText(L"Список пуст, нечего отправлять.");
            #endif
	  std::cout << "Список пуст, нечего отправлять."s <<  "\n";
	  continue;
	}
	try
	  {
           std::string name_pack = jr.SaveTransportPackage(ec);
	     #if EXTERNAL_DISPLAY
	      print_display_text(L"Транспортный пакет записан.");
             #endif
	   std::cout << "Транспортный пакет записан. Имя пакета:"s << name_pack << "\n";
	  }
	catch(...){
	    #if EXTERNAL_DISPLAY
	     PrintDisplayText(L"Ошибка при записи транспортного пакета в файл");
            #endif
	   std::cerr << "Ошибка при записи транспортного пакета в файл"s << std::endl;
	}
      }
      else{
	std::optional<std::string_view> name_product = ec.GetNameProduct(barcode);

	if(name_product.has_value()){
	  #if EXTERNAL_DISPLAY
	    std::string tmp_string = std::string(name_product.value());
	    auto text_wstring = Utf8ToWchar(tmp_string.c_str());
	    PrintDisplayText(text_wstring.c_str());
          #endif
	  std::cout << name_product.value() << std::endl;
	}
	else{
	  #if EXTERNAL_DISPLAY
	     PrintDisplayText(L"Неопознанный штрихкод");
          #endif
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
