#include "network_client.h"
#include <fstream>
#include <iostream>
#include <string>
#include <curl/curl>


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

bool network_client::downloadJsonData(const std::string& url, const std::string& filename) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Инициализация CURL
    curl = curl_easy_init();
    if(curl) {
        // Установка URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
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
        
        // Запись данных в файл
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Ошибка при открытии файла для записи: " << filename << std::endl;
            return false;
        }
        
        outFile << readBuffer;
        outFile.close();
        
        return true;
    }
    
    return false;
}


/**
 * @brief Отправляет транспортный пакет (файл) на сервер по адресу http://127.0.0.1/upload.
 *
 * Использует библиотеку libcurl для выполнения HTTP POST запроса с файлом в виде multipart/form-data.
 * После отправки выводит код ответа и текст ответа сервера.
 *
 * @param filename Путь к файлу транспортного пакета, который нужно отправить.
 */
void network_client::SendTransportPackage(const std::string& filename) {
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
