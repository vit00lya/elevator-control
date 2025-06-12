#pragma once
#include <string>
#include <string_view>
#include <stdexcept>

namespace input_reader{

    class InputReader {
    public:
      InputReader() = default;
      std::string ParseLine(std::string_view line);
    };
      

}
