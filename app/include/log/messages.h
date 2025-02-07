#ifndef CONSTANTS_H
#define CONSTANTS_H
 
#include <string>

namespace msg 
{
	inline const std::string ERROR_SETTING_PARSING { "\033[31mНеверный аргумент:\033[0m " };

	inline const std::string ERROR_FIRMWARE_OPENING { "\033[31mФайл с прошивкой не открылся:\033[0m " };
	inline const std::string ERROR_FIRMWARE_PARSING { "\033[31mФайл с прошивкой не получилось распарсить:\033[0m " };

	inline const std::string FIRMWARE_OPENED { "Файл с прошивкой открыт: " };
	inline const std::string FIRMWARE_PARSED { "Файл с прошивкой распарсен, размер прошивки (байт): " };

	inline const std::string ERROR_INTERFACE_OPENING { "\033[31mНе удалось поднять интерфейс:\033[0m " };
	inline const std::string INTERFACE_OPENING { "Интерфейс поднят: " };

	inline const std::string ARE_YOU_SURE { "Начать прошивку? (y - да, n - нет)" };
	inline const std::string ABORT { "\033[31mПрервано\033[0m" };

	inline const std::string CONNECTING { "Время ожидания: " };
	inline const std::string SECOND { " секунд. " };
	inline const std::string WAIT { "Ожидание..." };
	inline const std::string CONNECT_TIMEOUT { "\033[31mТаймаут ожидания ответа от модуля\033[0m" };
	inline const std::string CONNECTED { "Соединение установлено" };

	inline const std::string FLASH_DONE { "Модуль прошит" };
	inline const std::string ERROR_ERASE { "\033[31mОшибка очистки памяти модуля\033[0m" };
	inline const std::string ERROR_WRITE { "\033[31mОшибка записи в память модуля\033[0m" };
	inline const std::string ERROR_WRITE_IN_BOOT { "\033[31mОшибка попытки записи в область загрузчика\033[0m" };

	inline const std::string PREFIX_PROGRESSBAR { "Прошито: " };

	inline const std::string information
    {
        "Использование\n\n"
        "   sudo ./flasher --exec [options] path=<path-to-firmware>\n\n"
        "Укажите путь до прошивки. Запустите программу от root, выключите и включите модуль.\n\n"
		"Выполните sudo ./flasher --help для подробной информации.\n"
    };

	inline const std::string help
    {
        "Использование\n\n"
        "   sudo ./flasher --exec [options] path=<path-to-firmware>\n\n"
        "Укажите путь до прошивки. Запустите программу, выключите и включите модуль.\n\n"
        "Опции:\n"
        "   --help                          вывод команд и инструкции\n"
        "   --exec                          выполнить прошивку\n"
        "   --can                           используется CAN-интерфейс (по умолчанию)\n\n"
        "Параметры:\n"
        "   --path=<path-to-firmware>       путь (абсолютный или относительный) до прошивки\n"
        "   --timeout=<value>               время ожидания соединения с модулем (в секундах). По умолчанию 180 секунд."
    };
}

#endif
