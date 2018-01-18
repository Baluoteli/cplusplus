#include "Thread.h"

HHOOK CThread::ms_hHookMsg = 0;
DWORD CThread::ms_dwMainThreadID = 0;
HWND CThread::ms_wndMain = 0;
LONG CThread::ms_lHookCount = 0;

CThread::CThread(bool bCreateSuspended)
{
	m_bCreateSuspended =  bCreateSuspended;
	m_bSuspended = bCreateSuspended;
	m_bFinished = false;
	Terminated = false;
	ReturnValue = 0;

	m_method = NULL;
	bFreeOnTerminate = false;
	OnTerminate = NULL;

	if (ms_dwMainThreadID == 0)
	{
		ms_dwMainThreadID = GetMainThreadID(GetCurrentProcessId());
	}

	m_handle = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, bCreateSuspended ? CREATE_SUSPENDED : 0,
		&m_threadID);
	if (m_handle == 0)
	{
		m_bFinished = true;
		m_bSuspended = false;
	}
	else
	{
		InterlockedIncrement(&ms_lHookCount);
	}
}

CThread::~CThread()
{
	Quit();
}

void CThread::Quit()
{
	if (m_threadID != 0 && !m_bFinished)
	{
		Terminate();
		if (m_bCreateSuspended)
		{
			Resume();
		}
		WaitFor();
		m_threadID = 0;
	}
	if (m_handle != 0)
	{
		CloseHandle((HANDLE)m_handle);

		InterlockedDecrement(&ms_lHookCount);
		if (ms_hHookMsg != NULL && ms_lHookCount <= 0)
		{
			UnhookWindowsHookEx(ms_hHookMsg);
			ms_hHookMsg = NULL;
		}

		m_handle = 0;
	}
}

void CThread::Resume()
{
	if (m_bSuspended)
	{
		if (ResumeThread((HANDLE)m_handle) == 1)
		{
			m_bSuspended = false;
		}
	}
}

void CThread::Suspend()
{
	if (!m_bSuspended)
	{
		if (SuspendThread((HANDLE)m_handle) != 0xFFFFFFFF)
		{
			m_bSuspended = true;
		}
	}
}

void CThread::Terminate()
{
	Terminated = true;
}

unsigned CThread::WaitFor()
{
	DWORD Result = 0;
	MSG Msg;
	HANDLE H;

	H = (HANDLE)m_handle;
	if (GetCurrentThreadId() == ms_dwMainThreadID)
	{
		while (::MsgWaitForMultipleObjects(1, &H, false, INFINITE,
			QS_SENDMESSAGE) == WAIT_OBJECT_0 + 1)
		{
			::PeekMessage(&Msg, 0, 0, 0, PM_NOREMOVE);
		}
	}
	else
	{
		WaitForSingleObject(H, INFINITE);
	}
	GetExitCodeThread(H, &Result);

	return Result;
}

void CThread::SetPriority(int Value)
{
	SetThreadPriority((HANDLE)m_handle, Value);
}

int CThread::GetPriority()
{
	return GetThreadPriority((HANDLE)m_handle);
}

bool CThread::IsSuspended()
{
	return m_bSuspended;
}

HWND CThread::GetMainWnd()
{
	if (ms_wndMain != NULL && IsWindow(ms_wndMain))
	{
		return ms_wndMain;
	}

	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)this);
	return ms_wndMain;
}

DWORD CThread::GetMainThreadID(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	DWORD dwThreadID = 0;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	te32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);
	}

	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			dwThreadID = te32.th32ThreadID;
			break;
		}
	}
	while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return(dwThreadID);
}

BOOL CALLBACK CThread::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	CThread *thread = (CThread *)lParam;
	if (GetWindowThreadProcessId(hwnd, NULL) == thread->ms_dwMainThreadID)
	{
		thread->ms_wndMain = hwnd;
		return FALSE;
	}
	return TRUE;
}

unsigned  __stdcall CThread::ThreadProc(void *p)
{
	bool bFreeThread;
	int Result;
	CThread *Thread = (CThread *)p;

	__try
	{
		if (!Thread->Terminated)
		{
			Thread->Execute();
		}
	}
	__finally
	{
		bFreeThread = Thread->bFreeOnTerminate;
		Result = Thread->ReturnValue;
		Thread->m_bFinished = true;
		Thread->DoTerminate();
		if (bFreeThread)
		{
			delete Thread;
		}
	}
	return Result;
}

LRESULT CALLBACK CThread::GetMsgProc(int code,       // hook code
								     WPARAM wParam,  // removal flag
								     LPARAM lParam   // address of structure with message
									 )
{
	MSG *pMsg = (MSG *)lParam;

	if (code == HC_ACTION && pMsg->message == WM_NULL)
	{
		CThread *Thread = (CThread *)(pMsg->lParam);
		if (Thread != NULL)
		{
// 			try
// 			{
				if (Thread->m_method != NULL)
				{
					Thread->m_method();
				}
//			}
// 			catch (...)
// 			{
// 
// 			}
			SetEvent(Thread->m_hMsgEvt);
		}
		return 1;
	}
	return CallNextHookEx(ms_hHookMsg, code, wParam, lParam);
}

void CThread::DoTerminate() 
{
	if (OnTerminate != NULL)
	{
		Synchronize(OnTerminate);
	}
}

void CThread::Synchronize(ThreadMethod Method, DWORD dwThreadID)
{
	if (ms_hHookMsg == NULL)
	{
		ms_hHookMsg = SetWindowsHookEx(WH_GETMESSAGE,
			(HOOKPROC)GetMsgProc, NULL, dwThreadID);
		if (ms_hHookMsg == NULL)
		{
			//可能ThreadID错误或者其它原因
			return;  
		}
		PostThreadMessage(dwThreadID, WM_NULL, 0, NULL);
		Sleep(1);
	}

	m_method = Method;
	m_hMsgEvt = CreateEvent(NULL, false, false, NULL);

	if (PostThreadMessage(dwThreadID, WM_NULL, 0, (LPARAM)this))
	{
		WaitForSingleObject(m_hMsgEvt, INFINITE);
	}
	else 
	{
		//ThreadID对应线程没有消息队列
	    //注意：由调用者自己保证同步
		Method();
	}

	CloseHandle(m_hMsgEvt);
}
