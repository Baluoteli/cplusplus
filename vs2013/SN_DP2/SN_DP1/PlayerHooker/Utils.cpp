#include "Utils.h"
#include <psapi.h>
#include <TlHelp32.h>


#pragma warning (disable : 4311)

tstring ExtractFileName( const tstring& strFilePath )
{
	tstring strFileName;
	tstring::size_type nDotPos;
	nDotPos = strFilePath.rfind(_T("\\"));
	if (nDotPos != tstring::npos)
	{
		strFileName = strFilePath.substr(nDotPos + 1, strFilePath.size() - nDotPos - 1);
	}

	return strFileName;
}

BOOL StartupProcess( const TCHAR* pProcessName )
{
	BOOL bWorked;
	STARTUPINFO suInfo;
	PROCESS_INFORMATION procInfo;

	memset (&suInfo, 0, sizeof(suInfo));
	suInfo.cb = sizeof(suInfo);

	bWorked = ::CreateProcess(pProcessName, NULL, NULL, NULL, FALSE,
		NORMAL_PRIORITY_CLASS, NULL, NULL, &suInfo, &procInfo);
	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);
	return bWorked;
}

BOOL IsProcessRunning( const TCHAR* pProcessName )
{
	HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD dwsma = GetLastError();
	DWORD dwExitCode = 0;


	PROCESSENTRY32 procEntry = {0};
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hndl, &procEntry);
	do
	{
		HANDLE hHandle;
		hHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procEntry.th32ProcessID);
		if (hHandle == NULL)
		{
			continue;
		}

		TCHAR processFileName[MAX_PATH];
		GetModuleFileNameEx(hHandle, NULL, processFileName, sizeof(processFileName));
		CloseHandle(hHandle);
		if (_tcsicmp(processFileName, pProcessName) == 0)
		{
			return TRUE;
		}
	}
	while (Process32Next(hndl, &procEntry));

	if (hndl != INVALID_HANDLE_VALUE)
	{
		 CloseHandle(hndl);
	}

	return FALSE;
}

BOOL KillProcess( LPTSTR pProcessName )
{
	HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD dwsma = GetLastError();
	DWORD dwExitCode = 0;
	vector<DWORD> terminateProcessIDs;

	PROCESSENTRY32 procEntry = {0};
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hndl, &procEntry);
	do
	{
		HANDLE hHandle;
		hHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procEntry.th32ProcessID);
		if (hHandle == NULL)
		{
			continue;
		}

		TCHAR processFileName[MAX_PATH];
		GetModuleFileNameEx(hHandle, NULL, processFileName, sizeof(processFileName));
		CloseHandle(hHandle);
		if (_tcsicmp(processFileName, pProcessName) == 0)
		{
			terminateProcessIDs.push_back(procEntry.th32ProcessID);
		}
	}
	while (Process32Next(hndl, &procEntry));

	vector<DWORD>::iterator itr = terminateProcessIDs.begin();
	for (; itr != terminateProcessIDs.end(); ++itr)
	{
		HANDLE hHandle = ::OpenProcess(PROCESS_TERMINATE, 0, *itr); 
		if (hHandle != NULL)
		{
			::GetExitCodeProcess(hHandle, &dwExitCode);
			::TerminateProcess(hHandle, dwExitCode);
			CloseHandle(hHandle);
		}
	}
	
	if (hndl != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hndl);
	}

	Sleep(100);
	return !terminateProcessIDs.empty();
}