#pragma once

#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

//unsigned long CRC32Value(int i);
//unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class VehicleControl : public UGV_module
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
	~VehicleControl();

public:
	SM_VehicleControl* VehicleControlData;

};