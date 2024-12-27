#ifndef ISERIALFLASHER_H
#define ISERIALFLASHER_H

#include <vector>
#include <string>
#include <optional>

#include "firmwaredata.h"

class ISerialFlasher
{
    public:
        enum STATUS_FLASHING : int
        {
            STATUS_SUCCESS = 1,
            STATUS_ERROR_WRITE_IN_BOOT = 2,
            STATUS_ERROR_ERASE = 3,
            STATUS_ERROR_WRITE = 4,
        };

        virtual ~ISerialFlasher() = default;
        virtual bool openInterface( const std::string& interfaceName, const uint32_t interfaceBitrate ) = 0;
        virtual bool connect( uint32_t connectTimeoutTimeSeconds ) = 0;
        virtual STATUS_FLASHING flash( std::vector<FirmwareData> firmwareDataVector ) = 0;

    protected:
        enum STATUS_REG_VALUE : int
        {
            /* Состояние статусного регистра по умолчанию */
            STATE_DEFAULT = 0b00000000, 
            STAY_IN_BOOT = 0b00000001,
            NEW_DATA = 0b00000010,
            ERROR_WRITE_IN_BOOT = 0b00000100,
            ERROR_ERASE = 0b00001000,
            ERROR_WRITE = 0b00010000,
        };

        virtual std::optional<uint8_t> getStatusRegister() = 0;
        virtual bool setStatusRegister(STATUS_REG_VALUE status) = 0;
};

#endif