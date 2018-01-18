#include "stdafx.h"
#include "ExtendAudioFrameObserver.h"

#include "AgoraManager.h"
#include <timeapi.h>

extern AgoraManager*	pAgoraManager;

CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{
	pPlayerData = new BYTE[0x800000];
	m_fileBkMusicDest.openMedia("D:\\V6room\\BkMusicDest.pcm");
	m_fileSource.openMedia("D:\\V6room\\source.pcm");
	m_fileMix.openMedia("D:\\V6room\\mix.pcm");
}


CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
	delete[] pPlayerData;
	m_fileBkMusicDest.close();
	m_fileSource.close();
	m_fileMix.close();
}

static inline int16_t MixerAddS16(int16_t var1, int16_t var2) {
	static const int32_t kMaxInt16 = 32767;
	static const int32_t kMinInt16 = -32768;
	int32_t tmp = (int32_t)var1 + (int32_t)var2;
	int16_t out16;

	if (tmp > kMaxInt16) {
		out16 = kMaxInt16;
	}
	else if (tmp < kMinInt16) {
		out16 = kMinInt16;
	}
	else {
		out16 = (int16_t)tmp;
	}

	return out16;
}

void MixerAddS16(int16_t* src1, const int16_t* src2, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		src1[i] = MixerAddS16(src1[i], src2[i]);
	}
}

BOOL mixAudioData(char* psrc, char* pdst, int datalen)
{
	if (!psrc || !pdst || datalen <= 0)
	{
		return FALSE;
	}
	
	for (int i = 0; i < datalen; i++)
	{
		pdst[i] += psrc[i];
	}
	return TRUE;
}

DWORD timeold1 = timeGetTime();
int timeinc1 = 0;
FILE * outfile = NULL;
bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame)
{
#if 0
	timeinc1++;
	DWORD timenow1 = timeGetTime();
	if (timenow1 - timeold1 >= 1000)
	{
		timeold1 = timenow1;
		char loginfo[128] = { 0 };

		sprintf_s(loginfo, 128, "audio fps[%d]\n", timeinc1);
		OutputDebugStringA(loginfo);
		timeinc1 = 0;
	}
#endif

	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	unsigned int datalen = 0;
	pAgoraManager->pPlayerCaptureManager->getCircleBufferObject()->readBuffer(this->pPlayerData, nSize, &datalen);
//	memcpy(this->pPlayerData, pAgoraManager->pPlayerCaptureManager->pPlayerData, pAgoraManager->pPlayerCaptureManager->nPlayerDataLen);

	if (nSize > 0 && datalen > 0)
	{
		int nMixLen = 0;
		if (datalen > nSize)
			nMixLen = nSize;
		else
			nMixLen = datalen;

		char loginfo[128] = { 0 };
		sprintf_s(loginfo, 128, "len_need[%d] len[%d] \n", nSize, datalen);
		//OutputDebugStringA(loginfo);

#if 0
		outfile = fopen("e:/player.pcm", "ab+");
		if (outfile)
		{
			fwrite(this->pPlayerData, 1, datalen, outfile);
			fclose(outfile);
			outfile = NULL;
		}
#endif
		m_fileBkMusicDest.write((char*)pPlayerData, datalen);
		m_fileSource.write((char*)audioFrame.buffer, nSize);
		//mixAudioData((char*)this->pPlayerData, (char*)audioFrame.buffer, nMixLen);
		MixerAddS16((int16_t*)audioFrame.buffer,(int16_t*)pPlayerData,(audioFrame.channels * audioFrame.bytesPerSample) *  audioFrame.samples/ sizeof(int16_t));
		m_fileMix.write((char*)audioFrame.buffer, nMixLen);
#if 0
		outfile = fopen("D:/mix.pcm", "ab+");
		if (outfile)
		{
			fwrite(audioFrame.buffer, 1, nMixLen, outfile);
			fclose(outfile);
			outfile = NULL;
		}
#endif
	}
// 	outfile = fopen("e:/mic.pcm", "ab+");
// 	if (outfile)
// 	{
// 		fwrite(audioFrame.buffer, 1, nSize, outfile);
// 		fclose(outfile);
// 		outfile = NULL;
// 	}

	//	CAudioCapturePackageQueue::GetInstance()->PopAudioPackage(audioFrame.buffer, &nSize);
	
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
//	CAudioPlayPackageQueue::GetInstance()->PushAudioPackage(audioFrame.buffer, nSize);
	
	return true;
}

bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}
