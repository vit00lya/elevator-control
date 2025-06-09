#include "json_reader.h"

namespace jsonreader
{

    std::string JsonReader::ProcessJson(std::istream& input_json, std::ostream& out){
      using namespace std::literals;
      json::Document doc = json::Load(input_json);
      json::Dict map = doc.GetRoot().AsMap();

      std::string result;

      SettingsOutput settings_output;

      for(const auto& [key, value]: map){
          if(key == "base_requests"s){
              ProcessBaseRequest(value.AsArray()); // @suppress("Invalid arguments") // @suppress("Method cannot be resolved")
          }
          else if(key == "stat_requests"s){
        	  ProcessStatRequest(value.AsArray(), out, settings_output); // @suppress("Method cannot be resolved") // @suppress("Invalid arguments")
          }
          else if(key == "routing_settings"s){
              settings_output.routing_settings = GetRoutingSettings(value.AsMap());
              router_ = {settings_output.routing_settings, std::make_unique<transport_catalogue::TransportCatalogue>(transport_catalogue_)};
          }
          else if(key == "render_settings"s){
              settings_output.render_settings = GetSettingsRender(value.AsMap()); // @suppress("Invalid arguments") // @suppress("Method cannot be resolved")
          }

      }

      return result;

    }

    /**
     * Парсит маршрут.
     * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
     * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
     */
    std::vector<std::string_view> JsonReader::ParseRoute(const json::Array& route, bool is_roundtrip){
        std::vector<std::string_view> results;
            if(route.size() != 0){
                 for(const auto &stop: route){
                     results.push_back(stop.AsString()); // @suppress("Invalid arguments") // @suppress("Method cannot be resolved")
                 }
                 if(!is_roundtrip){
                     std::vector<std::string_view> v_tmp;
                     std::vector<std::string_view>::reverse_iterator ri;
                     for (ri = results.rbegin()+1; ri != results.rend(); ++ri ){
                         v_tmp.push_back(*ri); // @suppress("Invalid arguments")
                     }
                    for(auto &item: v_tmp) {
                        results.push_back(std::move(item)); // @suppress("Invalid arguments")
                    }
                 } else{
                     if (results.size() != 0 && results[results.size()-1] != results[0]) {
                         std::string_view firs_stop = results[0];
                         results.push_back(std::move(firs_stop));
                     }
                 }
            }

             return results;
    }

    svg::Color JsonReader::ConvertToColor(const Node &node){
        svg::Color color;
        if (node.IsString()) color = node.AsString();
        if (node.IsArray()){
            const json::Array& tmp_arr = node.AsArray();
            if (tmp_arr.size() == 3){
               svg::Rgb rgb_color;
               rgb_color.red = tmp_arr[0].AsInt();
               rgb_color.green = tmp_arr[1].AsInt();
               rgb_color.blue = tmp_arr[2].AsInt();
               color = std::move(rgb_color);
            }
            else if(tmp_arr.size() == 4){
                svg::Rgba rgba_color;
                rgba_color.red = tmp_arr[0].AsInt();
                rgba_color.green = tmp_arr[1].AsInt();
                rgba_color.blue = tmp_arr[2].AsInt();
                rgba_color.opacity = tmp_arr[3].AsDouble();
                color = std::move(rgba_color);
            }
        }
        return color;
    }

    map_render::RenderSettings JsonReader::GetSettingsRender(const json::Dict& dict){
        using namespace std::literals;
        map_render::RenderSettings render_settings;
        for (const auto& [key, value] : dict) {
            if(key == "width"s) render_settings.width = value.AsDouble(); // @suppress("Method cannot be resolved")
            else if(key == "height"s) render_settings.height = value.AsDouble(); // @suppress("Method cannot be resolved")
            else if(key == "padding"s) render_settings.padding = value.AsDouble(); // @suppress("Method cannot be resolved")
            else if(key == "stop_radius"s) render_settings.stop_radius = value.AsDouble(); // @suppress("Method cannot be resolved")
            else if(key == "line_width"s) render_settings.line_width = value.AsDouble(); // @suppress("Method cannot be resolved")
            else if(key == "bus_label_font_size"s)  render_settings.bus_label_font_size = value.AsInt(); // @suppress("Method cannot be resolved")
            else if(key == "bus_label_offset"s) {
                const json::Array& tmp_arr = value.AsArray(); // @suppress("Method cannot be resolved")
                render_settings.bus_label_offset = {tmp_arr[0].AsDouble(),tmp_arr[1].AsDouble()};
            }
            else if(key == "stop_label_font_size"s) render_settings.stop_label_font_size = value.AsInt(); // @suppress("Method cannot be resolved")
            else if(key == "stop_label_offset"s){
                const json::Array& tmp_arr = value.AsArray(); // @suppress("Method cannot be resolved")
                render_settings.stop_label_offset = {tmp_arr[0].AsDouble(),tmp_arr[1].AsDouble()};
            }
            else if(key == "underlayer_color"s) render_settings.underlayer_color = {ConvertToColor(value)}; // @suppress("Invalid arguments")
            else if(key == "underlayer_width"s) render_settings.underlayer_width = value.AsDouble(); // @suppress("Method cannot be resolved")
            else if(key == "color_palette"s) {
                    const json::Array& tmp_arr = value.AsArray(); // @suppress("Method cannot be resolved")
                    for (const auto& it : tmp_arr){
                        render_settings.color_palette.push_back(ConvertToColor(it)); // @suppress("Invalid arguments")
                    }
            };
        }
        return render_settings;
    }

