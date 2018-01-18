#include "stdafx.h"
#include "AgoraManager.h"

AgoraManager::AgoraManager()
{
}

AgoraManager::~AgoraManager()
{

}

BOOL AgoraManager::setClientHwnd(HWND wnd, HWND wndL, HWND wndR)
{
	m_RenderWnd = wnd;
	m_RenderR = wndR;
	m_RenderL = wndL;
	return TRUE;
}

BOOL AgoraManager::setEventHandler(HWND hWnd)
{
	this->m_EngineEventHandler->SetMsgReceiver(hWnd);
	return TRUE;
}

BOOL AgoraManager::setRtcEngineVideoProfile(int nwidth, int nheight, int nfps, int nbitrate, BOOL bfineturn)
{
	IRtcEngine2 *lpRtcEngine2 = (IRtcEngine2 *)this->pRTCEngine;
	int nRet = lpRtcEngine2->setVideoProfileEx(nwidth, nheight, nfps, nbitrate);
	//pRTCEngine->setVideoProfile(VIDEO_PROFILE_TYPE::VIDEO_PROFILE_240P, true);

	// int ret = this->pRTCEngine->setVideoProfile2(nwidthsetRtcEngineAudioProfile, nheight)
	return nRet == 0 ? TRUE : FALSE;
}

BOOL AgoraManager::setRtcEngineAudioProfile(int samplerate, int channel, BOOL bfineturn)
{
// 		AUDIO_PROFILE_MUSIC_STANDARD = 2, // 48Khz, 50kbps, mono, music
// 		AUDIO_PROFILE_MUSIC_STANDARD_STEREO = 3, // 48Khz, 50kbps, stereo, music
// 		AUDIO_PROFILE_MUSIC_HIGH_QUALITY = 4, // 48Khz, 128kbps, mono, music
// 		AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO = 5, // 48Khz, 128kbps, stereo, music
	
	//int ret = pRTCEngine->setAudioProfile(AUDIO_PROFILE_MUSIC_STANDARD_STEREO, AUDIO_SCENARIO_CHATROOM);
	int ret = pRTCEngine->setAudioProfile(AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO, AUDIO_SCENARIO_GAME_STREAMING);
	return ret == 0 ? TRUE : FALSE;
}

