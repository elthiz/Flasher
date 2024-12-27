#include "settings.h"

#include <iostream>

#include "messages.h"

using namespace setting;

namespace option
{
    const std::string work { "--exec" };
    const std::string help { "--help" };
    const std::string can { "--can" };
}

namespace parametr
{
    const std::string path { "--path" };
    const std::string timeout { "--timeout" };
}

bool Settings::parseSettings( int settingsNum, char *settingsRaw[] )
{
    for ( int i = 1; i < settingsNum; i++ )
    {
        if ( !parseSetting( settingsRaw[i] ) ) 
        {
            std::cout << msg::ERROR_SETTING_PARSING << settingsRaw[i] << std::endl;
            return false;
        };
    }
    return true;
}

bool Settings::parseSetting( const std::string& setting ) 
{
    size_t delimiterPos = setting.find("=");

    if ( delimiterPos == std::string::npos )
    {
        return parseOptionSetting( setting );
    }
    else
    {
        return parseParametrSetting( setting, delimiterPos );
    }
}

bool Settings::parseOptionSetting( const std::string& setting ) 
{
    if ( setting == option::help )
    {
        execution = EXECUTION::HELP;
        return true;
    }

    if ( setting == option::work )
    {
        execution = EXECUTION::WORK;
        return true;
    }

    if ( setting == option::can )
    {
        interface = INTERFACE::CAN;
        return true;
    }

    return false;
}

bool Settings::parseParametrSetting( const std::string& setting, size_t delimiterPos ) 
{
    const std::string key = setting.substr(0, delimiterPos);

    if ( key == parametr::path )
    {
        pathFirmware = setting.substr( delimiterPos + 1 );
        return true;
    }

    if ( key == parametr::timeout )
    {
        connectTimeoutTime = static_cast<uint32_t>( std::stoul( setting.substr( delimiterPos + 1 ) ) );
        return true;
    }

    return false;
}

EXECUTION Settings::getExecution() const
{
    return execution;
}

INTERFACE Settings::getInterface() const
{
    return interface;
}

FIRMWARE Settings::getTypeFirmware() const
{
    return firmwareType;
}

const std::string Settings::getPathFirmware() const
{
    return pathFirmware;
}

uint32_t Settings::getConnectTimeoutTime() const
{
    return connectTimeoutTime;
}

const std::string Settings::getNameInterface() const
{
    return nameInterface;
}

uint32_t Settings::getBitrate() const
{
    return bitrate;
}