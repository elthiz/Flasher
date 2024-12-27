#ifndef HEXPARSERFIRMWARE_H
#define HEXPARSERFIRMWARE_H

#include <fstream>
#include <iostream>

#include "iparserfirmware.h"

class HexParserFirmware final : public IParserFirmware 
{
    public:
        HexParserFirmware() = default;
        ~HexParserFirmware();

        bool openFirmware( const std::string& firmwarePath ) override;
        std::optional<std::vector<FirmwareData>> parseFirmware() override;
        
    private:
        std::optional<std::vector<FirmwareData>> parseLine( std::string& hexLine );

        std::ifstream fileStream;

        uint32_t addressOffset { 0 };
};

#endif