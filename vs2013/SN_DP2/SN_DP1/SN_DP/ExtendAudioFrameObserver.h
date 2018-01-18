#pragma once
#include "../SDK/include/IAgoraMediaEngine.h"
#include "FileIO.h"

class CExtendAudioFrameObserver :
	public agora::media::IAudioFrameObserver
{
public:
	CExtendAudioFrameObserver();
	~CExtendAudioFrameObserver();
	LPBYTE pPlayerData;
	int    nPlayerDataLen;
	CFileIO m_fileBkMusicDest;
	CFileIO m_fileSource;
	CFileIO m_fileMix;

	virtual bool onRecordAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame);
	virtual bool onMixedAudioFrame(AudioFrame& audioFrame);
	virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame);
};

