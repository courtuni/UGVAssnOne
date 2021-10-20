#pragma once
#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class GPS : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int disconnect() override;
	int authenticateUser(String^ StudID) override;
	int setupSharedMemory() override;
	int getData() override;
	int printData() override;
	int calculateData() override;
	int sendDataToSharedMemory() override;
	bool getShutdownFlag() override;
	int setHeartbeat(bool heartbeat) override;
	int getTimestamp() override;
	~GPS();
protected:
	
	SM_GPS* GPSData;

	TcpClient^ GPSClient;					// Handle for TCP connection
	NetworkStream^ GPSStream;				// Handle for TCP data stream
	String^ GPSResponseData;				// Handle for raw response
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)

};