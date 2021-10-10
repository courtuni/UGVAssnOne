
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

bool IsProcessRunning(const char* processName);
void StartProcesses();

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
	array<UGVProcessHealth>^ ProcessHealthList = gcnew array<UGVProcessHealth>
	{
		{ "Camera",	 0, 0, 10, gcnew Process },
		{ "Display", 0, 0, 10, gcnew Process },
		{ "GPS",	 0, 0, 10, gcnew Process },
		{ "Laser",	 0, 0, 10, gcnew Process },
		{ "VehicleControl", 0, 0, 10, gcnew Process },
	};

	// Creation and access request of shared memory
	PMObj.SMCreate();
	PMObj.SMAccess();

	// Creation of PMData as type Process Management, throughct pData 
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	// Start Diagnostics for all
	for (int i = 0; i < ProcessHealthList->Length; i++)
	{
		if (Process::GetProcessesByName(ProcessHealthList[i].ModuleName)->Length == 0)
		{
			ProcessHealthList[i].ProcessName = gcnew Process;
			// TODO remove magic string for working directory
			ProcessHealthList[i].ProcessName->StartInfo->WorkingDirectory = "C:\\Users\\z5175357\\Source\\Repos\\UGVAssnOne\\Debug";
			// Console::WriteLine("Process" + ProcessHealthList[i].ProcessName->StartInfo->WorkingDirectory + "YAY");
			ProcessHealthList[i].ProcessName->StartInfo->FileName = ProcessHealthList[i].ModuleName;
			ProcessHealthList[i].ProcessName->Start();
			Console::WriteLine("Process " + ProcessHealthList[i].ModuleName + ".exe has started.");
		}
	}

	// Start all 5 modules
	StartProcesses();

	// Main Loop
	while (!_kbhit())
	{
		// Diagnostics
		for (int i = 0; i < ProcessHealthList->Length; i++)
		{
			// Console::WriteLine("Process Management Still Happy");
			Console::WriteLine(ProcessHealthList[i].ModuleName + " process crash count: " + ProcessHealthList[i].CrashCount);
		}
		Console::WriteLine(" ");
		Sleep(500);
	}

	PMData->Shutdown.Status = 0x01;

	//Shutdown processes
	//ShutdownProcesses();

	return 0;
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
			Sleep(10000);
		}
	}
}
