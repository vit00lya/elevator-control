#include "input_reader.h"

using namespace std::literals;

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(
            std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim, bool first_occurrence = false) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
            if (first_occurrence){
                return result;
            }
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim, прекращает работу после нахождения первого делителя.
 */
std::vector<std::string_view> SplitFirstOccurrence(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
            if (auto substr2 = Trim(string.substr(delim_pos - pos, string.size() -1)); !substr2.empty()) {
                result.push_back(substr2);
            }
            return result;
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит строку вида " 55.611087, 37.20829, 3900m to Marushkino"s и возвращает объект Distance
 * содержащий точку А типа string, точку Б типа string и расстояние между ними uint32_t в метрах
 */
void ParseStopDistance(transport_catalogue::TransportCatalogue &catalogue, const std::string_view description, std::string_view name_stop) {

    uint32_t distance = 0;
    std::vector<std::string_view> comma = Split(description, ',');
    const domain::Stop* stop_from = catalogue.FindStop(name_stop);
    for(auto &com : comma){
        std::vector<std::string_view> v_dist = SplitFirstOccurrence(com, 'm');
        if(v_dist.size() > 1){
            std::from_chars(v_dist[0].data(), v_dist[0].data() + v_dist[0].size(), distance);
            std::string_view stop_to(v_dist[1].begin() + 5, v_dist[1].end());
            catalogue.SetDistanceBetweenStop(stop_to, stop_from, distance);
        }
    }

}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end()); // @suppress("Invalid arguments")
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend()); // @suppress("Invalid arguments")

    return results;
}

input_reader::CommandDescription ParseCommandDescription(
        std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
        std::string(line.substr(not_space, colon_pos - not_space)),
        std::string(line.substr(colon_pos + 1))};
}

namespace input_reader {

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {

        if (command_description.command == "Stop"s) {
            add_stop_commands_.push_back(std::move(command_description));
        } else {
            commands_.push_back(std::move(command_description));
        }
    }
}

void InputReader::ApplyCommands(
        [[maybe_unused]] transport_catalogue::TransportCatalogue &catalogue) const {

    //std::vector<transport_catalogue::StopDistance> stp_dis_v;

    for (auto &it : add_stop_commands_) {
        domain::Stop stop;
        stop.name = it.id; // @suppress("Field cannot be resolved")
        stop.coordinates = ParseCoordinates(it.description); // @suppress("Field cannot be resolved") // @suppress("Invalid arguments")
        catalogue.AddStop(stop);

    }
    for (auto &it : add_stop_commands_) {
        ParseStopDistance(catalogue, it.description, it.id); // @suppress("Field cannot be resolved") // @suppress("Invalid arguments")
    }

    //catalogue.SetDistanceBetweenStops(stp_dis_v);

    for (auto &it : commands_) {
        if (it.command == "Bus"s) { // @suppress("Field cannot be resolved")
            bool ring_route = (it.description.find(">") != std::string::npos); // @suppress("Method cannot be resolved") // @suppress("Field cannot be resolved")
            catalogue.AddBus(it.id, ParseRoute(it.description), ring_route); // @suppress("Invalid arguments") // @suppress("Field cannot be resolved")
        }
    }
}

void InputReader::InsertCommands(std::istringstream &command,
        transport_catalogue::TransportCatalogue &catalogue) {
    int base_request_count;
    command >> base_request_count;

    for (int i = 0; i <= base_request_count; ++i) {
        std::string line;
        std::getline(command, line);
        ParseLine(line);
    }

    ApplyCommands(catalogue);

}
}
