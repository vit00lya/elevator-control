#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <chrono>
#include <thread>
#include "input_reader.h"
#include "elevator_control.h"
#include "json_reader.h"
#include "xserial.hpp"
#include "magic_enum.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>

using namespace std::literals;

#if EXTERNAL_DISPLAY
#include "gpio.h"
#include "display.h"

 void CheckСonditionDoor(elevator_control::ElevatorControl& ec){
  if(ec.IsDoorLocked())
    digitalWrite(ec.GetSettings().pin_unlock_door, HIGH);
  else {
    digitalWrite(ec.GetSettings().pin_unlock_door, LOW);
    PrintDisplayText(L"Доступ открыт");
    }
 }

#endif

void InitLog(){
  // Инициализация log4cpp
  log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
  layout->setConversionPattern("%d [%p] %m%n");
  
  log4cpp::Appender* consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
  consoleAppender->setLayout(layout);
  
  log4cpp::Appender*fileAppender = new log4cpp::RollingFileAppender(
        "file", "elevator_control.log", 5000000, 1  // 5 МБ, 1 архив
    );
  fileAppender->setLayout(layout);
  
  log4cpp::Category& root = log4cpp::Category::getRoot();
  root.setPriority(log4cpp::Priority::INFO);
  root.addAppender(consoleAppender);
  root.addAppender(fileAppender);
}

int main()
{

  InitLog();
  elevator_control::ElevatorControl ec;
  input_reader::InputReader ir;
  jsonreader::JsonReader jr;
  xserial::ComPort scanner;

  jr.LoadSettings(ec);

  elevator_control::Settings settings;
  settings = ec.GetSettings();

  jr.StartBackgroundDownloadBarcode(ec);
  ec.SendTransportPackage_RoutineAssignment();
  ec.GetDoorIsLock_RoutineAssignment();
  
#if EXTERNAL_DISPLAY
  InitGpio(settings);
  InitDisplay(settings);
  signal(SIGINT, ReleaseWiringRP);
#endif
	
  if (settings.scanner_enable)
  {
    if (!scanner.open((short)settings.scanner_num_com_port,
                      (long)settings.scanner_baud_rate,
                      magic_enum::enum_cast<xserial::ComPort::eParity>(settings.scanner_parity).value(),
                      (long)settings.scanner_data_bits,
                      magic_enum::enum_cast<xserial::ComPort::eStopBit>(settings.scanner_stop_bits).value(),
                      0,
                      settings.scanner_linux_com_port))

    {
      #if EXTERNAL_DISPLAY
        PrintDisplayText(L"Не удалось подключить сканер штрихкодов. Возможно он не подключен или настроен другой порт. ");
      #endif
      log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Не удалось подключить сканер штрихкодов. Возможно он не подключен или настроен другой порт.";
      return 1;
    }
  }

  try
  {
    jr.FilligBarcodes(ec);
  }
catch (...)
{
  #if EXTERNAL_DISPLAY
      PrintDisplayText(L"Не возможно прочитать штрихкоды, возможно не верная кодировка файла barcode.json");
  #endif

  log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Не возможно прочитать штрихкоды, возможно не верная кодировка файла barcode.json";
  return 1;
}


  std::string input_string;
  std::string barcode;
while (true)
{
log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Значение блокировки дверей: " << ec.IsDoorLocked();

#if EXTERNAL_DISPLAY
    CheckСonditionDoor(ec);
    if(!ec.IsDoorLocked()){
      delay(5*60*1000);
      continue;
    }
    PrintDisplayText(L"Введите штрихкод");
#endif
    input_string = ""s;
    
    // Таймер для прерывания ввода
    auto start_time = std::chrono::steady_clock::now();
    const auto timeout = std::chrono::seconds(600); // Таймаут 30 секунд
    
    if (settings.scanner_enable)
    {
      while (std::chrono::steady_clock::now() - start_time < timeout) {
        if (scanner.bytesToRead() > 0) { // Проверяем наличие данных
          input_string = scanner.getLine();
          scanner.flushRxAndTx();
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Небольшая задержка
      }
    }
    else
    {
      std::cout << "Введите штрихкод: ";
      std::cin >> input_string;
    }

    if(input_string == ""s) continue;

    try
    {
      barcode = ir.ParseLine(input_string);
      if (barcode == "9999999999999"sv)
      {
        if (ec.EmptyBarcodesToSend())
        {
      #if EXTERNAL_DISPLAY
          PrintDisplayText(L"Список пуст, нечего отправлять.");
      #endif
          log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Список пуст, нечего отправлять.";
          continue;
        }
        try
        {
          std::string name_pack = jr.SaveTransportPackage(ec);
      #if EXTERNAL_DISPLAY
          log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Транспортный пакет записан.";
          PrintDisplayText(L"Транспортный пакет записан.");
          digitalWrite(settings.pin_unlock_door, HIGH); // Даем возможность нажать на кнопку открытия ворот
          log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Доступ открыт.";
          PrintDisplayText(L"Доступ открыт", settings.time_unlock_door);
          digitalWrite(settings.pin_unlock_door, LOW); // Выключаем кнопку открытия ворот
          digitalWrite(settings.pin_close_door, HIGH); // Закрываем ворота.
          delay(1000);
          digitalWrite(settings.pin_close_door, LOW);

      #endif
          log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Транспортный пакет записан." << name_pack;
          continue;
        }
        catch (...)
        {
      #if EXTERNAL_DISPLAY
          PrintDisplayText(L"Ошибка при записи транспортного пакета в файл");
      #endif
          log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Ошибка при записи транспортного пакета в файл."s;
        }
      }
      else
      {
        std::optional<std::string_view> name_product = ec.GetNameProduct(barcode);

        if (name_product.has_value())
        {
      #if EXTERNAL_DISPLAY
          std::string tmp_string = std::string(name_product.value());
          auto text_wstring = Utf8ToWchar(tmp_string.c_str());
          PrintDisplayText(text_wstring.c_str());
      #endif
          log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Переданный файл:" << name_product.value();
        }
        else
        {
      #if EXTERNAL_DISPLAY
          PrintDisplayText(L"Неопознанный штрихкод");
      #endif
          log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Транспортный пакет записан.";
        }
        ec.AddBarcodeToSend(input_string);
      }
    }
    catch (const std::exception &e)
    {
      log4cpp::Category::getRoot() << log4cpp::Priority::ERROR <<  e.what(); 
    }
  }

  return 0;
}
