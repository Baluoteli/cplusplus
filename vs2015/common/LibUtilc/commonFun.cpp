#include "stdafx.h"
#include "commonFun.h"

namespace plusFCL_BTL
{
	char* getAbsoluteDir()
	{
		return __FUNCDNAME__;
	}

	char* getFilePath()
	{
		return __FUNCDNAME__;
	}

	char* getRelativeDir()
	{
		return __FUNCDNAME__;
	}

	char* getCurRunningExeName()
	{
		return __FUNCDNAME__;
	}

	char* getFileDir(const std::string &file)
	{
		return __FUNCDNAME__;
	}

	char* getPirorDir(const std::string &file)
	{
		return __FUNCDNAME__;
	}

	char* getRootDir(const std::string &file)
	{
		return __FUNCDNAME__;
	}

	std::string getPath(const std::string &pathSrc, const std::string &pathKey)
	{
		return __FUNCDNAME__;
	}

	DWORD getProcessID(const std::string &processName)
	{
		return 0;
	}
	
	float getCPUPercent()
	{
		return 0.0;
	}

	float getCPUPercent(const std::string &appName)
	{
		return 0.0;
	}

	float getMemPercent()
	{
		return 0.0;
	}

	float getMemPercent(const std::string &appName)
	{
		return 0.0;
	}

	void closeProcess(const std::string &processName)
	{

	}

	std::string int2str(int nNum)
	{
		return "";
	}

	std::string float2str(float fValue)
	{
		return "";
	}

	std::string gbk2Utf8(const char* gbkStr)
	{
		return "";
	}

	std::string utf82gbk(const char* utf8Str)
	{
		return "";
	}

	int str2int(const std::string &str)
	{
		return 0;
	}

	float str2float(const std::string &str)
	{
		return 0.0;
	}

	CString s2cs(const std::string &str)
	{
		return _T("");
	}

	std::string cs2s(const CString &str)
	{
		return "";
	}

	std::string getLocalIp()
	{
		return "";
	}

	bool IsAccessible(const std::string &remoteHost)
	{
		return true;
	}
}