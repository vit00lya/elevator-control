#include "input_reader.h"

using namespace std::literals;


namespace input_reader {

  std::string InputReader::ParseLine(std::string_view line) {
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
  if (line.empty() || line.length() < 13){
     throw std::invalid_argument("Входящая строка не является штрихкодом"s);
  }

  std::string result = ""s;
  
  if(line[0] == '2' || line[0] == '9' ){
    for (size_t i = 0; i < 13 ; ++i)
      {
	result.push_back(line[i]);
      }
  }
  else if(line[0] == '0'){
       for (size_t i = 2; i < 15 ; ++i)
      {
	result.push_back(line[i]);
      }
  }
  else{
     throw std::invalid_argument("Не верный формат штрихкода"s);
  }

  return result;
  
}


}
 
