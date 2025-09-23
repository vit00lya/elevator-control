#include "json_reader.h"

using namespace jsonreader;

void JsonReader::FilligBarcodes(elevator_control::ElevatorControl& ec){
      using namespace std::literals;

      std::ifstream input_json;
      input_json.open(ec.GetSettings().path_exchange_file, std::ios::binary);
      
      json::Document doc = json::Load(input_json);
      json::Array arr = doc.GetRoot().AsArray();
      std::string barcode;
      std::string name_product;
      
      for (const auto& elem: arr){
	for(const auto& [key, value]: elem.AsMap()){
	  if(key == "barcode"s){
	    barcode = value.AsString();
	  }
	  if(key == "name_product"s){
	    name_product = value.AsString();
	  }
      }
	ec.AddBarcode(name_product, barcode);
	
      }
}

void JsonReader::LoadSettings(elevator_control::ElevatorControl& ec){
      using namespace std::literals;

      std::ifstream input_json;
      try{
	input_json.open("settings.json", std::ios::binary);
	if (!input_json.good()) {
	   std::cerr << "Не возможно прочитать файл настроек settings.json" << std::endl;
	   throw ;
	}
      }
      catch(...){
	std::cerr << "Не возможно прочитать файл настроек settings.json" << std::endl;
	throw ;
      } 
      
      elevator_control::Settings settings;
      
      json::Document doc = json::Load(input_json);
      json::Dict dict = doc.GetRoot().AsMap();
	for(const auto& [key, value]: dict){
	  if(key == "path_exchange_file"s){
	    settings.path_exchange_file = value.AsString();
	  }
	   else if(key == "scanner_enable"){
	     settings.scanner_enable = value.AsBool();
	  }
	  else if(key == "scanner_num_com_port"){
	    settings.scanner_num_com_port = value.AsInt();
	  }
	  else if(key == "scanner_baud_rate"){
	    settings.scanner_baud_rate = value.AsInt();
	  }
	  else if(key == "scanner_parity"){
	    settings.scanner_parity = value.AsString();
	  }
	  else if(key == "scanner_data_bits"){
	    settings.scanner_data_bits = value.AsInt();
	  }
	  else if(key == "scanner_stop_bits"){
	    settings.scanner_stop_bits = value.AsString();
	  }
	  else if(key == "scanner_linux_com_port"){
	    settings.scanner_linux_com_port = value.AsString();
	  }
	  else if(key == "display_width"){
	    settings.display_width = value.AsInt();
	  }
	  else if(key == "display_height"){
	    settings.display_height = value.AsInt();
	  }
	  else if(key == "pin_reset"){
	    settings.pin_reset = value.AsInt();
	  }
	  else if(key == "pin_rs"){
	    settings.pin_rs = value.AsInt();
	  }
	  else if(key == "pin_en"){
	    settings.pin_en = value.AsInt();
	  }
	  else if(key == "pin_cs1"){
	    settings.pin_cs1 = value.AsInt();
	  }
	  else if(key == "pin_cs2"){
	    settings.pin_cs2 = value.AsInt();
	  }
	  else if(key == "pin_d0"){
	    settings.pin_d0 = value.AsInt();
	  }
	  else if(key == "pin_d1"){
	    settings.pin_d1 = value.AsInt();
	  }
	  else if(key == "pin_d2"){
	    settings.pin_d2 = value.AsInt();
	  }
	  else if(key == "pin_d3"){
	    settings.pin_d3 = value.AsInt();
	  }
	  else if(key == "pin_d4"){
	    settings.pin_d4 = value.AsInt();
	  }
	  else if(key == "pin_d5"){
	    settings.pin_d5 = value.AsInt();
	  }
	  else if(key == "pin_d6"){
	    settings.pin_d6 = value.AsInt();
	  }
	  else if(key == "pin_d7"){
	    settings.pin_d7 = value.AsInt();
	  }
	  
      }
      ec.SaveSettings(settings);	
}

// Возвращает имя создаваемого пакета
std::string JsonReader::SaveTransportPackage(elevator_control::ElevatorControl& ec){ 
 
  using namespace std::literals;
  std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
  time_t now_t = std::chrono::system_clock::to_time_t(time);

  std::ofstream out;
  out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  auto name_file = "tranport_package_" + std::to_string(ec.GetTransportPacketId()) + "_" + std::to_string(now_t) + ".json";
  out.open(name_file);

  // Заполнение пакета
  elevator_control::TransportPacket tp;
  
  tp.id = ec.GetTransportPacketId();
  tp.time_point = std::to_string(now_t);
  tp.array_barcodes = std::move(ec.GetBarcodesToSend());

  std::vector<Node> tmp_v {tp.array_barcodes.begin(), tp.array_barcodes.end()};

  //Создание JSON файла
   auto result = json::Builder{}.StartDict() 
     .Key("id"s).Value(tp.id)
     .Key("time_point"s).Value(tp.time_point)
     .Key("array_barcodes").Value(tmp_v).EndDict().Build(); 

    json::Document doc = json::Document(result);
    json::Print(doc, out);

    ec.IncTrasportPacketId();

    return name_file;
    
}
