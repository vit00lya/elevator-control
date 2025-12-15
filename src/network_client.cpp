#include "network_client.h"
#include <fstream>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <sstream>

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
static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(contents, total_size);
    return total_size;
}

bool network_client::DownloadBarcodeJsonData(const std::string& url, const std::string& filename, const std::string& userpassword, std::string& error_message) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string url_plus_prefix = url + "/product";
    // Инициализация CURL
    curl = curl_easy_init();
    if(curl) {
        // Установка URL
        curl_easy_setopt(curl, CURLOPT_URL, url_plus_prefix.c_str());
        curl_easy_setopt(curl, CURLOPT_USERPWD, userpassword.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

        // Установка callback функции для записи данных
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Выполнение запроса
        res = curl_easy_perform(curl);
        
        // Освобождение ресурсов
        curl_easy_cleanup(curl);

         
        // Проверка результата
        if(res != CURLE_OK) {
            std::stringstream error_stream;
            error_stream << "Ошибка при загрузке файла штрихкодов: " << curl_easy_strerror(res);
            error_message = error_stream.str();
            return false;
        }

        long http_code = 0;
        CURLcode info_res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200)
        {
            std::ofstream outFile(filename);
            if (!outFile.is_open()) {
                 std::stringstream error_stream;
                 error_stream << "Ошибка при открытии файла для записи файла штрихкодов: " << filename ;
                 error_message = error_stream.str();
                return false;
            }
            outFile << readBuffer;
            outFile.close();
            return true;
        }
        else{
             std::stringstream error_stream;
             error_stream << "Ошибка при скачвании файла штрихкодов. Код ответа: " << http_code ;
             error_message = error_stream.str();
        }
    }
        
    return false;
}


/**
 * @brief Отправляет транспортный пакет (файл) на сервер по адресу который указан в переменной url.
 *
 * Использует библиотеку libcurl для выполнения HTTP POST запроса с файлом в виде multipart/form-data.
 * После отправки выводит код ответа и текст ответа сервера.
 *
 * @param url Адрес сервера
 * @param data данные транспортного пакета json/xml
 * @param userpassword имя пользователя/пароль
 */
bool network_client::SendTransportPackage(const std::string& url, const std::string& data, const std::string& userpassword, std::string& error_message) {
    CURL* curl;
    CURLcode res;
    std::string response_string;
    std::string url_plus_prefix = url + "/barcodes";

    curl = curl_easy_init();
    if (curl) {
    
        curl_easy_setopt(curl, CURLOPT_URL, url_plus_prefix.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, userpassword.c_str());

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::stringstream error_stream;
            error_stream << "Ошибка при отправке транспортного пакета: " << curl_easy_strerror(res);
            error_message = error_stream.str();
            return false;
        } else {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            std::stringstream response_info;
            response_info << "Код ответа: " << http_code << std::endl;
            response_info << "Ответ: " << response_string ;
            if(http_code != 200) {
                return false;
            }
        }
        // Cleanup
        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}

bool network_client::DoorIsLocked(const std::string& url, const std::string& userpassword, std::string& error_message) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string url_plus_prefix = url + "/door_block";
    // Инициализация CURL
    curl = curl_easy_init();
    if(curl) {
        // Установка URL
        curl_easy_setopt(curl, CURLOPT_URL, url_plus_prefix.c_str());
        curl_easy_setopt(curl, CURLOPT_USERPWD, userpassword.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        // Выполнение запроса
        res = curl_easy_perform(curl);
        
        // Освобождение ресурсов
        curl_easy_cleanup(curl);
  
        // Проверка результата
        if(res != CURLE_OK) {
            std::stringstream error_stream;
            error_stream << "Ошибка при получении статуса блокировки: " << curl_easy_strerror(res);
            error_message = error_stream.str();
            return false;
        }

        long http_code = 0;
        CURLcode info_res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200)
        {
            std::stringstream ss;
            ss << readBuffer;
            std::string tmp = ss.str();
            auto res =  tmp.find("true");
            if (res != std::string::npos){
                return true;
            }
            else{
                return false;
            }
        }
        else{
            std::stringstream error_stream;
            error_stream << "Ошибка при получении статуса блокировки. Код ответа: " << http_code;
            error_message = error_stream.str();
        }
    }
        
    return false;
}
