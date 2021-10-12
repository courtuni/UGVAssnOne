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

// stream information from LMS manual
//struct LaserStream {
//	sRA / sSN LMDscandata
//		VersionNumber
//		DeviceNumber
//		SerialNumber
//		DeviceStatus
//		MessageCounter
//		ScanCounter
//		PowerUpDuration
//		TransmissionDuration
//		InputStatus
//		OutputStatus
//		ReservedByteA
//		ScanningFrequency
//		MeasurementFrequency
//		NumberEncoders
//		[EncoderPosition
//		EncoderSpeed]
//	NumberChannels16Bit
//		[MeasuredDataContent
//		ScalingFactor
//		ScalingOffset
//		StartingAngle
//		AngularStepWidth
//		NumberData
//		[Data_1
//		Data_n]]
//	NumberChannels8Bit
//		[MeasuredDataContent
//		ScalingFactor
//		ScalingOffset
//		StartingAngle
//		AngularStepWidth
//		[NumberData
//		Data_1
//		Data_n]
//	Position
//		[XPosition
//		YPosition
//		ZPosition
//		XRotation
//		YRotation
//		ZRotation
//		RotationType]
//	Name
//}

int main()
{
	// Declarations and Memory Setup

	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject LaserObj(TEXT("Laser"), sizeof(SM_Laser));

	double TimeStamp;
	__int64 Frequency, Counter;
	int Shutdown = 0x00;
	int ResponseBufferCount = 0;
	int ResponseBufferLimit = 10;

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);

	//Shared Memory creation and seeking access

	PMObj.SMCreate();
	PMObj.SMAccess();

	LaserObj.SMCreate();
	LaserObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	SM_Laser* LaserData = (SM_Laser*)LaserObj.pData;


	// Laser Data Declarations

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

	// User Authentication

	AuthData = System::Text::Encoding::ASCII->GetBytes(StudID);
	Stream->Write(AuthData, 0, AuthData->Length);

	System::Threading::Thread::Sleep(100);

	Stream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(ResponseData);

	
	while (1)
	{
		// Check status of Process Management

		if (PMData->Heartbeat.Flags.Laser == 1)
		{
			if (ResponseBufferCount > ResponseBufferLimit)
			{
				Console::WriteLine("No Process Management response, shutting down.");
				break;
			}
			else
			{
				ResponseBufferCount++;
			}
		}
		else if (PMData->Shutdown.Status)
		{
			Console::WriteLine("Routine shutdown.");
			break;
		}
		else
		{
			Console::WriteLine("Received Process Management response.");
			ResponseBufferCount = 0;
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
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		Console::WriteLine(ResponseData);


		// Print Laser coordinates in [x,y]

		for (int i = 0; i < 361; i++)
		{
			Console::WriteLine("Point {0,3:F0}: [{0,8:F3},{0,8:F3}]", i, (LaserData->x[356]), (LaserData->y[i]));
		}
		

		// Set Laser heartbeat to 1 (Laser is alive)

		PMData->Heartbeat.Flags.Laser = 1;
		Sleep(100);
	}

	Stream->Close();
	Client->Close();
	return 0;
}
