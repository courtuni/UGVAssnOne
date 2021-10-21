#using <System.dll>
#include <Windows.h>
#include <conio.h>

#include <SMObject.h>
#include <smstructs.h>

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

	// Connect to UGV
	int PortNumber = 23000;
	String^ HostName = "192.168.1.200";

	Console::WriteLine("Connecting...");
	LaserFunctions.connect(HostName, PortNumber);

	// Authenticate with Student ID
	String^ StudID = gcnew String("5175357\n");

	Console::WriteLine("Authenticating...");
	LaserFunctions.authenticateUser(StudID);

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	//PMObj.SMCreate();
	PMObj.SMAccess();
	LaserObj.SMCreate();
	LaserObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	SM_Laser* LaserData = (SM_Laser*)LaserObj.pData;


	// Port number of Laser
	int PortNumber = 23000;

	TcpClient^ Client;

	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	array<unsigned char>^ AuthData;
	array<unsigned char>^ ReceiveData;

	String^ AskScan = gcnew String("sRN LMDscandata");
	String^ StudID = gcnew String("5175357\n");
	String^ ResponseData;

	Client = gcnew TcpClient("192.168.1.200", PortNumber);

	Client->NoDelay = true;
	Client->ReceiveTimeout = 500; //ms
	Client->SendTimeout = 500; //ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	AuthData = gcnew array<unsigned char>(StudID->Length);
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	ReceiveData = gcnew array<unsigned char>(5000);

	NetworkStream^ Stream = Client->GetStream();


	//Authenticate User

	AuthData = System::Text::Encoding::ASCII->GetBytes(StudID);
	Stream->Write(AuthData, 0, AuthData->Length);

	System::Threading::Thread::Sleep(100);

	Stream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(ResponseData);

	while (1)
	{
		// Check status of Process Management
		/*if (PMData->Shutdown.Status || PMData->Heartbeat.Flags.ProcessManagement == 1)
		{
			Console::WriteLine("Shutting down.");
			break;
		}*/
		if (PMData->Shutdown.Status)
		{
			Console::WriteLine("Shutting down.");
			break;
		}

		// Get timestamp of Laser
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
		Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);

		Console::WriteLine("Setting Heartbeat...");
		LaserFunctions.setHeartbeat(1);

		Console::WriteLine("Setting Heartbeat...");
		LaserFunctions.setHeartbeat(1);

		// Laser Data
		Console::WriteLine("Getting Data...");
		LaserFunctions.getData();
		Console::WriteLine("Sending Data...");
		LaserFunctions.sendDataToSharedMemory();
		Console::WriteLine("Printing Data...");
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


	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	ReceiveData = gcnew array<unsigned char>(5000);

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

	return 1;
}

int Laser::getData()
{
	// Check for known beginning of stream
	Console::WriteLine("asking scan");
	String^ AskScan = gcnew String("sRN LMDscandata");

	Console::WriteLine("setting send data");
	array<unsigned char>^ SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

	Console::WriteLine("writing bytes");
	LaserStream->WriteByte(0x02);
	LaserStream->Write(SendData, 0, SendData->Length);
	LaserStream->WriteByte(0x03);

		System::Threading::Thread::Sleep(10);

	Console::WriteLine("reading in the data");
	// Read in data at that point
	ReadData = gcnew array<unsigned char>(2500);

	Stream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(ResponseData);

		//TODO make proper offset variable
		//int offset = 110;

		// Offset calculations

		int NumberEncodersPos = 75; // const message size until this position
		// int NumberEncoders = ReadData[NumberEncodersPos] * 256 + ReadData[NumberEncodersPos + 1];
		int NumberEncoders = 0; // 0 ENCODERS
		Console::WriteLine(" ReadData[NumberEncodersPos]: " + ReadData[NumberEncodersPos]);
		Console::WriteLine("NumberEncoders: " + NumberEncoders);
		int EncoderInfoSize = 8;
		int NumberEncodersSize = 3;
		int NumberChannelsPos = NumberEncodersPos + NumberEncoders * EncoderInfoSize + NumberEncodersSize;
		int NumberChannelsSize = 3;

		int MeasuredDataPos = NumberChannelsPos + NumberChannelsSize;
		int MeasuredDataSize = 6; // 5 + 1
		Console::WriteLine("MeasuredDataPos: "+ ReadData[MeasuredDataPos]);

		int DataPos = MeasuredDataPos + MeasuredDataSize;
		// note - assuming single channel

		// Scaling Factor
		//int ScalingFactorPos = MeasuredDataPos + MeasuredDataSize;
		//byte b1 = ReadData[ScalingFactorPos];
		//byte b2 = ReadData[ScalingFactorPos + 1];
		//byte b3 = ReadData[ScalingFactorPos + 2];
		//byte b4 = ReadData[ScalingFactorPos + 3];
		//float ScalingFactor = (b1 << 24) | (b2 << 16) | (b3 << 16) | b4;
		//Console::WriteLine(ScalingFactor);

		// Print Laser coordinates in [x,y]

		for (int i = 0; i < 361; i++)
		{
			//LaserData->x[i] = 
			//Console::WriteLine("Point {0,3:F0}: [{0,8:F3},{0,8:F3}]", i, (LaserData->x[i]), (LaserData->y[i]));
		}
		

		// Set Laser heartbeat to 1 (Laser is alive)

int Laser::setHeartbeat(bool heartbeat)
{
	//PMData->Heartbeat.Flags.Laser = heartbeat;
	return 1;
}

	Stream->Close();
	Client->Close();
	return 0;
}
