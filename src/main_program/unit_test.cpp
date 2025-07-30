 #include "unit_test.h"
#include "transport_catalogue.h"

using namespace std::literals;


template<typename TestFunc>
    void test::RunTestImpl(const TestFunc &func, const std::string &test_name) {
        func();
        std::cerr << test_name << " OK"s << std::endl;
    }

    transport_catalogue::TransportCatalogue FillingRoutes(){

        transport_catalogue::TransportCatalogue sut;
        geo::Coordinates coordinates = {55.611087, 37.208290};
        domain::Stop stop = {"Tolstopaltsevo"s, coordinates};
        sut.AddStop(stop);
        coordinates = {55.632761, 37.333324};
        stop = {"Rasskazovka"s, coordinates};
        sut.AddStop(stop);
        coordinates = {55.574371, 37.651700};
        stop = {"Biryulyovo Zapadnoye"s, coordinates};
        sut.AddStop(stop);

        return sut;

    }

// sut - system under test

    void test::Adding_the_correct_new_stop(){
        transport_catalogue::TransportCatalogue sut;
        geo::Coordinates coordinates = {55.611087, 37.208290};
        domain::Stop Stop_A = {"Tolstopaltsevo"s, coordinates};

        sut.AddStop(Stop_A);

        ASSERT_EQUAL_HINT(sut.NumberOfStops(), 1u,
                        "Не правильно добавляется новая остановка."s);

    }
    void test::Adding_a_new_stop_without_a_name(){
        transport_catalogue::TransportCatalogue sut;
        geo::Coordinates coordinates = {55.611087, 37.208290};
        domain::Stop Stop_A = {""s, coordinates};
        sut.AddStop(Stop_A);

        ASSERT_EQUAL_HINT(sut.NumberOfStops(), 0u,
                        "Остановка не должна добавляться, если у неё не задано имя."s);
    }
    void test::Add_stop_again(){
        transport_catalogue::TransportCatalogue sut;
        geo::Coordinates coordinates = {55.611087, 37.208290};
        domain::Stop Stop_A = {"Tolstopaltsevo"s, coordinates};
        sut.AddStop(Stop_A);
        sut.AddStop(Stop_A);

        ASSERT_EQUAL_HINT(sut.NumberOfStops(), 1u,
                             "Остановка не должна добавляться повторно."s);
    }

    void test::Search_for_stop_without_name(){
        transport_catalogue::TransportCatalogue sut;
        geo::Coordinates coordinates = {55.611087, 37.208290};
        domain::Stop Stop_A = {"Tolstopaltsevo"s, coordinates};
        sut.AddStop(Stop_A);

        ASSERT_NOT_EQUAL_HINT_POINTER(sut.FindStop(""s), nullptr,
                             "Класс не правильно обрабатывает случай, когда введеное для поиска имя пустое."s);
    }

    void test::Search_for_an_existing_stop(){
        transport_catalogue::TransportCatalogue sut;
        geo::Coordinates coordinates = {55.611087, 37.208290};
        domain::Stop Stop_A = {"Tolstopaltsevo"s, coordinates};
        sut.AddStop(Stop_A);

        ASSERT_EQUAL_HINT_POINTER(sut.FindStop("Tolstopaltsevo"s), nullptr,
                                    "Класс не находит уже введеную остановку."s);
    }

    void test::Adding_the_correct_route(){

        transport_catalogue::TransportCatalogue sut = FillingRoutes();
        std::vector<std::string_view> bus_stop = {"Tolstopaltsevo"sv,"Rasskazovka"sv,"Biryulyovo Zapadnoye"sv};
        sut.AddBus("750"s, bus_stop);

        ASSERT_EQUAL_HINT(sut.NumberOfRoutes(), 1u,
                                    "Маршрут добавляется не корректно."s);
    }

    void test::Adding_a_route_with_an_empty_stop(){

        transport_catalogue::TransportCatalogue sut = FillingRoutes();

        std::vector<std::string_view> bus_stop = {"Tolstopaltsevo"sv,""s,"Biryulyovo Zapadnoye"sv};
        sut.AddBus("750"s, bus_stop);

        ASSERT_EQUAL_HINT(sut.NumberOfRoutes(), 0u,
                                    "Маршрут был построен даже с пустой остановкой."s);
    }

    void test::Adding_a_route_with_an_unknown_stop(){

        transport_catalogue::TransportCatalogue sut = FillingRoutes();

        std::vector<std::string_view> bus_stop = {"Tolstopaltsevo"sv,"Rasskazovka1"sv,"Biryulyovo Zapadnoye"sv};
        sut.AddBus("750"s, bus_stop);

        ASSERT_EQUAL_HINT(sut.NumberOfRoutes(), 0u,
                                    "Маршрут был построен даже с неизвестной остановкой."s);
    }

    void test::Search_for_an_existing_route(){

        transport_catalogue::TransportCatalogue sut = FillingRoutes();

        std::vector<std::string_view> bus_stop = {"Tolstopaltsevo"sv,"Rasskazovka"sv,"Biryulyovo Zapadnoye"sv};
        sut.AddBus("750"s, bus_stop);

        ASSERT_EQUAL_HINT_POINTER(sut.FindBus("750"s), nullptr,
                                    "Не находит автобус который введен в систему."s);
    }

    void test::Search_for_not_added_route(){

        transport_catalogue::TransportCatalogue sut = FillingRoutes();

        std::vector<std::string_view> bus_stop = {"Tolstopaltsevo"sv,"Rasskazovka"sv,"Biryulyovo Zapadnoye"sv};
        sut.AddBus("750"s, bus_stop);

        ASSERT_NOT_EQUAL_HINT_POINTER(sut.FindBus("880"s), nullptr,
                                    "Находит маршрут который не был введен в систему."s);
    }

    void test::Check_for_unique_stops(){
        transport_catalogue::TransportCatalogue sut = FillingRoutes();

        std::vector<std::string_view> bus_stop = {"Tolstopaltsevo"sv,"Rasskazovka"sv,"Biryulyovo Zapadnoye"sv,"Rasskazovka"sv};
        sut.AddBus("750"s, bus_stop);
        auto info = sut.GetBusInfo("750"s);

        ASSERT_EQUAL_HINT(info.unique_stops_count, 3u,
                                           "Не верно считает количество уникльных остановок."s);

        ASSERT_EQUAL_HINT(info.no_unique_stops_count, 4u,
                                           "Не верно считает общее количество остановок."s);

    }

    void test::Checking_route_in_which_the_distance_is_set_only_in_one_way(){
        transport_catalogue::TransportCatalogue sut = FillingRoutes();

        sut.SetDistanceBetweenStop("Rasskazovka"sv, sut.FindStop("Tolstopaltsevo"sv), 200);
        std::vector<std::string_view> bus_stop = {"Tolstopaltsevo"sv,"Rasskazovka"sv};
        sut.AddBus("750"s, bus_stop, false);
        auto info = sut.GetBusInfo("750"s);

        ASSERT_EQUAL_HINT(info.route_length, 200u,
                                                "Не верно считает расстояние между остановками."s);

    }

    void test::Checking_the_correctness_of_input_data_processing(){

        // Arrange
        transport_catalogue::TransportCatalogue catalogue;
        {
             std::istringstream input{
                   "13\n"
                   "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\n"
                   "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\n"
                   "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                   "Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\n"
                   "Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino\n"
                   "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
                   "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam\n"
                   "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
                   "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
                   "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
                   "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
                   "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
                   "Stop Prazhskaya: 55.611678, 37.603831\n"};


             input_reader::InputReader ir;
             ir.InsertCommands(input,catalogue);
         }

        std::istringstream input{
                     "6\n"
                     "Bus 256\n"
                     "Bus 750\n"
                     "Bus 751\n"
                     "Stop Samara\n"
                     "Stop Prazhskaya\n"
                     "Stop Biryulyovo Zapadnoye\n"};

        std::string out_test{
            "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature\n"
            "Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature\n"
            "Bus 751: not found\n"
            "Stop Samara: not found\n"
            "Stop Prazhskaya: no buses\n"
            "Stop Biryulyovo Zapadnoye: buses 256 828\n"
        };

        std::ostringstream out;

        //Act

        stat_reader::ParseCommadStream(input, catalogue, out);
       // std::cout << out.str();
        //Assert
        ASSERT_EQUAL_HINT(out.str(), out_test, "Не верно обрабатывает данные из примера."s);

    }


    void test::TestTransportCatalogue() {
        RUN_TEST(Adding_the_correct_new_stop);
        RUN_TEST(Adding_a_new_stop_without_a_name);
        RUN_TEST(Add_stop_again);
        //
        RUN_TEST(Search_for_stop_without_name);
        RUN_TEST(Search_for_an_existing_stop);
        //
        RUN_TEST(Adding_the_correct_route);
        RUN_TEST(Adding_a_route_with_an_empty_stop);
        RUN_TEST(Adding_a_route_with_an_unknown_stop);
        //
        RUN_TEST(Search_for_an_existing_route);
        RUN_TEST(Search_for_not_added_route);

        RUN_TEST(Check_for_unique_stops);
        RUN_TEST(Checking_route_in_which_the_distance_is_set_only_in_one_way);
        RUN_TEST(Checking_the_correctness_of_input_data_processing);
    }





