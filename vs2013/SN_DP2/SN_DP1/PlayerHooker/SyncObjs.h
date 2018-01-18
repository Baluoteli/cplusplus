#ifndef _SYNC_OBJS_H_
#define _SYNC_OBJS_H_

#include <assert.h>
#include "tstring.h"

static int ms_Num = 0;

static void TraceInfo(const char* format,...)
{
#define MAX_DBG_MSG_LEN (1024)
	char buf[MAX_DBG_MSG_LEN];
	va_list ap;

	va_start(ap, format);

	_vsnprintf_s(buf, sizeof(buf), format, ap);

	OutputDebugStringA(buf);

	va_end(ap);
}

class CLock
{
public:
	int Enter(const char* pTrack = NULL) 
	{
		if (m_bTrace && pTrack != NULL)
		{
			TraceInfo("\n%d: %s is enter\n", ms_Num, pTrack);
		}
		EnterCriticalSection(&m_sec);
		return ++m_nLockCount;
	}

	int Leave(const char* pTrack = NULL) 
	{
		int nResult = --m_nLockCount;
		if (m_bTrace && pTrack != NULL)
		{
			TraceInfo("\n%d: %s is leave\n", ms_Num, pTrack);
		}
		LeaveCriticalSection(&m_sec);
		return nResult;
	}

	int GetLockCount() 
	{
		return m_nLockCount;
	}

	int GetLockCountCheck() 
	{
		Enter();
		return Leave();
	}

	inline void AssertLocked() 
	{
		assert(GetLockCountCheck() > 0);
	}

	inline void AssertNotLocked() 
	{
		assert(GetLockCountCheck() == 0);
	}

	CLock(bool bTrace = false) 
	{
		InitializeCriticalSection(&m_sec);
		m_bTrace = bTrace;
		m_nLockCount = 0;
		ms_Num++;
	}

	~CLock() 
	{
		DeleteCriticalSection(&m_sec);
	}

private:
	CRITICAL_SECTION m_sec;
	int m_nLockCount;
	bool m_bTrace;
};


class CInsync
{
public:
	CInsync(CLock* pCriticalSection, char* pFunc = NULL) 
	{
		m_pCriticalSection = pCriticalSection;
		if (m_pCriticalSection != NULL)
		{
#ifdef _DEBUG
			m_strFunc = pFunc;
			m_pCriticalSection->Enter(pFunc);
#else
			m_pCriticalSection->Enter();
#endif
		}
	}

	CInsync(CLock& criticalSection, char* pFunc = NULL)
	{
		m_pCriticalSection = &criticalSection;
		if (m_pCriticalSection != NULL)
		{
#ifdef _DEBUG
			m_strFunc = pFunc;
			m_pCriticalSection->Enter(pFunc);
#else
			m_pCriticalSection->Enter();
#endif
		}
	}

	~CInsync()
	{
		if (m_pCriticalSection != NULL)
		{
#ifdef _DEBUG
			m_pCriticalSection->Leave(m_strFunc.c_str());
#else
			m_pCriticalSection->Leave();
#endif
		}
	}

private:
	CLock* m_pCriticalSection;
#ifdef _DEBUG
	string m_strFunc;
#endif
};

#ifdef _DEBUG
	#define INSYNC(X) CInsync blah____sync(X, __FUNCTION__)
#else
	#define INSYNC(X) CInsync blah____sync(X)
#endif

#endif