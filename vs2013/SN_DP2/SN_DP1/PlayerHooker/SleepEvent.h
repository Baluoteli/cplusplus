#ifndef _SLEEP_EVENT_H_
#define _SLEEP_EVENT_H_

#include <windows.h>
#include "SyncObjs.h"

class CSleepEvent
{
public:
	CSleepEvent(int sleepInterval = 50)
	{
		m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_timeId = timeSetEvent(sleepInterval, 0, LPTIMECALLBACK(m_event), 0, 
			TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
		m_maxWaitTime = sleepInterval * 2;
	}

	~CSleepEvent()
	{
		CloseHandle(m_event);
		m_event = NULL;
		timeKillEvent(m_timeId);
		m_timeId = 0;
	}

	void Wait()
	{
		if (m_event != NULL)
		{
			WaitForSingleObject(m_event, m_maxWaitTime);
		}
	}

	void Wakeup()
	{
		if (m_event != NULL)
		{
			SetEvent(m_event);
		}
	}

private:
	HANDLE m_event;
	UINT m_timeId;
	DWORD m_maxWaitTime;
};

#endif