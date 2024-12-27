#ifndef IPARSERFIRMWARE_H
#define IPARSERFIRMWARE_H

#include "firmwaredata.h"

#include <vector>
#include <string>
#include <optional>

class IParserFirmware
{
    public:
        virtual ~IParserFirmware() = default;

        virtual bool openFirmware( const std::string& firmwarePath ) = 0;
        virtual std::optional<std::vector<FirmwareData>> parseFirmware() = 0;
};

#endif