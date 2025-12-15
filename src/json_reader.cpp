#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include "json_reader.h"
#include <curl/curl.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include "network_client.h"

using namespace jsonreader;

void JsonReader::FilligBarcodes(elevator_control::ElevatorControl &ec)
{
  using namespace std::literals;

  try{
      // Открываем файл обмена, путь к которому указан в настройках
      std::ifstream input_json;
      input_json.open("barcode.json", std::ios::binary);
      if (!input_json.good())
      {
        throw std::runtime_error("Не возможно прочитать файл штрихкодов barcode.json");
      }

      // Загружаем и парсим JSON документ
      json::Document doc = json::Load(input_json);
      json::Array arr = doc.GetRoot().AsArray();

      // Переменные для хранения текущего штрихкода и названия продукта
      std::string barcode;
      std::string name_product;

      // Проходим по каждому элементу массива (каждый элемент - это объект с данными о продукте)
      for (const auto &elem : arr)
      {
        // Проходим по ключам объекта
        for (const auto &[key, value] : elem.AsMap())
        {
          // Если ключ "barcode", сохраняем значение
          if (key == "barcode"s)
          {
            barcode = value.AsString();
          }
          // Если ключ "name_product", сохраняем значение
          if (key == "name_product"s)
          {
            name_product = value.AsString();
          }
        }
        // Добавляем найденную пару (название продукта, штрихкод) в систему управления лифтом
        ec.AddBarcode(name_product, barcode);
      }
}
  catch (std::logic_error e)
  {
    log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Ошибка при разборе JSON:" << e.what();
  }
  catch (json::ParsingError e)
  {
    log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Ошибка при разборе JSON:" << e.what();
  }
   catch(...){
     log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Неизвестная ошибка";
   }

}

/**
 * @brief Загружает настройки из файла settings.json в структуру настроек.
 *
 * Открывает файл settings.json, парсит его как JSON-документ, извлекает все параметры
 * конфигурации и сохраняет их в структуре настроек.
 *
 * @param ec Ссылка на объект настроек, в который будут загружены настройки.
 * @throws std::runtime_error Если файл не может быть открыт или прочитан.
 */
void JsonReader::LoadSettings(elevator_control::ElevatorControl &ec)
{
  using namespace std::literals;

  std::ifstream input_json;
  try
  {
    input_json.open("settings.json", std::ios::binary);
    if (!input_json.good())
    {
      throw std::runtime_error("Не возможно прочитать файл настроек settings.json");
    }
  }
  catch (...)
  {
    throw std::runtime_error("Не возможно прочитать файл настроек settings.json");
  }

  elevator_control::Settings settings;

  json::Document doc = json::Load(input_json);
  json::Dict dict = doc.GetRoot().AsMap();
  for (const auto &[key, value] : dict)
  {
    if (key == "device_id")
    {
      settings.device_id = value.AsInt();
    }
    if (key == "scanner_enable")
    {
      settings.scanner_enable = value.AsBool();
    }
    if (key == "userpassword")
    {
      settings.userpassword = value.AsString();
    }
    else if (key == "server_address")
    {
      settings.server_address = value.AsString();
    }
    else if (key == "scanner_num_com_port")
    {
      settings.scanner_num_com_port = value.AsInt();
    }
    else if (key == "scanner_baud_rate")
    {
      settings.scanner_baud_rate = value.AsInt();
    }
    else if (key == "scanner_parity")
    {
      settings.scanner_parity = value.AsString();
    }
    else if (key == "scanner_data_bits")
    {
      settings.scanner_data_bits = value.AsInt();
    }
    else if (key == "scanner_stop_bits")
    {
      settings.scanner_stop_bits = value.AsString();
    }
    else if (key == "scanner_linux_com_port")
    {
      settings.scanner_linux_com_port = value.AsString();
    }
    else if (key == "display_width")
    {
      settings.display_width = value.AsInt();
    }
    else if (key == "display_height")
    {
      settings.display_height = value.AsInt();
    }
    else if (key == "display_pin_reset")
    {
      settings.display_pin_reset = value.AsInt();
    }
    else if (key == "display_pin_rs")
    {
      settings.display_pin_rs = value.AsInt();
    }
    else if (key == "display_pin_en")
    {
      settings.display_pin_en = value.AsInt();
    }
    else if (key == "display_pin_cs1")
    {
      settings.display_pin_cs1 = value.AsInt();
    }
    else if (key == "display_pin_cs2")
    {
      settings.display_pin_cs2 = value.AsInt();
    }
    else if (key == "display_pin_d0")
    {
      settings.display_pin_d0 = value.AsInt();
    }
    else if (key == "display_pin_d1")
    {
      settings.display_pin_d1 = value.AsInt();
    }
    else if (key == "display_pin_d2")
    {
      settings.display_pin_d2 = value.AsInt();
    }
    else if (key == "display_pin_d3")
    {
      settings.display_pin_d3 = value.AsInt();
    }
    else if (key == "display_pin_d4")
    {
      settings.display_pin_d4 = value.AsInt();
    }
    else if (key == "display_pin_d5")
    {
      settings.display_pin_d5 = value.AsInt();
    }
    else if (key == "display_pin_d6")
    {
      settings.display_pin_d6 = value.AsInt();
    }
    else if (key == "display_pin_d7")
    {
      settings.display_pin_d7 = value.AsInt();
    }
    else if (key == "display_pin_led")
    {
      settings.display_pin_led = value.AsInt();
    }
    else if (key == "time_unlock_door")
    {
      settings.time_unlock_door = value.AsInt();
    }
    else if (key == "pin_unlock_door")
    {
      settings.pin_unlock_door = value.AsInt();
    }
    else if (key == "pin_close_door")
    {
      settings.pin_close_door = value.AsInt();
    }
  }
  ec.SaveSettings(settings);
}


