#ifndef IFIRMWARE_H
#define IFIRMWARE_H

#include <vector>
#include <string>
#include <optional>
#include "firmware.h"

class IFirmware
{
    public:
        virtual ~IFirmware() = default;

        virtual bool openFirmware( const std::string& path ) = 0;
        virtual std::optional<std::vector<Firmware>> parseFirmware() = 0;
        
};

#endif