#include "stdafx.h"
#include "AgoraVideoPlay.h"
#include "AgoraManager.h"

#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

extern HWND			hRenderWnd;
extern AgoraManager*	pAgoraManager;

VideoPlayManager::VideoPlayManager()
{
	this->nHeight = 320;
	this->nWidth = 240;
	this->nFps = 15;

	this->bVideoPlay = FALSE;
	this->hVideoPlay = NULL;
	this->pdisplay = NULL;
}

VideoPlayManager::~VideoPlayManager()
{
}

void VideoPlayManager::initPlayInfo(int nwidth, int nheight, int nfps)
{
	this->nWidth = nwidth;
	this->nHeight = nheight;
	this->nFps = nfps;
}

void VideoPlayManager::startVideoPlay()
{
	this->bVideoPlay = TRUE;
	this->hVideoPlay = CreateThread(NULL, 0, VideoPlayManager::VideoPlayThread, this, 0, 0);
}

void VideoPlayManager::stopVideoPlay()
{
	this->bVideoPlay = FALSE;
	WaitForSingleObject(this->hVideoPlay, INFINITE);
	CloseHandle(this->hVideoPlay);
	this->hVideoPlay = NULL;
}

int CombinePictures_2(unsigned char* pDst, unsigned char* pSrc1, unsigned char* pSrc2,
	int srcWidth, int srcHeight)
{
	if (pDst == NULL || pSrc1 == NULL || pSrc2 == NULL)
		return -1;
	if (srcWidth <= 0 || srcHeight <= 0)
		return -2;
	int dstWidth = srcWidth * 2;
	int dstHeight = srcHeight;
	int dstHalfWidth = dstWidth >> 1;
	int dstHalfHeight = dstHeight >> 1;

	int srcHalfWidth = srcWidth >> 1;
	int srcHalfHeight = srcHeight >> 1;

	unsigned char * pDstY = pDst;
	unsigned char * pDstU = pDstY + dstWidth*dstHeight;
	unsigned char * pDstV = pDstU + dstHalfWidth*dstHalfHeight;

	unsigned char * pSrcY1 = pSrc1;
	unsigned char * pSrcU1 = pSrcY1 + srcHeight*srcWidth;
	unsigned char * pSrcV1 = pSrcU1 + srcHalfWidth*srcHalfHeight;

	unsigned char * pSrcY2 = pSrc2;
	unsigned char * pSrcU2 = pSrcY2 + srcHeight*srcWidth;
	unsigned char * pSrcV2 = pSrcU2 + srcHalfWidth*srcHalfHeight;

	int dstOffset = 0, srcOffset1 = 0, srcOffset2 = 0;
	for (int i = 0; i < srcHeight; i++)
	{
		memcpy(pDstY + dstOffset, pSrc1 + srcOffset1, srcWidth);
		dstOffset += srcWidth;
		srcOffset1 += srcWidth;
		memcpy(pDstY + dstOffset, pSrc2 + srcOffset2, srcWidth);
		dstOffset += srcWidth;
		srcOffset2 += srcWidth;
	}
	dstOffset = 0;
	srcOffset1 = 0;
	srcOffset2 = 0;
	for (int i = 0; i < dstHalfHeight; i++)
	{
		memcpy(pDstU + dstOffset, pSrcU1 + srcOffset1, srcHalfWidth);
		memcpy(pDstV + dstOffset, pSrcV1 + srcOffset1, srcHalfWidth);
		dstOffset += srcHalfWidth;
		srcOffset1 += srcHalfWidth;
		memcpy(pDstU + dstOffset, pSrcU2 + srcOffset2, srcHalfWidth);
		memcpy(pDstV + dstOffset, pSrcV2 + srcOffset2, srcHalfWidth);
		dstOffset += srcHalfWidth;
		srcOffset2 += srcHalfWidth;
	}
	return 0;
}

static int CutHalfPicture(unsigned char* pDst, unsigned char* pSrc2, int srcWidth, int srcHeight)
{
	if (pDst == NULL || pSrc2 == NULL)
		return -1;
	if (srcWidth <= 0 || srcHeight <= 0)
		return -2;
	int dstWidth = srcWidth / 2;
	int dstHeight = srcHeight;

	int dstHalfWidth = dstWidth / 2;
	int dstHalfHeight = dstHeight / 2;

	int srcHalfWidth = srcWidth / 2;
	int srcHalfHeight = srcHeight / 2;

	unsigned char * pDstY = pDst;
	unsigned char * pDstU = pDstY + dstWidth * dstHeight;
	unsigned char * pDstV = pDstU + dstHalfWidth * dstHalfHeight;

	unsigned char * pSrcY2 = pSrc2;
	unsigned char * pSrcU2 = pSrcY2 + srcHeight * srcWidth;
	unsigned char * pSrcV2 = pSrcU2 + srcHalfWidth * srcHalfHeight;

	int newWid = srcHalfWidth;
	int dstOffset = 0, srcOffset2 = srcHalfWidth / 2;
	for (int i = 0; i < srcHeight; i++)
	{
		memcpy(pDstY + dstOffset, pSrc2 + srcOffset2, newWid);
		dstOffset += dstWidth;
		srcOffset2 += srcWidth;
	}

	dstOffset = 0;
	srcOffset2 = srcHalfWidth / 4;
	int newWid2 = srcHalfWidth / 2;
	for (int i = 0; i < dstHalfHeight; i++)
	{
		memcpy(pDstU + dstOffset, pSrcU2 + srcOffset2, newWid2);
		memcpy(pDstV + dstOffset, pSrcV2 + srcOffset2, newWid2);
		dstOffset += dstHalfWidth;
		srcOffset2 += srcHalfWidth;
	}

	return 0;
}