BOOL AgoraManager::setRtcEngineAudioProfileEx(int nSampleRate, int nChannels, int nSamplesPerCall)
{
	RtcEngineParameters rep(pRTCEngine);

	int nRet = rep.setRecordingAudioFrameParameters(nSampleRate, nChannels, RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, nSamplesPerCall);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL AgoraManager::setLocalCanvas(uid_t uid, HWND hwnd)
{
	VideoCanvas vc;
	vc.uid = uid;
	vc.renderMode = RENDER_MODE_TYPE::RENDER_MODE_FIT;
	vc.view = hwnd;
	int ret = pRTCEngine->setupLocalVideo(vc);
	if (ret == 0)
		return TRUE;
	return FALSE;
}

BOOL AgoraManager::setRemoteCanvas(uid_t id, HWND hwnd)
{
	VideoCanvas vc1;
	vc1.uid = id;
	vc1.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
	vc1.view = hwnd;
	int ret = pRTCEngine->setupRemoteVideo(vc1);
	if (ret == 0)
		return TRUE;
	return FALSE;
}

BOOL AgoraManager::enableVideo()
{
	int nRet = pRTCEngine->enableVideo();
	if (nRet != 0)
		return FALSE;
	bVideoEnable = TRUE;
	return TRUE;
}

BOOL AgoraManager::disableVideo()
{
	int nRet = pRTCEngine->disableVideo();
	if (nRet != 0)
		return FALSE;
	bVideoEnable = FALSE;
	return TRUE;
}

BOOL AgoraManager::setCompositingLayout(uid_t nLocal, uid_t nRemote)
{
	uid_t nleft = nLocal;
	uid_t nright = nRemote;
	if (!this->ChatRoomInfo.bLeft)
	{
		nleft = nRemote;
		nright = nLocal;
	}
	VideoCompositingLayout::Region aRegion[2];
	aRegion[0].x = 0.0;
	aRegion[0].y = 0.0;
	aRegion[0].width = 0.5;
	aRegion[0].height = 1.0;
	aRegion[0].alpha = 1.0;
	aRegion[0].zOrder = 0;
	aRegion[0].renderMode = RENDER_MODE_HIDDEN;
	aRegion[0].uid = nleft;

	//VideoCompositingLayout::Region bRegion;
	aRegion[1].x = 0.5;
	aRegion[1].y = 0.0;
	aRegion[1].width = 0.5;
	aRegion[1].height = 1.0;
	aRegion[1].renderMode = RENDER_MODE_HIDDEN;
	aRegion[1].uid = nright;
	aRegion[1].alpha = 1.0;
	aRegion[1].zOrder = 0;

	VideoCompositingLayout aLayout;
	aLayout.regionCount = 2;

	aLayout.regions = aRegion;//{800, 600,  "#C0C0C0", &aRegion, 1, NULL, 0};
	this->pRTCEngine->setVideoCompositingLayout(aLayout);
	return TRUE;
}

BOOL AgoraManager::JoinChannel(char* szChannelName, UINT nUID, char* lpDynamicKey)
{
	int nRet = 0;
	char tempInfo[1024] = { 0 };
	if (this->RtmpPushInfo.brtmppush)
	{
		char tempPath[128] = { 0 };
		sprintf_s(tempPath, 128, "rtmp://%s:%s/%s/%s", this->RtmpPushInfo.ip.c_str(), this->RtmpPushInfo.port.c_str(), this->RtmpPushInfo.appname.c_str(), this->RtmpPushInfo.streamname.c_str());
		sprintf_s(tempInfo, 1024, "{\"owner\":true,\"lifecycle\":2,\"defaultLayout\":0,\"width\":%d,\"height\":%d,\"framerate\":%d,\"audiosamplerate\":44100,\"audiobitrate\":96000,\"audiochannels\":2,\"bitrate\":%d,\"mosaicStream\":\"%s\",\"extraInfo\":\"{\\\"lowDelay\\\":true}\"}",
			this->RtmpPushInfo.width, this->RtmpPushInfo.height, this->RtmpPushInfo.fps, this->RtmpPushInfo.bitrate, tempPath/*"rtmp://aliliveup.6rooms.com/liverecord/v587"*/);

		nRet = pRTCEngine->joinChannel(lpDynamicKey, szChannelName, tempInfo, nUID);
	}
	return nRet == 0 ? TRUE : FALSE;
}

BOOL AgoraManager::LeaveChannel()
{
//	this->pPlayerCaptureManager->startHook(TRUE);
	this->pRTCEngine->leaveChannel();
	return TRUE;
}

BOOL AgoraManager::initEngine(char* app_id)
{
	RtcEngineContext ctx;
	ctx.appId = app_id;
	ctx.eventHandler = this->m_EngineEventHandler;
	this->pRTCEngine->initialize(ctx);

	return TRUE;
}

BOOL AgoraManager::enableAudioObserver(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(pRTCEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);

	int nRet = 0;

	if (mediaEngine.get() == NULL)
		return FALSE;

	if (bEnable && m_CapAudioFrameObserver != NULL)
		nRet = mediaEngine->registerAudioFrameObserver(m_CapAudioFrameObserver);
	else
		nRet = mediaEngine->registerAudioFrameObserver(NULL);

	return nRet == 0 ? TRUE : FALSE;
}

///open ob server
BOOL AgoraManager::enableVideoObserver(BOOL bEnable)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(this->pRTCEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);

	int nRet = 0;
	AParameter apm(*this->pRTCEngine);

	if (mediaEngine.get() == NULL)
		return FALSE;

	if (bEnable && this->m_CapVideoFrameObserver != NULL) {
		apm->setParameters("{\"che.video.local.camera_index\":1024}");//1024 means we use observer instead of camera.
		nRet = mediaEngine->registerVideoFrameObserver(this->m_CapVideoFrameObserver);
	}
	else {
		nRet = mediaEngine->registerVideoFrameObserver(NULL);
		apm->setParameters("{\"che.video.local.camera_index\":0}");
	}
	return TRUE;
}

BOOL AgoraManager::muteLocalAudio(BOOL bMute)
{
	RtcEngineParameters rep(*this->pRTCEngine);
	int ret = rep.muteLocalAudioStream(bMute);
	if (ret == 0)
		return TRUE;
	return FALSE;
}

