#ifndef _HOOK_DSOUND_BUFFER_H_
#define _HOOK_DSOUND_BUFFER_H_

#include <Windows.h>
#include "dsound.h"
#include "AudioDataPool.h"
#include "SyncObjs.h"

class CHookDSoundBuffer
{
public:
	CHookDSoundBuffer(IDirectSoundBuffer* pDirectSoundBuffer, int totalBufferSize, LPWAVEFORMATEX lpwfxFormat);
	~CHookDSoundBuffer();
	
	void Play();
	void SetCurrentPosition(DWORD dwNewPosition);
	void SetVolume(LONG lVolume);
	void Stop();
	void CapturePlayData(bool forceRead);
	void OnTrigger();

private:
	DWORD m_currentPlayPos;
	DWORD m_totalBufferSize;
	BYTE* m_pBufferAddr;
	bool m_stop;
	CAudioDataPool* m_pAudioDataPool;
	IDirectSoundBuffer* m_pDirectSoundBuffer;
	int m_maxUpdateSize;
	int m_minUpdateSize;
	CLock m_lock;
};

#endif