void JsonReader::StartBackgroundDownloadBarcode(elevator_control::ElevatorControl &ec)
{
  // Проверяем, не запущен ли уже поток
  if (background_thread_barcode_.joinable())
  {
    throw std::runtime_error("Фоновый поток уже запущен!");
  }

  elevator_control::Settings settings = ec.GetSettings();

  // Сбрасываем флаг остановки
  stop_flag_barcode_.store(false);

  // Запускаем новый поток
  background_thread_barcode_ = std::thread([this, settings]()
                                           {
        using namespace std::chrono_literals;
        
        while (!stop_flag_barcode_.load()) {

            // Проверяем еще раз флаг остановки после сна
            if (stop_flag_barcode_.load()) {
                break;
            }
            std::string error_message;
            network_client::DownloadBarcodeJsonData(settings.server_address, "barcode.json", settings.userpassword, error_message);
            if (!error_message.empty()) {
                log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Error in DownloadBarcodeJsonData: " << error_message;
            }
            // Ждем 60 минут
            std::this_thread::sleep_for(6000s);
        } });
      }

/**
 * @brief Создает и сохраняет транспортный пакет с штрихкодами в JSON-файл.
 *
 * Формирует транспортный пакет, содержащий идентификатор, временную метку и массив штрихкодов,
 * сериализует его в JSON и записывает в файл. После этого увеличивает счетчик пакетов.
 *
 * @param ec Ссылка на объект управления лифтом, из которого берутся данные для пакета.
 * @return Имя созданного файла пакета.
 */
// Возвращает имя создаваемого пакета
std::string JsonReader::SaveTransportPackage(elevator_control::ElevatorControl &ec)
{

  using namespace std::literals;
  std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
  time_t now_t = std::chrono::system_clock::to_time_t(time);

  std::ofstream out;
  out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  auto name_file = "tranport_package_" + std::to_string(ec.GetTransportPacketId()) + "_" + std::to_string(now_t) + ".json";
  try
  {
    out.open(name_file);
  }
  catch(const std::exception& e)
  {
    throw std::runtime_error("Ошибка при создании файла транспортного пакета. Возможно закончилась память.");
  }

  // Заполнение пакета
  elevator_control::TransportPacket tp;

  tp.id = std::to_string(ec.GetTransportPacketId());
  tp.dev_id = std::to_string(ec.GetSettings().device_id);
  tp.time_point = std::to_string(now_t);
  tp.array_barcodes = std::move(ec.GetBarcodesToSend());

  std::vector<Node> tmp_v{tp.array_barcodes.begin(), tp.array_barcodes.end()};

  // Создание JSON файла
  auto result = json::Builder{}.StartDict().Key("id"s).Value(tp.id).Key("dev_id"s).Value(tp.dev_id).Key("time_point"s).Value(tp.time_point).Key("array_barcodes").Value(tmp_v).EndDict().Build();

  json::Document doc = json::Document(result);
  json::Print(doc, out);

  ec.IncTrasportPacketId();
  ec.ClearBarcodeToSend();

  return name_file;
}
