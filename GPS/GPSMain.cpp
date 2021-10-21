#using <System.dll>
#include <Windows.h>
#include <conio.h>

#include <SMObject.h>
#include <smstructs.h>

#include "GPS.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;


int main()
{
	//Declaration
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	//SM Creation and seeking access
	double TimeStamp;
	__int64 Frequency, Counter;
	int Shutdown = 0x00;

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	while (1)
	{
		Console::WriteLine("Checking Health...");
		// Health check
		if (GPSFunctions.getShutdownFlag())
		{
			Console::WriteLine("Shutting down.");
			//break;
		}
		if (_kbhit())
			break;
	}


	return 0;

	//while (1)
	//{
	//	Console::WriteLine("Framework Versions Changing");
	//}
	//return 0;
}

int GPS::connect(String^ hostName, int portNumber)
{
	GPSClient = gcnew TcpClient(HostName, PortNumber);

	GPSClient->NoDelay = true;
	GPSClient->ReceiveTimeout = 500; //ms
	GPSClient->SendTimeout = 500; //ms
	GPSClient->ReceiveBufferSize = 1024;
	GPSClient->SendBufferSize = 1024;


	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	ReceiveData = gcnew array<unsigned char>(5000);

	//NetworkStream^GPSStream = GPSClient->GetStream();
	GPSStream = GPSClient->GetStream();

	return 1;
}

int GPS::setupSharedMemory()
{
	// YOUR CODE HERE
	return 1;
}

int GPS::getData()
{
	Console::WriteLine("In get data, auth is: ");
	Console::WriteLine(AuthData);
	Console::WriteLine("and that's the end");
	String^ AskScan = gcnew String("sRN LMDscandata");

	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	Console::WriteLine("writing byte 0x02...");
	GPSStream->WriteByte(0x02);
	Console::WriteLine("writing byte senddata style...");
	GPSStream->Write(SendData, 0, SendData->Length);
	Console::WriteLine("writing byte 0x03...");
	GPSStream->WriteByte(0x03);

	System::Threading::Thread::Sleep(10);


	ReadData = gcnew array<unsigned char>(2500);

	GPSStream->Read(ReadData, 0, ReadData->Length);
	GPSResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine("is it the response data...");
	Console::WriteLine(GPSResponseData);

	return 1;
}

int GPS::calculateData()
{
	return 1;
}

int GPS::checkData()
{
	// YOUR CODE HERE
	return 1;
}

int GPS::sendDataToSharedMemory()
{
	Console::WriteLine("In send to shared, PMData is: ");
	Console::WriteLine(PMData->Heartbeat.Flags.GPS);
	Console::WriteLine("and auth data is: ");
	Console::WriteLine(AuthData);
	Console::WriteLine("and that's the end");
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
	// YOUR CODE HERE
	return 1;
}

int GPS::setHeartbeat(bool heartbeat)
{
	// YOUR CODE HERE
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
