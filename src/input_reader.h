#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <charconv>

#include "domain.h"
#include "geo.h"
#include "transport_catalogue.h"


namespace input_reader{

    struct CommandDescription {
        // Определяет, задана ли команда (поле command непустое)
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;      // Название команды
        std::string id;           // id маршрута или остановки
        std::string description;  // Параметры команды
    };

    class InputReader {
    public:
        InputReader() = default;
        /**
         * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
         */
        void ParseLine(std::string_view line);

        /**
         * Наполняет данными транспортный справочник, используя команды из commands_
         */
        void ApplyCommands(transport_catalogue::TransportCatalogue& catalogue) const; // @suppress("Type cannot be resolved")

        void InsertCommands(std::istringstream& command, transport_catalogue::TransportCatalogue& catalogue);

    private:
        std::vector<CommandDescription> commands_;
        std::vector<CommandDescription> add_stop_commands_;
    };

}

input_reader::CommandDescription ParseCommandDescription(std::string_view line);
