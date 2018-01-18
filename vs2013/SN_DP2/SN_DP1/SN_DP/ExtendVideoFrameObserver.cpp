#include "stdafx.h"
#include "ExtendVideoFrameObserver.h"
#include <timeapi.h>
#include "FileIO.h"

CExtendVideoFrameObserver::CExtendVideoFrameObserver()
{
	m_lpImageBuffer = new BYTE[0x800000];

	m_lpRenderBuffer = new BYTE[0x800000];

	m_RenderWidth = 0;
	m_RenderHeight = 0;
	
}


CExtendVideoFrameObserver::~CExtendVideoFrameObserver()
{
	delete[] m_lpImageBuffer;
	delete[] m_lpRenderBuffer;
}

void DoMirror(unsigned char* yuv_temp, int nw, int nh, int w,
	int h) {
	int deleteW = (nw - h) / 2;
	int deleteH = (nh - w) / 2;
	int i, j;

	int a, b;
	unsigned char temp;
	//mirror y  
	for (i = 0; i < h; i++) {
		a = i * w;
		b = (i + 1) * w - 1;
		while (a < b) {
			temp = yuv_temp[a];
			yuv_temp[a] = yuv_temp[b];
			yuv_temp[b] = temp;
			a++;
			b--;
		}
	}
	//mirror u  
	int uindex = w * h;
	for (i = 0; i < h / 2; i++) {
		a = i * w / 2;
		b = (i + 1) * w / 2 - 1;
		while (a < b) {
			temp = yuv_temp[a + uindex];
			yuv_temp[a + uindex] = yuv_temp[b + uindex];
			yuv_temp[b + uindex] = temp;
			a++;
			b--;
		}
	}
	//mirror v  
	uindex = w * h / 4 * 5;
	for (i = 0; i < h / 2; i++) {
		a = i * w / 2;
		b = (i + 1) * w / 2 - 1;
		while (a < b) {
			temp = yuv_temp[a + uindex];
			yuv_temp[a + uindex] = yuv_temp[b + uindex];
			yuv_temp[b + uindex] = temp;
			a++;
			b--;
		}
	}

}

DWORD timeold = timeGetTime();
int timeinc = 0;
bool CExtendVideoFrameObserver::onCaptureVideoFrame(VideoFrame& videoFrame)
{
	CFileIO fileYuv;
	fileYuv.openMedia("D:\\V6room\\src.yuv");
	fileYuv.write((char*)videoFrame.yBuffer, videoFrame.yStride * videoFrame.height);
	fileYuv.write((char*)videoFrame.uBuffer, videoFrame.uStride * videoFrame.height / 2);
	fileYuv.write((char*)videoFrame.vBuffer, videoFrame.vStride * videoFrame.height / 2);
	fileYuv.close();
	timeinc++;
	DWORD timenow = timeGetTime();
	if (timenow - timeold >= 5000)
	{
		timeold = timenow;
		char loginfo[128] = { 0 };

		sprintf_s(loginfo, 128, "video fps[%d]\n", timeinc);
		//OutputDebugStringA(loginfo);
		timeinc = 0;
	}

	SIZE_T nBufferSize = 0x800000;
	int nUvLen = videoFrame.height * videoFrame.width / 4;
	int nYLen = nUvLen * 4;
//	DoMirror(m_lpImageBuffer, 320, 240, 320, 240);
	m_lpY = m_lpImageBuffer;
	m_lpU = m_lpImageBuffer + nUvLen * 4;
	m_lpV = m_lpImageBuffer + 5 * nUvLen;

//  	FILE* outfile = fopen("D:\\wechat file\\WeChat Files\\baluoteliz\\Files\\SN_DP1\\Debugou.yuv", "wb");
//  	fwrite(m_lpImageBuffer, 1, 320*240 * 3 / 4, outfile);
//  	fclose(outfile);

	memcpy_s(videoFrame.yBuffer, nYLen, m_lpY, nYLen);
	videoFrame.yStride = videoFrame.width;
	
	memcpy_s(videoFrame.uBuffer, nUvLen, m_lpU, nUvLen);
	videoFrame.uStride = videoFrame.width/2;

	memcpy_s(videoFrame.vBuffer, nUvLen, m_lpV, nUvLen);
	videoFrame.vStride = videoFrame.width/2;

	videoFrame.type = FRAME_TYPE_YUV420;
	videoFrame.rotation = 0;

	return true;
}

bool CExtendVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame)
{
// 	char loginfo[128] = { 0 };
// 	snprintf(loginfo, 128, "uid[%d] w[%d] h[%d] type[%d]\n", uid, videoFrame.width, videoFrame.height, videoFrame.type);
// 	OutputDebugStringA(loginfo);
	return true;
	this->m_RenderWidth = videoFrame.width;
	this->m_RenderHeight = videoFrame.height;
	int uv_len = videoFrame.width * videoFrame.height / 4;
	memcpy(this->m_lpRenderBuffer, videoFrame.yBuffer, uv_len * 4);
	memcpy(this->m_lpRenderBuffer + uv_len * 4, videoFrame.uBuffer, uv_len);
	memcpy(this->m_lpRenderBuffer + uv_len * 5, videoFrame.vBuffer, uv_len);
// 	FILE *outfile = fopen("d:/out.yuv", "wb");
// 	fwrite(videoFrame.yBuffer, 1, videoFrame.width * videoFrame.height, outfile);
// 	fwrite(videoFrame.uBuffer, 1, videoFrame.width * videoFrame.height / 4, outfile);
// 	fwrite(videoFrame.vBuffer, 1, videoFrame.width * videoFrame.height / 4, outfile);
// 	fclose(outfile);
	//get remote video
	return true;
}
