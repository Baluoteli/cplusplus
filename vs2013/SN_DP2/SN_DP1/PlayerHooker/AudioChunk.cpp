#include "AudioChunk.h"

static bool bMachineIsBigEndian = false;
static bool bTestMachineIsBigEndian = false;

inline bool MachineIsBigEndian()
{
	if (!bTestMachineIsBigEndian)
	{
		BYTE temp[4];
		*(DWORD*)temp = 0xDEADBEEF;
		bTestMachineIsBigEndian = true;
		bMachineIsBigEndian = temp[0]==0xDE;
		return bMachineIsBigEndian;
	}
	else
	{
		return bMachineIsBigEndian; 
	}
}

inline bool MachineIsLittleEndian()
{
	return !MachineIsBigEndian();
}

void SwapOrder(void* p, unsigned nBytes)
{
	BYTE* ptr = (BYTE*)p;
	BYTE t;
	unsigned n;
	for(n = 0; n < nBytes>>1; n++)
	{
		t = ptr[n];
		ptr[n] = ptr[nBytes - n - 1];
		ptr[nBytes - n - 1] = t;
	}
}

CAudioChunk::CAudioChunk()
{
	m_nSampleRate = 0;
	m_nChannels = 0;
	m_nSamples = 0;
	m_nDataSize = 0;
	m_pData = NULL;
}

CAudioChunk::~CAudioChunk()
{
	free(m_pData);
}

void CAudioChunk::CheckDataSize(UINT nSize)
{
	if (nSize > GetDataSize()) 
	{
		SetDataSize(nSize); 
	}
}

bool CAudioChunk::SetData(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits, bool bFloat)
{
	if (bFloat)
	{
		return SetDataFloatingPoint(pSrc, nSize, nSampleRate, nChannels, nBits);
	}
	else
	{
		return SetDataFixedPoint(pSrc, nSize, nSampleRate, nChannels, nBits);	
	}
}

bool CAudioChunk::AppendData(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, 
							 UINT nBits, bool bFloat)
{
	if (bFloat)
	{
		return AppendDataFloatingPoint(pSrc, nSize, nSampleRate, nChannels, nBits);
	}
	else
	{
		return AppendDataFixedPoint(pSrc, nSize, nSampleRate, nChannels, nBits);	
	}
}

bool CAudioChunk::SetDataFloatingPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits)
{
	if (nBits != 32 && nBits != 64)
	{
		return false;
	}
	UINT nBytesPerSample = nBits>>3;
	UINT nCount = nSize / nBytesPerSample;
	CheckDataSize(nSize);

	if (m_pData == NULL) 
	{
		Reset();
		return false;
	}

	SetSampleCount(nCount / nChannels);
	SetSampleRate(nSampleRate);
	SetChannels(nChannels);

	unsigned char szTemp[8];
	const unsigned char* pcSrc = (const unsigned char*)pSrc;
	AudioSample* out = (AudioSample*)m_pData;
	
	while(nCount)
	{
		AudioSample val;
		memcpy(szTemp, pcSrc, nBytesPerSample);
		if (nBits == 32) 
		{
			val = (AudioSample)*(float*)&szTemp;
		}
		else 
		{
			val = (AudioSample)*(double*)&szTemp;
		}
		*(out++) = val;
		pcSrc += nBytesPerSample;
		nCount--;
	}	
	return true;
}

bool CAudioChunk::AppendDataFloatingPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits)
{
	if (m_nSampleRate != nSampleRate || m_nChannels != nChannels)
	{
		return false;
	}

	UINT nBytesPerSample = nBits>>3;
	UINT nCount = nSize / nBytesPerSample;
	
	if ((m_nSamples * m_nChannels * 4 + nSize) > m_nDataSize)
	{
		return false;
	}

	if (m_pData == NULL) 
	{
		Reset();
		return false;
	}

	unsigned char szTemp[8];
	const unsigned char* pcSrc = (const unsigned char*)pSrc;
	AudioSample* out = (AudioSample*)m_pData + m_nSamples * m_nChannels;

	while(nCount)
	{
		AudioSample val;
		memcpy(szTemp, pcSrc, nBytesPerSample);
		if (nBits == 32) 
		{
			val = (AudioSample)*(float*)&szTemp;
		}
		else 
		{
			val = (AudioSample)*(double*)&szTemp;
		}
		*(out++) = val;
		pcSrc += nBytesPerSample;
		nCount--;
	}	

	SetSampleCount(nCount / nChannels + m_nSamples);

	return true;
}

