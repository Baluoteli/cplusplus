#ifndef _RESAMPLER_EX_H_ 
#define _RESAMPLER_EX_H_

#include <Windows.h>
#include "SpeexResampler.h"
#include <string>

using namespace std;

class IResamplerEx
{
public:
	virtual void Destroy() = 0;
	virtual bool Init(int channels, int srcRate, int dstRate, int outSampleCount, bool useSSE2 = false) = 0;
	virtual bool Process(IAudioChunk* pSrcChunk, IAudioChunk* pDstChunk) = 0;
};

class CResamplerEx: public IResamplerEx
{
public:
	CResamplerEx();
	~CResamplerEx();

	void Destroy();

	bool Init(int channels, int srcRate, int dstRate, int outSampleCount, bool useSSE2 = false);
	bool Process(IAudioChunk* pSrcChunk, IAudioChunk* pDstChunk);

private:
	void Clear();
	IResampler* m_pResampler;
	int m_outSampleCount;
	int m_dstSampleRate;
	string m_outSampleLeftData;
};

extern "C" __declspec(dllexport) IResamplerEx* CreateResamplerEx();

#endif