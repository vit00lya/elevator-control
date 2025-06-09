#pragma once

#include <string>
#include "json.h"
#include "transport_catalogue.h"
#include <fstream>
#include "domain.h"
#include <memory>
#include <sstream>
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"
#include <optional>

namespace jsonreader
{
    using Node = json::Node;
	using NodeUniquePair = std::pair<std::unique_ptr<Node>,std::unique_ptr<Node>>;
	using NodeUnique = std::unique_ptr<Node>;

	using CoordinatesWithCorrection = std::unique_ptr<std::map<std::string_view, std::vector<domain::Point>>>;


	struct SettingsOutput{
	    map_render::RenderSettings render_settings;
	    transport_router::RoutingSettings routing_settings;
	};

    class JsonReader{
    public:
        JsonReader() = default;
        std::string ProcessJson(std::istream& input_json, std::ostream& out);

    private:

        std::vector<std::string_view> ParseRoute(const json::Array& route, bool is_roundtrip);
        void AddStop(const json::Node& node);
        void ProcessBaseRequest(const json::Array& array);
        void ProcessStatRequest(const json::Array& array, std::ostream& out, SettingsOutput& settings_output);
        void ParseStopDistance();
        void ParseBus();
        map_render::RenderSettings GetSettingsRender(const json::Dict& dict);
        svg::Color ConvertToColor(const Node&);
        transport_router::RoutingSettings GetRoutingSettings(const json::Dict& dict);
        std::vector<NodeUniquePair> road_distances_;
        std::vector<NodeUnique> bus_;
        transport_catalogue::TransportCatalogue transport_catalogue_;
        transport_router::TransportRouter router_;

        std::optional<transport_router::EdgeDescriptions> BuildOptimalRoute(std::string_view stop_from, std::string_view stop_to) const;

        json::Node MakeJSONRouteResponse(const transport_router::EdgeDescriptions& route_description,
                                         const json::Node& elem);
        json::Node ProcessStopQuery(const json::Node& elem);
        json::Node ProcessBusQuery(const json::Node& elem);
        json::Node ProcessRouteQuery(const json::Node& elem);
        json::Node ProcessMapQuery(const json::Node& elem, SettingsOutput& settings);
        json::Node MakeErrorResponse(const json::Node& elem);
        json::Node MakeJSONBusResponse(const json::Node& elem,  const domain::BusInfo& bus_info);
        json::Node MakeJSONStopResponse(const json::Node& elem, const std::set<std::string> stop_info);
        json::Node MakeJSONMapResponse(const json::Node& elem,
           		                                   const transport_catalogue::BusesListPointer& buses,
       											   SettingsOutput& settings);
    };


}
