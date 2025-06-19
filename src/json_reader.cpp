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


void JsonReader::SaveTransportPackage(elevator_control::ElevatorControl ec){

   using namespace std::literals;
  std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
  time_t now_t = std::chrono::system_clock::to_time_t(time);

  std::ofstream out;
  out.open("tranport_package_" + std::to_string(ec.GetTransportPacketId()) + "_" + std::to_string(now_t) + ".txt");

  elevator_control::TransportPacket tp;
  
  tp.id = ec.GetTransportPacketId();
  tp.time_point = std::to_string(now_t);
  tp.array_barcodes = std::move(ec.GetBarcodesToSend());

  // auto result = json::Builder{}.StartDict() 
  //   .Key("request_id"s).Value(elem.AsMap().at("id").AsInt())
  //   .Key("map"s).Value(os.str())
  //   .EndDict().Build();

  //  json::Document doc = json::Document(result);
  //  json::Print(doc, out);
  
}
