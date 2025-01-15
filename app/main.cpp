#include <functional>
#include <map>
#include <iostream>
#include <memory>

#include "include/setting/settings.h"
#include "include/log/messages.h"
#include "include/log/out.h"
#include "include/logic/iparserfirmware.h"
#include "include/logic/iserialflasher.h"
#include "include/can/canflasher.h"
#include "include/parser/hexparserfirmware.h"

static Settings settings;

void flash();
std::optional<std::vector<FirmwareData>> getParsedFirmware();
std::unique_ptr<ISerialFlasher> getSerialFlasher();

int main( int argc, char *argv[] ) 
{
	if ( !settings.parseSettings( argc, argv) )
	{
		return 0;
	};

	switch ( settings.getTypeExecution() )
	{
		case Settings::TYPE_EXECUTION::HELP:
			print(msg::help);
			break;
		case Settings::TYPE_EXECUTION::WORK:
			flash();
			break;
		case Settings::TYPE_EXECUTION::DEFAULT:
			print(msg::information);
			break;
	}

	return 0;
}

std::optional<std::vector<FirmwareData>> getParsedFirmware()
{
    std::unique_ptr<IParserFirmware> ptrParser;

    switch ( settings.getTypeFirmware() )
    {
        case Settings::TYPE_FIRMWARE::HEX:
            ptrParser = std::make_unique<HexParserFirmware>();
            break;
        default:
            return std::nullopt;
    }

    if ( !ptrParser->openFirmware( settings.getFirmwarePath() ) )
	{
		print(msg::ERROR_FIRMWARE_OPENING + settings.getFirmwarePath());
		return std::nullopt;
	}
    print(msg::FIRMWARE_OPENED + settings.getFirmwarePath());

    return ptrParser->parseFirmware();
}

std::unique_ptr<ISerialFlasher> getSerialFlasher( Settings::TYPE_INTERFACE interface )
{
	if ( interface == Settings::TYPE_INTERFACE::CAN )
	{
		return std::make_unique<CanFlasher>();
	}
	return nullptr;
}

void flash()
{
	std::optional<std::vector<FirmwareData>> parsedFirmware = getParsedFirmware();
    if ( !parsedFirmware.has_value() )
    {
        print(msg::ERROR_FIRMWARE_PARSING + settings.getFirmwarePath());
        return;
    }
	print(msg::FIRMWARE_PARSED + std::to_string( parsedFirmware.value().size() ) );
	

	std::unique_ptr<ISerialFlasher> flasher = getSerialFlasher( settings.getTypeInterface() );
	if ( !flasher->openInterface( settings.getInterfaceName(), settings.getInterfaceBitrate() ) )
	{
		print(msg::ERROR_INTERFACE_OPENING + std::to_string( static_cast<int>( settings.getTypeInterface() ) ) );
		return;
	}
	print(msg::INTERFACE_OPENING + settings.getInterfaceName());

	print(msg::ARE_YOU_SURE);
	char response = ' ';
	std::cin >> response;
	if ( !( response == 'y' || response == 'Y' ) )
	{
		print(msg::ABORT);
		return;
	}

	std::cout << msg::CONNECTING << settings.getConnectTimeoutSeconds() <<  msg::SECOND << msg::WAIT << std::endl;
	if ( !flasher->connect( settings.getConnectTimeoutSeconds() ) )
	{
		std::cout << msg::CONNECT_TIMEOUT << std::endl;
		return;
	};
	std::cout << msg::CONNECTED << std::endl;

	ISerialFlasher::STATUS_FLASHING statusFlashed = flasher->flash( parsedFirmware.value() );
	
	switch ( statusFlashed )
	{
		case ISerialFlasher::STATUS_FLASHING::STATUS_SUCCESS:
			std::cout << msg::FLASH_DONE << std::endl;
			break;
		case ISerialFlasher::STATUS_FLASHING::STATUS_ERROR_ERASE:
			std::cout << msg::ERROR_ERASE << std::endl;
			break;
		case ISerialFlasher::STATUS_FLASHING::STATUS_ERROR_WRITE:
			std::cout << msg::ERROR_WRITE << std::endl;
			break;
		case ISerialFlasher::STATUS_FLASHING::STATUS_ERROR_WRITE_IN_BOOT:
			std::cout << msg::ERROR_WRITE_IN_BOOT << std::endl;
			break;
	}
}