    transport_router::RoutingSettings JsonReader::GetRoutingSettings(const json::Dict& dict){

           return {dict.at("bus_wait_time").AsDouble(),
                   dict.at("bus_velocity").AsDouble()};
    }


    void JsonReader::AddStop(const json::Node& node) {
        using namespace std::literals;
          domain::Stop stop;
          const auto &tmp =  node.AsMap();
          stop.name = tmp.at("name"s).AsString();
          stop.coordinates = {tmp.at("latitude"s).AsDouble(),tmp.at("longitude"s).AsDouble()};
          transport_catalogue_.AddStop(stop);
          road_distances_.push_back({std::make_unique<Node>(tmp.at("name"s)), // @suppress("Invalid arguments")
              std::make_unique<Node>(tmp.at("road_distances"s))});
    }

    void JsonReader::ProcessBaseRequest(const json::Array& array){
    	using namespace std::literals;
        for(const auto &elem : array){
            const auto &type = elem.AsMap().at("type").AsString(); // @suppress("Method cannot be resolved")
            if (type == "Stop"sv) {
            	AddStop(elem); // @suppress("Invalid arguments")
            }
            else if(type == "Bus"sv){
                bus_.push_back(std::make_unique<Node>(elem.AsMap())); // @suppress("Method cannot be resolved")
            }
        }

        ParseStopDistance();
        ParseBus();

    }

    void JsonReader::ParseStopDistance() {

        const domain::Stop* stop_from;
        std::string_view stop_to;
        int dist;
        for(const NodeUniquePair &dis : road_distances_){
                const Node& stop_form_name = *dis.first;
                const Node& stop_second_node = *dis.second;
                    const auto& map = stop_second_node.AsMap();
                    if (map.size() != 0) {
                        for (const auto& [key, value] : map) {
                            stop_from = transport_catalogue_.FindStop(stop_form_name.AsString());
                            dist =  value.IsInt() ? value.AsInt() : 0; // @suppress("Method cannot be resolved")
                            transport_catalogue_.SetDistanceBetweenStop(key, stop_from, dist); // @suppress("Invalid arguments")
                        }
                    }
            }
        }

    void JsonReader::ParseBus() {
        for(const NodeUnique &bus : bus_){
            const auto &tmp = *bus;
            const auto &bus_map = tmp.AsMap();
            bool is_roundtrip =  bus_map.at("is_roundtrip").AsBool();
            std::vector<std::string_view> route = ParseRoute(bus_map.at("stops").AsArray(),is_roundtrip);
            transport_catalogue_.AddBus(bus_map.at("name").AsString(), route, is_roundtrip);
        }
    }

    json::Node JsonReader::MakeJSONStopResponse(const json::Node& elem, const std::set<std::string> stop_info){
    			std::vector<Node> tmp_v {stop_info.begin(), stop_info.end()};
    	        return json::Builder{}.StartDict()
    	                     .Key("request_id"s).Value(elem.AsMap().at("id").AsInt())
    	                     .Key("buses"s).Value(tmp_v)
    	                     .EndDict().Build();
    }

    json::Node JsonReader::MakeJSONRouteResponse(const transport_router::EdgeDescriptions& route_description,
                                                                    const json::Node& elem) {
        json::Array items;
        double total_time = 0.0;
        for (auto description : route_description) {
            total_time += description.time_;
            if (description.type_ == transport_router::EdgeType::WAIT) {
                json::Node dict = json::Builder{}.StartDict()
                                                     .Key("type").Value("Wait")
                                                     .Key("stop_name").Value(std::string (description.edge_name_))
                                                     .Key("time").Value(description.time_)
                                                 .EndDict()
                                             .Build();
                items.push_back(dict);
            } else if (description.type_ == transport_router::EdgeType::BUS) {
                json::Node dict = json::Builder{}.StartDict()
                                                     .Key("type").Value("Bus")
                                                     .Key("bus").Value(std::string (description.edge_name_))
                                                     .Key("span_count").Value(description.span_count_.value())
                                                     .Key("time").Value(description.time_)
                                                 .EndDict()
                                             .Build();
                items.push_back(dict);
            }
        }
        return json::Builder{}.StartDict()
                                    .Key("request_id").Value(elem.AsMap().at("id").GetValue())
                                    .Key("total_time").Value(total_time)
                                    .Key("items").Value(items)
                              .EndDict()
                          .Build();
    }

