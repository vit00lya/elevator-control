#pragma once

#include <string>
#include "json.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <sstream>
#include "elevator_control.h"
#include "json_builder.h"

namespace jsonreader
{
    using Node = json::Node;

    class JsonReader{
    public:
        JsonReader() = default;
        void FilligBarcodes(const std::string& path_input_json, elevator_control::ElevatorControl& ec);
        void LoadSettings(elevator_control::ElevatorControl& ec);
        std::string SaveTransportPackage(elevator_control::ElevatorControl& ec);
    };


}
