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
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
		Console::WriteLine("GPS time stamp      : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
		Thread::Sleep(25);
		if (PMData->Shutdown.Status)
			break;
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
	// YOUR CODE HERE
	return 1;
}

int GPS::setupSharedMemory()
{
	// YOUR CODE HERE
	return 1;
}

int GPS::getData()
{
	// YOUR CODE HERE
	return 1;
}

int GPS::checkData()
{
	// YOUR CODE HERE
	return 1;
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
