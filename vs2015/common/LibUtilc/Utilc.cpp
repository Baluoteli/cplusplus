#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "utilc.h"
#include "Interface_Utilc.h"
#include "FileIO.h"
#include "FrameFrequencyCtrl.h"
#include <tlhelp32.h>
#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")
#include <assert.h>
#include <IPExport.h>
#include <IcmpAPI.h>
#pragma comment(lib,"Iphlpapi")
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

namespace plusFCL_BTL
{
	std::string getAbsoluteDir()
	{
		TCHAR path[MAXPATHLEN] = { 0 };
		GetModuleFileName(nullptr, path, MAXPATHLEN);

		std::string filePath = CStringA(path).GetBuffer();
		return filePath.substr(0, filePath.rfind("\\") + 1);
	}

	std::string getFilePath()
	{
		TCHAR path[MAXPATHLEN] = { 0 };
		GetModuleFileName(nullptr, path, MAXPATHLEN);
		return CStringA(path).GetBuffer();
	}

	std::string getCurRunningExeName()
	{
		TCHAR path[MAXPATHLEN] = { 0 };
		GetModuleFileName(nullptr, path, MAXPATHLEN);

		std::string filePath = CStringA(path).GetBuffer();
		return filePath.substr(filePath.rfind("\\") + 1, filePath.length() - filePath.rfind("\\"));
	}

	std::string getFileAbsolutePath(const std::string &file)
	{
		HMODULE hModule = GetModuleHandle(CString(file.c_str()));
		TCHAR path[MAXPATHLEN] = { 0 };
		GetModuleFileName(hModule, path, MAXPATHLEN);
		return CStringA(path).GetBuffer();
	}

	std::string getPirorDir(const std::string &file)
	{
		HMODULE hModule = GetModuleHandle(CString(file.c_str()));
		TCHAR path[MAXPATHLEN] = { 0 };
		GetModuleFileName(hModule, path, MAXPATHLEN);
		std::string fullpath = CStringA(path).GetBuffer();
		return fullpath.substr(0, fullpath.rfind("\\") + 1);
	}

	std::string getRootDir(const std::string &file)
	{
		std::string FileDir = getFileAbsolutePath(file);
		return FileDir.substr(0, FileDir.find("\\") + 1);
	}

	std::string getMediaPath(const std::string &pathSrc, const std::string &pathKey)
	{
		std::string pathDest = pathSrc;
		if (std::string::npos == pathSrc.find(pathKey))
		{
			int nlastSlash = (int)pathSrc.find("/");
			int nNextSlash = (int)pathSrc.find("/",1);
			pathDest = pathDest.replace(nlastSlash, nNextSlash - nlastSlash + 1, pathKey);
		}

		return pathDest;
	}

	std::string MergePath(const std::string &FormatStr, const std::string &category, const std::string &classId /*= ""*/)
	{
		return FormatStr + category + classId;
	}

