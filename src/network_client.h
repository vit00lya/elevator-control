#include <string>

namespace network_client{

/**
 * @brief Загрузить данные с сервера и сохранить в файл
 * 
 * @param url URL сервера для получения данных
 * @param filename Имя файла для сохранения данных
 * @return true в случае успеха, false в случае ошибки
 */
bool DownloadJsonData(const std::string& url, const std::string& filename);
void SendTransportPackage(const std::string& filename);
}
#endif // NETWORK_CLIENT_H