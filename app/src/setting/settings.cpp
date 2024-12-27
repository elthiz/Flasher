#include "settings.h"

#include <iostream>

#include "messages.h"
#include "out.h"

namespace options
{
    const std::string WORK { "--exec" };
    const std::string HELP { "--help" };
    const std::string CAN { "--can" };
}

namespace parameters
{
    const std::string PATH_FIRMWARE { "--path" };
    const std::string CONNECT_TIMEOUT_SECONDS { "--timeout" };
}

bool Settings::parseSettings( int settingsNum, char *settingsRaw[] )
{
    for ( int argNum = 1; argNum < settingsNum; argNum++ ) 
    {
        if ( !parseSetting( settingsRaw[argNum] ) ) 
        {
            print( msg::ERROR_SETTING_PARSING + settingsRaw[argNum] );
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
        return parseParameterSetting( setting, delimiterPos );
    }
}

bool Settings::parseOptionSetting( const std::string& setting ) 
{
    if ( setting == options::HELP )
    {
        typeExecution = TYPE_EXECUTION::HELP;
        return true;
    }

    if ( setting == options::WORK )
    {
        typeExecution = TYPE_EXECUTION::WORK;
        return true;
    }

    if ( setting == options::CAN )
    {
        typeInterface = TYPE_INTERFACE::CAN;
        return true;
    }

    return false;
}

bool Settings::parseParameterSetting( const std::string& setting, size_t delimiterPos ) 
{
    const std::string key = setting.substr(0, delimiterPos);

    if ( key == parameters::PATH_FIRMWARE )
    {
        firmwarePath = setting.substr( delimiterPos + 1 );
        return true;
    }

    if ( key == parameters::CONNECT_TIMEOUT_SECONDS )
    {
        connectTimeoutSeconds = static_cast<uint32_t>( std::stoul( setting.substr( delimiterPos + 1 ) ) );
        return true;
    }

    return false;
}

Settings::TYPE_EXECUTION Settings::getTypeExecution() const
{
    return typeExecution;
}

Settings::TYPE_INTERFACE Settings::getTypeInterface() const
{
    return typeInterface;
}

Settings::TYPE_FIRMWARE Settings::getTypeFirmware() const
{
    return typeFirmware;
}

const std::string Settings::getFirmwarePath() const
{
    return firmwarePath;
}

uint32_t Settings::getConnectTimeoutSeconds() const
{
    return connectTimeoutSeconds;
}

const std::string Settings::getInterfaceName() const
{
    return interfaceName;
}

uint32_t Settings::getInterfaceBitrate() const
{
    return interfaceBitrate;
}