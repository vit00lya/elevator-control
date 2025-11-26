#pragma once

#include <regex>
#include <iostream>
#include <string.h>
#include "input_reader.h"
#include "stat_reader.h"

namespace test{

    #define RUN_TEST(func) RunTestImpl(func, #func);
    #define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s);
    #define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint));
    #define ASSERT_EQUAL_HINT_POINTER(a, b, hint) AssertEqualImplPointer((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint));
    #define ASSERT_NOT_EQUAL_HINT_POINTER(a, b, hint) AssertNotEqualImplPointer((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint));

    template<typename TestFunc>
    void RunTestImpl(const TestFunc &func, const std::string &test_name);

    template<typename T, typename U>
    void AssertEqualImpl(const T &t, const U &u, const std::string &t_str,
            const std::string &u_str, const std::string &file,
            const std::string &func, unsigned line, const std::string &hint) {
        if (t != u) {
            std::cout << std::boolalpha;
            std::cout << file << "(" << line << "): " << func << ": ";
            std::cout << "ASSERT_EQUAL(" << t_str << ", " << u_str << ") failed: ";
            std::cout << t << " != " << u << ".";
            if (!hint.empty()) {
                std::cout << " Hint: " << hint;
            }
            std::cout << std::endl;
            abort();
        }
    }

    template<typename T, typename U>
    void AssertEqualImplPointer(const T &t, const U &u, const std::string &t_str,
            const std::string &u_str, const std::string &file,
            const std::string &func, unsigned line, const std::string &hint) {
        if (t == u) {
            std::cout << std::boolalpha;
            std::cout << file << "(" << line << "): " << func << ": ";
            std::cout << "ASSERT_EQUAL_POINTER(" << t_str << ", " << u_str << ") failed: ";
            std::cout << t << " != " << "nullptr" << ".";
            if (!hint.empty()) {
                std::cout << " Hint: " << hint;
            }
            std::cout << std::endl;
            abort();
        }
    }

    template<typename T, typename U>
    void AssertNotEqualImplPointer(const T &t, const U &u, const std::string &t_str,
            const std::string &u_str, const std::string &file,
            const std::string &func, unsigned line, const std::string &hint) {
        if (t != u) {
            std::cout << std::boolalpha;
            std::cout << file << "(" << line << "): " << func << ": ";
            std::cout << "ASSERT_NOT_EQUAL_POINTER(" << t_str << ", " << u_str << ") failed: ";
            std::cout << t << " != " << "nullptr" << ".";
            if (!hint.empty()) {
                std::cout << " Hint: " << hint;
            }
            std::cout << std::endl;
            abort();
        }
    }


    void Adding_the_correct_new_stop();
    void Adding_a_new_stop_without_a_name();
    void Add_stop_again();

    void Search_for_stop_without_name();
    void Search_for_an_existing_stop();

    void Adding_the_correct_route();
    void Adding_a_route_with_an_empty_stop();
    void Adding_a_route_with_an_unknown_stop();

    void Search_for_an_existing_route();
    void Search_for_not_added_route();

    void Check_for_unique_stops();
    void Checking_the_correctness_of_input_data_processing();

    void Checking_the_correctness_of_input_data_processing();
    void Checking_route_in_which_the_distance_is_set_only_in_one_way();

    void TestTransportCatalogue();
    
    // Тесты для ElevatorControl
    void TestReadAndDeleteFilesByMask();
    
    void TestElevatorControl();

}
