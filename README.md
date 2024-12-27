# Flasher
Linux утилита по обновлению ПО для K15.Bootloader.
## Оглавление
0. [Цель](#Цель)
1. [Сборка проекта](#Сборка-прокета)
2. [Алгоритм работы](#Алгоритм-работы)
    1. [Алгоритм CAN-модуля](#Алгоритм-Can-модуля)

## Цель
Flasher - Linux утилита по обновлению ПО для линейки модулей K15 с pагрузчиком K15.Bootloader. Семейство поддерживаемых МК такой же, как и у K15.Bootloader. Поддерживаемые интерфейсы: CAN/CanOpen. Поддерживаемые форматы прошивок: Intel Hex.

## Сборка проекта
Внутри директории с исходниками создайте папку .build (или любое другое название). Перейдите в неё и сгенерируйте файлов сборки командой:
```bash
cmake ..
```
Затем соберите проект командой:
```bash
cmake --build .
```
После этого в директории .build появится исполняемый файл *Flasher*.

Прим.: Если необходимо отключить вывод предупреждений компилятора, передайте опцию:
```bash
cmake .. -D COMPILE_WARNINGS=OFF
```

Полученный исходный файл является конечным приложением. Все необходимые библиотеки интерфейсов собираются статически.

## Алгоритм работы
1. Укажите режим работы опцией: --exec.
2. Укажите используемый интерфейс опциями (только одно!): --can. По умолчанию: --can.
3. Укажите путь до прошивки параметром: --path=<путь>.
4. Укажите, если необходимо, тайм-аут на соединение параметром: --timeout=<время-в-секундах>
Примеры вызова:
Прошивка модуля прошивкой MODULE_NAME.hex, взятой из папки *hex* в директории с приложением
```bash
sudo ./Flasher --exec --path=hex/MODULE_NAME.hex
```
Прошивка модуля прошивкой MODULE_NAME.hex, взятой по абсолютному пути, с тайм-аутом в 300 секунд, по CAN интерфейсу
```bash
sudo ./Flasher --exec --path=/home/firefly/flasher/hex/MODULE_NAME.hex --timeout=300 --can
```
Вывод списка команд
```bash
sudo ./Flasher --help
```

### Алгоритм CAN-модуля
При старте соединения, утилита отправляет свой heartbeat с периодичность в 50ms и слушает шину, ожидая поймать ответный heartbeat от ***nodeID = 125***.
Если heartbeat пойман, утилита записывает в статусный регистр загрузчика значение ***STAY_ON_BOOT***.

Возможные значения статусного регистра на запись/чтение:
```c++
enum STATUS_REG_VALUE : int
{
    STATE_DEFAULT = 0b00000000, 
    STAY_IN_BOOT = 0b00000001,
    NEW_DATA = 0b00000010,
    ERROR_WRITE_IN_BOOT = 0b00000100,
    ERROR_ERASE = 0b00001000,
    ERROR_WRITE = 0b00010000,
};
```
После этого утилита начинает прошивку модуля. Сперва отправляется адрес, затем 4 байта данных. После в статусный регистр записывается ***NEW_DATA*** и ожидается сброс этого статуса. Действия повторяются до полной прошивки.

При ошибки утилита разрывает соединение, освобождает интерфейс и отдает статус ошибки, полученный из загрузчика.

## Пока всё!
