
#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <smstructs.h>
#include "SMObject.h"

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Threading;

#define NUM_UNITS 5

int main();
int Diagnostics();
bool IsProcessRunning(const char* processName);
void RestartProcess(int i);
void StartProcesses();

int HeartbeatBuffer = 0;
int BufferLimit = 5;

//defining start up sequence
TCHAR Units[10][20] = //
{
	TEXT("GPS.exe"),
	TEXT("Camera.exe"),
	TEXT("Display.exe"),
	TEXT("VehicleControl.exe"),
	TEXT("Laser.exe")
};

// put in header file?
value struct UGVProcessHealth
{
	String^ ModuleName;
	int Critical;
	int CrashCount;
	int CrashCountLimit;
	Process^ ProcessName;
};



int main()
{
	// Initialisation of shared memory object for Process Management
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	
	// Initialisation of array of UGV Processes' health
	// Order: module name, critical flag, crash count, crash count limit, process name

	// Creation and access request of shared memory
	PMObj.SMCreate();
	PMObj.SMAccess();

	// Creation of PMData as type Process Management, throughct pData 
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;


	// Start all 5 modules
	StartProcesses();
	PMData->SetUp = 1;

	// Main Loop
	while (!_kbhit())
	{
		// Diagnostics
		//int i = 0;

		// Laser
		if ((PMData->Heartbeat.Flags.Laser == CRITICALMASK) && (!PMData->Heartbeat.Flags.Laser))
		{
			break;
		}

		if ((PMData->Heartbeat.Flags.Laser == NONCRITICALMASK) && (!PMData->Heartbeat.Flags.Laser))
		{
			HeartbeatBuffer = 0;
			while (HeartbeatBuffer < BufferLimit && (!PMData->Heartbeat.Flags.Laser)) {
				HeartbeatBuffer++;
			}

			if (HeartbeatBuffer == BufferLimit)
			{
				// Heartbeat wasn't found
				PMData->Shutdown.Flags.Laser = 0x01;
				Sleep(2000);
				

				// Restart process
				RestartProcess(5);

				PMData->Heartbeat.Flags.Laser = 0;
			}
			else
			{
				// Heartbeat was found
				PMData->Heartbeat.Flags.Laser = 0;
			}

		}

		// TODO: other module diagnostics
		// GPS
		// Camera
		// Vehicle
		// Display

		Console::WriteLine("Heartbeats:");
		Console::WriteLine("GPS: " + PMData->Heartbeat.Flags.GPS);
		Console::WriteLine("Camera: " + PMData->Heartbeat.Flags.Camera);
		Console::WriteLine("Display: " + PMData->Heartbeat.Flags.Display);
		Console::WriteLine("VehicleControl: " + PMData->Heartbeat.Flags.VehicleControl);
		Console::WriteLine("Laser: " + PMData->Heartbeat.Flags.Laser);

		Sleep(500);

		// Set happy heartbeat
		PMData->Heartbeat.Status = 1;
	}

	PMData->Shutdown.Status = 0x01;
	return 0;
}

int Diagnostics()
{
	return 1;
}

//Is process running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp((const char *)entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}

void RestartProcess(int i)
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	if (!IsProcessRunning((const char*)Units[i]))
	{
		ZeroMemory(&s[i], sizeof(s[i]));
		s[i].cb = sizeof(s[i]);
		ZeroMemory(&p[i], sizeof(p[i]));

		if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
		{
			printf("%s failed (%d).\n", Units[i], GetLastError());
			_getch();
		}
		std::cout << "Started: " << Units[i] << std::endl;
		Sleep(100);
	}
}


void StartProcesses()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	for (int i = 0; i < NUM_UNITS; i++)
	{
		if (!IsProcessRunning((const char *)Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));

			if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
			}
			std::cout << "Started: " << Units[i] << std::endl;
			Sleep(1000);
		}
	}
}
