#ifndef _API_HOOK_LOG_H_
#define _API_HOOK_LOG_H_

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include "tstring.h"

#define _HOOK_LOG_

class CAPIHookLog
{
public:
	CAPIHookLog(TCHAR* pLogFileName = NULL);
	~CAPIHookLog();

	void Trace(TCHAR* pLog, ...);

private:

#ifdef _HOOK_LOG_
	void Lock();
	void UnLock();
#endif

	bool m_selfCloseMutex;
	tstring m_logFileName;
	HANDLE m_logMutex;
};

#endif