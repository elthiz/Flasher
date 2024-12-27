#ifndef HEXFIRMWARE_H
#define HEXFIRMWARE_H

#include <fstream>
#include <iostream>
#include "ifirmware.h"

class HexFirmware final : public IFirmware 
{
    public:
        HexFirmware() = default;
        ~HexFirmware();

        bool openFirmware( const std::string& path ) override;
        std::optional<std::vector<Firmware>> parseFirmware() override;
        
    private:
        std::optional<std::vector<Firmware>> parseLine( std::string& line );

        std::ifstream fileStream;

        uint32_t addressOffset { 0 };
};

#endif