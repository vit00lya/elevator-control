#pragma once
#include <unordered_map>
#include <string>
#include <string_view>
#include <deque>
#include <optional>
#include <chrono>
#include <vector>
#include <fstream>

namespace elevator_control{

  struct TransportPacket{
    long id;
    std::string time_point;
    std::vector<std::string> array_barcodes;
  };
  
    struct HasherBarcode {
    public:
        size_t operator()(const std::string_view& name) const {
            return hasher_(name);
        }

    private:
        std::hash<std::string_view> hasher_;
    };

   struct Settings{
     std::string path_exchange_file;
        };
  
  class ElevatorControl{
  public:
    
    ElevatorControl() = default;
    void AddBarcode(std::string& name_product, std::string& barcode);
    std::optional<std::string_view> GetNameProduct(std::string_view barcode);
    void AddBarcodeToSend(std::string& barcode);
    long GetTransportPacketId();
    void IncTrasportPacketId();
    bool EmptyBarcodesToSend();
    std::vector<std::string> GetBarcodesToSend();
    void SaveSettings(Settings& settings);
    Settings GetSettings();
 
    
  private:
    std::deque<std::string> names_products_;
    std::deque<std::string> barcodes_;
    std::unordered_map<std::string_view, std::string_view, HasherBarcode> barcode_map_;
    std::vector<std::string> barcodes_to_send_;
    Settings settings_;
    long transport_packet_id_ = 0;

  };
}
