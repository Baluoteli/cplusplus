#include "HookDSoundBuffer.h"
#include "AudioDataHooker.h"
#include "Utils.h"

CHookDSoundBuffer::CHookDSoundBuffer(IDirectSoundBuffer* pDirectSoundBuffer, int totalBufferSize, LPWAVEFORMATEX lpwfxFormat):
	m_pDirectSoundBuffer(pDirectSoundBuffer), m_totalBufferSize(totalBufferSize), m_currentPlayPos(0), m_stop(true)
{
	m_pAudioDataPool = CAudioDataHooker::Instance()->CreateAudioDataPool(m_totalBufferSize);
	m_pAudioDataPool->SetWaveFormatEx(lpwfxFormat);
	m_minUpdateSize = m_pAudioDataPool->GetNotifySize() / 2;
	m_maxUpdateSize = m_totalBufferSize - m_pAudioDataPool->GetNotifySize();
	CAudioDataHooker::ms_log.Trace(_T("CHookDirectSoundBuffer Create: %d\n"), GetTickCount());
	void* p1 = 0;
	void* p2 = 0;
	DWORD s1 = 0;
	DWORD s2 = 0;
	if (SUCCEEDED(pDirectSoundBuffer->Lock(0, totalBufferSize, &p1, &s1, &p2, &s2, 0)))
	{
		if (p1 != NULL)
		{
			m_pBufferAddr = (BYTE*)p1;
		}
		pDirectSoundBuffer->Unlock(p1, s1, p2, s2);
	}
}

CHookDSoundBuffer::~CHookDSoundBuffer()
{
	CAudioDataHooker::ms_log.Trace(_T("~CHookDirectSoundBuffer: %d\n"), GetTickCount());
	CAudioDataHooker::Instance()->DeleteHookDataPool(m_pAudioDataPool);
}

void CHookDSoundBuffer::Play()
{
	INSYNC(m_lock);
	m_stop = false;
	CAudioDataHooker::ms_log.Trace(_T("CHookDirectSoundBuffer Play: %d\n"), GetTickCount());
	m_pAudioDataPool->SetEndWrite(false);
	m_pAudioDataPool->SetCanRead(true);
}

void CHookDSoundBuffer::SetCurrentPosition(DWORD dwNewPosition)
{
	INSYNC(m_lock);
	if (m_stop)
	{
		CAudioDataHooker::ms_log.Trace(_T("CHookDirectSoundBuffer SetCurrentPosition: %d\n"), dwNewPosition);
		//m_pAudioDataPool->Flush();
	}
	m_currentPlayPos = dwNewPosition;
}

void CHookDSoundBuffer::SetVolume(LONG lVolume)
{
	INSYNC(m_lock);
	m_pAudioDataPool->SetVolume(GetBasicAudioVolume(lVolume) / 100);
}

void CHookDSoundBuffer::Stop()
{
	m_stop = true;
	CAudioDataHooker::ms_log.Trace(_T("CHookDirectSoundBuffer Stop:%d\n"), GetTickCount());
	m_pAudioDataPool->SetEndWrite(true);
	m_pAudioDataPool->SetCanRead(false);
}

void CHookDSoundBuffer::CapturePlayData(bool forceRead)
{
	INSYNC(m_lock);
	if (!m_pAudioDataPool->GetEndWrite())
	{
		DWORD currentPlayPos;
		int updateSize = 0;
		int minUpdateSize = m_minUpdateSize;
		m_pDirectSoundBuffer->GetCurrentPosition(&currentPlayPos, NULL);

		updateSize = currentPlayPos - m_currentPlayPos;
		if (updateSize < 0)
		{
			updateSize += m_totalBufferSize;
		}

		if (forceRead)
		{
			minUpdateSize = 0;
		}

		//超过最大可能更新数据大小，认为是GetCurrentPosition错误返回currentPlayPos导致的
		if (updateSize > m_maxUpdateSize)
		{
			return;
		}
		if (updateSize >= minUpdateSize)
		{
			if (currentPlayPos > m_currentPlayPos)
			{
				m_pAudioDataPool->Write(m_pBufferAddr + m_currentPlayPos, currentPlayPos - m_currentPlayPos);
			}
			else if (currentPlayPos != m_currentPlayPos)
			{
				m_pAudioDataPool->Write(m_pBufferAddr + m_currentPlayPos, m_totalBufferSize - m_currentPlayPos);
				m_pAudioDataPool->Write(m_pBufferAddr, currentPlayPos);
			}
// 	 		CAudioDataHooker::ms_log.Trace(_T("IDirectSoundBuffer->Lock: %d, %d, %d, %d, %d\n"), 
// 	 			updateSize, m_currentPlayPos, currentPlayPos, *ppvAudioPtr1, *ppvAudioPtr2);
			m_currentPlayPos = currentPlayPos;
		}
	}	
}

void CHookDSoundBuffer::OnTrigger()
{
	CapturePlayData(false);
}