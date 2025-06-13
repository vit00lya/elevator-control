#pragma once
#include <vector>

namespace transport_package{

  struct TransportPackage{
    long id;
    std::chrono::time_point<std::chrono::system_clock> time_point;
    std::vector<std::string> array_barcodes;
  };

  void Save();
  void Send();
  void Load();
  void Delete();
  
}
