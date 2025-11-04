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
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>

namespace jsonreader
{
    using Node = json::Node;

    class JsonReader{
    public:
        JsonReader() = default;
        void FilligBarcodes(elevator_control::ElevatorControl& ec);
        void LoadSettings(elevator_control::ElevatorControl& ec);
        std::string SaveTransportPackage(elevator_control::ElevatorControl& ec);
        void SendTransportPackage(const std::string& filename);
        
        // Методы для управления фоновой отправкой пакетов
        /**
         * @brief Запускает фоновый поток для периодической отправки транспортных пакетов.
         *
         * Поток будет работать каждые 10 минут, создавая и отправляя пакет.
         *
         * @param ec Ссылка на объект управления лифтом.
         */
        void StartBackgroundSender(elevator_control::ElevatorControl& ec);
        
        /**
         * @brief Останавливает фоновый поток отправки пакетов.
         *
         * Устанавливает флаг остановки и дожидается завершения потока.
         */
        void StopBackgroundSender();

    private:
        std::thread background_thread_; ///< Поток для фоновой отправки
        std::atomic<bool> stop_flag_{false}; ///< Флаг для остановки потока
    };


}
