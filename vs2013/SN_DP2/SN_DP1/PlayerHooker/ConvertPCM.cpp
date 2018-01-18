#include "ConvertPCM.h"

inline BitSample24 Cvt32BitTo24(int nSample)
{
	BitSample24	bitSample24;
	bitSample24.a0 = nSample;
	bitSample24.a1 = nSample >> 8;
	bitSample24.a2 = nSample >> 16;
	return bitSample24;
}

inline int Clip24(int nVal)
{
	if (nVal > 8388607) 
	{
		return 8388607;
	}
	else if (nVal < -8388608)
	{
		return -8388608;
	}
	return nVal;
}

int ConvertFloatTo8Bit(char* pData, UINT nDataSize)
{
	float* pSrc = (float*)pData;
	BYTE* pDst = (BYTE*)pData;			
	UINT processCount = nDataSize / 4;
	for (UINT i = 0; i < processCount; i++)
	{
		if (*pSrc >= 1.0) 
		{
			*pDst = 255;
		}
		else if (*pSrc < -1.0) 
		{
			*pDst = 0;
		}
		else 
		{
			*pDst = (BYTE)(128 + (*pSrc) * 128);
		}

		pSrc++;
		pDst++;
	}
	return nDataSize / 4;
}

int ConvertFloatTo16Bit(char* pData, UINT nDataSize)
{
	if (nDataSize <= 0)
	{
		return -1;
	}
	float* pSrc = (float*)pData;
	short* pDst = (short*)pData;				
	union {float f; DWORD i;} u;
	UINT processCount = nDataSize / 4;
	for (UINT i = 0; i < processCount; i++)
	{
		u.f = float(*pSrc + 384.0);
		if (u.i > 0x43c07fff) 
		{
			*pDst = 32767;
		}
		else if (u.i < 0x43bf8000) 
		{
			*pDst = -32768;
		}
		else 
		{
			*pDst = short(u.i - 0x43c00000);
		}
		pSrc++;
		pDst++;
	}
	return nDataSize / 2;
}

int ConvertFloatTo24Bit(char* pData, UINT nDataSize)
{
	float* pSrc = (float*)pData;
	BitSample24* pDst = (BitSample24*)pData;		
	UINT processCount = nDataSize / 4;
	for (UINT i = 0; i < processCount; i++)
	{
		*pDst = Cvt32BitTo24(Clip24(int((*pSrc) * 8388608))); 
		pSrc++;
		pDst++;
	}	
	return (nDataSize * 3) / 4;
}

int ConvertFloatTo32Bit(char* pData, UINT nDataSize)
{
	float* pSrc = (float*)pData;
	long* pDst = (long*)pData;		
	UINT processCount = nDataSize / 4;
	for (UINT i = 0; i < processCount; i++)
	{
		if (*pSrc >= 1.0) 
		{
			*pDst = 2147483647;
		}
		else if (*pSrc < -1.0) 
		{
			*pDst = -2147483647;
		}
		else 
		{
			*pDst = long((*pSrc) * 2147483647);
		}
		pSrc++;
		pDst++;
	}
	return nDataSize;
}

int ConvertFloatToLinear(AudioSample* pData, UINT dataSize, int bps)
{
	switch (bps)
	{
	case 8:
		{
			return ConvertFloatTo8Bit((char*)pData, dataSize);
		}
		break;
	case 16:
		{
			return ConvertFloatTo16Bit((char*)pData, dataSize);
		}
		break;
	case 24:
		{
			return ConvertFloatTo24Bit((char*)pData, dataSize);
		}
		break;
	case 32:
		{
			return ConvertFloatTo32Bit((char*)pData, dataSize);
		}
		break;
	default:
		return -1;
	}
}

void ConvertMonoChunkToStereo(IAudioChunk* pChunk)
{
	if (pChunk->GetChannels() != 1)
	{
		return;
	}

	UINT sampleCount = pChunk->GetSampleCount();
	CAudioChunk tempChunk;
	tempChunk.SetDataSize(sampleCount * 2 * sizeof(float));
	tempChunk.SetChannels(2);
	tempChunk.SetSampleRate(pChunk->GetSampleRate());
	tempChunk.SetSampleCount(sampleCount);
	AudioSample* pSrc = pChunk->GetData();
	AudioSample* pDst = tempChunk.GetData();
	for (UINT i = 0; i < sampleCount; i++)
	{
		pDst[i * 2] = pSrc[i];
		pDst[i * 2 + 1] = pSrc[i];
	}
	pChunk->Copy(&tempChunk);
}

