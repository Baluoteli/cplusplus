// SN_DP.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SN_DP.h"
#include "AgoraManager.h"
#include "PlayerHooker.h"

#include "AGEventDef.h"

#define MAX_LOADSTRING 100

class IPlayerHooker;
class IAudioCaptureCallback;

AgoraManager*	pAgoraManager = NULL;
HWND			hRenderWnd = NULL;
HWND			hRenderChildL = NULL;
HWND			hRenderChildR = NULL;
extern uint32_t nLastErrorCode;

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	HANDLE hS = CreateSemaphoreA(NULL, 0, 1, APPVERSION);
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;
 //   // TODO: 在此放置代码。
	if (!initSystem())
	{
		return FALSE;
	}

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SN_DP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	ULONG_PTR gditoken;
	GdiplusStartupInput gdiinput;
	GdiplusStartup(&gditoken, &gdiinput, NULL);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SN_DP));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	GdiplusShutdown(gditoken);
    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_DROPSHADOW;//CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SN_DP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);//(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;// MAKEINTRESOURCEW(IDC_SN_DP);
    wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = NULL;// LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
int nClientWidth = 0;
int nClientHeight = 0;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   RECT RenderRect;
   SetRect(&RenderRect, 0, 0, 640, 480);
   AdjustWindowRectEx(&RenderRect, WS_POPUP | WS_MINIMIZEBOX, FALSE, WS_EX_DLGMODALFRAME);
   nClientWidth = RenderRect.right - RenderRect.left;
   nClientHeight = RenderRect.bottom - RenderRect.top;
   hRenderWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,
	   szTitle,
	   szTitle,
	   WS_POPUP | WS_MINIMIZEBOX,
	   300,
	   200,
	   nClientWidth,
	   nClientHeight,
	   NULL,
	   NULL,
	   hInstance,
	   NULL);

   if (!hRenderWnd)
   {
      return FALSE;
   }

   ShowWindow(hRenderWnd, nCmdShow);
   UpdateWindow(hRenderWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
RECT DesckTopRect;
void initEngine()
{
	pAgoraManager->initEngine(APP_ID);
	//set hwnd for message post by event_handler
	pAgoraManager->setEventHandler(hRenderWnd);
	//open log
	RtcEngineParameters rep(pAgoraManager->pRTCEngine);
	rep.setLogFile("D:\\wechat file\\WeChat Files\\baluoteliz\\Files\\SN_DP1\\Debug");
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT curRect;
	POINT p_mouse;
	LPVOID lpData;
    switch (message)
    {
	case WM_CREATE:
		hRenderChildL = CreateWindowW(L"static", L"LW", WS_VISIBLE | WS_CHILD | SS_BLACKFRAME,
									0, 0, 320, 480, hWnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hRenderChildR = CreateWindow(L"static", L"RW", WS_VISIBLE | WS_CHILD | SS_BLACKFRAME,
									320, 0, 320, 480, hWnd, (HMENU)3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		pAgoraManager->setClientHwnd(hWnd, hRenderChildL, hRenderChildR);
		initEngine();
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_ERASEBKGND:
		return TRUE;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
// 			Graphics graphics(hdc);
// 
// 			BitmapData bitmapData;
// 			bitmap->LockBits(NULL, 0, PixelFormat24bppRGB, &bitmapData);
// 			if (pAgoraManager->pVideoPlayManager->pdisplay)
// 				memcpy(bitmapData.Scan0, pAgoraManager->pVideoPlayManager->pdisplay, 320 * 240 * 3);
// 			bitmap->UnlockBits(&bitmapData);
// 			bitmap->RotateFlip(RotateFlipType::Rotate180FlipX);
// 			graphics.DrawImage(bitmap, 0, 0, 640, 480);
            EndPaint(hWnd, &ps);

			static bool bStart = false;
			if (!bStart)
			{
				bStart = true;
				pAgoraManager->start();
			}
        }
		break;
	case WM_SIZE:
		break;
	case WM_CHAR:
	{
		if (VK_ESCAPE == wParam){
			//PostQuitMessage(0);
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
	}
	break;
	case WM_MSGID(EID_JOINCHANNEL_SUCCESS):
	{
		lpData = (LPAGE_JOINCHANNEL_SUCCESS)wParam;
		//pAgoraManager->setEventHandler(hRenderWnd);
		delete lpData;
		break;
	}
	case WM_MSGID(EID_ERROR):
		lpData = (LPAGE_ERROR)wParam;
		delete lpData;
		break;
	case WM_MSGID(EID_FIRST_LOCAL_VIDEO_FRAME):
	{
		lpData = (LPAGE_FIRST_LOCAL_VIDEO_FRAME)wParam;
		//pAgoraManager->JoinChannel("test66", 233, NULL);
		delete lpData;
		break;
	}
	case WM_MSGID(EID_APICALL_EXECUTED):
		lpData = (LPAGE_APICALL_EXECUTED)wParam;
		delete lpData;
		break;
	case WM_MSGID(EID_LOCAL_VIDEO_STAT):
		/*LPAGE_LOCAL_VIDEO_STAT*/ lpData = (LPAGE_LOCAL_VIDEO_STAT)wParam;
		delete lpData;
		break;
	case WM_MSGID(EID_USER_JOINED):
	{
		/*LPAGE_FIRST_REMOTE_VIDEO_DECODED*/ lpData = (LPAGE_FIRST_REMOTE_VIDEO_DECODED)wParam;
		uid_t uid = ((LPAGE_FIRST_REMOTE_VIDEO_DECODED)lpData)->uid;
		if (pAgoraManager->ChatRoomInfo.bLeft)
			pAgoraManager->setRemoteCanvas(uid, hRenderChildR);
		else
			pAgoraManager->setRemoteCanvas(uid, hRenderChildL);

		pAgoraManager->setCompositingLayout(pAgoraManager->ChatRoomInfo.nUID, uid);
		delete lpData;
		break;
	}
	case WM_MSGID(EID_USER_OFFLINE):
		/*LPAGE_USER_OFFLINE*/ lpData = (LPAGE_USER_OFFLINE)wParam;
		delete lpData;
		break;
	case WM_LBUTTONUP:
		GetCursorPos(&p_mouse);
		GetWindowRect(hWnd, &curRect);
		if ((p_mouse.y > curRect.top) && (p_mouse.y < curRect.top + 30))
		{
			if ((p_mouse.x > curRect.right - 30) && (p_mouse.x < curRect.right))
			{
				ShowWindow(hWnd, SW_MINIMIZE);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		SendMessageA(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		GetWindowRect(hWnd, &curRect);
 		SetWindowPos(hWnd, HWND_TOPMOST, curRect.left, curRect.top, nClientWidth, nClientHeight, SWP_NOSIZE);
		break;
	case WM_DESTROY:
		pAgoraManager->stop();
        //PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL quitSystem()
{
	if (pAgoraManager->pRTCEngine)
	{
		delete pAgoraManager->pRTCEngine;
	}

	if (pAgoraManager->pVideoCaptureManager)
	{
		delete pAgoraManager->pVideoCaptureManager;
	}

	if (pAgoraManager->pVideoPlayManager)
	{
		delete pAgoraManager->pVideoPlayManager;
	}

	if (pAgoraManager->m_devAudioin)
	{
		delete pAgoraManager->m_devAudioin;
	}

	if (pAgoraManager->m_devCamera)
	{
		delete pAgoraManager->m_devCamera;
	}

	if (pAgoraManager->m_devPlayout)
	{
		delete pAgoraManager->m_devPlayout;
	}

	if (pAgoraManager->m_EngineEventHandler)
	{
		delete pAgoraManager->m_EngineEventHandler;
	}

	if (pAgoraManager->m_CapVideoFrameObserver)
	{
		delete pAgoraManager->m_CapVideoFrameObserver;
	}

	if (pAgoraManager->m_CapAudioFrameObserver)
	{
		delete pAgoraManager->m_CapAudioFrameObserver;
	}

	if (pAgoraManager->pPlayerCaptureManager)
	{
		delete pAgoraManager->pPlayerCaptureManager;
	}

	if (pAgoraManager->mpAudioCaptureCallback)
	{
		delete pAgoraManager->mpAudioCaptureCallback;
	}

	if (pAgoraManager)
	{
		delete pAgoraManager;
	}

#ifdef NEED_PING
	KillTimer(hRenderWnd, TimerID_SDPlay);
#endif
	return TRUE;
}

BOOL initSystem()
{
	int nstep = 0;
	pAgoraManager = new AgoraManager();
	if (!pAgoraManager)
	{
		goto InitError;
	}

	nstep = 1;
	pAgoraManager->pRTCEngine = (IRtcEngine *)createAgoraRtcEngine();
	if (!pAgoraManager->pRTCEngine)
	{
		goto InitError;
	}

// 	nstep = 2;
	pAgoraManager->pVideoCaptureManager = new VideoCaptureManager();
	if (!pAgoraManager->pVideoCaptureManager)
	{
		goto InitError;
	}

	nstep = 3;
	pAgoraManager->pVideoPlayManager = new VideoPlayManager();
	if (!pAgoraManager->pVideoPlayManager)
	{
		goto InitError;
	}

	nstep = 4;
	pAgoraManager->m_devAudioin = new CAgoraAudInputManager();
	if (!pAgoraManager->m_devAudioin)
	{
		goto InitError;
	}

	nstep = 5;
	pAgoraManager->m_devCamera = new CAgoraCameraManager();
	if (!pAgoraManager->m_devCamera)
	{
		goto InitError;
	}

	nstep = 6;
	pAgoraManager->m_devPlayout = new CAgoraPlayoutManager();
	if (!pAgoraManager->m_devPlayout)
	{
		goto InitError;
	}

	nstep = 7;
	pAgoraManager->m_EngineEventHandler = new CAGEngineEventHandler();
	if (!pAgoraManager->m_EngineEventHandler)
	{
		goto InitError;
	}

	nstep = 8;
	pAgoraManager->m_CapVideoFrameObserver = new CExtendVideoFrameObserver();
	if (!pAgoraManager->m_CapVideoFrameObserver)
	{
		goto InitError;
	}

	nstep = 9;
	pAgoraManager->m_CapAudioFrameObserver = new CExtendAudioFrameObserver();
	if (!pAgoraManager->m_CapAudioFrameObserver)
	{
		goto InitError;
	}

	nstep = 10;
	pAgoraManager->pPlayerCaptureManager = new AgoraPlayerManager();
	if (!pAgoraManager->pPlayerCaptureManager)
	{
		goto InitError;
	}

	nstep = 11;
	pAgoraManager->mpAudioCaptureCallback = new CAudioCaptureCallback();
	if (!pAgoraManager->mpAudioCaptureCallback)
	{
		goto InitError;
	}
	return TRUE;
InitError:
	quitSystem();
	return FALSE;
}