	DWORD getProcessID(const std::string &processName)
	{//当存在多实例的情形时,返回第一个实例.
		HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hProcessSnap)
		{
			CloseHandle(hProcessSnap);
			return -1;
		}

		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);
			return -1;
		}
		do 
		{
			std::string processNameEnum = CStringA(pe32.szExeFile).GetBuffer();
			if (processNameEnum == processName)
			{
				CloseHandle(hProcessSnap);
				hProcessSnap = INVALID_HANDLE_VALUE;
				return pe32.th32ProcessID;
			}
		} while (Process32Next(hProcessSnap,&pe32));

		CloseHandle(hProcessSnap);
		return -1;
	}

	std::vector<DWORD> getProcessIdMutil(const std::string &processName)
	{
		std::vector<DWORD> vecProcessid;
		HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hProcessSnap)
		{
			return vecProcessid;
		}
		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);     // Must clean up the snapshot object!
			return vecProcessid;
		}

		do 
		{
			vecProcessid.push_back(pe32.th32ProcessID);
			printf("processName: %s, processId: %d\n",CStringA(pe32.szExeFile).GetBuffer(),pe32.th32ProcessID);

		} while (Process32Next(hProcessSnap,&pe32));

		CloseHandle(hProcessSnap);
		return vecProcessid;
	}

	__int64 CompareFileTime(FILETIME time1, FILETIME time2)
	{
		__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
		__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;

		return   (b - a);
	}

	float getCPUPercentAll()
	{
		FILETIME idleTime;
		FILETIME kernelTime;
		FILETIME userTime;
		GetSystemTimes(&idleTime, &kernelTime, &userTime);
		static FILETIME idleTime1 = idleTime;
		static FILETIME kernelTime1 = kernelTime;
		static FILETIME userTime1 = userTime;
		
		int idle = (int)CompareFileTime(idleTime1, idleTime);
		int kernel = (int)CompareFileTime(kernelTime1, kernelTime);
		int user = (int)CompareFileTime(userTime1, userTime);

		if (kernel + user == 0)
			return 0.0;
		//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
		double cpu = (kernel + user - idle) * 100.0 / (kernel + user);

		idleTime1 = idleTime;
		kernelTime1 = kernelTime;
		userTime1 = userTime;
		return (float)cpu;
	}
	
	float getCPUPercent(const std::string &appName)
	{
		return 0.0;
	}

	float getMemPercentAll()
	{
		MEMORYSTATUSEX mse;
		mse.dwLength = sizeof(mse);
		GlobalMemoryStatusEx(&mse);
		return (float)mse.dwMemoryLoad;
	}

	float getMemPercent(const std::string &appName /*= ""*/)
	{
		DWORD processId = -1;
		HANDLE processHandle = INVALID_HANDLE_VALUE;
		if (appName.empty())
		{
			//processHandle = GetCurrentProcess();排除伪句柄带来的操作异常.
			processId = GetCurrentProcessId();
			processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processId);
		}
		else
		{
			processId = getProcessID(appName);
			processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processId);
		}
		if (INVALID_HANDLE_VALUE == processHandle)
		{
			CloseHandle(processHandle);
			return 0.0;
		}
		PROCESS_MEMORY_COUNTERS pmc; SIZE_T processMem = 0;
		if (GetProcessMemoryInfo(processHandle, &pmc, sizeof(pmc)))
		{
			processMem = pmc.WorkingSetSize;//bit
			//printf("processMem : %d\n",processMem/1024/8);
		}

		MEMORYSTATUSEX mse;
		mse.dwLength = sizeof(mse);
		GlobalMemoryStatusEx(&mse);
		DWORDLONG memsize = mse.ullTotalPhys;

		CloseHandle(processHandle);
		return (float)processMem/memsize;
	}

	void closeProcess(const std::string &processName)
	{
		DWORD processId = getProcessID(processName);
		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
		if (INVALID_HANDLE_VALUE != processHandle)
		{
			if (TerminateProcess(processHandle, 0))
			{
				CloseHandle(processHandle);
				return;
			}
			else
			{
				WaitForSingleObject(processHandle, 2000);
			}
		}

		CloseHandle(processHandle);
		return;
	}

	std::string int2str(int nNum)
	{
		char str[MAXPATHLEN] = { 0 };
		_itoa_s(nNum, str, 10);
		return str;
	}
	
	std::string float2str(float fValue)
	{
		char str[MAXPATHLEN] = { 0 };
		sprintf_s(str, "%f", fValue);
		return str;
	}

	std::string gbk2Utf8(const std::string &gbkStr)
	{
		return "";
	}

	std::string  utf82gbk(const std::string &utf8Str)
	{
		return "";
	}

	int str2int(const std::string &str)
	{
		return atoi(str.c_str());
	}

	float str2float(const std::string &str)
	{
		return (float)atof(str.c_str());
	}

	CString s2cs(const std::string &str)
	{
		return CString(str.c_str());
	}

	std::string cs2s(const CString &str)
	{
		CString sTemp(str);
		return CStringA(sTemp.GetBuffer()).GetBuffer();
	}

	std::string getHostIp(const std::string &hostName /*= ""*/)
	{
		//初始化.
		WSAData wd;
		int err = WSAStartup(MAKEWORD(2, 2), &wd);
		if (0 != err)
		{
			return "";
		}
		
		char hostname[MAXPATHLEN] = { 0 }; 
		HOSTENT *pHost = nullptr;
		if (hostName.empty())
		{
			gethostname(hostname, MAXPATHLEN);
			pHost = gethostbyname(hostname);
		}
		else
		{//域名或者IP
			pHost = gethostbyname(hostName.c_str());
		}
		char* p = pHost->h_addr_list[0];
		in_addr addr;
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		char *Ip = inet_ntoa(addr);

		WSACleanup();
		return Ip;
	}

	bool IsAccessible(const std::string &remoteHost)
	{
		std::string Ip = getHostIp(remoteHost);
		ULONG hAddr = inet_addr(Ip.c_str());
		assert(INADDR_NONE != hAddr);
		HANDLE hIP = IcmpCreateFile();
		IP_OPTION_INFORMATION ipIn;
		memset(&ipIn, 0, sizeof(IP_OPTION_INFORMATION));
		ipIn.Ttl = 128;
		unsigned char pSend[36];                                                                   //发送包   
		memset(pSend, 'E', 32);

		int repSize = sizeof(ICMP_ECHO_REPLY) + 32;
		unsigned char pReply[100];                               
		DWORD timeOut = 1000;
		DWORD nPackets = IcmpSendEcho(hIP, (IPAddr)hAddr, pSend, 32, &ipIn, pReply, repSize, timeOut);

		//接收包   
		ICMP_ECHO_REPLY* pEchoReply = (ICMP_ECHO_REPLY*)pReply;
		if(pEchoReply->Status)
		{
			IcmpCloseHandle(hIP);
			return false;
		}
		else
		{
			IcmpCloseHandle(hIP);
			return true;
		}
	}

#ifdef exportUTILC
__IMPLEMENTEXPORTCLASS(CFileIO)
__IMPLEMENTEXPORTCLASS(CFrameCtrl)
__IMPLEMENTEXPORTCLASS(CHighResoluteFrameCtrl)
#endif
}