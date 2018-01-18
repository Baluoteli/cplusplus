#include "AudioDataPool.h"
#include "AudioDataHooker.h"
#include <MMReg.h>
#include <MMSystem.h>

CAudioDataPool::CAudioDataPool(int bufferSize): m_buffer(bufferSize * 3 / 2), m_endWrite(false), m_endHook(false),
	m_volume(100), m_canRead(false), m_notifySize(0)
{

}

CAudioDataPool::~CAudioDataPool()
{

}

void CAudioDataPool::Resize(int bufferSize)
{
	m_buffer.Resize(bufferSize * 3 / 2);
}

void CAudioDataPool::SetWaveFormatEx(const WAVEFORMATEX* pFormatEx)
{
	if (pFormatEx == NULL)
	{
		return;
	}
	int sampleSize = pFormatEx->nChannels * (pFormatEx->wBitsPerSample / 8);
	m_waveFormatEx = *pFormatEx;
	m_notifySize = UINT(double(pFormatEx->nChannels * (pFormatEx->wBitsPerSample / 8) * pFormatEx->nSamplesPerSec)
		/ dwHOOK_AUDIO_DATA_AVGBYTESPERSEC * dwNOTIFY_SIZE);
	if (sampleSize != 0)
	{
		m_notifySize -= m_notifySize % sampleSize;
	}
	if (pFormatEx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		try
		{
			WAVEFORMATEXTENSIBLE* pFormatExtend = (WAVEFORMATEXTENSIBLE*)pFormatEx;
			if (IsEqualGUID(pFormatExtend->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
			{
				m_waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;	
			}
			else
			{
				m_waveFormatEx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
			}
		}
		catch (...)
		{	
			m_waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
		}
	}
	// 	CAudioDataHooker::ms_log.Trace(_T("CAudioDataPool::SetWaveFormatEx: %d, %d, %d, %d\n"), m_notifySize, 
	// 		sampleSize, waveFormatEx.nChannels, waveFormatEx.wBitsPerSample);
}

WAVEFORMATEX CAudioDataPool::GetWaveFormatEx()
{
	return m_waveFormatEx;
}

UINT CAudioDataPool::Write(void* pData, UINT dataLen)
{
	if (m_endWrite)
	{
		return 0;
	}

	if (m_buffer.GetFreeSize() < dataLen)
	{
		UINT bytesToRead = m_buffer.GetBufferSize() / 3 * 2;
		m_buffer.Read(NULL, bytesToRead, &bytesToRead);
	}
	return m_buffer.Write(pData, dataLen);
}

UINT CAudioDataPool::Read(void* pData, UINT dataReadLen)
{
	if (!m_canRead || m_buffer.GetUsedSize() < dataReadLen)
	{
		return 0;
	}
	UINT realReadLen = 0;
	m_buffer.Read(pData, dataReadLen, &realReadLen);
	return realReadLen;
}

void CAudioDataPool::SetEndWrite(bool endWrite)
{
	m_endWrite = endWrite;
}

bool CAudioDataPool::GetEndWrite()
{
	return m_endWrite;
}

void CAudioDataPool::SetEndHook(bool endHook)
{
	m_endHook = endHook;
}

bool CAudioDataPool::CanDestory()
{
	return m_endHook && m_buffer.GetUsedSize() < m_notifySize;
}

UINT CAudioDataPool::GetSize()
{
	return m_buffer.GetBufferSize();
}

void CAudioDataPool::Flush()
{
	m_buffer.Flush();
}

void CAudioDataPool::SetVolume(int volume)
{
	m_volume = volume;
}

int CAudioDataPool::GetVolume()
{
	return m_volume;
}

UINT CAudioDataPool::GetNotifySize()
{
	return m_notifySize;
}

UINT CAudioDataPool::GetUsedSize()	
{
	return m_buffer.GetUsedSize();
}

void CAudioDataPool::SetCanRead(bool canRead)
{
	CAudioDataHooker::ms_log.Trace(_T("CAudioDataPool::SetCanRead: %d\n"), canRead);
	m_canRead = canRead;
}

bool CAudioDataPool::GetCanRead()
{
	return m_canRead;
}

//

CWaveOutAudioDataPool::CWaveOutAudioDataPool(): m_firstWrite(NULL), m_canAutoResize(true)
{

}

CWaveOutAudioDataPool::~CWaveOutAudioDataPool()
{

}

//static int s_bufferSize = 0;

UINT CWaveOutAudioDataPool::WriteWaveHdr(LPWAVEHDR pwh)
{
	UINT bytesWrite = 0;
	//CAudioDataHooker::ms_log.Trace(_T("CWaveOutAudioDataPool::Write: %d, %d\n"), pwh, pwh->dwBufferLength);
	//s_bufferSize += pwh->dwBufferLength;
	if (m_firstWrite == pwh)
	{
		m_canAutoResize = false;	
		//CAudioDataHooker::ms_log.Trace(_T("m_firstWrite == pData, %d\n"), s_bufferSize);
	}

	if (m_canAutoResize)
	{
		//CAudioDataHooker::ms_log.Trace(_T("m_canAutoResize: %d\n"), pwh->dwBufferLength);
		if (m_buffer.GetBufferSize() < 655360)
		{
			m_buffer.Resize(m_buffer.GetBufferSize() + 2 * pwh->dwBufferLength);
		}
		else
		{
			m_canAutoResize = false;
		}
	}

	if (m_buffer.GetFreeSize() < pwh->dwBufferLength)
	{
		UINT bytesToRead = pwh->dwBufferLength * 2;
		m_buffer.Read(NULL, bytesToRead, &bytesToRead);
	}

	//CAudioDataHooker::ms_log.Trace(_T("m_buffer.GetFreeSize() < pwh->dwBufferLength: %d\n"), m_buffer.GetFreeSize());
	bytesWrite = m_buffer.Write(pwh->lpData, pwh->dwBufferLength);

	m_canRead = true;
	return bytesWrite;
}


