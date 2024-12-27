#include "hexparserfirmware.h"

#include <iomanip>

#include "messages.h"

HexParserFirmware::~HexParserFirmware()
{
    fileStream.close();
}

bool HexParserFirmware::openFirmware( const std::string& path )
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

std::optional<std::vector<FirmwareData>> HexParserFirmware::parseFirmware() 
{
    std::vector<FirmwareData> hexFirmwareVector;
    std::string line;
    try
    {
        while ( std::getline( fileStream, line ) )
        {
            std::optional<std::vector<FirmwareData>> hexFirmwareData = parseLine( line ); 
            if ( hexFirmwareData.has_value() )
            {
                hexFirmwareVector.insert( hexFirmwareVector.end(), hexFirmwareData.value().begin(), hexFirmwareData.value().end() );
            }
        }

        return hexFirmwareVector;
    }
    catch(...)
    {
        return std::nullopt;
    }
}

std::optional<std::vector<FirmwareData>> HexParserFirmware::parseLine( std::string& hexLine )
{
    static uint8_t recordType = 0;
    static uint8_t dataSize = 0;
    static uint16_t address = 0;

    /* Убираем стартовое двоеточие */
    hexLine.erase(0, 1); 
    /* Вытягиваем кол-во байтов данных на строку */
    dataSize = static_cast<uint8_t>( std::stoul( hexLine.substr(0, 2), nullptr, 16 ) );
    /* Убираем кол-во байтов */
    hexLine.erase(0, 2); 
    /* Достаем адрес */
    address = static_cast<uint16_t>( std::stoul( hexLine.substr(0, 4), nullptr, 16 ) );
    /* Убираем адрес */
    hexLine.erase(0, 4);
    /* Достаем тип записи */
    recordType = static_cast<uint8_t>( std::stoul( hexLine.substr(0, 2), nullptr, 16 ) );
    /* Убираем тип-записи */
    hexLine.erase(0, 2);
    /* Убираем чек-сумму */
    hexLine.erase(hexLine.size() - 3, 2);

    size_t wordPerLineCount = 0;
    switch ( recordType )
    {
        case 0x04:
            addressOffset = static_cast<uint32_t>( std::stoul( hexLine.substr(0, 4), nullptr, 16 ) );
            break;
        case 0x00:
            std::vector<FirmwareData> firmwareLine;
            wordPerLineCount = dataSize / 4; //TODO: проверка на остаток от деления на 4
            
            for ( size_t i = 0; i < wordPerLineCount; i++ )
            {
                FirmwareData firmwareItem;
                firmwareItem.data = static_cast<uint32_t>( std::stoul( hexLine.substr(0, 8), nullptr, 16 ) );
                firmwareItem.address = ( static_cast<uint32_t>( addressOffset ) << 16) | address;
                firmwareLine.push_back(firmwareItem);
                address += 4;
                hexLine.erase(0, 8);
            }
            return firmwareLine; 
    }
    return std::nullopt;
}