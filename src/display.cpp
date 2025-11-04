#include "display.h"
// Максимальное количество строк для отображения на экране
static const size_t MAX_LINES = 6;
// Высота одной строки в пикселях
static const size_t HEIGHT_LINE = 10;
// Максимальное количество символов в одной строке
static const size_t MAX_CHARS_PER_LINE = 21;

/**
 * @brief Преобразует строку из кодировки UTF-8 в широкую строку (wchar_t)
 * 
 * @param utf8_str Указатель на строку в кодировке UTF-8
 * @return std::wstring Широкая строка (wchar_t)
 */
std::wstring Utf8ToWchar(const char* utf8_str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8_str);
}

/**
 * @brief Выводит строки на дисплей
 * 
 * @param lines Вектор строк для вывода
 */
void PrintLines(std::vector<std::wstring>& lines){
  for (size_t i = 0; i < lines.size() && i < MAX_LINES; ++i){
    writeString(0, 5 + HEIGHT_LINE * i, lines[i].c_str(), System5x7R);
  }
}

/**
 * @brief Разбивает строку на подстроки с учетом максимального количества символов в строке
 * 
 * @param str Исходная строка
 * @return std::vector<std::wstring> Вектор подстрок
 */
std::vector<std::wstring> GetStrings(const std::wstring& str){

  std::vector<std::wstring> result;
  
  if(str.size() < MAX_CHARS_PER_LINE){
    result.push_back(str);
  }
  else{

    size_t iter_count = str.size() /  MAX_CHARS_PER_LINE + 1;
    size_t remaining_characters = str.size();
    for(size_t i = 0; i < iter_count; ++i) {
      size_t end_line = std::min(MAX_CHARS_PER_LINE,remaining_characters);
      result.push_back(str.substr(MAX_CHARS_PER_LINE * i, end_line));
      remaining_characters -= MAX_CHARS_PER_LINE - 1;
      
    }
  }
  return result;
}

/**
 * @brief Выводит текст на дисплей с возможностью ожидания
 * 
 * @param str Текст для вывода
 * @param wait Флаг, указывающий, нужно ли ждать после вывода (по умолчанию true)
 */
void PrintDisplayText(const std::wstring& str, bool wait = true){
  
  clearScreen();
  std::vector<std::wstring> lines = GetStrings(str);
  PrintLines(lines);
  syncBuffer();
  if(wait){
    delay(5000);
  }
}

/**
 * @brief Инициализирует дисплей с заданными настройками
 * 
 * @param settings Ссылка на структуру настроек
 */
void InitDisplay(elevator_control::Settings& settings){

  if(setupWiringRP(WRP_MODE_PHYS) < 0)
         exit(EXIT_FAILURE);

    init(settings.display_width,
	 settings.display_height,
	 settings.pin_reset,
	 settings.pin_rs,
	 settings.pin_en,
	 settings.pin_cs1,
	 settings.pin_cs2,
	 settings.pin_d0,
	 settings.pin_d1,
	 settings.pin_d2,
	 settings.pin_d3,
	 settings.pin_d4,
	 settings.pin_d5,
	 settings.pin_d6,
	 settings.pin_d7,
	 settings.pin_led
	 );
    ledOn();
    drawBitmap(logo,128,64);
    shiftBufferHorizontal(30);
    syncBuffer();
    delay(5000);
    clearScreen();

}

/**
 * @brief Освобождает ресурсы WiringRP и выключает подсветку дисплея
 */
void ReleaseWiringRP(){
  clearScreen();
  ledOff();
  releaseWiringRP();
}
