#include "ResamplerEx.h"

IResamplerEx* CreateResamplerEx()
{
	return new CResamplerEx();
}

CResamplerEx::CResamplerEx(): m_pResampler(NULL), m_outSampleCount(0), m_dstSampleRate(0)
{

}

CResamplerEx::~CResamplerEx()
{
	Clear();
}

void CResamplerEx::Destroy()
{
	delete this;
}

void CResamplerEx::Clear()
{
	if (m_pResampler != NULL)
	{
		m_pResampler->Destroy();
		m_pResampler = NULL;
	}
	m_outSampleCount = 0;
	m_outSampleLeftData.clear();
}

bool CResamplerEx::Init(int channels, int srcRate, int dstRate, int outSampleCount, bool useSSE2)
{
	Clear();
	m_pResampler = CreateResampler();
	bool initResult = m_pResampler->Init(channels, srcRate, dstRate, useSSE2);
	if (initResult)
	{
		m_outSampleCount = outSampleCount;
		m_dstSampleRate = dstRate;
	}
	return initResult;
}

bool CResamplerEx::Process(IAudioChunk* pSrcChunk, IAudioChunk* pDstChunk)
{
	pDstChunk->Reset();
	int channelCount = pSrcChunk->GetChannels();
	if (m_pResampler != NULL && channelCount > 0)
	{
		int outSampleLeftDataCount = (int)m_outSampleLeftData.size() / sizeof(float) / channelCount;
		if (outSampleLeftDataCount > m_outSampleCount)
		{
			pDstChunk->SetDataSize(m_outSampleCount * sizeof(float) * channelCount);
			pDstChunk->SetChannels(pSrcChunk->GetChannels());
			pDstChunk->SetSampleRate(m_dstSampleRate);
			pDstChunk->SetSampleCount(m_outSampleCount);
			memcpy(pDstChunk->GetData(), m_outSampleLeftData.c_str(), pDstChunk->GetDataSize());	
			m_outSampleLeftData.erase(0, pDstChunk->GetDataSize());
			return true;
		}

		CAudioChunk tempChunk;
		if (m_pResampler->Process(pSrcChunk, &tempChunk) > 0)
		{
			if ((int)tempChunk.GetSampleCount() + outSampleLeftDataCount >= m_outSampleCount)
			{
				int needMoreReadSize = 0;
				int needMoreWritePos = 0;
				pDstChunk->SetDataSize(m_outSampleCount * sizeof(float) * channelCount);
				pDstChunk->SetChannels(tempChunk.GetChannels());
				pDstChunk->SetSampleRate(tempChunk.GetSampleRate());
				pDstChunk->SetSampleCount(m_outSampleCount);
				if (outSampleLeftDataCount <= m_outSampleCount)
				{
					memcpy(pDstChunk->GetData(), m_outSampleLeftData.c_str(), m_outSampleLeftData.size());	
					needMoreWritePos = (int)m_outSampleLeftData.size();
					needMoreReadSize = pDstChunk->GetDataSize() - needMoreWritePos;
					m_outSampleLeftData.clear();
				}
				else
				{
					return false;
				}
				if (needMoreReadSize > 0)
				{
					memcpy((char*)pDstChunk->GetData() + needMoreWritePos, tempChunk.GetData(), needMoreReadSize);
					m_outSampleLeftData.append((char*)tempChunk.GetData() + needMoreReadSize, 
						tempChunk.GetDataSize() - needMoreReadSize);
				}
				else
				{
					m_outSampleLeftData.append((char*)tempChunk.GetData(), tempChunk.GetDataSize());
				}
			}
			else
			{
				m_outSampleLeftData.append((char*)tempChunk.GetData(), tempChunk.GetDataSize());
			}
		}
		if (m_outSampleCount != 0 && pDstChunk->GetSampleCount() == m_outSampleCount)
		{
			return true;
		}
	}
	return false;
}
