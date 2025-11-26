#pragma once
#include <unordered_map>
#include <string>
#include <string_view>
#include <deque>
#include <optional>
#include <chrono>
#include <vector>
#include <fstream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include <filesystem>
#include <regex>
#include "network_client.h"

namespace elevator_control{

  struct TransportPacket{
    std::string id;
    std::string dev_id;
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
     long device_id = 0;
     std::string server_address = "";
     std::string userpassword = "";
     bool scanner_enable = false;
     int scanner_num_com_port = 0;
     int scanner_baud_rate = 9600;
     std::string scanner_parity = "";
     int scanner_data_bits = 8;
     std::string scanner_stop_bits = "";
     std::string scanner_linux_com_port = "";
     int display_width = 128;
     int display_height = 64;
     int display_pin_rs = 0;
     int display_pin_en = 0;
     int display_pin_reset = 0;
     int display_pin_cs1 = 0;
     int display_pin_cs2 = 0;
     int display_pin_d0 = 0;
     int display_pin_d1 = 0;
     int display_pin_d2 = 0;
     int display_pin_d3 = 0;
     int display_pin_d4 = 0;
     int display_pin_d5 = 0;
     int display_pin_d6 = 0;
     int display_pin_d7 = 0;
     int display_pin_led = 0;
        };
  
  class ElevatorControl{
  public:
    
    ElevatorControl() = default;
    void AddBarcode(std::string& name_product, std::string& barcode);
    std::optional<std::string_view> GetNameProduct(std::string_view barcode);
    void AddBarcodeToSend(std::string& input_string);
    long GetTransportPacketId();
    void IncTrasportPacketId();
    bool EmptyBarcodesToSend();
    std::vector<std::string> GetBarcodesToSend();
    void SaveSettings(Settings& settings);
    Settings GetSettings();
    void ClearBarcodeToSend();
    void SendTransportPackage_RoutineAssignment();
    void ReadAndDeleteFilesByMask(const std::string& mask);
 
    
  private:
    std::thread background_thread_sender_; 
    std::atomic<bool> stop_flag_sender_{false}; 
    std::deque<std::string> names_products_;
    std::deque<std::string> barcodes_;
    std::unordered_map<std::string_view, std::string_view, HasherBarcode> barcode_map_;
    std::vector<std::string> barcodes_to_send_;
    Settings settings_;
    long transport_packet_id_ = 0;
  };
}
