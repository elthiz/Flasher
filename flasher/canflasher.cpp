#include "canflasher.h"

#include <iostream>

#include "messages.h"
#include "out.h"

const uint16_t CanFlasher::HEARTBEAT_TIME = 50;

const uint8_t MODULE_NODE_ID = 125;

const uint16_t STATUS_REG_INDEX = 0x2000;
const uint8_t STATUS_REG_SUBINDEX = 0x0;
const uint16_t DATA_INDEX = 0x2001;
const uint16_t ADDRESS_INDEX = 0x2001;
const uint8_t DATA_SUBINDEX = 0x1;
const uint8_t ADDRESS_SUBINDEX = 0x0;

bool CanFlasher::openInterface( const std::string& name, const uint32_t bitrate )
{
    if ( !canopen.openInterface( name, bitrate ) ) 
    {
        return false;
    };
    return true;
}

bool CanFlasher::connect( uint32_t timeoutTime )
{
    size_t numberAttempts = ( timeoutTime * 1000 ) / HEARTBEAT_TIME;

    for (size_t i = 0; i < numberAttempts; i++)
    {
        canopen.sendHeartbeat( canopen::OPERATIONAL );
        std::optional<can_frame> frame = canopen.getLastFrameRx();
		if ( frame.has_value() )
		{
            if ( canopen.checkHeartbeat(frame.value(), MODULE_NODE_ID ) )
            {
                if ( setStatus(IFlasher::STATUS_REG_VALUE::IN_BOOT) )
                {
                    return true;
                } 
            }
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_TIME));
    }
    return false;
}

IFlasher::STATUS_FLASH CanFlasher::flash( std::vector<Firmware> firmware )
{
    std::optional<uint8_t> status;
    status = std::nullopt;
    std::vector<Firmware>::iterator iterator = firmware.begin();

    int countFirmware = 0;

    while ( iterator != firmware.end() )
    {
        std::optional<uint8_t> status = getStatus();
        if ( !status.has_value() )
        {
            continue;
        }

        if ( !( status.value() == IFlasher::STATUS_REG_VALUE::IN_BOOT ) )
        {
            if ( !(status.value() & 0b00011100) )
            {
                continue;
            }

            if ( status.value() & IFlasher::STATUS_REG_VALUE::ERROR_WRITE_IN_BOOT )
            {
                std::cout << std::endl;
                return IFlasher::STATUS_FLASH::STATUS_ERROR_WRITE_IN_BOOT;
            }

            if ( status.value() & IFlasher::STATUS_REG_VALUE::ERROR_ERASE )
            {
                std::cout << std::endl;
                return IFlasher::STATUS_FLASH::STATUS_ERROR_ERASE;
            }

            if ( status.value() & IFlasher::STATUS_REG_VALUE::ERROR_WRITE )
            {
                std::cout << std::endl;
                return IFlasher::STATUS_FLASH::STATUS_ERROR_WRITE;
            }
        }

        if ( !canopen.writeSDO
            (
                MODULE_NODE_ID, 
                ADDRESS_INDEX, 
                ADDRESS_SUBINDEX, 
                { static_cast<unsigned char>( ( (*iterator).address >> 24 ) & 0xFF ),
                  static_cast<unsigned char>( ( (*iterator).address >> 16 ) & 0xFF ),
                  static_cast<unsigned char>( ( (*iterator).address >> 8 ) & 0xFF ),
                  static_cast<unsigned char>( (*iterator).address & 0xFF ),
                } 
            ) 
        ) 
        {
            continue;
        };

        if ( !canopen.writeSDO
            (
                MODULE_NODE_ID, 
                DATA_INDEX, 
                DATA_SUBINDEX, 
                { static_cast<unsigned char>( ( (*iterator).data >> 24 ) & 0xFF ),
                  static_cast<unsigned char>( ( (*iterator).data >> 16 ) & 0xFF ),
                  static_cast<unsigned char>( ( (*iterator).data >> 8 ) & 0xFF ),
                  static_cast<unsigned char>( (*iterator).data  & 0xFF ),
                } 
            ) 
        ) 
        {
            continue;
        };

        if ( !setStatus(IFlasher::STATUS_REG_VALUE::NEW_MACHINE_WORD) )
        {
            continue;
        }
        iterator++;
        countFirmware++;
        printProgressBar( countFirmware, msg::PREFIX_PROGRESSBAR + std::to_string(countFirmware) + " / " + std::to_string(firmware.size()), static_cast<int>( firmware.size() ));
    }

    printProgressBar( countFirmware, msg::PREFIX_PROGRESSBAR + std::to_string(countFirmware) + " / " + std::to_string(firmware.size()), static_cast<int>( firmware.size() ));
    canopen.writeSDO(MODULE_NODE_ID, STATUS_REG_INDEX, STATUS_REG_SUBINDEX, {0});
    std::cout << std::endl;
    return IFlasher::STATUS_FLASH::STATUS_DONE;
}

std::optional<uint8_t> CanFlasher::getStatus()
{
    std::optional<std::vector<uint8_t>> statusRegister = canopen.readSDO( MODULE_NODE_ID, STATUS_REG_INDEX, STATUS_REG_SUBINDEX, 1 );
    if ( statusRegister.has_value() )
    {
        uint8_t status = statusRegister.value().at(0);
        return status;
    }
    else
    {
        return std::nullopt;
    }
}

bool CanFlasher::setStatus(IFlasher::STATUS_REG_VALUE status) 
{
    std::optional<std::vector<uint8_t>> statusRegister = canopen.readSDO( MODULE_NODE_ID, STATUS_REG_INDEX, STATUS_REG_SUBINDEX, 1 );
    if ( statusRegister.has_value() )
    {
        uint8_t newStatus = static_cast<uint8_t>( statusRegister.value().at(0) | status );
        canopen.writeSDO( MODULE_NODE_ID, STATUS_REG_INDEX, STATUS_REG_SUBINDEX, { newStatus } );
        return true;
    }
    return false;
}