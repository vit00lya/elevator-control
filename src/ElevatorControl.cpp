#include "ElevatorControl.h"

void elevator_control::ElevatorControl::AddBarcode(std::string& name_product, std::string& barcode){
       names_products_.push_back(std::move(name_product));
       barcodes_.push_back(std::move(barcode));
       std::string_view name_product_sv = barcodes_.back();
       std::string_view barcode_sv      = names_products_.back();
       barcode_map_[name_product_sv] = barcode_sv;
}

void elevator_control::ElevatorControl::Init(std::istream& input_json){

      using namespace std::literals;
      json::Document doc = json::Load(input_json);
      json::Dict map = doc.GetRoot().AsMap();

      for(const auto& [key, value]: map){
        

      }

}




