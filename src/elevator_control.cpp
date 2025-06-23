#include "elevator_control.h"

using namespace elevator_control;

void ElevatorControl::AddBarcode(std::string& name_product, std::string& barcode){
       names_products_.push_back(std::move(name_product));
       barcodes_.push_back(std::move(barcode));
       std::string_view name_product_sv = barcodes_.back();
       std::string_view barcode_sv      = names_products_.back();
       barcode_map_[name_product_sv] = barcode_sv;
}

void ElevatorControl::AddBarcodeToSend(std::string& barcode){
     barcodes_to_send_.push_back(std::move(barcode));
}

long ElevatorControl::GetTransportPacketId(){
  return transport_packet_id_;
}

void ElevatorControl::IncTrasportPacketId(){
  ++transport_packet_id_; 
}

bool ElevatorControl::EmptyBarcodesToSend(){
  return barcodes_to_send_.empty();
}

std::vector<std::string> ElevatorControl::GetBarcodesToSend(){
  return barcodes_to_send_;
}

std::optional<std::string_view> ElevatorControl::GetNameProduct(std::string_view barcode){

  if (barcode.empty()) {
    return {};
  }
  
  auto it = barcode_map_.find(barcode);
  if (it != barcode_map_.end()){
    return it->second;
  }
  
  return {};
}





