#ifndef _RESAMPLE_H_
#define _RESAMPLE_H_

#include <Windows.h>
#include <stdlib.h>

#include "AudioChunk.h"

//just support float32 pcm data

class IResampler
{
public:
	virtual void Destroy() = 0;
	virtual bool Init(int channels, int srcRate, int dstRate, bool useSSE2 = false) = 0;
	virtual int Process(IAudioChunk* pSrcChunk, IAudioChunk* pDstChunk) = 0;
	virtual void Flush() = 0;
};

extern "C" __declspec(dllexport) IResampler* CreateResampler();

#endif