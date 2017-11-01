#pragma once

namespace plusFCL_BTL
{
	/*@Detail App path operator
	 * Main Function is get the Dir or Executable path of the curring Running File that
	 can be Dll ,Executable,Etc.
	*/
	EXPORTS_UTILC char* getAbsoluteDir();
	EXPORTS_UTILC char* getFilePath();
	EXPORTS_UTILC char* getRelativeDir();
	EXPORTS_UTILC char* getCurRunningExeName();
	EXPORTS_UTILC char* getFileDir(const std::string &file);
	EXPORTS_UTILC char* getPirorDir(const std::string &file);
	EXPORTS_UTILC char* getRootDir(const std::string &file);

	/*@Detail Media Type GUID .
	* It's Maybe Audio or Video
	*/ 
	EXPORTS_UTILC std::string getPath(const std::string &pathSrc, const std::string &pathKey);

	/*process operator
	*/
	EXPORTS_UTILC DWORD getProcessID(const std::string &processName);
	EXPORTS_UTILC float getCPUPercent();
	EXPORTS_UTILC float getCPUPercent(const std::string &appName);
	EXPORTS_UTILC float getMemPercent();
	EXPORTS_UTILC float getMemPercent(const std::string &appName);
	EXPORTS_UTILC void closeProcess(const std::string &processName);

	/*Data transfer
	*/
	EXPORTS_UTILC std::string int2str(int nNum);
	EXPORTS_UTILC std::string float2str(float fValue);
	EXPORTS_UTILC std::string gbk2Utf8(const char* gbkStr);
	EXPORTS_UTILC std::string utf82gbk(const char* utf8Str);
	EXPORTS_UTILC int str2int(const std::string &str);
	EXPORTS_UTILC float str2float(const std::string &str);
	EXPORTS_UTILC CString s2cs(const std::string &str);
	EXPORTS_UTILC std::string cs2s(const CString &str);

	/*SOCKET IP
	*/
	EXPORTS_UTILC std::string getLocalIp();
	EXPORTS_UTILC bool IsAccessible(const std::string &remoteHost);

	/*Time Format
	*/
	
}

