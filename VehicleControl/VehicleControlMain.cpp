#using <System.dll>
#include <Windows.h>
#include <conio.h>
#include "SMObject.h"
#include <smstructs.h>

#include "VehicleControl.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;


int HeartbeatBuffer = 0;
int BufferLimit = 5;


int main()
{
	VehicleControl VehicleControlFunctions;

	VehicleControlFunctions.setupSharedMemory();

	while (1) {

		if (VehicleControlFunctions.getShutdownFlag())
		{
			Console::WriteLine("Got shutdown flag, shutting down.");
			break;
		}

		if (!VehicleControlFunctions.getHeartbeat())
		{
			HeartbeatBuffer = 0;

			while (HeartbeatBuffer < BufferLimit && (!VehicleControlFunctions.getHeartbeat())) {
				HeartbeatBuffer++;
			}
		}

		if ((HeartbeatBuffer == BufferLimit) || _kbhit())
		{
			break;
		}

		// Get timestamp of VehicleControl
		VehicleControlFunctions.getTimestamp();

		// VehicleControl Data
		VehicleControlFunctions.getData();
		VehicleControlFunctions.calculateData();
		VehicleControlFunctions.printData();

		// Set happy heartbeat
		VehicleControlFunctions.setHeartbeat(1);
	}

	return 0;
}

int VehicleControl::getTimestamp()
{
	QueryPerformanceCounter((LARGE_INTEGER*)Counter);
	TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
	Console::WriteLine("VehicleControl time stamp    : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
	return 1;
}

int VehicleControl::connect(String^ HostName, int PortNumber)
{
	return 1;
}

int VehicleControl::disconnect()
{
	return 1;
}

int VehicleControl::authenticateUser(String^ StudID)
{
	return 1;
}

int VehicleControl::setupSharedMemory()
{
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject VehicleControlObj(TEXT("SM_VehicleControl"), sizeof(SM_VehicleControl));

	PMObj.SMAccess();

	VehicleControlObj.SMCreate();
	VehicleControlObj.SMAccess();

	PMData = (ProcessManagement*)PMObj.pData;
	VehicleControlData = (SM_VehicleControl*)VehicleControlObj.pData;

	return 1;
}

int VehicleControl::getData()
{
	//SendData = gcnew array<unsigned char>(16);
	//ReadData = gcnew array<unsigned char>(2500);

	//// Check for known beginning of stream
	//String^ AskScan = gcnew String("sRN LMDscandata");

	//SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

	//VehicleControlStream->WriteByte(0x02);
	//VehicleControlStream->Write(SendData, 0, SendData->Length);
	//VehicleControlStream->WriteByte(0x03);

	//System::Threading::Thread::Sleep(10);

	//// Read in data at that point
	//VehicleControlStream->Read(ReadData, 0, ReadData->Length);
	//ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	return 1;
}

int VehicleControl::calculateData()
{
	// Known sizes from VehicleControl manual
	//NumberEncodersPos = 75;
	//EncoderInfoSize = 8;
	//NumberEncodersSize = 3;
	//NumberChannelsSize = 3;
	//MeasuredDataSize = 6;
	//DataGeneralInfoSize = 21;
	//NumberEncoders = 0; // assuming 0 ENCODERS for now

	//// Calculated positions from VehicleControl manual
	//NumberChannelsPos = NumberEncodersPos + NumberEncoders * EncoderInfoSize + NumberEncodersSize;
	//MeasuredDataPos = NumberChannelsPos + NumberChannelsSize;
	//DataPos = MeasuredDataPos + MeasuredDataSize;
	//XYDataPos = DataPos + DataGeneralInfoSize;

	//// Calculated constants from data
	//StartingAngle = ReadData[DataPos + 10];
	//AngularStepWidth = ReadData[DataPos + 15];

	//// XY calculations from VehicleControl stream
	//for (int i = 0; i < 361; i++)
	//{
	//	// Polar Coordinates
	//	Distance = ReadData[XYDataPos + 2 * i] * 256 + ReadData[XYDataPos + 2 * i + 1];
	//	Sleep(100);

	//	// Cartesian Coordinates put in shared memory
	//	VehicleControlData->x[i] = Distance * cos(StartingAngle + i * AngularStepWidth);
	//	VehicleControlData->y[i] = Distance * sin(StartingAngle + i * AngularStepWidth);
	//}

	return 1;
}

int VehicleControl::printData()
{
	// Print raw data
	//Console::WriteLine(ResponseData);

	//// Print good data
	//for (int i = 0; i < 361; i++)
	//{
	//	Console::WriteLine("x is: " + VehicleControlData->x[i]);
	//	Console::WriteLine("y is: " + VehicleControlData->y[i]);
	//}

	return 1;
}

int VehicleControl::sendDataToSharedMemory()
{
	return 1;
}

bool VehicleControl::getShutdownFlag()
{
	return (PMData->Shutdown.Status || PMData->Shutdown.Flags.VehicleControl);
}

int VehicleControl::getHeartbeat()
{
	return PMData->Heartbeat.Flags.ProcessManagement;
}

int VehicleControl::setHeartbeat(bool heartbeat)
{
	PMData->Heartbeat.Flags.VehicleControl = heartbeat;
	return 1;
}

VehicleControl::~VehicleControl()
{
	// YOUR CODE HERE
}
