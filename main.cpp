#include <functional>
#include <map>
#include <iostream>
#include <memory>

#include "setting/settings.h"
#include "log/messages.h"
#include "log/out.h"
#include "interface/ifirmware.h"
#include "interface/iflasher.h"
#include "flasher/canflasher.h"
#include "firmware/hexfirmware.h"

static Settings settings;

void flash();
std::unique_ptr<IFirmware> getFirmware( setting::FIRMWARE type );
std::unique_ptr<IFlasher> getFlasher( setting::INTERFACE interface );

int main( int argc, char *argv[] ) 
{
	if ( !settings.parseSettings( argc, argv) )
	{
		return 0;
	};

	switch ( settings.getExecution() )
	{
		case setting::EXECUTION::HELP:
			print(msg::help);
			break;
		case setting::EXECUTION::WORK:
			flash();
			break;
		case setting::EXECUTION::DEFAULT:
			print(msg::information);
			break;
	}

	return 0;
}

std::unique_ptr<IFirmware> getFirmware( setting::FIRMWARE type )
{
	if ( type == setting::FIRMWARE::HEX )
	{
		return std::make_unique<HexFirmware>();
	}
	return nullptr;
}

std::unique_ptr<IFlasher> getFlasher( setting::INTERFACE interface )
{
	if ( interface == setting::INTERFACE::CAN )
	{
		return std::make_unique<CanFlasher>();
	}
	return nullptr;
}

void flash()
{
	std::unique_ptr<IFirmware> firmwareFile = getFirmware( settings.getTypeFirmware() );
	if ( !firmwareFile->openFirmware( settings.getPathFirmware() ) )
	{
		print(msg::ERROR_FIRMWARE_OPENING + settings.getPathFirmware());
		return;
	}
	print(msg::FIRMWARE_OPENED + settings.getPathFirmware());

	std::optional<std::vector<Firmware>> parsedFirmware = firmwareFile->parseFirmware();
	if ( !parsedFirmware.has_value() )
	{
		print(msg::ERROR_FIRMWARE_PARSING + settings.getPathFirmware());
		return;
	}
	print(msg::FIRMWARE_PARSED + std::to_string( parsedFirmware.value().size() ) );

	std::unique_ptr<IFlasher> flasher = getFlasher( settings.getInterface() );
	if ( !flasher->openInterface( settings.getNameInterface(), settings.getBitrate() ) )
	{
		print(msg::ERROR_INTERFACE_OPENING + std::to_string( static_cast<int>( settings.getInterface() ) ) );
		return;
	}
	print(msg::INTERFACE_OPENING + settings.getNameInterface());

	print(msg::ARE_YOU_SURE);
	char response = ' ';
	std::cin >> response;
	if ( !( response == 'y' || response == 'Y' ) )
	{
		print(msg::ABORT);
		return;
	}

	std::cout << msg::CONNECTING << settings.getConnectTimeoutTime() <<  msg::SECOND << msg::WAIT << std::endl;
	if ( !flasher->connect( settings.getConnectTimeoutTime() ) )
	{
		std::cout << msg::CONNECT_TIMEOUT << std::endl;
		return;
	};
	std::cout << msg::CONNECTED << std::endl;

	IFlasher::STATUS_FLASH statusFlashed = flasher->flash( parsedFirmware.value() );
	
	switch ( statusFlashed )
	{
		case IFlasher::STATUS_FLASH::STATUS_DONE:
			std::cout << msg::FLASH_DONE << std::endl;
			break;
		case IFlasher::STATUS_FLASH::STATUS_ERROR_ERASE:
			std::cout << msg::ERROR_ERASE << std::endl;
			break;
		case IFlasher::STATUS_FLASH::STATUS_ERROR_WRITE:
			std::cout << msg::ERROR_WRITE << std::endl;
			break;
		case IFlasher::STATUS_FLASH::STATUS_ERROR_WRITE_IN_BOOT:
			std::cout << msg::ERROR_WRITE_IN_BOOT << std::endl;
			break;
	}
}