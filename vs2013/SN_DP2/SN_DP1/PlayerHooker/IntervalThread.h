#ifndef _INTERVAL_THREAD_H_
#define _INTERVAL_THREAD_H_

#include "Thread.h"
#include "SleepEvent.h"

class IIntervalThreadNotify
{
public:
	virtual void OnIntervalExecute() = 0;
	virtual void OnThreadTerminate() = 0;
};

class CIntervalThread : public CThread
{
public: 
	CIntervalThread(IIntervalThreadNotify* pNotify, int intervalTime = 50, bool autoRun = true): CThread(true), m_intervalTime(intervalTime)
		, m_sleep(intervalTime)
	{
		m_pNotify = pNotify;	
		if (autoRun)
		{
			Resume();
		}
	}

	void Start()
	{
		Resume();
	}

	void Wakeup()
	{
		m_sleep.Wakeup();
	}

	~CIntervalThread()
	{
		Quit();
	}

protected:
	void Execute()
	{
		CoInitialize(NULL);
		SetPriority(THREAD_PRIORITY_TIME_CRITICAL);
// #ifndef _DEBUG
// 		try
// 		{
// #endif
			while (!Terminated)
			{
				if (m_pNotify != NULL)
				{
					m_pNotify->OnIntervalExecute();
				}
				if (Terminated)
				{
					break;
				}
				m_sleep.Wait();
			}
// #ifndef _DEBUG
// 		}
// 		catch (...)
// 		{
// 			ShowDebugInfo(_T("CIntervalThread Execute Error!\n"));
// 		}
// #endif
		CoUninitialize();
	}

	IIntervalThreadNotify* m_pNotify;
private:
	virtual void DoTerminate()
	{
		CThread::DoTerminate();
		if (m_pNotify != NULL)
		{
			m_pNotify->OnThreadTerminate();
		}
	}

	CSleepEvent m_sleep;
	int m_intervalTime;
};

#endif