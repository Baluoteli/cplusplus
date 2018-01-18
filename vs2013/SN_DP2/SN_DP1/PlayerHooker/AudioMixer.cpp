#include "AudioMixer.h"
#include <assert.h>

CAudioMixer::CAudioMixer():m_audioSamplePtrs(NULL), m_inChunksCount(0), m_mixAdj(NULL), m_lastMixAdj(NULL),
	m_mixFactorCount(0)
{
}

CAudioMixer::~CAudioMixer()
{
	Clear();
}

void CAudioMixer::Clear()
{
	if (m_audioSamplePtrs != NULL)
	{
		delete[] m_audioSamplePtrs; 	
		m_audioSamplePtrs = NULL;
	}
	if (m_mixAdj != NULL)
	{
		delete[] m_mixAdj;
		m_mixAdj = NULL;
	}
	if (m_lastMixAdj != NULL)
	{
		delete[] m_lastMixAdj;
		m_lastMixAdj = NULL;
	}
	m_mixFactorCount = 0;
	m_audioSamplePtrs = NULL;
	m_inChunksCount = 0;
}

bool CAudioMixer::Process(const AudioChunkList& inChunks, CAudioChunk& outChunk)
{
	if (inChunks.size() <= 0)
	{
		return false;
	}

	if (inChunks.size() != m_inChunksCount)
	{
		if (m_audioSamplePtrs != NULL)
		{
			delete[] m_audioSamplePtrs; 	
		}
		m_inChunksCount = (int)inChunks.size();
		m_audioSamplePtrs = new AudioSample*[m_inChunksCount];
	}

	UINT sampleRate = 0;
	UINT channels = 0;
	UINT sampleCount = 0;
	for (int i = 0; i < m_inChunksCount; i++)
	{
		if (sampleRate != 0 && sampleRate != inChunks[i]->GetSampleRate())
		{
			assert(false);
			return false;
		}
		sampleRate = inChunks[i]->GetSampleRate();

		if (channels != 0 && channels != inChunks[i]->GetChannels())
		{
			assert(false);
			return false;
		}
		channels = inChunks[i]->GetChannels();

		if (sampleCount != 0 && sampleCount != inChunks[i]->GetSampleCount())
		{
			assert(false);
			return false;
		}
		sampleCount = inChunks[i]->GetSampleCount();

		m_audioSamplePtrs[i] = inChunks[i]->GetData();
	}

	if (sampleRate == 0 || channels == 0 || sampleCount == 0)
	{
		assert(false);
		return false;
	}
	
	outChunk.SetChannels(channels);
	outChunk.SetSampleCount(sampleCount);
 	outChunk.SetSampleRate(sampleRate);
 	outChunk.SetDataSize(inChunks[0]->GetDataSize());

	MixToChunk(outChunk);

	return true;
}

inline bool IsOverFlow(AudioSample sample)
{
	if (sample > 1.0 || sample < -1.0)
	{
		return true;
	}
	return false;
}

void CAudioMixer::MixToChunk(CAudioChunk& outChunk)
{
	AudioSample* pOut = outChunk.GetData();
	AudioSample mixTotal = 0.0;

	UINT channels = outChunk.GetChannels();
	UINT sampleCount = outChunk.GetSampleCount();
	if (m_mixFactorCount != channels)
	{
		if (m_mixAdj != NULL)
		{
			delete[] m_mixAdj;
		}
		if (m_lastMixAdj != NULL)
		{
			delete[] m_lastMixAdj;
		}
		m_mixFactorCount = channels;
		m_mixAdj = new double[m_mixFactorCount];
		m_lastMixAdj = new double[m_mixFactorCount];
		for (int i = 0; i < m_mixFactorCount; i++)
		{
			m_lastMixAdj[i] = 1.0;
		}
	}

	for (UINT i = 0; i < channels; i++)
	{
		m_mixAdj[i] = 1.0;

		for (UINT j = 0; j < sampleCount; j++)
		{
			mixTotal = 0.0;
			for (int k = 0; k < m_inChunksCount; k++)
			{
				//do mix
				mixTotal += *(m_audioSamplePtrs[k] + j * channels + i); 
			}

			*(pOut + j * channels + i) = mixTotal;

			if (IsOverFlow(mixTotal))
			{
				double tempAdj = 1.0 / mixTotal;
				if (tempAdj < 0.0)
				{
					tempAdj = -tempAdj;
				}
				if (tempAdj < m_mixAdj[i])
				{
					m_mixAdj[i] = tempAdj;
				}
			}
		}
	}	

	MixAdjustChunk(pOut, channels, sampleCount);
}

inline void SmoothAdjust(double last, double& target)
{
	if (target < last)
	{
		if (last > dFACTOR_STEP)
		{
			target = last - dFACTOR_STEP;
		}
	}
	else
	{
		if (last + dFACTOR_STEP < 1.0)
		{
			target = last + dFACTOR_STEP;
		}
	}
}

void CAudioMixer::MixAdjustChunk(AudioSample* pData, UINT channels, UINT sampleCount)
{
	AudioSample sample = 0.0;
	for (UINT i = 0; i < channels; i++)
	{	
		SmoothAdjust(m_lastMixAdj[i], m_mixAdj[i]);
		m_lastMixAdj[i] = m_mixAdj[i];

		for (UINT j = 0; j < sampleCount; j++)
		{
			sample =  AudioSample(*(pData + j * channels + i) * m_mixAdj[i]);
			if (sample > 1.0)
			{
				sample = 1.0;
			}
			if (sample < -1.0)
			{
				sample = -1.0;
			}

			*(pData + j * channels + i) = sample;
		}
	}
}






