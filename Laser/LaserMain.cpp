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

int main()
{
	// Create instance of Laser Functions
	Laser LaserFunctions;
	LaserFunctions.setupSharedMemory();

	// Timestamp Variables
	double^ TimeStamp;
	__int64 Frequency, Counter;

	// Shutdown Variable
	//int Shutdown;
		// Connect to UGV
	int PortNumber = 23000;
	String^ HostName = "192.168.1.200";

	Console::WriteLine("Connecting...");
	LaserFunctions.connect(HostName, PortNumber);

	// Authenticate with Student ID
	String^ StudID = gcnew String("5175357\n");

	Console::WriteLine("Authenticating...");
	LaserFunctions.authenticateUser(StudID);

	while (1)
	{
		// Check status of Process Management
		/*if (PMData->Shutdown.Status || PMData->Heartbeat.Flags.ProcessManagement == 1)
		{
			Console::WriteLine("Shutting down.");
			break;
		}*/
		// Health Check
		if (LaserFunctions.getShutdownFlag())
		{
			Console::WriteLine("Shutting down.");
			break;
		}
		if (_kbhit())
			break;

		// Get timestamp of Laser
		LaserFunctions.getTimestamp();

		// Laser Data
		LaserFunctions.getData();
		LaserFunctions.calculateData();
		LaserFunctions.sendDataToSharedMemory();
		LaserFunctions.printData();

		// Scan for beginning of Laser information


		// Read and decode Laser information
		//Stream->Read(ReadData, 0, ReadData->Length);
		//for (int i = 0; i < ReadData->Length; i++)
		//{
		//	Console::WriteLine("Byte { 0 }\t{ 1 }", i, ReadData[i]);
		//}
		//ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		//Console::WriteLine(ResponseData);


		// Print Laser coordinates in [x,y]

		//for (int i = 0; i < 361; i++)
		//{
		//	//LaserData->x[i] = 
		//	//Console::WriteLine("Point {0,3:F0}: [{0,8:F3},{0,8:F3}]", i, (LaserData->x[i]), (LaserData->y[i]));
		//}
		

		// Set Laser heartbeat to 1 (Laser is alive)

		//PMData->Heartbeat.Flags.Laser = 1;
	}


	return 0;
}

int Laser::getTimestamp()
{
	//QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
	//TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
	//Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
	return 1;
}

int Laser::connect(String^ HostName, int PortNumber)
{
	//TcpClient^ LaserClient; //already in ugv module

	LaserClient = gcnew TcpClient(HostName, PortNumber);

	LaserClient->NoDelay = true;
	LaserClient->ReceiveTimeout = 500; //ms
	LaserClient->SendTimeout = 500; //ms
	LaserClient->ReceiveBufferSize = 1024;
	LaserClient->SendBufferSize = 1024;

	//NetworkStream^ Stream = LaserClient->GetStream();
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
	//Declaration of PMObj
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject LaserObj(TEXT("SM_Laser"), sizeof(SM_Laser));

	//SM Creation and seeking access
	Shutdown = 0x00;
	double TimeStamp;
	__int64 Frequency, Counter;
	//int Shutdown = 0x00;

	PMObj.SMCreate();
	PMObj.SMAccess();
	LaserObj.SMCreate();
	LaserObj.SMAccess();

	PMData = (ProcessManagement*)PMObj.pData;
	LaserData = (SM_Laser*)LaserObj.pData;
	//&LaserData 
	//*(LaserData->x) = gcnew array<double>(361);
	LaserData->x[3] = 2;
	Console::WriteLine("changed it to: ", LaserData->x[0]);
	return 1;
}

int Laser::getData()
{
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	ReceiveData = gcnew array<unsigned char>(5000);

	// Check for known beginning of stream
	String^ AskScan = gcnew String("sRN LMDscandata");

	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	
	//Console::WriteLine(LaserClient);
	//LaserStream = LaserClient->GetStream();
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
	//^LaserData->x = gcnew
	//LaserData->x[0]; //= new array<double>(361);
	// Offset calculations
	// Known constants from Laser manual
	int NumberEncodersPos = 75; // const message size until this position
	int EncoderInfoSize = 8;
	int NumberEncodersSize = 3;
	int NumberChannelsSize = 3;
	int MeasuredDataSize = 6; // 5 + 1
	int DataGeneralInfoSize = 21;


			// int NumberEncoders = ReadData[NumberEncodersPos] * 256 + ReadData[NumberEncodersPos + 1];
	int NumberEncoders = 0; // assuming 0 ENCODERS for now
	int NumberChannelsPos = NumberEncodersPos + NumberEncoders * EncoderInfoSize + NumberEncodersSize;
	int MeasuredDataPos = NumberChannelsPos + NumberChannelsSize;
	
	Console::WriteLine("MeasuredDataPos: " + ReadData[MeasuredDataPos]);

	DataPos = MeasuredDataPos + MeasuredDataSize;
	StartingAngle = ReadData[DataPos + 10];
	AngularStepWidth = ReadData[DataPos + 15];

	int XYDataPos = DataPos + DataGeneralInfoSize;
	//double dist = System::Convert::ToInt32(ReadData[DataPos + DataGeneralInfoSize], 16);
	//int dist = (ReadData[DataPos + DataGeneralInfoSize] << 8) | (ReadData[DataPos + DataGeneralInfoSize + 1]);
	double Dist; // ReadData[XYDataPos] * 256 + ReadData[XYDataPos + 1];
	//Console::WriteLine("dist: ", dist);
	
	//SM_Laser* LaserData;

	unsigned char* BytePtrX;
	unsigned char* BytePtrY;
	BytePtrX = (unsigned char*)&(LaserData->x[0]);
	BytePtrY = (unsigned char*)&(LaserData->y[0]);
	double valueX;
	
	for (int i = 0; i < 361; i++)
	{
		double sdist = ReadData[XYDataPos + 2 * i] * 256 + ReadData[XYDataPos + 2 * i + 1];
		Console::WriteLine("value Dist boyz: " + sdist);
		valueX = sdist * cos(StartingAngle + i * AngularStepWidth);
		Console::WriteLine("value X boyz: " + valueX);
		//*(BytePtrX+i) = 
		Sleep(1000);
		*(BytePtrY+i) = sdist * sin(StartingAngle + i * AngularStepWidth);
		//LaserData->x[i] = dist * cos(StartingAngle + i * AngularStepWidth);
		//LaserData->y[i] = dist * sin(StartingAngle + i * AngularStepWidth);
	}

	return 1;
}

int Laser::printData()
{
	// Print raw data
	//Console::WriteLine(ResponseData);

	// Print good data
	for (int i = 0; i < 361; i++)
	{
		//Console::WriteLine("x is: " + LaserData->x[i]);
		//Console::WriteLine("y is: " + LaserData->y[i]);
	}

	return 1;
}

int Laser::sendDataToSharedMemory()
{
	// this is currently in calculate data
	
	// TODO read in goood calculated data not random readdata at good pos
	//for (int i = 0; i < 361; i++)
	//{
	//	//LaserData->x[i] = ReadData[DataPos + i];
	//	//LaserData->y[i] = ReadData[DataPos + i];
	//}

	return 1;
}

bool Laser::getShutdownFlag()
{
	Shutdown = 0;
	/*Console::WriteLine(PMData->Heartbeat.Status);
	Shutdown = PMData->Shutdown.Status;*/
	return Shutdown;
}

int Laser::setHeartbeat(bool heartbeat)
{
	// YOUR CODE HERE
	return 1;
}

Laser::~Laser()
{
	// YOUR CODE HERE
}
