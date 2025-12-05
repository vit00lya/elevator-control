#include "elevator_control.h"

using namespace elevator_control;

/// @brief Добавляет штрихкод номенклатуры в систему
/// @param name_product Название номенклатуры
/// @param barcode Штрихкод номенклатуры
void ElevatorControl::AddBarcode(std::string& name_product, std::string& barcode){
       names_products_.push_back(std::move(name_product));
       barcodes_.push_back(std::move(barcode));
       std::string_view name_product_sv = barcodes_.back();
       std::string_view barcode_sv      = names_products_.back();
       barcode_map_[name_product_sv] = barcode_sv;
}

/// @brief Добавляет штрихкод в очередь для отправки
/// @param barcode Штрихкод для отправки
void ElevatorControl::AddBarcodeToSend(std::string& input_string){
     barcodes_to_send_.push_back(input_string);
}

/// @brief Очищает очередь для отправки штрихкодов
void ElevatorControl::ClearBarcodeToSend(){
     barcodes_to_send_.clear();
}

/// @brief Возвращает идентификатор транспортного пакета
/// @return Идентификатор транспортного пакета
long ElevatorControl::GetTransportPacketId(){
  return transport_packet_id_;
}

/// @brief Увеличивает идентификатор транспортного пакета на 1
void ElevatorControl::IncTrasportPacketId(){
  ++transport_packet_id_;
}

/// @brief Проверяет, пуста ли очередь штрихкодов для отправки
/// @return true, если очередь пуста, иначе false
bool ElevatorControl::EmptyBarcodesToSend(){
  return barcodes_to_send_.empty();
}

std::vector<std::string> ElevatorControl::GetBarcodesToSend(){
  return barcodes_to_send_;
}


void ElevatorControl::SaveSettings(Settings& settings){
  settings_ = std::move(settings);
}

Settings ElevatorControl::GetSettings(){
  return settings_;
}

/// @brief Возвращает название продукта по штрихкоду
/// @param barcode Штрихкод продукта
/// @return Название продукта, если штрихкод найден, иначе пустой optional
std::optional<std::string_view> ElevatorControl::GetNameProduct(std::string_view barcode){

  if (barcode.empty()) {
    return {};
  }
  
  auto it = barcode_map_.find(barcode);
  if (it != barcode_map_.end()){
    return it->second;
  }
  
  return {};
}

void ElevatorControl::SendTransportPackage_RoutineAssignment(){
   if (background_thread_sender_.joinable())
  {
    std::cerr << "Фоновый поток уже запущен!" << std::endl;
    return;
  }

  elevator_control::Settings settings = GetSettings();

  // Сбрасываем флаг остановки
  stop_flag_sender_.store(false);

  // Запускаем новый поток
  background_thread_sender_ = std::thread([this, &settings]()
                                          {
        using namespace std::chrono_literals;
        
        while (!stop_flag_sender_.load()) {
            
            // Проверяем еще раз флаг остановки после сна
            if (stop_flag_sender_.load()) {
                break;
            }
            
            try {
              ReadAndDeleteFilesByMask("tranport_package_");
            } catch (const std::exception& e) {
                std::cerr << "Ошибка при создании или отправке транспортного пакета: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Неизвестная ошибка при создании или отправке транспортного пакета" << std::endl;
            }
            // Ждем 10 минут
            std::this_thread::sleep_for(600s);
        } });

  std::cout << "Фоновый поток для отправки пакетов запущен." << std::endl;
}


/// @brief Читает файлы по указанной маске и удаляет их
/// @param mask Маска для поиска файлов
void ElevatorControl::ReadAndDeleteFilesByMask(const std::string& mask) {
    try {
    
        // Определяем директорию для поиска (текущая директория)
        std::filesystem::path searchDir = std::filesystem::current_path();
        
        // Итерируемся по файлам в директории
        for (const auto& entry : std::filesystem::directory_iterator(searchDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Проверяем, соответствует ли файл маске
                if (filename.find(mask) != filename.npos) {
                    std::cout << "Processing file: " << filename << std::endl;
                    
                    // Читаем содержимое файла
                    std::ifstream file(entry.path(), std::ios::binary);
                    if (file.is_open()) {
                        // Читаем содержимое файла
                        std::string content((std::istreambuf_iterator<char>(file)),
                                           std::istreambuf_iterator<char>());
                        file.close();
                        
                        network_client::SendTransportPackage(GetSettings().server_address,content,GetSettings().userpassword);
                        
                        // Удаляем файл
                        if (std::filesystem::remove(entry.path())) {
                            std::cout << "File deleted: " << filename << std::endl;
                        } else {
                            std::cerr << "Failed to delete file: " << filename << std::endl;
                        }
                    } else {
                        std::cerr << "Failed to open file: " << filename << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in ReadAndDeleteFilesByMask: " << e.what() << std::endl;
    }
}

void ElevatorControl::CheckGateBeedsLocked_RoutineAssignment(){
   if (background_thread_beeds_locked_.joinable())
  {
    std::cerr << "Фоновый поток уже запущен!" << std::endl;
    return;
  }

  elevator_control::Settings settings = GetSettings();

  // Сбрасываем флаг остановки
  stop_flag_beeds_locked_.store(false);

  // Запускаем новый поток
  background_thread_beeds_locked_ = std::thread([this, &settings]()
                                          {
        using namespace std::chrono_literals;
        
        while (!stop_flag_beeds_locked_.load()) {
            
            // Проверяем еще раз флаг остановки после сна
            if (stop_flag_beeds_locked_.load()) {
                break;
            }
            
            try {
             
            } catch (const std::exception& e) {
                std::cerr << "Ошибка при создании или отправке транспортного пакета: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Неизвестная ошибка при создании или отправке транспортного пакета" << std::endl;
            }
            // Ждем 10 минут
            std::this_thread::sleep_for(600s);
        } });
std::cout << "Фоновый поток для отправки пакетов запущен." << std::endl;
}

void ElevatorControl::GetDoorIsLock_RoutineAssignment(){
   if (background_thread_door_lock_.joinable())
  {
    std::cerr << "Фоновый поток уже запущен!" << std::endl;
    return;
  }

  Settings settings = GetSettings();

  // Сбрасываем флаг остановки
  stop_flag_door_lock_.store(false);

  // Запускаем новый поток
  background_thread_door_lock_ = std::thread([this, settings]()
                                          {
        using namespace std::chrono_literals;
        
        while (!stop_flag_door_lock_.load()) {
            
            // Проверяем еще раз флаг остановки после сна
            if (stop_flag_door_lock_.load()) {
                break;
            }
            
            try {
                bool door_locked = network_client::DoorIsLocked(settings.server_address, settings.userpassword);
                // Сохраняем результат в переменную
                door_lock_status_ = door_locked;
            } catch (const std::exception& e) {
                std::cerr << "Ошибка при получении статуса блокировки двери: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Неизвестная ошибка при получении статуса блокировки двери" << std::endl;
            }
            // Ждем 10 минут
            std::this_thread::sleep_for(600s);
        } });

  std::cout << "Фоновый поток для проверки блокировки двери запущен." << std::endl;
}

bool ElevatorControl::IsDoorLocked() const {
    return door_lock_status_;
}






