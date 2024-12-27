#ifndef CANFLASHER_H
#define CANFLASHER_H

#include "iflasher.h"
#include "canopen.h"

class CanFlasher final : public IFlasher
{
    public:
        CanFlasher() = default;

        bool openInterface( const std::string& name, const uint32_t bitrate ) override;
        bool connect( uint32_t timeoutTime  ) override;
        STATUS_FLASH flash( std::vector<Firmware> firmware ) override;

    private:
        std::optional<uint8_t> getStatus() override;
        bool setStatus( STATUS_REG_VALUE status ) override;
        
        const static uint16_t HEARTBEAT_TIME;
        CanOpen canopen;
};

#endif