BOOL AgoraManager::enableOBServer(BOOL bAudioEnable, BOOL bVideoEnable)
{
	BOOL bDone = FALSE;
	///set observer
	bDone = this->enableVideoObserver(bVideoEnable);
	if (!bDone)
		return FALSE;
	bDone = this->enableAudioObserver(bAudioEnable);
	if (!bDone)
	{
		this->enableVideoObserver(FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL AgoraManager::closeDevices()
{
	this->m_devAudioin->Close();
	this->m_devPlayout->Close();
	return TRUE;
}

BOOL AgoraManager::setDevices(std::string cMic, std::string cCamera)
{
	BOOL bdone = FALSE;
	int ret = -1;
	//enable function
	ret = this->pRTCEngine->enableVideo();
	if (ret != 0)
		return FALSE;
	ret = this->pRTCEngine->enableAudio();
	if (ret != 0)
		return FALSE;
	//create devices
	if (!this->m_devAudioin->Create(this->pRTCEngine))
		return FALSE;
	if (!this->m_devPlayout->Create(this->pRTCEngine))
		return FALSE;
	//set devices
	char device_id[MAX_DEVICE_ID_LENGTH] = { 0 };

	bdone = this->m_devAudioin->GetDevice(cMic, device_id);
	if (bdone)
		bdone = this->m_devAudioin->SetCurDevice(device_id);
	if (!bdone)
		return FALSE;

#if 0
	memset(device_id, 0, MAX_DEVICE_ID_LENGTH);
	bdone = this->m_devPlayout->GetDevice(cCamera, device_id);
	if (bdone)
		bdone = this->m_devPlayout->SetCurDevice(device_id);
	if (!bdone)
		return FALSE;
#endif

#if 1
	char cameraId[MAX_DEVICE_ID_LENGTH] = { '\0' };
	memset(device_id, 0, MAX_DEVICE_ID_LENGTH);
	m_devCamera->GetDevice(0, (char*)cCamera.data(), cameraId);
	m_devCamera->SetCurDevice(cameraId);
#endif

	return TRUE;
}

BOOL AgoraManager::setDevicesParam()
{
	BOOL bDone = FALSE;

	bDone = this->setRtcEngineVideoProfile(this->ChatRoomInfo.nWidth, this->ChatRoomInfo.nHeight, this->ChatRoomInfo.nFps, this->ChatRoomInfo.nBitRateVideo, TRUE);
	bDone = this->setRtcEngineAudioProfile(this->ChatRoomInfo.nSampleRate, this->ChatRoomInfo.nMicChannel,TRUE);

	return TRUE;
}

BOOL AgoraManager::setChannelAndRole(CHANNEL_PROFILE_TYPE ch_type, CLIENT_ROLE_TYPE cl_type, char* permissionkey)
{
	int res = -1;
	//set channel type
	res = this->pRTCEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	if (res != 0)
		return FALSE;
	//set client type
	res = this->pRTCEngine->setClientRole(CLIENT_ROLE_BROADCASTER, permissionkey);
	if (res != 0)
		return FALSE;
	return TRUE;
}

BOOL AgoraManager::EnableLocalMirrorImage(BOOL bMirrorLocal)
{
	int nRet = 0;

	AParameter apm(*this->pRTCEngine);

	if (bMirrorLocal)
		nRet = apm->setParameters("{\"che.video.localViewMirrorSetting\":\"forceMirror\"}");
	else
		nRet = apm->setParameters("{\"che.video.localViewMirrorSetting\":\"disableMirror\"}");

	return nRet == 0 ? TRUE : FALSE;
}

BOOL AgoraManager::initParam()
{
	this->ChatRoomInfo.bLeft = TRUE;
	this->ChatRoomInfo.bMix = TRUE;
	this->ChatRoomInfo.bPublishing = TRUE;
	this->ChatRoomInfo.bWaterMark = TRUE;
	this->ChatRoomInfo.display_height = 600;
	this->ChatRoomInfo.display_width = 800;
	this->ChatRoomInfo.nBitRateVideo = 400;
	this->ChatRoomInfo.nFps = 15;
	this->ChatRoomInfo.nHeight = 240;
	this->ChatRoomInfo.nMicChannel = 2;
	this->ChatRoomInfo.nRID = 1111;
	this->ChatRoomInfo.nSampleRate = 44100;
	this->ChatRoomInfo.nUID = 1111;
	this->ChatRoomInfo.nWidth = 320;
	this->ChatRoomInfo.sCamerName = "Integrated Webcam";
	this->ChatRoomInfo.sChannelKey = "";
	this->ChatRoomInfo.sChannelName = "123test";
	this->ChatRoomInfo.sMicName = "";
	this->ChatRoomInfo.sPlayerPath = "";
	this->ChatRoomInfo.sPublishUrl = "";

	this->RtmpPushInfo.ip = "aliliveup.6rooms.com";
	this->RtmpPushInfo.port = "1935";
	this->RtmpPushInfo.appname = "liverecord";
	this->RtmpPushInfo.streamname = "v587";
	this->RtmpPushInfo.bitrate = 1000;
	this->RtmpPushInfo.brtmppush = TRUE;
	this->RtmpPushInfo.brtmpset = TRUE;
	this->RtmpPushInfo.fps = 15;
	this->RtmpPushInfo.height = 600;
	this->RtmpPushInfo.width = 800;
	return TRUE;
}

BOOL AgoraManager::start()
{
	this->initParam();
	
	int res = -1;
	BOOL bdone = FALSE;
	int build = 0;
	const char* pVersion  =pRTCEngine->getVersion(&build);
// 
// 	this->initEngine(APP_ID);
// 	//set hwnd for message post by event_handler
// 	this->setEventHandler(m_RenderWnd);

	//open log
	RtcEngineParameters rep(pRTCEngine);
	res = rep.setLogFile("D:\\V6room\\v6room.log");

	this->pPlayerCaptureManager->startHook(TRUE, L"D:\\programe file\\KGMusic\\KuGou.exe");
	//this->pPlayerCaptureManager->startHook(TRUE, L"D:\\Program Files\\KuGou\\KGMusic\\KuGou.exe");

 	this->pVideoCaptureManager->initCapture(this->ChatRoomInfo.nWidth, this->ChatRoomInfo.nHeight, this->ChatRoomInfo.nFps);
 	//this->pVideoCaptureManager->startCapture();
 	 //	this->pVideoPlayManager->initPlayInfo(320, 240, 15);
 	// 	this->pVideoPlayManager->startVideoPlay();
	
	setRtcEngineAudioProfileEx(44100, 2, 44100 * 2 / 100);
	this->enableOBServer(TRUE, FALSE);
// set upload stream info
// 	struct  PublisherConfiguration config;
// 	config.bitrate = 800;
// 	config.width = 320;
// 	config.height = 240;
// 	config.framerate = 15;
// 	config.publishUrl = "rtmp://aliliveup.6rooms.com/liverecord/v587";
// 	config.injectStreamWidth = 0;
// 	config.injectStreamHeight = 0;
// 	config.defaultLayout = 0;
// 	config.owner = true;
// 	config.lifecycle = 2;
//	this->pRTCEngine->configPublisher(config);

	char* permissionkey = NULL;
	this->setChannelAndRole(CHANNEL_PROFILE_LIVE_BROADCASTING, CLIENT_ROLE_BROADCASTER, permissionkey);
	//std::string mic = "内装麦克风 (Conexant SmartAudio HD)";
	//std::string sounder = "扬声器 (Conexant SmartAudio HD)";
	//this->setDevices(mic, sounder);
	
	this->EnableLocalMirrorImage(FALSE);
	if (this->ChatRoomInfo.bLeft)
		this->setLocalCanvas(this->ChatRoomInfo.nUID, m_RenderL);
	else
		this->setLocalCanvas(this->ChatRoomInfo.nUID, m_RenderR);
	this->setDevicesParam();
	this->setDevices(this->ChatRoomInfo.sMicName, this->ChatRoomInfo.sCamerName);

// 	char cRID[64] = { 0 };
// 	snprintf(cRID, 64, "test%d", this->ChatRoomInfo.nRID);
	res = pRTCEngine->startPreview();
	char* lpDynamicKey = NULL;
	this->JoinChannel((char*)this->ChatRoomInfo.sChannelName.c_str(), this->ChatRoomInfo.nUID, lpDynamicKey);
	
	return TRUE;
}

BOOL AgoraManager::stop()
{
	this->pVideoCaptureManager->stopCapture();
	this->pPlayerCaptureManager->startHook(FALSE, NULL);
	this->enableOBServer(FALSE, FALSE);
	this->LeaveChannel();//close all engine resources
	pRTCEngine->stopPreview();
	this->ChatRoomInfo.bPublishing = FALSE;
	return TRUE;
}