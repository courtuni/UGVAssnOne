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
		int getHeartbeat() override;
		int setHeartbeat(bool heartbeat) override;
		int getTimestamp() override;
		~Laser();
	
	public:

		SM_Laser* LaserData;

		TcpClient^ LaserClient;					// Handle for TCP connection
		NetworkStream^ LaserStream;				// Handle for TCP data stream

		
		int DataPos;
		INT32 StartingAngle;
		UINT16 AngularStepWidth;

		int NumberEncodersPos;
		int EncoderInfoSize;
		int NumberEncodersSize;
		int NumberChannelsSize;
		int MeasuredDataSize;
		int DataGeneralInfoSize;
		int NumberEncoders;
		int NumberChannelsPos;
		int MeasuredDataPos;
		int XYDataPos;

		double Distance;
};