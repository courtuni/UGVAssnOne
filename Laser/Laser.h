#pragma once
#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

//unsigned long CRC32Value(int i);
//unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class Laser : public UGV_module
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
		~Laser();
	
	public:
		SMObject* LaserSMObj;
		SM_Laser* LaserData;
	public:

		TcpClient^ LaserClient;					// Handle for TCP connection
		NetworkStream^ LaserStream;				// Handle for TCP data stream
		String^ LaserResponseData;				// Handle for raw response

		
		int DataPos;
		INT32 StartingAngle;
		UINT16 AngularStepWidth;
};