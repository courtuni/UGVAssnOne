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
	int PortNumber = 23000;

	TcpClient^ Client;

	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	array<unsigned char>^ AuthData;

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
		
	/*SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);*/

	NetworkStream^ Stream = Client->GetStream();


	//Authenticate User

	AuthData = System::Text::Encoding::ASCII->GetBytes(StudID);
	Stream->Write(AuthData, 0, AuthData->Length);

	System::Threading::Thread::Sleep(100);

	Stream->Read(ReadData, 0, ReadData->Length);
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(ResponseData);



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
		SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		TimeStamp = (double)Counter / (double)Frequency * 1000; // ms
		Console::WriteLine("Laser time stamp    : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);

		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);

		System::Threading::Thread::Sleep(10);

		Stream->Read(ReadData, 0, ReadData->Length);
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		Console::WriteLine(ResponseData);


		if (PMData->Shutdown.Status)
			break;
		if (_kbhit())
			break;
	}

	Stream->Close();
	Client->Close();
	return 0;
}
