#include "canopen.h"

#include <iostream>
#include <sys/socket.h>

#include <net/if.h>
#include <cstring>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <libsocketcan.h>  

namespace SDO
{
    static const unsigned int SIGN_REQUEST_SDO { 0x600 };
    static const unsigned int SIGN_RESPONSE_SDO { 0x580 };

    static const unsigned int REQUEST_WRITE_SDO { 0x02 };
    static const unsigned int RESPONSE_WRITE_SDO { 0x06 };

    static const unsigned int REQUEST_READ_SDO { 0x04 };
    static const unsigned int RESPONSE_READ_SDO { 0x04 };

    static const float SDO_TIMEOUT { 0.002f };
}

namespace GENERAL
{
	static const unsigned int MASTER_NODE_ID { 127 };
}

namespace HEARTBEAT
{
	static const unsigned int SIGN_HEARTBEAT { 0x700 };
}

CanOpen::~CanOpen() 
{
	closeInterface();
}

bool CanOpen::openInterface( const std::string& interfaceName, const uint32_t bitrate )
{
	if ( _isProcess )
	{
		return false;
	}

	_interfaceName = interfaceName;
	/* Открываем сокет */
	_interfaceSocket = socket( PF_CAN, SOCK_RAW, CAN_RAW ); 
	/* Проверяем открылся ли сокет */
	if ( _interfaceSocket < 0 ) 
	{
		std::cerr << "Ошибка открытия CAN сокета!" << std::endl;
		return false;
	}

	struct ifreq ifr;
	std::strcpy( ifr.ifr_name, _interfaceName.c_str() );

	if ( ioctl( _interfaceSocket, SIOCGIFINDEX, &ifr ) < 0 ) 
	{
		std::cerr << "Ошибка получение информации о CAN интерфейсе!" << std::endl;
		return false;
	}

	struct sockaddr_can addr;
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if ( bind( _interfaceSocket, reinterpret_cast<const sockaddr*>( &addr ), sizeof(addr)) < 0 ) 
	{
		std::cerr << "Ошибка привязки CAN сокета!" << std::endl;
		return false;
	} 

	if ( fcntl( _interfaceSocket, F_SETFL, O_NONBLOCK ) )
	{
		std::cerr << "Ошибка установки асинхронного процесса CAN сокета!" << std::endl;
		return false;
	}

	can_do_stop( _interfaceName.c_str() );
	
	if (can_set_bitrate( _interfaceName.c_str(), bitrate ) != 0) 
	{
		std::cerr << "Ошибка установки битрейта: " << std::endl;
		return false;
	}

	if (can_do_start( _interfaceName.c_str() ) < 0 ) {
	    return false;
	}

	_isProcess = true;
	_featureProcess = std::async(std::launch::async, &CanOpen::process, this);
	return true;
}

void CanOpen::closeInterface()
{
	if ( _featureProcess.valid() )
	{
		_isProcess = false;
		_featureProcess.get();
	}
	close( _interfaceSocket );
}

bool CanOpen::checkSDOWrited( const uint8_t nodeID )
{
    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;

    while ( duration.count() <= SDO::SDO_TIMEOUT )
    {
        duration = std::chrono::high_resolution_clock::now() - start;
		
		std::optional<can_frame> frame = getLastFrameRx();
		if ( frame.has_value() )
		{
			if ( ( frame.value().can_id == SDO::SIGN_RESPONSE_SDO + nodeID ) )
			{
				if ( (frame.value().data[0] >> 4) == SDO::RESPONSE_WRITE_SDO )  
				{
					return true;
				}
			}
		}
    }
    return false;    
}

std::optional<std::vector<uint8_t>> CanOpen::checkSDOReaded( const uint8_t nodeID, const uint8_t size )
{
    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;

    while ( duration.count() <= SDO::SDO_TIMEOUT )
    {
        duration = std::chrono::high_resolution_clock::now() - start;
		
		std::optional<can_frame> frame = getLastFrameRx();
		if ( frame.has_value() )
		{
			if ( ( frame.value().can_id == SDO::SIGN_RESPONSE_SDO + nodeID ) )
			{
				if ( (frame.value().data[0] >> 4) == SDO::RESPONSE_READ_SDO )  
				{
					std::vector<uint8_t> response(size);
					for ( size_t i = 0; i < size; i++ )
					{
						response[i] = frame.value().data[4+i];
					}
					return response;
				}
			}
		}
    }
    return std::nullopt;    
}

