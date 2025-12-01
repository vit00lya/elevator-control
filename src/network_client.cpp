#include "network_client.h"
#include <fstream>
#include <iostream>
#include <string>
#include <curl/curl.h>


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

bool network_client::DownloadBarcodeJsonData(const std::string& url, const std::string& filename, const std::string& userpassword) {
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
            std::cerr << "Ошибка при выполнении запроса: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        long http_code = 0;
        CURLcode info_res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 200)
        {
            std::ofstream outFile(filename);
            if (!outFile.is_open()) {
                std::cerr << "Ошибка при открытии файла для записи: " << filename << std::endl;
                return false;
            }
            outFile << readBuffer;
            outFile.close();  
            std::cerr << "Штрихкоды сохранены в файле: " << filename << std::endl;
            return true;
        }
        else{
             std::cerr << "Ошибка при выполнении запроса. Код ответа: " << http_code << std::endl;
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
bool network_client::SendTransportPackage(const std::string& url, const std::string& data, const std::string& userpassword) {
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
            std::cerr << "curl_easy_perform() ошибка: " << curl_easy_strerror(res) << std::endl;
            return false;
        } else {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            std::cout << "Код ответа: " << http_code << std::endl;
            std::cout << "Ответ: " << response_string << std::endl;
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
