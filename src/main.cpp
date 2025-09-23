#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include "input_reader.h"
#include "elevator_control.h"
#include "json_reader.h"
#include "xserial.hpp" 
#include "magic_enum.hpp"

#if EXTERNAL_DISPLAY
#include "wiringRP.h"
#include "logo.h"
#include "devices/ks0108.h"
#include "devices/fonts/System5x7R.h"
#endif

using namespace std::literals;

void print_display_text(const std::wstring& str){
  clearScreen();
  writeString(0,5,str.c_str(),System5x7R);
  syncBuffer();
}

void init_display(elevator_control::Settings& settings){
    if(setupWiringRP(WRP_MODE_PHYS) < 0)
         exit(EXIT_FAILURE);

    init(settings.display_width,
	 settings.display_height,
	 settings.pin_reset,
	 settings.pin_rs,
	 settings.pin_en,
	 settings.pin_cs1,
	 settings.pin_cs2,
	 settings.pin_d0,
	 settings.pin_d1,
	 settings.pin_d2,
	 settings.pin_d3,
	 settings.pin_d4,
	 settings.pin_d5,
	 settings.pin_d6,
	 settings.pin_d7
	 );
    drawBitmap(logo,128,64);
     shiftBufferHorizontal(30);
     syncBuffer();
     delay(10000);
     clearScreen();
}

int main(){

  elevator_control::ElevatorControl ec;
  input_reader::InputReader         ir;
  jsonreader::JsonReader            jr;
  xserial::ComPort                 scanner; 
  
  jr.LoadSettings(ec);
  
  elevator_control::Settings settings;
  settings = ec.GetSettings();

  #if EXTERNAL_DISPLAY
    init_display(settings);
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
     std::cerr << "Не возможно прочитать штрихкоды, возможно не верная кодировка файла barcode.json"s << std::endl;
     return 1;
  }
  
  std::string input_string;
  std::string barcode;
  
  while(true){

    #if EXTERNAL_DISPLAY
      print_display_text(L"Готов");
    #endif
    std::cout << "Готов"s << "\n";
    input_string = "";
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
      if(barcode == "9999999999999"s){
	if(ec.EmptyBarcodesToSend()){
	    #if EXTERNAL_DISPLAY
	     print_display_text(L"Список пуст, нечего отправлять.");
            #endif
	  std::cout << "Список пуст, нечего отправлять."s <<  "\n";
	  continue;
	}
	try
	  {
           std::string name_pack = jr.SaveTransportPackage(ec);
	    // #if EXTERNAL_DISPLAY
	    //  print_display_text(L"Транспортный пакет записан. Имя пакета:" + name_pack);
            // #endif
	   std::cout << "Транспортный пакет записан. Имя пакета:"s << name_pack << "\n";
	  }
	catch(...){
	    #if EXTERNAL_DISPLAY
	     print_display_text(L"Ошибка при записи транспортного пакета в файл");
            #endif
	   std::cerr << "Ошибка при записи транспортного пакета в файл"s << std::endl;
	}
      }
      else{
	std::optional<std::string_view> name_product = ec.GetNameProduct(barcode);

	if(name_product.has_value()){
	  std::cout << name_product.value() << std::endl;
	}
	else{
	  #if EXTERNAL_DISPLAY
	     print_display_text(L"Неопознанный штрихкод");
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
