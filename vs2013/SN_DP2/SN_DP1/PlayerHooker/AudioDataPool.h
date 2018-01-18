#ifndef _AUDIO_DATA_POOL_H_
#define _AUDIO_DATA_POOL_H_

#include <windows.h>
#include <MMSystem.h>
#include <math.h>
#include "CycBuffer.h"

class CAudioDataPool
{
public:
	CAudioDataPool(int bufferSize = 0);
	virtual ~CAudioDataPool();

	virtual void SetWaveFormatEx(const WAVEFORMATEX* pFormatEx);
	virtual WAVEFORMATEX GetWaveFormatEx();

	virtual UINT Write(void* pData, UINT dataLen);
	virtual UINT Read(void* pData, UINT dataReadLen);

	virtual void SetEndWrite(bool endWrite);
	virtual bool GetEndWrite();

	virtual void SetCanRead(bool canRead);
	virtual bool GetCanRead();

	virtual void SetEndHook(bool endHook);
	virtual bool CanDestory();

	virtual void Resize(int bufferSize);
	virtual UINT GetSize();

	virtual void Flush();

	void SetVolume(int volume);
	int GetVolume();

	UINT GetNotifySize();
	UINT GetUsedSize();

protected:
	WAVEFORMATEX m_waveFormatEx;
	CCycBuffer m_buffer;
	bool m_endWrite;
	bool m_endHook;
	bool m_canRead;
	int m_volume;
	UINT m_notifySize;
};

class CWaveOutAudioDataPool : public CAudioDataPool
{
public:
	CWaveOutAudioDataPool();
	~CWaveOutAudioDataPool();

	UINT WriteWaveHdr(LPWAVEHDR pwh);
private:
	PVOID m_firstWrite;
	bool m_canAutoResize;
};

#endif