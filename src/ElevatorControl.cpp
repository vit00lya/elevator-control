#include "ElevatorControl.h"

void elevator_control::ElevatorControl::AddBarcode(std::string& name_product, std::string& barcode){
       names_products_.push_back(std::move(name_product));
       barcodes_.push_back(std::move(barcode));
       std::string_view name_product_sv = barcodes_.back();
       std::string_view barcode_sv      = names_products_.back();
       barcode_map_[name_product_sv] = barcode_sv;
}

std::optional<std::string_view> elevator_control::ElevatorControl::GetName(std::string_view barcode){

  if (barcode.empty()) {
    return {};
  }
  
  auto it = barcode_map_.find(barcode);
  if (it != barcode_map_.end()){
    return it->second;
  }
  
  return {};
}





