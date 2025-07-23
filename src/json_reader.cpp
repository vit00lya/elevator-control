#include "json_reader.h"

using namespace jsonreader;

void JsonReader::FilligBarcodes(const std::string& path_input_json, elevator_control::ElevatorControl& ec){
      using namespace std::literals;

      std::ifstream input_json;
      input_json.open(path_input_json, std::ios::binary);
      
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
      }
      catch(...){
	
	throw "Не возможно прочитать файл настроек settings.json"s;

      } 
      
      elevator_control::Settings setiings;
      
      json::Document doc = json::Load(input_json);
      json::Dict dict = doc.GetRoot().AsMap();
	for(const auto& [key, value]: dict.AsMap()){
	  if(key == "path_exchange_file"s){
	    setiings.path_exchange_file = value.AsString();
	  }
      }

     ec.settings
	
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

    return name_file;
    
}
