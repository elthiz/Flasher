#include <iomanip>

#include "hexfirmware.h"
#include "messages.h"


HexFirmware::~HexFirmware()
{
    fileStream.close();
}

bool HexFirmware::openFirmware( const std::string& path )
{
    fileStream.open(path);
    if ( fileStream.is_open() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::optional<std::vector<Firmware>> HexFirmware::parseFirmware() 
{
    if ( fileStream.is_open() )
    {
        std::vector<Firmware> hexFirmware;
        std::string line;
        try
        {
            while ( std::getline( fileStream, line ) )
            {
                std::optional<std::vector<Firmware>> firmwareByLine = parseLine( line );
                if ( firmwareByLine.has_value() )
                {
                    hexFirmware.insert( hexFirmware.end(), firmwareByLine.value().begin(), firmwareByLine.value().end() );
                }
            }

            fileStream.close(); //TODO: не очень в трех местах делать одно и то же
            return hexFirmware;
        }
        catch(...)
        {
            fileStream.close();
            return std::nullopt;
        }
    }
    else
    {
        fileStream.close();
        return std::nullopt;
    }
}

std::optional<std::vector<Firmware>> HexFirmware::parseLine( std::string& line )
{
    static uint8_t recordType = 0;
    static uint8_t dataSize = 0;
    static uint16_t address = 0;

    /* Убираем стартовое двоеточие */
    line.erase(0, 1); 
    /* Вытягиваем кол-во байтов данных на строку */
    dataSize = static_cast<uint8_t>( std::stoul( line.substr(0, 2), nullptr, 16 ) );
    /* Убираем кол-во байтов */
    line.erase(0, 2); 
    /* Достаем адрес */
    address = static_cast<uint16_t>( std::stoul( line.substr(0, 4), nullptr, 16 ) );
    /* Убираем адрес */
    line.erase(0, 4);
    /* Достаем тип записи */
    recordType = static_cast<uint8_t>( std::stoul( line.substr(0, 2), nullptr, 16 ) );
    /* Убираем тип-записи */
    line.erase(0, 2);
    /* Убираем чек-сумму */
    line.erase(line.size() - 3, 2);

    size_t wordPerLineCount = 0;
    switch ( recordType )
    {
        case 0x04:
            addressOffset = static_cast<uint32_t>( std::stoul( line.substr(0, 4), nullptr, 16 ) );
            break;
        case 0x00:
            std::vector<Firmware> firmwareLine;
            wordPerLineCount = dataSize / 4; //TODO: проверка на остаток от деления на 4
            
            for ( size_t i = 0; i < wordPerLineCount; i++ )
            {
                Firmware firmwareItem;
                firmwareItem.data = static_cast<uint32_t>( std::stoul( line.substr(0, 8), nullptr, 16 ) );
                firmwareItem.address = ( static_cast<uint32_t>( addressOffset ) << 16) | address;
                firmwareLine.push_back(firmwareItem);
                address += 4;
                line.erase(0, 8);
            }
            return firmwareLine; 
    }
    return std::nullopt;
}