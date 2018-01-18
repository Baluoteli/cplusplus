#ifndef _THREAD_H_
#define _THREAD_H_

#include <windows.h>
#include <process.h>
#include <tlhelp32.h>

typedef void(__stdcall *ThreadMethod)();
typedef void(__stdcall *TerminateEvent)();

class CThread
{
public:
	CThread(bool bCreateSuspended = false);

	virtual ~CThread();

	void Resume();

	void Suspend();

	void Terminate();

	unsigned WaitFor();

	inline HANDLE GetHandle()
	{
		return m_handle;
	}

	inline unsigned GetThreadId()
	{
		return m_threadID;
	}

	inline bool GetTerminated()
	{
		return Terminated;
	}

	void SetPriority(int Value);

	int GetPriority();

	bool IsSuspended();

	HWND GetMainWnd();

	void Quit();

	bool bFreeOnTerminate;
	TerminateEvent OnTerminate;

protected:
	virtual void Execute()
	{
		//
	}

	virtual void DoTerminate();

	void Synchronize(ThreadMethod method, DWORD dwThreadID = ms_dwMainThreadID);
	int ReturnValue;
	bool Terminated;

private:
	DWORD GetMainThreadID(DWORD dwOwnerPID);

	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	static unsigned  __stdcall ThreadProc(void *p);

	static LRESULT CALLBACK GetMsgProc(int code,       // hook code
									   WPARAM wParam,  // removal flag
									   LPARAM lParam   // address of structure with message
									   );

	HANDLE m_handle;                    
	unsigned m_threadID;
	bool m_bCreateSuspended;
	bool m_bSuspended;
	bool m_bFinished;

	static HHOOK ms_hHookMsg;
	static DWORD ms_dwMainThreadID;
	static HWND ms_wndMain;
	static LONG ms_lHookCount;

	HANDLE m_hMsgEvt;
	ThreadMethod m_method;
};

#endif 