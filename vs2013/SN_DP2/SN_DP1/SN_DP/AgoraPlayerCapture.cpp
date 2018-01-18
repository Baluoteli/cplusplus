#include "stdafx.h"


#include "AgoraPlayerCapture.h"
#include "PlayerHooker.h"

#include "AgoraManager.h"
#include <timeapi.h>
extern AgoraManager*	pAgoraManager;

CAudioCaptureCallback::CAudioCaptureCallback()
	: mpFile(NULL)
{
	m_fileBkMusicSrc.openMedia("D:\\V6room\\BkMusicSrc.pcm");
}

CAudioCaptureCallback::~CAudioCaptureCallback()
{
	if (mpFile != NULL)
	{
		fclose(mpFile);
	}
	m_fileBkMusicSrc.close();
}

void CAudioCaptureCallback::onCaptureStart()
{

}

void CAudioCaptureCallback::onCaptureStop()
{
	PostQuitMessage(0);
}

//get audio data
DWORD timelast = timeGetTime();
int ninclen = 0;
void CAudioCaptureCallback::onCapturedData(void* data, UINT dataLen, WAVEFORMATEX* format)
{
//	memset((void*)pAgoraManager->pPlayerCaptureManager->pPlayerData, 0, sizeof(pAgoraManager->pPlayerCaptureManager->pPlayerData));
// 	if (dataLen > sizeof(pAgoraManager->pPlayerCaptureManager->pPlayerData))
// 	{
// 		return;
// 	}
	pAgoraManager->pPlayerCaptureManager->getCircleBufferObject()->writeBuffer(data, dataLen);
// 	memcpy((void*)pAgoraManager->pPlayerCaptureManager->pPlayerData, data, dataLen);
// 	pAgoraManager->pPlayerCaptureManager->nPlayerDataLen = dataLen;

	char loginfo[128] = { 0 };
	sprintf_s(loginfo, 128, "hook len[%d] \n", dataLen);
	//OutputDebugStringA(loginfo);

	ninclen++;
	DWORD timenow = timeGetTime();
	if (timenow - timelast >= 1000)
	{
		timelast = timenow;
		char loginfo1[128] = { 0 };
		sprintf_s(loginfo1, 128, "hook rate :[%d],dataLen : %d  \n", ninclen,dataLen);
		ninclen = 0;
		OutputDebugStringA(loginfo1);
		ninclen = 0;
	}
#if 0
	FILE * outfile = fopen("D:\\playerout.pcm", "ab+");
	if (outfile)
	{
		fwrite(data, 1, dataLen, outfile);
		fclose(outfile);
		outfile = NULL;
	}
	else
	{
		OutputDebugStringA("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}
#endif
	m_fileBkMusicSrc.write((char*)data, dataLen);
// 	if (mpFile == NULL)
// 	{
// 		mpFile = fopen("e:/hook.pcm", "wb");
// 	}
// 	if (mpFile != NULL)
// 	{
// 		fwrite(data, 1, dataLen, mpFile);
// 	}
}


AgoraPlayerManager::AgoraPlayerManager()
{
	this->pCircleBuffer = new CicleBuffer(48000 * 2 * 2, 0);
	this->mpPlayerHooker = createPlayerHookerInstance();
	this->pPlayerData = new BYTE[0x800000];
	this->bHook = FALSE;
}

AgoraPlayerManager::~AgoraPlayerManager()
{
	delete this->pCircleBuffer;
	delete[] this->pPlayerData;
// 	if (mpAudioCaptureCallback)
// 	{
// 		delete mpAudioCaptureCallback;
// 		mpAudioCaptureCallback = NULL;
// 	}
	if (mpPlayerHooker)
	{
		delete mpPlayerHooker;
		mpPlayerHooker = NULL;
	}
}

BOOL AgoraPlayerManager::startHook(BOOL bstart, TCHAR* pPlayerPath)
{
	//clear circle buffer
	this->pCircleBuffer->flushBuffer();

	//TCHAR* pPlayerPath = L"D:\\Program Files (x86)\\KuGou\\KGMusic\\KuGou.exe";
	if (this->bHook)
	{
		mpPlayerHooker->stopAudioCapture();
		mpPlayerHooker->stopHook();
	}
	else
	{
		mpPlayerHooker->startHook(pPlayerPath);
		mpPlayerHooker->startAudioCapture(pAgoraManager->mpAudioCaptureCallback);
	}
	this->bHook = !this->bHook;
	return TRUE;
}