bool CanOpen::writeSDO( const uint8_t nodeID, const uint16_t index, const uint8_t subIndex, const std::vector<uint8_t> data )
{
	can_frame frame;
    static uint8_t sizeMask = 0;
	
	switch ( data.size() )
	{
		case 0x1:
			frame.data[4] = data[0];
			sizeMask = 0x0F;
			break;
		case 0x2:
			frame.data[4] = data[1];
			frame.data[5] = data[0];
			sizeMask = 0x0B;
			break;
		case 0x4:
			frame.data[4] = data[3];
			frame.data[5] = data[2];
			frame.data[6] = data[1];
			frame.data[7] = data[0];
			sizeMask = 0x03;
			break;
		default:
			return false;
	}
	
	frame.can_id = SDO::SIGN_REQUEST_SDO + nodeID;
	frame.can_dlc = static_cast<uint8_t>( data.size() + 4 );
	frame.data[0] = static_cast<uint8_t>( ( SDO::REQUEST_WRITE_SDO << 4 ) | (sizeMask & 0x0F) ); 
	frame.data[1] = static_cast<uint8_t>( index & 0xFF );
	frame.data[2] = static_cast<uint8_t>( ( index >> 8 ) & 0xFF );
	frame.data[3] = subIndex;

	appendFrameTx(frame);

	return checkSDOWrited( nodeID );
}

std::optional<std::vector<uint8_t>> CanOpen::readSDO( const uint8_t nodeID, const uint16_t index, const uint8_t subIndex, const uint8_t size )
{
	can_frame frame;
	static uint8_t sizeMask = 0;

	switch ( size )
	{
		case 0x1:
			sizeMask = 0x0F;
			break;
		case 0x2:
			sizeMask = 0x0B;
			break;
		case 0x4:
			sizeMask = 0x03;
			break;
		default:
			return std::nullopt;
	}

	frame.can_id = SDO::SIGN_REQUEST_SDO + nodeID;
	frame.can_dlc = static_cast<uint8_t>( 4 + size );
	frame.data[0] = static_cast<uint8_t>( ( SDO::REQUEST_READ_SDO << 4 ) | (sizeMask & 0x0F) ); 
	frame.data[1] = static_cast<uint8_t>( index & 0xFF );
	frame.data[2] = static_cast<uint8_t>( ( index >> 8 ) & 0xFF );
	frame.data[3] = subIndex;

	appendFrameTx(frame);

	return checkSDOReaded(nodeID, size);
}

bool CanOpen::sendHeartbeat( const canopen::HEARTBEAT_STATUS status )
{
	appendFrameTx( 
	{
		HEARTBEAT::SIGN_HEARTBEAT + GENERAL::MASTER_NODE_ID,
		1,
		0,
		0,
		0,
		{ static_cast<unsigned char>(status), 0, 0, 0 }
	});

	return true;
}

void CanOpen::process()
{
	while ( _isProcess )
	{
		readFromSocket();
		writeToSocket();
	}
}

void CanOpen::readFromSocket()
{
	static ssize_t nbytes = 0;
	static can_frame frame;

	std::lock_guard<std::mutex> lock(_readWriteLock);
	nbytes = read( _interfaceSocket, &frame, sizeof(struct can_frame) );

	if (nbytes > 0) 
	{
		_rxBuffer.push( frame );
	}
}

void CanOpen::writeToSocket()
{
	static can_frame frame;
	
	std::lock_guard<std::mutex> lock( _readWriteLock );
	if ( _txBuffer.empty() )
	{
		return;
	}
	frame = _txBuffer.top();
	_txBuffer.pop();
	write( _interfaceSocket, &frame, sizeof( frame ) );
}

std::optional<can_frame> CanOpen::getLastFrameRx()
{
	std::lock_guard<std::mutex> lock( _readWriteLock );
	if ( _rxBuffer.empty() )
	{
		return std::nullopt;
	} 
	else
	{
		can_frame frame = _rxBuffer.top();
		_rxBuffer.pop();
		return frame;
	}
}

void CanOpen::appendFrameTx( const can_frame& frame )
{
	std::lock_guard<std::mutex> lock(_readWriteLock);
	_txBuffer.push(frame);
}

bool CanOpen::checkHeartbeat( const can_frame& frame, const uint8_t nodeID )
{
	if ( frame.can_id == HEARTBEAT::SIGN_HEARTBEAT + nodeID )
	{
		if ( frame.data[0] == canopen::HEARTBEAT_STATUS::OPERATIONAL )
		{
			return true;
		}
	}
	return false;
}