#include "elevator_control.h"

using namespace elevator_control;

/// @brief Добавляет штрихкод номенклатуры в систему
/// @param name_product Название номенклатуры
/// @param barcode Штрихкод номенклатуры
void ElevatorControl::AddBarcode(std::string& name_product, std::string& barcode){
       names_products_.push_back(std::move(name_product));
       barcodes_.push_back(std::move(barcode));
       std::string_view name_product_sv = barcodes_.back();
       std::string_view barcode_sv      = names_products_.back();
       barcode_map_[name_product_sv] = barcode_sv;
}

/// @brief Добавляет штрихкод в очередь для отправки
/// @param barcode Штрихкод для отправки
void ElevatorControl::AddBarcodeToSend(std::string& barcode){
     barcodes_to_send_.push_back(std::move(barcode));
}

/// @brief Возвращает идентификатор транспортного пакета
/// @return Идентификатор транспортного пакета
long ElevatorControl::GetTransportPacketId(){
  return transport_packet_id_;
}

/// @brief Увеличивает идентификатор транспортного пакета на 1
void ElevatorControl::IncTrasportPacketId(){
  ++transport_packet_id_;
}

/// @brief Проверяет, пуста ли очередь штрихкодов для отправки
/// @return true, если очередь пуста, иначе false
bool ElevatorControl::EmptyBarcodesToSend(){
  return barcodes_to_send_.empty();
}

std::vector<std::string> ElevatorControl::GetBarcodesToSend(){
  return barcodes_to_send_;
}


void ElevatorControl::SaveSettings(Settings& settings){
  settings_ = std::move(settings);
}

Settings ElevatorControl::GetSettings(){
  return settings_;
}

/// @brief Возвращает название продукта по штрихкоду
/// @param barcode Штрихкод продукта
/// @return Название продукта, если штрихкод найден, иначе пустой optional
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






