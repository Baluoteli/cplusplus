#ifndef __INTERFACE_UTILC_H__
#define __INTERFACE_UTILC_H__

#include "utilc.h"
 
namespace plusFCL_BTL
{
extern "C"
{
	/*@Detail App path operator
	* Main Function is get the Dir or Executable path of the curring Running File that
	can be Dll ,Executable,Etc.
	*/
	DLLEXPORT std::string getAbsoluteDir();
	DLLEXPORT std::string getFilePath();
	DLLEXPORT std::string getCurRunningExeName();
	DLLEXPORT std::string getFileAbsolutePath(const std::string &file);
	DLLEXPORT std::string getPirorDir(const std::string &file);
	DLLEXPORT std::string getRootDir(const std::string &file);

	/*@Detail Media Type GUID .
	* It's Maybe Audio or Video
	*/
	DLLEXPORT std::string getMediaPath(const std::string &pathSrc, const std::string &pathKey);
	DLLEXPORT std::string MergePath(const std::string &FormatStr,const std::string &category,const std::string &classId = "");

	/*process operator
	*/
	DLLEXPORT DWORD getProcessID(const std::string &processName);
	DLLEXPORT float getCPUPercentAll();
	DLLEXPORT float getCPUPercent(const std::string &appName);
	DLLEXPORT float getMemPercentAll();
	DLLEXPORT float getMemPercent(const std::string &appName = "");
	DLLEXPORT void closeProcess(const std::string &processName);

	/*Data transfer
	*/
	DLLEXPORT std::string int2str(int nNum);
	DLLEXPORT std::string float2str(float fValue);
	DLLEXPORT std::string gbk2Utf8(const std::string gbkStr);
	DLLEXPORT std::string utf82gbk(const std::string utf8Str);
	DLLEXPORT int str2int(const std::string &str);
	DLLEXPORT float str2float(const std::string &str);
	DLLEXPORT CString s2cs(const std::string &str);
	DLLEXPORT std::string cs2s(const CString &str);

	/*SOCKET IP
	*/
	DLLEXPORT std::string getHostIp(const std::string &hostName = "");
	DLLEXPORT bool IsAccessible(const std::string &remoteHost);

	/*Time Format
	*/


	/*Function for export Class .
	*/

#ifdef exportUTILC
	class CFileIO;
	class CFrameCtrl;
	class CHighResoluteFrameCtrl;

	DLLEXPORT __DECLAREEXPORTCLASS(CFileIO)
	DLLEXPORT __DECLAREEXPORTCLASS(CFrameCtrl)
	DLLEXPORT __DECLAREEXPORTCLASS(CHighResoluteFrameCtrl)
#endif

}

DLLEXPORT std::vector<DWORD> getProcessIdMutil(const std::string &processName);
}

#endif