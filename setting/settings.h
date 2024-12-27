#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <array>
#include <map>


namespace setting
{
    enum INTERFACE : int
    {
        CAN = 1
    };

    enum EXECUTION : int
    {
        WORK = 1, 
        HELP = 2,
        DEFAULT = 3,
    };

    enum FIRMWARE : int
    {
        HEX = 1,
    };
}

class Settings
{
    public:
        Settings() = default;
        bool parseSettings( int settingsNum, char *settingsRaw[] );

        setting::EXECUTION getExecution() const;
        setting::INTERFACE getInterface() const;
        setting::FIRMWARE getTypeFirmware() const;
        const std::string getPathFirmware() const;
        const std::string getNameInterface() const;
        uint32_t getBitrate() const;
        uint32_t getConnectTimeoutTime() const;

    private:
        setting::INTERFACE interface { setting::INTERFACE::CAN };
        setting::EXECUTION execution { setting::EXECUTION::DEFAULT };
        setting::FIRMWARE firmwareType {setting::FIRMWARE::HEX};
        
        std::string nameInterface { "can0" };
        uint32_t bitrate { 500000 };

        std::string pathFirmware {};

        uint32_t connectTimeoutTime { 180 };

        bool parseSetting( const std::string& setting );
        bool parseOptionSetting( const std::string& setting );
        bool parseParametrSetting( const std::string& setting, size_t delimiterPos );
};

#endif