    std::optional<transport_router::EdgeDescriptions> JsonReader::BuildOptimalRoute(std::string_view stop_from, std::string_view stop_to) const {
        return router_.BuildRoute(stop_from, stop_to);
    }

    json::Node JsonReader::ProcessRouteQuery(const json::Node& elem) {
        using namespace std::literals;
        const auto &tmp = elem.AsMap();
        auto route_description =
              BuildOptimalRoute(tmp.at("from").AsString(), tmp.at("to").AsString());
              if (!route_description.has_value()) {
                  return MakeErrorResponse(elem);
              } else {
                  return MakeJSONRouteResponse(route_description.value(), elem);
              }
    }

    json::Node JsonReader::ProcessStopQuery(const json::Node& elem) {

        const std::string& name_stop = elem.AsMap().at("name").AsString();
         if(transport_catalogue_.StopExists(name_stop)){
             auto stop_info = transport_catalogue_.GetStopInfo(name_stop);
             return MakeJSONStopResponse(elem, stop_info);
         }
         else{
             return MakeErrorResponse(elem);
         }
    }

    json::Node JsonReader::MakeErrorResponse(const json::Node& elem) {
             return json::Builder{}.StartDict()
                           .Key("request_id"s).Value(elem.AsMap().at("id").AsInt())
                           .Key("error_message"s).Value("not found"s)
                           .EndDict().Build();
    }

    json::Node JsonReader::MakeJSONBusResponse(const json::Node& elem, const domain::BusInfo& bus_info){
    	 return json::Builder{}.StartDict()
					 .Key("request_id"s).Value(elem.AsMap().at("id").AsInt())
					 .Key("curvature"s).Value(bus_info.curvature)
					 .Key("route_length"s).Value(bus_info.route_length)
					 .Key("stop_count"s).Value(static_cast<int>(bus_info.no_unique_stops_count))
					 .Key("unique_stop_count"s).Value(static_cast<int>(bus_info.unique_stops_count))
					 .EndDict().Build();
    }

    json::Node JsonReader::MakeJSONMapResponse(const json::Node& elem,
    		                                   const transport_catalogue::BusesListPointer& buses,
											   SettingsOutput& settings){
         map_render::MapRender render(settings.render_settings);
    	 domain::StopCoordinatesListPointer coordinates_bus = transport_catalogue_.GetCoordinatesStopBuses(*buses);
    	 std::ostringstream os;
    	 render.RenderSvg(coordinates_bus, os);

    	 return json::Builder{}.StartDict()
    	                   .Key("request_id"s).Value(elem.AsMap().at("id").AsInt())
    	                   .Key("map"s).Value(os.str())
    	                   .EndDict().Build();
    }

    json::Node JsonReader::ProcessBusQuery(const json::Node& elem) {

        auto bus_info = transport_catalogue_.GetBusInfo(elem.AsMap().at("name").AsString());
        if (bus_info.found == false) {
            return MakeErrorResponse(elem);
        }else{
        	return MakeJSONBusResponse(elem, bus_info);
        }
    }

    json::Node JsonReader::ProcessMapQuery(const json::Node& elem, SettingsOutput& settings) {


        transport_catalogue::BusesListPointer buses = transport_catalogue_.GetBuses();
        if (buses->size() != 0){
        	return	MakeJSONMapResponse(elem, buses, settings);
        }
        else{
        	return MakeErrorResponse(elem);
        }
    }

    void JsonReader::ProcessStatRequest(const json::Array& array, std::ostream& out, SettingsOutput& settings_output){
        using namespace std::literals;
        json::Array result;

    	domain::BusInfo bus_info;
        for(const auto &elem : array){
            const auto &type = elem.AsMap().at("type").AsString();
            if (type == "Stop"sv) {
                result.push_back(ProcessStopQuery(elem));
            }
            else if(type == "Bus"sv){
                result.push_back(ProcessBusQuery(elem));
            }
            else if(type == "Map"sv){
                result.push_back(ProcessMapQuery(elem, settings_output));
            }
            else if(type == "Route"sv){
                result.push_back(ProcessRouteQuery(elem));
            }
        }

        json::Print(json::Document{result}, out);

    }

}
