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
#include <codecvt>
#include <locale>
#include <algorithm>
#include "wiringRP.h"
#include "logo.h"
#include "devices/ks0108.h"
#include "devices/fonts/System5x7R.h"

static const size_t MAX_LINES = 6;
static const size_t HEIGHT_LINE = 10;
static const size_t MAX_CHARS_PER_LINE = 21;

std::wstring Utf8ToWchar(const char* utf8_str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8_str);
}

void PrintLines(std::vector<std::wstring>& lines){
  for (size_t i = 0; i < lines.size() && i < MAX_LINES; ++i){
    writeString(0, 5 + HEIGHT_LINE * i, lines[i].c_str(), System5x7R);
  }
}

std::vector<std::wstring> GetStrings(const std::wstring& str){

  std::vector<std::wstring> result;
  
  if(str.size() < MAX_CHARS_PER_LINE){
    result.push_back(str);
  }
  else{

    size_t iter_count = str.size() /  MAX_CHARS_PER_LINE + 1;
    size_t remaining_characters = str.size();
    for(size_t i = 0; i < iter_count; ++i) {
      size_t end_line = std::min(MAX_CHARS_PER_LINE,remaining_characters);
      result.push_back(str.substr(MAX_CHARS_PER_LINE * i, end_line));
      remaining_characters -= MAX_CHARS_PER_LINE - 1;
      
    }
  }
  return result;
}

void PrintDisplayText(const std::wstring& str){
  
  clearScreen();
  std::vector<std::wstring> lines = GetStrings(str);
  PrintLines(lines);
  syncBuffer();
  delay(5000);
}

void InitDisplay(elevator_control::Settings& settings){
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
     delay(5000);
     clearScreen();
}

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
      PrintDisplayText(L"Введите штрихкод");
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
	    // #if EXTERNAL_DISPLAY
	    //  print_display_text(L"Транспортный пакет записан. Имя пакета:" + name_pack);
            // #endif
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
	  // std::wcout << text_wstring <<  std::endl;
	   PrintDisplayText(text_wstring.c_str());
	  // std::wstring st = L"Пл";
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
