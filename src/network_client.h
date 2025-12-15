#include <string>

namespace network_client{

/**
 * @brief Загрузить данные с сервера и сохранить в файл
 *
 * @param url URL сервера для получения данных
 * @param filename Имя файла для сохранения данных
 * @param error_message Строка для возврата сообщения об ошибке
 * @return true в случае успеха, false в случае ошибки
 */
bool DownloadBarcodeJsonData(const std::string& url, const std::string& filename, const std::string& userpassword, std::string& error_message);
bool SendTransportPackage(const std::string& url, const std::string& data, const std::string& userpassword, std::string& error_message);
bool DoorIsLocked(const std::string& url, const std::string& userpassword, std::string& error_message);
}