void initYUVBuffer(char* psrc, int width, int height, int color = 0)
{
	if (!psrc || width <= 0 || height <= 0)
		return;

	int len = width * height;

	char* y = psrc;
	char* u = y + len;
	char* v = u + len / 4;

	char value = 16;
	for (int i = 0; i < len; i++)
	{
		y[i] = value;
	}
	len /= 4;
	for (int i = 0; i < len; i++)
	{
		u[i] = 128;
		v[i] = 126;
	}
}

void RGB_SetUpDown(BYTE *pData, int image_width, int image_height, int bpp)
{
	int index = bpp / 8;
	for (int h = 0; h < image_height / 2; h++)
	{
		for (int w = 0; w < image_width; w++)
		{
			const int iCoordM = index*(h*image_width + w);
			const int iCoordN = index*((image_height - h - 1)*image_width + w);
			BYTE Tmp = pData[iCoordM];
			pData[iCoordM] = pData[iCoordN];
			pData[iCoordN] = Tmp;
			Tmp = pData[iCoordM + 1];
			pData[iCoordM + 1] = pData[iCoordN + 1];
			pData[iCoordN + 1] = Tmp;
			Tmp = pData[iCoordM + 2];
			pData[iCoordM + 2] = pData[iCoordN + 2];
			pData[iCoordN + 2] = Tmp;
		}
	}
}

void WriteBMP(char* inPic, int width, int height, int bitsCount, TCHAR* strFileName)
{
	int DataSize = width * height * bitsCount / 8;
	HANDLE hf = CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	//写文件头
	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));  //内存块置0
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh) + DataSize + sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	DWORD dwWritten = 0;
	WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);

	//写位图格式
	BITMAPINFOHEADER bin;
	memset(&bin, 0, sizeof(bin));  //内存块置0
	bin.biSize = sizeof(bin);
	bin.biWidth = width;
	bin.biHeight = height;
	bin.biPlanes = 1;
	bin.biBitCount = bitsCount;
	WriteFile(hf, &bin, sizeof(bin), &dwWritten, NULL);

	//写位图数据
	WriteFile(hf, inPic, DataSize, &dwWritten, NULL);
	CloseHandle(hf);
}

DWORD VideoPlayManager::VideoPlayThread(LPVOID lparam)
{
	VideoPlayManager* pThis = (VideoPlayManager*)lparam;

	int nsize_display = pThis->nWidth * pThis->nHeight * 4;
	int nsize_single = pThis->nWidth * pThis->nHeight / 2 * 3 / 2;
	int nsize_combine = pThis->nWidth * pThis->nHeight * 3 / 2;

	BYTE * plocal_half = (BYTE *)malloc(nsize_single);
	BYTE * premote_half = (BYTE *)malloc(nsize_single);
	BYTE * pcombine = (BYTE *)malloc(nsize_combine);

	BYTE * premote = (BYTE *)malloc(nsize_combine);
	BYTE * plocal = (BYTE*)malloc(nsize_combine);

	pThis->pdisplay = (BYTE *)malloc(nsize_display);

	//gdiplus
 	HDC MemDc = CreateCompatibleDC(GetDC(0));
	Gdiplus::Graphics graphg(MemDc);
	graphg.SetSmoothingMode(Gdiplus::SmoothingModeNone);

	initYUVBuffer((char*)pAgoraManager->m_CapVideoFrameObserver->m_lpRenderBuffer, pThis->nWidth, pThis->nHeight);
	int nTimerTick = 1000 / pThis->nFps;
	HANDLE hTimer = CreateEventA(NULL, TRUE, FALSE, NULL);
	DWORD id = timeSetEvent(nTimerTick, 0, (LPTIMECALLBACK)hTimer, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	while (pThis->bVideoPlay)
	{
		//render data here;
		WaitForSingleObject(hTimer, INFINITE);
		ResetEvent(hTimer);

		//copy local data
		memcpy(plocal, pAgoraManager->m_CapVideoFrameObserver->m_lpImageBuffer, nsize_combine);
		//copy remote data
		memcpy(premote, pAgoraManager->m_CapVideoFrameObserver->m_lpRenderBuffer, nsize_combine);
		//cut local pic
		CutHalfPicture(plocal_half, plocal, pThis->nWidth, pThis->nHeight);
		//cut remote pic
		CutHalfPicture(premote_half, premote, pThis->nWidth, pThis->nHeight);
		//combine display pic
		CombinePictures_2(pcombine, plocal_half, premote_half, pThis->nWidth / 2, pThis->nHeight);
		//convert yuv to rgb24
		pThis->FormatTrans.I420ToRGB24(pcombine, pThis->pdisplay, nsize_display, pThis->nWidth, pThis->nHeight);
		RGB_SetUpDown(pThis->pdisplay, pThis->nWidth, pThis->nHeight, 24);

		//graphics.DrawImage(pAgoraManager->pVideoPlayManager->pLogo, 100, 0, 80, 44);

		InvalidateRect(hRenderWnd, NULL, TRUE);		
	}

	timeKillEvent(id);
	if (hTimer)
	{
		CloseHandle(hTimer);
		hTimer = NULL;
	}
	return 0;
}
