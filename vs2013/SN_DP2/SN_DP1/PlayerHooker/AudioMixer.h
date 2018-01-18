#ifndef _AUDIO_MIXER_H_
#define _AUDIO_MIXER_H_

#include <vector>

#include "AudioChunk.h"

using namespace std;

typedef vector<CAudioChunk*> AudioChunkList;

const double dFACTOR_STEP = 1.0 / 32;

//support only same samplerate, channels, samplecount, resample need to be done before use this

class __declspec(dllexport) CAudioMixer
{
public:
	CAudioMixer();
	~CAudioMixer();

	bool Process(const AudioChunkList& inChunks, CAudioChunk& outChunk);

private:
	void Clear();
	void MixToChunk(CAudioChunk& outChunk);
	void MixAdjustChunk(AudioSample* pData, UINT channels, UINT sampleCount);

	AudioSample** m_audioSamplePtrs;
	double* m_mixAdj;
	double* m_lastMixAdj;
	int m_mixFactorCount;
	int m_inChunksCount;
};

#endif