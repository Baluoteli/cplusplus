#ifndef _HOOK_AUDIO_INPUT_H_
#define _HOOK_AUDIO_INPUT_H_

#include "Thread.h"
#include "AudioChunk.h"
#include "CycBuffer.h"
#include "SyncObjs.h"
#include "AudioDataHooker.h"
#include "AudioMixer.h"

class IAudioCaptureCallback;

class CHookAudioInput : public CThread
{
public:
	CHookAudioInput();
	~CHookAudioInput();
	
	virtual bool Open(int sampleRate, int channels, int bps, int inputBufferSize, int notifySize);

	virtual void Close();

	virtual void Start(IAudioCaptureCallback* callback);

	virtual void Stop();

	virtual bool IsCapturing();

protected:
	void Execute();

private:
	void NotifyCaptureData();

	CSleepEvent m_sleep;
	CSharedMem m_sharedMem;
	char* m_pNotifyBuffer;
	CAudioChunk m_hookChunk;
	string m_notifyData;
	IResampler* m_pResampler;
	bool m_stop;
	DWORD m_hookRef;
	bool m_onceHaveHookData;
	CLock m_lock;
	bool m_mixDataFlag;


	WAVEFORMATEX m_formatEx;

	UINT m_notifySize;

	int m_bufferSize;
	IAudioCaptureCallback* mpCallback;
};

bool HaveHookAudioRunning();

#endif