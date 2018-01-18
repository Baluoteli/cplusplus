#include "SpeexResampler.h"
#include "speex\speex_resampler.h"

class CResampler : public IResampler
{
public:
	CResampler();
	~CResampler();

	bool Init(int channels, int srcRate, int dstRate, bool useSSE2 = false);
	int Process(IAudioChunk* pSrcChunk, IAudioChunk* pDstChunk);
	void Flush();
	void Destroy()
	{
		delete this;
	}

private:
	void Clear();

	SpeexResamplerState** m_resamplerStates;
	int m_dstRate;
	int m_srcRate;
	double m_resampleRatio;
	int m_channels;
};

IResampler* CreateResampler()
{
	return new CResampler();
}

CResampler::CResampler():m_channels(0), m_resamplerStates(NULL), m_dstRate(0), m_srcRate(0), m_resampleRatio(1.0)
{

}

CResampler::~CResampler()
{
	Clear();
}

bool CResampler::Init(int channels, int srcRate, int dstRate, bool useSSE2)
{
	Clear();
	int err = 0;
	if (channels > 0 && channels < 8 && srcRate > 0)
	{
		int resampleQuality = 0;
		if (dstRate > srcRate)
		{
			resampleQuality = 2;	
		}
		else
		{
			resampleQuality = 3;	
		}

		if (useSSE2)
		{
			//only use when can support sse2
			useSSE2 = false;
		}

		m_resamplerStates = new SpeexResamplerState*[channels];
		for (int i = 0; i < channels; i++)
		{
			m_resamplerStates[i] = speex_resampler_init(1, srcRate, dstRate, resampleQuality, &err); 
		}
		m_dstRate = dstRate;
		m_srcRate = srcRate;
		m_channels = channels;	
		m_resampleRatio = double(dstRate) / srcRate;
	}
	return (NULL != m_resamplerStates);
}

int CResampler::Process(IAudioChunk* pSrcChunk, IAudioChunk* pDstChunk)
{
	if ((m_resamplerStates == NULL) || (pSrcChunk->GetSampleRate() < 1) || (pSrcChunk->GetChannels() != m_channels))
	{
		return -1;
	}

	spx_uint32_t inBufferLen = pSrcChunk->GetSampleCount();
	float* pInBuffer = (float*)malloc(inBufferLen * pSrcChunk->GetChannels() * sizeof(float));

	spx_uint32_t outBufferLen = spx_uint32_t(inBufferLen * m_resampleRatio);
	outBufferLen += 1024 - outBufferLen % 1024;
	float* pOutBuffer = (float*)malloc(outBufferLen * pSrcChunk->GetChannels() * sizeof(float));
	bool initDstChunk = false;

	spx_uint32_t minOutBufferLen = 0;

	for (int channelsIdx = 0; channelsIdx < m_channels; channelsIdx++)
	{
		int err = 0;
		float* pDst = NULL;
		float* pSrc = pSrcChunk->GetData();
		ZeroMemory(pOutBuffer, sizeof(float) * outBufferLen);
		for (UINT i = 0; i < inBufferLen; i++)
		{
			pInBuffer[i] = pSrc[i * m_channels + channelsIdx];
		}
		err = speex_resampler_process_float(m_resamplerStates[channelsIdx], 0, pInBuffer, &inBufferLen, 
			pOutBuffer, &outBufferLen);

		if (minOutBufferLen == 0)
		{
			minOutBufferLen = outBufferLen;
		}
		else
		{
			if (minOutBufferLen > outBufferLen)
			{
				minOutBufferLen = outBufferLen;
			}
		}

		if (err == RESAMPLER_ERR_SUCCESS)
		{
			if (!initDstChunk)
			{
				initDstChunk = true;
				pDstChunk->SetChannels(m_channels);
				pDstChunk->SetSampleRate(m_dstRate);
				pDstChunk->SetSampleCount(outBufferLen);
				pDstChunk->SetDataSize(pDstChunk->GetSampleCount() * sizeof(float) * m_channels);
			}
			pDst = pDstChunk->GetData();
			for (UINT j = 0; j < outBufferLen; j++)
			{
				pDst[j * m_channels + channelsIdx] = pOutBuffer[j];
			}
		}
	}

	if (minOutBufferLen != pDstChunk->GetSampleCount())
	{
		pDstChunk->SetDataSize(minOutBufferLen * sizeof(float) * m_channels);
	}

	free(pInBuffer);
	free(pOutBuffer);

	return inBufferLen;  
}

void CResampler::Clear()
{
	if(NULL != m_resamplerStates)
	{
		for (int i = 0; i < m_channels; i++)
		{
			speex_resampler_destroy(m_resamplerStates[i]);
			m_resamplerStates[i] = NULL;
		}
		delete[] m_resamplerStates;
	}
	m_resamplerStates = NULL;
	m_resampleRatio = 0;
}

void CResampler::Flush()
{
	Init(m_channels, m_srcRate, m_dstRate);
}