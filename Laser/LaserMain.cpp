#using <System.dll>
#include <Windows.h>
#include <conio.h>

#include <SMObject.h>
#include <smstructs.h>

#include "Laser.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int HeartbeatBuffer = 0;
int BufferLimit = 5;

int main()
{
	// Create instance of Laser Functions
	Laser LaserFunctions;
	LaserFunctions.setupSharedMemory();

	// Connect to UGV
	int PortNumber = 23000;
	String^ HostName = "192.168.1.200";

	Console::WriteLine("Connecting...");
	//LaserFunctions.connect(HostName, PortNumber);

	// Authenticate with Student ID
	String^ StudID = gcnew String("5175357\n");

	Console::WriteLine("Authenticating...");
	//LaserFunctions.authenticateUser(StudID);

	if (!LaserFunctions.WaitForSharedMemory())
	{
		return 0;
	}

	while (1)
	{
		// Shutdown and Heartbeat check

		if (LaserFunctions.getShutdownFlag())
		{
			Console::WriteLine("Got shutdown flag, shutting down.");
			break;
		}

		if (!LaserFunctions.getHeartbeat())
		{
			HeartbeatBuffer = 0;

			while (HeartbeatBuffer < BufferLimit && (!LaserFunctions.getHeartbeat())) {
				HeartbeatBuffer++;
			}
		}

		if ((HeartbeatBuffer >= BufferLimit) || _kbhit())
		{
			break;
		}

		// Get timestamp of Laser
		LaserFunctions.getTimestamp();

		// Laser Data
		//LaserFunctions.getData();
		//LaserFunctions.calculateData();
		//LaserFunctions.printData();

		// Set happy heartbeat
		LaserFunctions.setHeartbeat(1);
	}

	return 0;
}

bool Laser::WaitForSharedMemory()
{
	int MaxRetry = 5;
	int RetryIntervalms = 500;
	int retry = 0;

	while (retry < MaxRetry && this->PMData == NULL)
	{
		Sleep(RetryIntervalms);
		retry++;
	}

	return (this->PMData != NULL);
}

int Laser::getTimestamp()
{
	QueryPerformanceCounter((LARGE_INTEGER*)Counter);
	TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
	Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
	return 1;
}

int Laser::connect(String^ HostName, int PortNumber)
{
	LaserClient = gcnew TcpClient(HostName, PortNumber);

	LaserClient->NoDelay = true;
	LaserClient->ReceiveTimeout = 500; //ms
	LaserClient->SendTimeout = 500; //ms
	LaserClient->ReceiveBufferSize = 1024;
	LaserClient->SendBufferSize = 1024;

	LaserStream = LaserClient->GetStream();

	return 1;
}

int Laser::disconnect()
{
	LaserStream->Close();
	LaserClient->Close();

	return 1;
}

int Laser::authenticateUser(String^ StudID)
{
	AuthData = gcnew array<unsigned char>(StudID->Length);
	AuthData = System::Text::Encoding::ASCII->GetBytes(StudID);

	LaserStream->Write(AuthData, 0, AuthData->Length);
	System::Threading::Thread::Sleep(100);

	return 1;
}

int Laser::setupSharedMemory()
{
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject LaserObj(TEXT("SM_Laser"), sizeof(SM_Laser));

	PMObj.SMAccess();

	LaserObj.SMCreate();
	LaserObj.SMAccess();

	PMData = (ProcessManagement*)PMObj.pData;
	LaserData = (SM_Laser*)LaserObj.pData;

	return 1;
}

int Laser::getData()
{
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);

	// Check for known beginning of stream
	String^ AskScan = gcnew String("sRN LMDscandata");

	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

	LaserStream->WriteByte(0x02);
	LaserStream->Write(SendData, 0, SendData->Length);
	LaserStream->WriteByte(0x03);

	System::Threading::Thread::Sleep(10);

	// Read in data at that point
	LaserStream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	return 1;
}

int Laser::calculateData()
{
	// Known sizes from Laser manual
	NumberEncodersPos = 75;
	EncoderInfoSize = 8;
	NumberEncodersSize = 3;
	NumberChannelsSize = 3;
	MeasuredDataSize = 6;
	DataGeneralInfoSize = 21;
	NumberEncoders = 0; // assuming 0 ENCODERS for now

	// Calculated positions from Laser manual
	NumberChannelsPos = NumberEncodersPos + NumberEncoders * EncoderInfoSize + NumberEncodersSize;
	MeasuredDataPos = NumberChannelsPos + NumberChannelsSize;
	DataPos = MeasuredDataPos + MeasuredDataSize;
	XYDataPos = DataPos + DataGeneralInfoSize;

	// Calculated constants from data
	StartingAngle = ReadData[DataPos + 10];
	AngularStepWidth = ReadData[DataPos + 15];
	
	// XY calculations from Laser stream
	for (int i = 0; i < 361; i++)
	{
		// Polar Coordinates
		Distance = ReadData[XYDataPos + 2 * i] * 256 + ReadData[XYDataPos + 2 * i + 1];
		Sleep(100);

		// Cartesian Coordinates put in shared memory
		LaserData->x[i] = Distance * cos(StartingAngle + i * AngularStepWidth);
		LaserData->y[i] = Distance * sin(StartingAngle + i * AngularStepWidth);
	}

	return 1;
}

int Laser::printData()
{
	// Print raw data
	Console::WriteLine(ResponseData);

	// Print good data
	for (int i = 0; i < 361; i++)
	{
		Console::WriteLine("x is: " + LaserData->x[i]);
		//Console::WriteLine("y is: " + LaserData->y[i]);
	}

	return 1;
}

int Laser::sendDataToSharedMemory()
{
	return 1;
}

bool Laser::getShutdownFlag()
{
	return (this->PMData->Shutdown.Status || this->PMData->Shutdown.Flags.Laser);
}

int Laser::getHeartbeat()
{
	return this->PMData->Heartbeat.Flags.ProcessManagement;
}

int Laser::setHeartbeat(bool heartbeat)
{
	this->PMData->Heartbeat.Flags.Laser = heartbeat;
	return 1;
}

Laser::~Laser()
{
	// YOUR CODE HERE
}
