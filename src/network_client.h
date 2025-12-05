#include <string>

namespace network_client{

/**
 * @brief Загрузить данные с сервера и сохранить в файл
 * 
 * @param url URL сервера для получения данных
 * @param filename Имя файла для сохранения данных
 * @return true в случае успеха, false в случае ошибки
 */
bool DownloadBarcodeJsonData(const std::string& url, const std::string& filename, const std::string& userpassword);
bool SendTransportPackage(const std::string& url, const std::string& data, const std::string& userpassword);
bool DoorIsLocked(const std::string& url, const std::string& userpassword);
}