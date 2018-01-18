#pragma once

#include "IAgoraMediaEngine.h"
#include "IAgoraRtcEngine.h"
#include "IAgoraRtcEngine2.h"
//
#include "PlayerHooker.h"

#include "AgoraPlayerCapture.h"
#include "AgoraVideoCapture.h"
#include "AgoraVideoPlay.h"
#include "AGEngineEventHandler.h"
#include "ExtendVideoFrameObserver.h"
#include "ExtendAudioFrameObserver.h"

//devices
#include "AgoraAudInputManager.h"
#include "AgoraPlayoutManager.h"
#include "AgoraCameraManager.h"

#include <string>

#define APPVERSION "v6AgoraVideo.0.0.1"
//#define NEED_PING
#define PING_TIME_FLASH 10000
// #include <memory>
// using namespace std;
typedef struct ChatInfo
{
	//room info
	BOOL bPublishing;
	BOOL bWaterMark;
	int  bLeft;
	int  nRID;
	int  nUID;
	//audio info
	std::string sMicName;
	int  nSampleRate;
	int  nMicChannel;
	//video info
	std::string sCamerName;
	int nBitRateVideo;
	int nWidth;
	int nHeight;
	int display_width;
	int display_height;
	BOOL bMix;
	int nFps;
	//player
	std::string sPlayerPath;

	std::string sChannelName;
	std::string sPublishUrl;
	std::string sChannelKey;
}CHATINFO;

typedef struct RTMPInfo
{
	std::string ip;
	std::string port;
	std::string appname;
	std::string streamname;
	int fps;
	int width;
	int height;
	int bitrate;
	BOOL brtmppush;
	BOOL brtmpset;
}RTMPINFO;

class AgoraManager
{
public:
	AgoraManager();
	~AgoraManager();
//	unique_ptr <VideoCaptureManager> pfffff;
	agora::rtc::IRtcEngine * pRTCEngine;
	CAGEngineEventHandler *m_EngineEventHandler;
	CExtendVideoFrameObserver * m_CapVideoFrameObserver;
	CExtendAudioFrameObserver * m_CapAudioFrameObserver;
	VideoCaptureManager * pVideoCaptureManager;
	VideoPlayManager * pVideoPlayManager;
	AgoraPlayerManager* pPlayerCaptureManager;

	IAudioCaptureCallback* mpAudioCaptureCallback;

	//devices
	CAgoraPlayoutManager*	m_devPlayout;
	CAgoraAudInputManager*	m_devAudioin;
	CAgoraCameraManager*	m_devCamera;

// 	//room info
// 	BOOL bPublishing;
// 	BOOL bPreview;
// 	BOOL bWaterMark;
// 	int  nRID;
// 	int  nUID;
// 	//audio info
// 	string sMicName;
// 	int  nSampleRate;
// 	int  nChannel;
// 	//video info
// 	string sCamerName;
// 	int nBitRateVideo;
// 	int nWidth;
// 	int nHeight;
// 	int nFps;
// 	//player
// 	string sPlayerPath;
	CHATINFO ChatRoomInfo;
	//rtmp info
	RTMPINFO RtmpPushInfo;

	BOOL initParam();
	BOOL start();
	BOOL stop();
	BOOL setClientHwnd(HWND wnd, HWND wndL, HWND wndR);
	BOOL EnableLocalMirrorImage(BOOL bMirrorLocal);
	BOOL setChannelAndRole(CHANNEL_PROFILE_TYPE ch_type, CLIENT_ROLE_TYPE cl_type, char* permissionkey);
	BOOL enableOBServer(BOOL bAudioEnable, BOOL bVideoEnable);
	BOOL setDevices(std::string cMic, std::string cCamera);
	BOOL closeDevices();
	BOOL setDevicesParam();
	BOOL initEngine(char* app_id);
	BOOL enableVideoObserver(BOOL bEnable);
	BOOL enableAudioObserver(BOOL bEnable);
	BOOL setEventHandler(HWND hWnd);
	BOOL setRtcEngineVideoProfile(int nwidth, int nheight, int nfps, int nbitrate, BOOL bfineturn);
	BOOL setRtcEngineAudioProfile(int samplerate, int channel, BOOL bfineturn);
	BOOL setRtcEngineAudioProfileEx(int nSampleRate, int nChannels, int nSamplesPerCall);
	BOOL setLocalCanvas(uid_t uid, HWND hwnd);
	BOOL setRemoteCanvas(uid_t id, HWND hwnd);
	BOOL setCompositingLayout(uid_t nLocal, uid_t nRemote);
	BOOL muteLocalAudio(BOOL bMute);
	BOOL enableVideo();
	BOOL disableVideo();
	BOOL JoinChannel(char* lpChannelName, UINT nUID, char* lpDynamicKey);
	BOOL LeaveChannel();
private:

	BOOL bVideoEnable;
	HWND m_RenderWnd;
	HWND m_RenderR;
	HWND m_RenderL;
};
