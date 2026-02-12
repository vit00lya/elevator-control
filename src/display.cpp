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
  size_t line_index = 0;
  for (const auto& line : lines) {
    // Разделяем строку по символу новой строки
    size_t start = 0;
    size_t end = line.find(L'\n');
    
    while (end != std::wstring::npos && line_index < MAX_LINES) {
      std::wstring sub_line = line.substr(start, end - start);
      writeString(0, 5 + HEIGHT_LINE * line_index, sub_line.c_str(), System5x7R);
      ++line_index;
      start = end + 1;
      end = line.find(L'\n', start);
    }
    
    // Выводим оставшуюся часть строки (или всю строку, если \n не найден)
    if (line_index < MAX_LINES) {
      std::wstring sub_line = line.substr(start);
      writeString(0, 5 + HEIGHT_LINE * line_index, sub_line.c_str(), System5x7R);
      ++line_index;
    }
    
    if (line_index >= MAX_LINES) {
      break;
    }
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
  
  // Разделяем строку по символу новой строки
  size_t start = 0;
  size_t end = str.find(L'\n');
  
  while (end != std::wstring::npos) {
    std::wstring line = str.substr(start, end - start);
    
    // Разбиваем длинные строки на части
    if (line.size() <= MAX_CHARS_PER_LINE) {
      result.push_back(line);
    } else {
      size_t iter_count = line.size() / MAX_CHARS_PER_LINE + 1;
      size_t remaining_characters = line.size();
      for(size_t i = 0; i < iter_count && result.size() < MAX_LINES; ++i) {
        size_t end_line = std::min(MAX_CHARS_PER_LINE, remaining_characters);
        result.push_back(line.substr(MAX_CHARS_PER_LINE * i, end_line));
        remaining_characters -= MAX_CHARS_PER_LINE;
        if (remaining_characters <= 0) break;
      }
    }
    
    start = end + 1;
    end = str.find(L'\n', start);
    
    if (result.size() >= MAX_LINES) break;
  }
  
  // Обрабатываем оставшуюся часть строки (или всю строку, если \n не найден)
  if (result.size() < MAX_LINES) {
    std::wstring line = str.substr(start);
    
    // Разбиваем длинные строки на части
    if (line.size() <= MAX_CHARS_PER_LINE) {
      result.push_back(line);
    } else {
      size_t iter_count = line.size() / MAX_CHARS_PER_LINE + 1;
      size_t remaining_characters = line.size();
      for(size_t i = 0; i < iter_count && result.size() < MAX_LINES; ++i) {
        size_t end_line = std::min(MAX_CHARS_PER_LINE, remaining_characters);
        result.push_back(line.substr(MAX_CHARS_PER_LINE * i, end_line));
        remaining_characters -= MAX_CHARS_PER_LINE;
        if (remaining_characters <= 0) break;
      }
    }
  }
  
  // Ограничиваем количество строк до MAX_LINES
  if (result.size() > MAX_LINES) {
    result.resize(MAX_LINES);
  }
  
  return result;
}

/**
 * @brief Выводит текст на дисплей с возможностью ожидания
 * 
 * @param str Текст для вывода
 * @param wait время задержки при выводе сообщения
 * @param countdown если true, то выводится обратный отсчет
 */
void PrintDisplayText(const std::wstring& str, long wait, bool countdown){
  if(wait != 0){
    if (countdown) { 
      for(long long time_wait = 0; time_wait < wait; ++time_wait) {
        clearScreen();
        const std::wstring tmp_str = str + L"\nОсталось: " + std::to_wstring(wait - time_wait) + L" сек";
        std::vector<std::wstring> updated_lines = GetStrings(tmp_str);
        PrintLines(updated_lines);
        syncBuffer();
        delay(1000);
      }
    }
    else{
        clearScreen();
        std::vector<std::wstring> lines = GetStrings(str);
        PrintLines(lines);
        syncBuffer();
        delay(1000 * wait);
    }
  }
}

/**
 * @brief Инициализирует дисплей с заданными настройками
 * 
 * @param settings Ссылка на структуру настроек
 */
void InitDisplay(elevator_control::Settings& settings){

    init(settings.display_width,
        settings.display_height,
        settings.display_pin_reset,
        settings.display_pin_rs,
        settings.display_pin_en,
        settings.display_pin_cs1,
        settings.display_pin_cs2,
        settings.display_pin_d0,
        settings.display_pin_d1,
        settings.display_pin_d2,
        settings.display_pin_d3,
        settings.display_pin_d4,
        settings.display_pin_d5,
        settings.display_pin_d6,
        settings.display_pin_d7,
        settings.display_pin_led
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
void ReleaseWiringRP(int sig){
  clearScreen();
  ledOff();
  releaseWiringRP();
}
