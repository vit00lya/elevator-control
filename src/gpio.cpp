#include "gpio.h"

/**
 * @brief Инициализирует дисплей с заданными настройками
 * 
 * @param settings Ссылка на структуру настроек
 */
void InitGpio(elevator_control::Settings& settings){
  if(setupWiringRP(WRP_MODE_PHYS) < 0)
         exit(EXIT_FAILURE);

    if (settings.pin_close_door != 0)
        pinMode(settings.pin_close_door, OUTPUT);
    if (settings.pin_unlock_door != 0)   
        pinMode(settings.pin_unlock_door, OUTPUT);
}