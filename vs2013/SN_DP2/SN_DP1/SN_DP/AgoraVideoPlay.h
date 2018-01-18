#pragma once

#include "YUVTrans.h"
#include "SN_DP.h"

class VideoPlayManager
{
public:
	VideoPlayManager();
	~VideoPlayManager();

	int nWidth;
	int nHeight;
	int nFps;
	BYTE * pdisplay;
	void initPlayInfo(int nwidth, int nheight, int nfps);
	void startVideoPlay();
	void stopVideoPlay();
private:
	HANDLE hVideoPlay;
	BOOL bVideoPlay;
	//transformat
	CYUVTrans FormatTrans;


	static DWORD WINAPI VideoPlayThread(LPVOID lparam);
};
