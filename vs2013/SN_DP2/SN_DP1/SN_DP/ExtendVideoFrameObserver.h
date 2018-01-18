#pragma once
#include "../SDK/include/IAgoraMediaEngine.h"

class CExtendVideoFrameObserver :
	public agora::media::IVideoFrameObserver
{
public:
	CExtendVideoFrameObserver();
	~CExtendVideoFrameObserver();

	virtual bool onCaptureVideoFrame(VideoFrame& videoFrame);
	virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame);
	LPBYTE				m_lpImageBuffer;
	LPBYTE				m_lpRenderBuffer;
	int					m_RenderWidth;
	int					m_RenderHeight;

private:
	LPBYTE				m_lpY;
	LPBYTE				m_lpU;
	LPBYTE				m_lpV;
};

