#using <System.dll>
#include <Windows.h>
#include <conio.h>

#include <SMObject.h>
#include <smstructs.h>

#include "GPS.h"

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
	// Create instance of GPS Functions
	GPS GPSFunctions;

	// Set up shared memory
	GPSFunctions.setupSharedMemory();
	
	// Connect to UGV
	int PortNumber = 24000;
	String^ HostName = "192.168.1.200";

	Console::WriteLine("Connecting...");
	//GPSFunctions.connect(HostName, PortNumber);

	// Authenticate with Student ID
	String^ StudID = gcnew String("5175357\n");

	Console::WriteLine("Authenticating...");
	//GPSFunctions.authenticateUser(StudID);

	while (1)
	{
		// Health check
		if (GPSFunctions.getShutdownFlag())
		{
			Console::WriteLine("Got shutdown flag, shutting down.");
			break;
		}

		if (!GPSFunctions.getHeartbeat())
		{
			HeartbeatBuffer = 0;

			while (HeartbeatBuffer < BufferLimit && (!GPSFunctions.getHeartbeat())) {
				HeartbeatBuffer++;
			}
		}

		if ((HeartbeatBuffer == BufferLimit) || _kbhit())
		{
			break;
		}

		//GPSFunctions.getTimestamp();

		// GPS Data
		Console::WriteLine("Getting Data...");
		//GPSFunctions.getData();
		Console::WriteLine("Sending Data...");
		//GPSFunctions.sendDataToSharedMemory();
		Console::WriteLine("Printing Data...");
		//GPSFunctions.printData();

		GPSFunctions.setHeartbeat(1);
	}

	return 0;
}

int GPS::getTimestamp()
{
	//QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
	//TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
	//Console::WriteLine("GPS time stamp      : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
	//Thread::Sleep(25);
	return 1;
}

int GPS::connect(String^ HostName, int PortNumber)
{
	GPSClient = gcnew TcpClient(HostName, PortNumber);

	GPSClient->NoDelay = true;
	GPSClient->ReceiveTimeout = 500; //ms
	GPSClient->SendTimeout = 500; //ms
	GPSClient->ReceiveBufferSize = 1024;
	GPSClient->SendBufferSize = 1024;


	//SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	ReceiveData = gcnew array<unsigned char>(5000);

	//NetworkStream^GPSStream = GPSClient->GetStream();
	GPSStream = GPSClient->GetStream();
	return 1;
}


int GPS::disconnect()
{
	GPSStream->Close();
	GPSClient->Close();

	return 1;
}

int GPS::authenticateUser(String^ StudID)
{
	AuthData = gcnew array<unsigned char>(StudID->Length);
	AuthData = System::Text::Encoding::ASCII->GetBytes(StudID);

	GPSStream->Write(AuthData, 0, AuthData->Length);
	System::Threading::Thread::Sleep(100);

	return 1;
}

int GPS::setupSharedMemory()
{
	//TODO: PMObj should be accessed, not created again
	//Declaration of PMObj
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject GPSObj(TEXT("SM_GPS"), sizeof(SM_GPS));

	//SM Creation and seeking access
	PMObj.SMAccess();
	GPSObj.SMCreate();
	GPSObj.SMAccess();

	PMData = (ProcessManagement*)PMObj.pData;
	GPSData = (SM_GPS*)GPSObj.pData;
	
	return 1;
}

int GPS::getData()
{

	String^ AskScan = gcnew String("sRN LMDscandata");

	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

	GPSStream->WriteByte(0x02);
	GPSStream->Write(SendData, 0, SendData->Length);
	GPSStream->WriteByte(0x03);

	System::Threading::Thread::Sleep(10);


	ReadData = gcnew array<unsigned char>(2500);

	GPSStream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(ResponseData);

	return 1;
}

int GPS::calculateData()
{
	return 1;
}

int GPS::printData()
{
	Console::WriteLine("Northing: " + GPSData->northing);
	Console::WriteLine("Easting: " + GPSData->easting);
	Console::WriteLine("Height: " + GPSData->height);

	return 1;
}

int GPS::sendDataToSharedMemory()
{
	// Get header length
	int HeaderLength = ReadData[3];

	// Get data positions - known constants from GPS manual
	int NorthingPos = HeaderLength + 16;
	int EastingPos = HeaderLength + 24;
	int HeightPos = HeaderLength + 32;
	int CRCPos = HeaderLength + 80;

	GPSData->northing = ReadData[NorthingPos];
	GPSData->easting = ReadData[EastingPos];
	GPSData->height = ReadData[HeightPos];

	return 1;
}

bool GPS::getShutdownFlag()
{
	return (PMData->Shutdown.Status || PMData->Shutdown.Flags.GPS);
}

int GPS::getHeartbeat()
{
	return PMData->Heartbeat.Flags.ProcessManagement;
}

int GPS::setHeartbeat(bool heartbeat)
{
	PMData->Heartbeat.Flags.GPS = heartbeat;
	return 1;
}

GPS::~GPS()
{
	// YOUR CODE HERE
}


unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}
