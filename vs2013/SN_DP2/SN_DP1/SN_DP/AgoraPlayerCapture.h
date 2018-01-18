#pragma once

#include "AgoraPlayerCapture.h"
#include "PlayerHooker.h"
#include "CicleBuffer.h"
#include "FileIO.h"

class CAudioCaptureCallback : public IAudioCaptureCallback
{
public:
	CAudioCaptureCallback();
	~CAudioCaptureCallback();

	virtual void onCaptureStart();
	virtual void onCaptureStop();
	virtual void onCapturedData(void* data, UINT dataLen, WAVEFORMATEX* format);

private:
	FILE* mpFile;

	CFileIO m_fileBkMusicSrc;
};

class AgoraPlayerManager
{
public:
	AgoraPlayerManager();
	~AgoraPlayerManager();

	BOOL startHook(BOOL bstart, TCHAR* pPlayerPath);
	CicleBuffer* getCircleBufferObject() { return pCircleBuffer; }
	LPBYTE pPlayerData;
	int nPlayerDataLen;
private:
	CicleBuffer * pCircleBuffer;
	BOOL bHook;
	IPlayerHooker* mpPlayerHooker;
	//IAudioCaptureCallback* mpAudioCaptureCallback;

};
