#ifndef _CONVERTPCM_H_ 
#define _CONVERTPCM_H_

#include "AudioChunk.h"

struct BitSample24
{
	byte a0;
	byte a1;
	char a2;
};

int ConvertFloatTo8Bit(char* pData, UINT nDataSize);

int ConvertFloatTo16Bit(char* pData, UINT nDataSize);

int ConvertFloatTo24Bit(char* pData, UINT nDataSize);

int ConvertFloatTo32Bit(char* pData, UINT nDataSize);

int ConvertFloatToLinear(AudioSample* pData, UINT dataSize, int bps);

void ConvertMonoChunkToStereo(IAudioChunk* pChunk);

void ConvertLeftChunkToMono(IAudioChunk* pChunk);

void ConvertRightChunkToMono(IAudioChunk* pChunk);

void ConvertStereoChunkToMono(IAudioChunk* pChunk);

void ConvertLeftToStereo(char* pPCM, UINT dataBytes);

void ConvertRightToStereo(char* pPCM, UINT dataBytes);

void ConvertStereoChunkToStereoUseLeft(IAudioChunk* pChunk);

void ConvertStereoChunkToStereoUseRight(IAudioChunk* pChunk);

int ConvertStereoToMono(char* pPCM, UINT dataLen);

int ConvertStereoToMonoEx(char* pSrc, UINT dataLen, char* pDst);

void ConvertMonoToStereo(const char* pSrc, UINT srcLen, char* pDst, UINT dstLen);

#endif