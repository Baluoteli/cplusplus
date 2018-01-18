#pragma once

#include "YUVTrans.h"
#include "SN_DP.h"
class VideoCaptureManager
{
public:
	VideoCaptureManager();
	~VideoCaptureManager();

	BOOL openSharedMemory();
	BOOL closeSharedMemory();

	BOOL initSharedMemory();
	void uninitSharedMemory();

	void initCapture(int nwidth, int nheight, int nfps);
	BOOL startCapture();
	void stopCapture();

	int nWidth;
	int nHeight;
	int nFps;
	int nBitrate;

	unsigned char* pCaptrueVideoData;

	//image convert
	

private:
	//logo
	Gdiplus::Image* pLogo;

	//time event
	HANDLE hTimerEvent;
	
	//control shared memory
	HANDLE hStartEvent;
	HANDLE hStopEvent;

	//shared memory
	HANDLE	hMapVideo;
	LPBYTE	pMapVideo;
	DWORD	dwVideoMapSize;

	//thread
	HANDLE hVideoFlush;
	BOOL bVideoCapture;
	//liveconut
	HANDLE hLiveCount;
	BOOL bLiveCount;
	//transformat
	CYUVTrans FormatTrans;

	BOOL startUpdate();
	BOOL stotUpdate();

	static DWORD WINAPI VideoBufferFlashThread(LPVOID lparam);
	static DWORD WINAPI UpdateLivecount(LPVOID lparam);
};
