
Сделать программу которая будет принимать с сканера штрихкодов строку. 

В проекте существует следующая структура модулей:

- `json` — библиотека для парсинга и вывода JSON;
- `json_reader` — выполняет разбор JSON-данных полученных товаров и настроек 
- `transport_package` - в модуле хранится структура, методы для сохранения и отправки транспортных пакетов
- `xserial` - библиотека для работы с COM-портом, в данном проекте используется для подключения сканера штрихкодов
- `elevator_control` - основной класс содержит список товаров и их штрихкода
- `main` — точка входа в приложение

При запуске программа считывает данные из полученного по http с сервера JSON файла.

Формат JSON:
	{
	}

Оператор которому нужно открыть лифт последовательно считывает штрихкоды.
На дисплее после каждого считывания высвечивается название позиции.
При считывании служебного штрихкода 0000000000000 лифт открывается и данные передаются на HTTP сервер в следующем формате:
	{
	}

Если список пуст тогда выходит надпись, о том что отправлять нечего.
Затем выходит надпись ГОТОВ.

Платформа одноплатный компьютер. В данном проекте использовалась [REPKA-PI-3](https://repka-pi.ru/)
Переферия сканер штрихкодов, 2-х строчный индикатор, реле для нажатия на кнопки лифта.

Для сборки используется CMAKE.

### При разработке использовались библиотеки:
Библиотека для работы с COM-портом [xserial](https://gith11ub.com/NewYaroslav/xserial/tree/master)