bool CAudioChunk::SetDataFixedPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits, UINT nFlags)
{
	UINT nCount = nSize / (nBits / 8);
	CheckDataSize(nCount * 4);
	if (m_pData == NULL)
	{
		Reset();
		return false;
	}

	SetSampleCount(nCount / nChannels);
	SetSampleRate(nSampleRate);
	SetChannels(nChannels);

	bool bSigned;
	bool bNeedSwap;

	if (nFlags > 0)
	{
		bSigned = !!(nFlags & FLAG_SIGNED);
		bNeedSwap = !!(nFlags & FLAG_BIG_ENDIAN);
		if (MachineIsBigEndian()) 
		{
			bNeedSwap = !bNeedSwap;
		}
	}
	else
	{
		bSigned = nBits > 8;
		bNeedSwap = false;
	}

	AudioSample* pBuffer = (AudioSample*)m_pData;
	switch(nBits)
	{
	case 8:
		sucks<char, false>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 16:
		sucks<short, false>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 24:
		sucks<long, true>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 32:
		sucks<long, false>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 40:
	case 48:
	case 56:
	case 64:
		sucks<__int64, true>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	default:
		break;
	}

	return true;
}

bool CAudioChunk::AppendDataFixedPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits, UINT nFlags)
{
	if (m_nSampleRate != nSampleRate || m_nChannels != nChannels)
	{
		return false;
	}

	UINT nCount = nSize / (nBits / 8);

	if ((m_nSamples * m_nChannels + nCount) > m_nDataSize / (nBits / 8))
	{
		return false;
	}

	if (m_pData == NULL)
	{
		Reset();
		return false;
	}

	bool bSigned;
	bool bNeedSwap;

	if (nFlags > 0)
	{
		bSigned = !!(nFlags & FLAG_SIGNED);
		bNeedSwap = !!(nFlags & FLAG_BIG_ENDIAN);
		if (MachineIsBigEndian()) 
		{
			bNeedSwap = !bNeedSwap;
		}
	}
	else
	{
		bSigned = nBits > 8;
		bNeedSwap = false;
	}

	AudioSample* pBuffer = (AudioSample*)m_pData + m_nSamples * m_nChannels;
	switch(nBits)
	{
	case 8:
		sucks<char, false>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 16:
		sucks<short, false>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 24:
		sucks<long, true>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 32:
		sucks<long, false>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	case 40:
	case 48:
	case 56:
	case 64:
		sucks<__int64, true>::DoFixedpointConvert(bNeedSwap,
			bSigned, pSrc, nBits, nCount, pBuffer);
		break;
	default:
		break;
	}

	SetSampleCount(nCount / nChannels + m_nSamples);

	return true;
}

AudioSample* CAudioChunk::GetData()
{
	return m_pData;
}

void CAudioChunk::Reset()
{
	SetSampleCount(0);
	SetSampleRate(0);
	SetChannels(0);
}

UINT CAudioChunk::GetDataSize()
{
	return m_nDataSize;
}

void CAudioChunk::SetDataSize(UINT nNewSize)
{
	if (m_pData == NULL)
	{
		m_nDataSize = nNewSize;
		m_pData = (AudioSample*)malloc(nNewSize);
	}
	else
	{
		if (m_nDataSize != nNewSize)
		{
			m_nDataSize = nNewSize;
			void* pData;
			pData = m_pData;
			m_pData = (AudioSample*)realloc(pData, nNewSize);
			if (m_pData == NULL && pData != NULL)
			{
				free(pData);
			}
		}
	}
}

UINT CAudioChunk::GetSampleRate()
{
	return m_nSampleRate;
}

void CAudioChunk::SetSampleRate(UINT nSampleRate)
{
	m_nSampleRate = nSampleRate;
}

void CAudioChunk::SetChannels(UINT nChannels)
{
	m_nChannels = nChannels;
}

void CAudioChunk::SetSampleCount(UINT nSampleCount)
{
	m_nSamples = nSampleCount;
}

bool CAudioChunk::IsEmpty()
{
	return GetChannels() == 0 || GetSampleRate() == 0 || GetSampleCount() == 0;
}

UINT CAudioChunk::GetDataLength()
{
	return GetSampleCount() * GetChannels();
}

void CAudioChunk::Flush()
{
	if (m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
		m_nDataSize = 0;
		Reset();
	}
}

void CAudioChunk::Copy(IAudioChunk* pSrcChunk)
{
	this->SetDataFloatingPoint(pSrcChunk->GetData(), pSrcChunk->GetDataSize(), pSrcChunk->GetSampleRate(), 
		pSrcChunk->GetChannels(), 32);
}
