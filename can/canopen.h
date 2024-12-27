#ifndef CANOPEN_H
#define CANOPEN_H

#include <stack>
#include <vector>
#include <future>
#include <atomic>
#include <mutex>
#include <optional>

#include <unistd.h>
#include <linux/can/raw.h>

namespace canopen
{
	enum HEARTBEAT_STATUS
	{
		OPERATIONAL = 0x05,
	};
}

class CanOpen
{
	public:
		CanOpen() = default;
		~CanOpen(); 

		bool openInterface( const std::string& interfaceName, const uint32_t bitrate );   
		void closeInterface();

		bool writeSDO( const uint8_t nodeID, const uint16_t index, const uint8_t subIndex, const std::vector<uint8_t> data );
		std::optional<std::vector<uint8_t>> readSDO( const uint8_t nodeID, const uint16_t index, const uint8_t subIndex, const uint8_t size );
		bool sendHeartbeat( const canopen::HEARTBEAT_STATUS status );
		bool checkHeartbeat( const can_frame& frame, const uint8_t nodeID );

		std::optional<can_frame> getLastFrameRx();
		void appendFrameTx( const can_frame& frame );

	private:
		void process();

		void readFromSocket();
		void writeToSocket();

		bool checkSDOWrited( const uint8_t nodeID );
		std::optional<std::vector<uint8_t>> checkSDOReaded( const uint8_t nodeID, const uint8_t size );

		std::string _interfaceName;
		int _interfaceSocket;

		std::atomic<bool> _isProcess { false };
		std::future<void> _featureProcess;

		std::mutex _readWriteLock;  
		std::stack<can_frame> _rxBuffer;   
		std::stack<can_frame> _txBuffer;  
};

#endif