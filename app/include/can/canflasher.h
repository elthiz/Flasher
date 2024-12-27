#ifndef CANFLASHER_H
#define CANFLASHER_H

#include "canopen.h"

#include "iserialflasher.h"

class CanFlasher final : public ISerialFlasher
{
    public:
        CanFlasher() = default;

        bool openInterface( const std::string& interfaceName, const uint32_t interfaceBitrate ) override;
        bool connect( uint32_t connectTimeoutTimeSeconds  ) override;
        STATUS_FLASHING flash( std::vector<FirmwareData> firmwareDataVector ) override;

    private:
        std::optional<uint8_t> getStatusRegister() override;
        bool setStatusRegister( STATUS_REG_VALUE status ) override;
        
        CanOpen canopen;
};

#endif