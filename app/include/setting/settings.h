#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <array>
#include <map>

class Settings
{
    public:
        /* Тип интерфейса общения с модулями */
        enum TYPE_INTERFACE : int
        {
            CAN = 1
        };
        /* Тип выполнения программы */
        enum TYPE_EXECUTION : int
        {
            WORK = 1, 
            HELP = 2,
            DEFAULT = 3,
        };
        /* Тип прошивки модуля */
        enum TYPE_FIRMWARE : int
        {
            HEX = 1,
        };

        Settings() = default;

        bool parseSettings( int settingsNum, char *settingsRaw[] );

        TYPE_EXECUTION getTypeExecution() const;
        TYPE_INTERFACE getTypeInterface() const;
        TYPE_FIRMWARE getTypeFirmware() const;
        const std::string getFirmwarePath() const;
        const std::string getInterfaceName() const;
        uint32_t getInterfaceBitrate() const;
        uint32_t getConnectTimeoutSeconds() const;

    private:
        TYPE_INTERFACE typeInterface { TYPE_INTERFACE::CAN };
        TYPE_EXECUTION typeExecution { TYPE_EXECUTION::DEFAULT };
        TYPE_FIRMWARE typeFirmware { TYPE_FIRMWARE::HEX };
        
        std::string interfaceName { "can0" };
        uint32_t interfaceBitrate { 500000 };

        std::string firmwarePath {};

        uint32_t connectTimeoutSeconds { 180 };

        bool parseSetting( const std::string& setting );
        bool parseOptionSetting( const std::string& setting );
        bool parseParameterSetting( const std::string& setting, size_t delimiterPos );
};

#endif