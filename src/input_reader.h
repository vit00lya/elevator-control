#pragma once
#include <string>
#include <string_view>
#include <stdexcept>

namespace input_reader{

    class InputReader {
    public:
      InputReader() = default;
      /**
       * @brief Парсит строку штрихкода и возвращает корректный формат
       *
       * Функция обрабатывает строки штрихкодов следующих форматов:
       * - Начинающиеся с '2' или '9': возвращаются первые 13 символов
       * - Начинающиеся с '0': возвращаются символы с 2-го по 14-й (индексы 2-14)
       *
       * @param line Строка штрихкода для парсинга
       * @return std::string Отформатированная строка штрихкода
       * @throws std::invalid_argument Если строка не является корректным штрихкодом
       */
      std::string ParseLine(std::string_view line);
    };
      

}
