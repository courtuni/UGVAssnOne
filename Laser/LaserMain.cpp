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
	//Declaration of PMObj
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject LaserObj(TEXT("SM_Laser"), sizeof(SM_Laser));
	
	//SM Creation and seeking access
	Shutdown = 0x00;

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	PMObj.SMCreate();
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

		// Scan for beginning of Laser information
		SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);

		System::Threading::Thread::Sleep(10);

		// Read and decode Laser information
		Stream->Read(ReadData, 0, ReadData->Length);
		for (int i = 0; i < ReadData->Length; i++)
		{
			Console::WriteLine("Byte { 0 }\t{ 1 }", i, ReadData[i]);
		}
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

		PMData->Heartbeat.Flags.Laser = 1;
	}

	Stream->Close();
	Client->Close();
	return 0;
}
