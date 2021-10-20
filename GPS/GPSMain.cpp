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
	// Create instance of GPS Functions
	GPS GPSFunctions;
	GPSFunctions.setupSharedMemory();
	
	// Port number of GPS
	int PortNumber = 24000;
	String^ HostName = "192.168.1.200";

	GPSFunctions.connect(HostName, PortNumber);

	// Student ID
	String^ StudID = gcnew String("5175357\n");
	GPSFunctions.authenticateUser(StudID);

	// Timestamp Variables
	double^ TimeStamp;
	__int64 Frequency, Counter;

	// Shutdown Variable
	int Shutdown;

	while (1)
	{
		TimeStamp = GPSFunctions.getTimestamp();
		Console::WriteLine("GPS time stamp      : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);

		if (PMData->Shutdown.Status)
			break;
		if (_kbhit())
			break;
	}

	return 0;
}

double^ GPS::getTimestamp()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
	TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
	Thread::Sleep(25);
}

int GPS::connect(String^ HostName, int PortNumber)
{
	TcpClient^ Client;

	//array<unsigned char>^ SendData;
	//array<unsigned char>^ ReadData;
	//array<unsigned char>^ ReceiveData;

	String^ AskScan = gcnew String("sRN LMDscandata");
	
	

	Client = gcnew TcpClient(HostName, PortNumber);

	Client->NoDelay = true;
	Client->ReceiveTimeout = 500; //ms
	Client->SendTimeout = 500; //ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	ReceiveData = gcnew array<unsigned char>(5000);

	NetworkStream^ Stream = Client->GetStream();

	// YOUR CODE HERE
	return 1;
}

int GPS::authenticateUser(String^ StudID)
{
	
	//array<unsigned char>^ AuthData;
	

	AuthData = gcnew array<unsigned char>(StudID->Length);

	AuthData = System::Text::Encoding::ASCII->GetBytes(StudID);

	Stream->Write(AuthData, 0, AuthData->Length);

	System::Threading::Thread::Sleep(100);


}

int GPS::setupSharedMemory()
{
	//Declaration of PMObj
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject GPSObj(TEXT("SM_GPS"), sizeof(SM_GPS));

	//SM Creation and seeking access
	Shutdown = 0x00;
	double TimeStamp;
	__int64 Frequency, Counter;
	//int Shutdown = 0x00;

	PMObj.SMCreate();
	PMObj.SMAccess();
	GPSObj.SMCreate();
	GPSObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	SM_GPS* GPSData = (SM_GPS*)GPSObj.pData;
	
	return 1;
}

int GPS::getData()
{
	Stream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(ResponseData);

	// YOUR CODE HERE
	return 1;
}

int GPS::checkData()
{
	// YOUR CODE HERE
	return 1;
}

int GPS::printData()
{
	// Get header length
	int HeaderLength = ReadData[3];

	// declare data positions
	int NorthingPos = HeaderLength + 16;
	int EastingPos = HeaderLength + 24;
	int HeightPos = HeaderLength + 32;
	int CRCPos = HeaderLength + 80;


	GPSData->Northing
	// take northing, easting, crc
	Console::WriteLine("Northing: " + ReadData[NorthingPos]);
	Console::WriteLine("Easting: " + ReadData[EastingPos]);
	Console::WriteLine("Height: " + ReadData[HeightPos]);
	Console::WriteLine("CRC: " + ReadData[CRCPos]);

}

int GPS::sendDataToSharedMemory()
{
	// YOUR CODE HERE
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
