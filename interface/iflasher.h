#ifndef IFLASHER_H
#define IFLASHER_H

#include <vector>
#include <string>
#include <optional>

#include "firmware.h"

class IFlasher
{
    public:
        enum STATUS_FLASH : int
        {
            STATUS_DONE = 1,
            STATUS_ERROR_WRITE_IN_BOOT = 2,
            STATUS_ERROR_ERASE = 3,
            STATUS_ERROR_WRITE = 4,
        };

        virtual ~IFlasher() = default;
        virtual bool openInterface( const std::string& name, const uint32_t bitrate ) = 0;
        virtual bool connect( uint32_t timeoutTime ) = 0;
        virtual STATUS_FLASH flash( std::vector<Firmware> firmware ) = 0;

    protected:
        enum STATUS_REG_VALUE : int
        {
            DEFAULT = 0b00000000,
            IN_BOOT = 0b00000001,
            NEW_MACHINE_WORD = 0b00000010,
            ERROR_WRITE_IN_BOOT = 0b00000100,
            ERROR_ERASE = 0b00001000,
            ERROR_WRITE = 0b00010000,
        };

        virtual std::optional<uint8_t> getStatus() = 0;
        virtual bool setStatus(STATUS_REG_VALUE status) = 0;
};

#endif