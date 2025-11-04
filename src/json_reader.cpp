#include "json_reader.h"
#include <curl/curl.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>

using namespace jsonreader;

void JsonReader::FilligBarcodes(elevator_control::ElevatorControl& ec){
      using namespace std::literals;

      // Открываем файл обмена, путь к которому указан в настройках
      std::ifstream input_json;
      input_json.open(ec.GetSettings().path_exchange_file, std::ios::binary);
      
      // Загружаем и парсим JSON документ
      json::Document doc = json::Load(input_json);
      json::Array arr = doc.GetRoot().AsArray();
      
      // Переменные для хранения текущего штрихкода и названия продукта
      std::string barcode;
      std::string name_product;
      
      // Проходим по каждому элементу массива (каждый элемент - это объект с данными о продукте)
      for (const auto& elem: arr){
	// Проходим по ключам объекта
	for(const auto& [key, value]: elem.AsMap()){
	  // Если ключ "barcode", сохраняем значение
	  if(key == "barcode"s){
	    barcode = value.AsString();
	  }
	  // Если ключ "name_product", сохраняем значение
	  if(key == "name_product"s){
	    name_product = value.AsString();
	  }
      }
	// Добавляем найденную пару (название продукта, штрихкод) в систему управления лифтом
	ec.AddBarcode(name_product, barcode);
	
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
void JsonReader::LoadSettings(elevator_control::ElevatorControl& ec){
      using namespace std::literals;

      std::ifstream input_json;
      try{
	input_json.open("settings.json", std::ios::binary);
	if (!input_json.good()) {
	   std::cerr << "Не возможно прочитать файл настроек settings.json" << std::endl;
	   throw ;
	}
      }
      catch(...){
	std::cerr << "Не возможно прочитать файл настроек settings.json" << std::endl;
	throw ;
      } 
      
      elevator_control::Settings settings;
      
      json::Document doc = json::Load(input_json);
      json::Dict dict = doc.GetRoot().AsMap();
	for(const auto& [key, value]: dict){
	  if(key == "path_exchange_file"s){
	    settings.path_exchange_file = value.AsString();
	  }
	   else if(key == "scanner_enable"){
	     settings.scanner_enable = value.AsBool();
	  }
	  else if(key == "scanner_num_com_port"){
	    settings.scanner_num_com_port = value.AsInt();
	  }
	  else if(key == "scanner_baud_rate"){
	    settings.scanner_baud_rate = value.AsInt();
	  }
	  else if(key == "scanner_parity"){
	    settings.scanner_parity = value.AsString();
	  }
	  else if(key == "scanner_data_bits"){
	    settings.scanner_data_bits = value.AsInt();
	  }
	  else if(key == "scanner_stop_bits"){
	    settings.scanner_stop_bits = value.AsString();
	  }
	  else if(key == "scanner_linux_com_port"){
	    settings.scanner_linux_com_port = value.AsString();
	  }
	  else if(key == "display_width"){
	    settings.display_width = value.AsInt();
	  }
	  else if(key == "display_height"){
	    settings.display_height = value.AsInt();
	  }
	  else if(key == "pin_reset"){
	    settings.pin_reset = value.AsInt();
	  }
	  else if(key == "pin_rs"){
	    settings.pin_rs = value.AsInt();
	  }
	  else if(key == "pin_en"){
	    settings.pin_en = value.AsInt();
	  }
	  else if(key == "pin_cs1"){
	    settings.pin_cs1 = value.AsInt();
	  }
	  else if(key == "pin_cs2"){
	    settings.pin_cs2 = value.AsInt();
	  }
	  else if(key == "pin_d0"){
	    settings.pin_d0 = value.AsInt();
	  }
	  else if(key == "pin_d1"){
	    settings.pin_d1 = value.AsInt();
	  }
	  else if(key == "pin_d2"){
	    settings.pin_d2 = value.AsInt();
	  }
	  else if(key == "pin_d3"){
	    settings.pin_d3 = value.AsInt();
	  }
	  else if(key == "pin_d4"){
	    settings.pin_d4 = value.AsInt();
	  }
	  else if(key == "pin_d5"){
	    settings.pin_d5 = value.AsInt();
	  }
	  else if(key == "pin_d6"){
	    settings.pin_d6 = value.AsInt();
	  }
	  else if(key == "pin_d7"){
	    settings.pin_d7 = value.AsInt();
	  }
	  else if(key == "pin_led"){
	    settings.pin_led = value.AsInt();
	  }
      }
      ec.SaveSettings(settings);	
}

void JsonReader::StartBackgroundSender(elevator_control::ElevatorControl& ec) {
    // Проверяем, не запущен ли уже поток
    if (background_thread_.joinable()) {
        std::cerr << "Фоновый поток уже запущен!" << std::endl;
        return;
    }

    // Сбрасываем флаг остановки
    stop_flag_.store(false);

    // Запускаем новый поток
    background_thread_ = std::thread([this, &ec]() {
        using namespace std::chrono_literals;
        
        while (!stop_flag_.load()) {
            // Ждем 10 минут
            std::this_thread::sleep_for(600s);
            
            // Проверяем еще раз флаг остановки после сна
            if (stop_flag_.load()) {
                break;
            }
            
            try {
                // Создаем транспортный пакет
                std::string filename = this->SaveTransportPackage(ec);
                std::cout << "Создан транспортный пакет: " << filename << std::endl;
                
                // Отправляем пакет
                this->SendTransportPackage(filename);
                std::cout << "Транспортный пакет отправлен: " << filename << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Ошибка при создании или отправке транспортного пакета: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Неизвестная ошибка при создании или отправке транспортного пакета" << std::endl;
            }
        }
    });
    
    std::cout << "Фоновый поток для отправки пакетов запущен." << std::endl;
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
std::string JsonReader::SaveTransportPackage(elevator_control::ElevatorControl& ec){
 
  using namespace std::literals;
  std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
  time_t now_t = std::chrono::system_clock::to_time_t(time);

  std::ofstream out;
  out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  auto name_file = "tranport_package_" + std::to_string(ec.GetTransportPacketId()) + "_" + std::to_string(now_t) + ".json";
  out.open(name_file);

  // Заполнение пакета
  elevator_control::TransportPacket tp;
  
  tp.id = ec.GetTransportPacketId();
  tp.time_point = std::to_string(now_t);
  tp.array_barcodes = std::move(ec.GetBarcodesToSend());

  std::vector<Node> tmp_v {tp.array_barcodes.begin(), tp.array_barcodes.end()};

  //Создание JSON файла
   auto result = json::Builder{}.StartDict() 
     .Key("id"s).Value(tp.id)
     .Key("time_point"s).Value(tp.time_point)
     .Key("array_barcodes").Value(tmp_v).EndDict().Build(); 

    json::Document doc = json::Document(result);
    json::Print(doc, out);

    ec.IncTrasportPacketId();

    return name_file;
    
}

/**
 * @brief Callback-функция для записи данных ответа от сервера в строку.
 *
 * Используется библиотекой libcurl для обработки полученных данных.
 *
 * @param contents Указатель на данные ответа.
 * @param size Размер одного элемента данных.
 * @param nmemb Количество элементов данных.
 * @param response Указатель на строку, в которую будут записаны данные.
 * @return Общее количество записанных байт.
 */
// Callback function to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total_size = size * nmemb;
    response->append((char*)contents, total_size);
    return total_size;
}

/**
 * @brief Отправляет транспортный пакет (файл) на сервер по адресу http://127.0.0.1/upload.
 *
 * Использует библиотеку libcurl для выполнения HTTP POST запроса с файлом в виде multipart/form-data.
 * После отправки выводит код ответа и текст ответа сервера.
 *
 * @param filename Путь к файлу транспортного пакета, который нужно отправить.
 */
void JsonReader::SendTransportPackage(const std::string& filename) {
    CURL* curl;
    CURLcode res;
    std::string response_string;

    curl = curl_easy_init();
    if (curl) {
    
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1/upload");

        // Set the form data
        curl_mime *form = NULL;
        curl_mimepart *field = NULL;
        form = curl_mime_init(curl);
        
        field = curl_mime_addpart(form);
        curl_mime_name(field, "file");
        curl_mime_filedata(field, filename.c_str());
        
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() ошибка: " << curl_easy_strerror(res) << std::endl;
        } else {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            std::cout << "Код ответа: " << response_code << std::endl;
            std::cout << "Ответ: " << response_string << std::endl;
        }

        // Cleanup
        curl_mime_free(form);
        curl_easy_cleanup(curl);
    }
}
