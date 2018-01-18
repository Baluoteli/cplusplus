#ifndef _AUDIOCHUNCK_H_
#define _AUDIOCHUNCK_H_

#include <windows.h>

#define AudioSample float 
#define AudioSampleAsm dword
#define AudioSampleBytes 4

void SwapOrder(void* p, unsigned nBytes);

inline bool MachineIsBigEndian();

inline bool MachineIsLittleEndian();

template<class T, bool b_swap, bool b_signed, bool b_pad> class sucks_v2 
{
public:
	inline static void DoFixedpointConvert(const void* source, unsigned bps, unsigned count, AudioSample* buffer)
	{
		const char * src = (const char *) source;
		unsigned bytes = bps>>3;
		unsigned n;
		T max = ((T)1)<<(bps-1);

		T negmask = - max;

		double div = 1.0 / (double)(1<<(bps-1));
		for(n = 0; n < count; n++)
		{
			T temp;
			if (b_pad)
			{
				temp = 0;
				memcpy(&temp, src, bytes);
			}
			else
			{
				temp = *reinterpret_cast<const T*>(src);
			}

			if (b_swap) 
				SwapOrder(&temp,bytes);

			if (!b_signed) temp ^= max;

			if (b_pad)
			{
				if (temp & max) temp |= negmask;
			}

			if (b_pad)
				src += bytes;
			else
				src += sizeof(T);

			buffer[n] = (AudioSample)((double)temp * div);
		}
	}
};

template <class T,bool b_pad> class sucks 
{ 
public:
	inline static void DoFixedpointConvert(bool b_swap, bool b_signed, const void* source, unsigned bps,
										   unsigned count, AudioSample* buffer)
	{
		if (sizeof(T) == 1)
		{
			if (b_signed)
			{
				sucks_v2<T, false, true, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
			}
			else
			{
				sucks_v2<T, false, false, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
			}
		}
		else if (b_swap)
		{
			if (b_signed)
			{
				sucks_v2<T, true, true, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
			}
			else
			{
				sucks_v2<T, true, false, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
			}
		}
		else
		{
			if (b_signed)
			{
				sucks_v2<T, false, true, b_pad>::DoFixedpointConvert(source,bps,count,buffer);
			}
			else
			{
				sucks_v2<T, false, false, b_pad>::DoFixedpointConvert(source,bps,count,buffer);
			}
		}
	}
};

class IAudioChunk
{
public:
	virtual bool SetData(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, 
		UINT nBits, bool bFloat) = 0;

	virtual bool AppendData(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, 
		UINT nBits, bool bFloat) = 0;

	virtual inline AudioSample* GetData() = 0;

	virtual UINT GetDataSize() = 0;
	virtual void SetDataSize(UINT nNewSize) = 0;

	virtual UINT GetDataLength() = 0;

	virtual void CheckDataSize(UINT nSize) = 0;

	virtual UINT GetSampleRate() = 0;
	virtual void SetSampleRate(UINT nSampleRate) = 0;

	virtual UINT inline GetChannels() = 0;
	virtual void SetChannels(UINT nChannels) = 0;

	virtual UINT inline GetSampleCount() = 0;
	virtual void SetSampleCount(UINT nSampleCount) = 0;

	virtual bool IsEmpty() = 0;

	virtual void Reset() = 0;

	virtual void Flush() = 0;

	virtual void Copy(IAudioChunk* pSrcChunk) = 0;

	enum
	{
		FLAG_LITTLE_ENDIAN = 1,
		FLAG_BIG_ENDIAN = 2,
		FLAG_SIGNED = 4,
		FLAG_UNSIGNED = 8,
	};
};

class CAudioChunk : public IAudioChunk
{
public:
	enum
	{
		FLAG_LITTLE_ENDIAN = 1,
		FLAG_BIG_ENDIAN = 2,
		FLAG_SIGNED = 4,
		FLAG_UNSIGNED = 8,
	};

	CAudioChunk();
	~CAudioChunk();

	bool SetData(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits, bool bFloat);

	bool AppendData(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, 
		UINT nBits, bool bFloat);

	inline AudioSample* GetData();

	UINT GetDataLength();

	UINT GetDataSize();
	void SetDataSize(UINT nNewSize);

	void CheckDataSize(UINT nSize);

	UINT GetSampleRate();
	void SetSampleRate(UINT nSampleRate);

	UINT inline GetChannels()
	{
		return m_nChannels;
	}

	void SetChannels(UINT nChannels);

	UINT inline GetSampleCount()
	{
		return m_nSamples;
	}

	void SetSampleCount(UINT nSampleCount);

	bool IsEmpty();

	void Reset();

	void Flush();

	void Copy(IAudioChunk* pSrcChunk);

	bool SetDataFloatingPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits);
	bool SetDataFixedPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits, UINT nFlags = 0);

	bool AppendDataFloatingPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits);
	bool AppendDataFixedPoint(void* pSrc, UINT nSize, UINT nSampleRate, UINT nChannels, UINT nBits, UINT nFlags = 0);

private:
	UINT m_nSampleRate;
	UINT m_nChannels;
	UINT m_nSamples;

	AudioSample* m_pData;

	UINT m_nDataSize;
};

#endif