void ConvertLeftChunkToMono(IAudioChunk* pChunk)
{
	if (pChunk->GetChannels() != 2)
	{
		return;
	}
	AudioSample* pData = pChunk->GetData();
	UINT sampleCount = pChunk->GetSampleCount();
	for (UINT i = 0; i < sampleCount; i++)
	{
		pData[i] = pData[2 * i];
	}
	pChunk->SetChannels(1);
	pChunk->SetDataSize(pChunk->GetDataSize() / 2);
}

void ConvertRightChunkToMono(IAudioChunk* pChunk)
{
	if (pChunk->GetChannels() != 2)
	{
		return;
	}
	AudioSample* pData = pChunk->GetData();
	UINT sampleCount = pChunk->GetSampleCount();
	for (UINT i = 0; i < sampleCount; i++)
	{
		pData[i] = pData[2 * i + 1];
	}
	pChunk->SetChannels(1);
	pChunk->SetDataSize(pChunk->GetDataSize() / 2);
}

void ConvertStereoChunkToMono(IAudioChunk* pChunk)
{
	if (pChunk->GetChannels() != 2)
	{
		return;
	}
	AudioSample* pData = pChunk->GetData();
	UINT sampleCount = pChunk->GetSampleCount();
	for (UINT i = 0; i < sampleCount; i++)
	{
		pData[i] = (pData[2 * i] + pData[2 * i + 1]) / 2;
	}
	pChunk->SetChannels(1);
	pChunk->SetDataSize(pChunk->GetDataSize() / 2);
}

void ConvertStereoChunkToStereoUseLeft(IAudioChunk* pChunk)
{
	if (pChunk->GetChannels() != 2)
	{
		return;
	}
	AudioSample* pData = pChunk->GetData();
	UINT sampleCount = pChunk->GetSampleCount();
	for (UINT i = 0; i < sampleCount; i++)
	{
		pData[2 * i + 1] = pData [2 * i];
	}
}

void ConvertStereoChunkToStereoUseRight(IAudioChunk* pChunk)
{
	if (pChunk->GetChannels() != 2)
	{
		return;
	}
	AudioSample* pData = pChunk->GetData();
	UINT sampleCount = pChunk->GetSampleCount();
	for (UINT i = 0; i < sampleCount; i++)
	{
		pData[2 * i] = pData [2 * i + 1];
	}
}

void ConvertLeftToStereo(char* pPCM, UINT dataBytes)
{
	UINT sampleCount = dataBytes / 4;
	short* pData =(short*) pPCM;
	for (UINT i = 0; i < sampleCount; i++)
	{
		pData[2 * i + 1] = pData[2 * i];
	}
}

void ConvertRightToStereo(char* pPCM, UINT dataBytes)
{
	UINT sampleCount = dataBytes / 4;
	short* pData =(short*) pPCM;
	for (UINT i = 0; i < sampleCount; i++)
	{
		pData[2 * i] = pData[2 * i + 1];
	}
}

int ConvertStereoToMono(char* pPCM, UINT dataLen)
{
	short* pShortPCM = (short*)pPCM;
	UINT sampleCount = dataLen / sizeof(short) / 2;
	for (UINT i = 0; i < sampleCount; i++)
	{
		pShortPCM[i] = (int(pShortPCM[2 * i]) + int(pShortPCM[2 * i + 1])) / 2;
	}
	return dataLen / 2;
}

int ConvertStereoToMonoEx(char* pSrc, UINT dataLen, char* pDst)
{
	short* pShortPCM = (short*)pSrc;
	short* pShortDst = (short*)pDst;
	UINT sampleCount = dataLen / sizeof(short) / 2;
	for (UINT i = 0; i < sampleCount; i++)
	{
		pShortDst[i] = (int(pShortPCM[2 * i]) + int(pShortPCM[2 * i + 1])) / 2;
	}
	return dataLen / 2;
}

void ConvertMonoToStereo(const char* pSrc, UINT srcLen, char* pDst, UINT dstLen)
{
	if (srcLen * 2 != dstLen)
	{
		return;
	}

	UINT sampleCount = srcLen / sizeof(short);
	short* pShortSrc = (short*)pSrc;
	short* pShortDst = (short*)pDst;
	for (UINT i = 0; i < sampleCount; i++)
	{
		pShortDst[2 * i] = pShortSrc[i];
		pShortDst[2 * i + 1] = pShortSrc[i];
	}
}