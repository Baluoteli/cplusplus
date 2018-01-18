#ifndef _CYC_BUFFER_H_ 
#define _CYC_BUFFER_H_

#include <windows.h>
#include "SyncObjs.h"

class CCycBuffer
{
public:
	CCycBuffer(const UINT iBufferSize = 0);
	~CCycBuffer();

	UINT GetBufferSize();
	UINT GetUsedSize();
	UINT GetFreeSize();
	BOOL IsFull();
	void Flush();

	UINT GetPosition();
	void SetPosition(UINT nPos);

	inline CLock* GetLocker()
	{
		return &m_cs;
	}

	bool Resize(const UINT iBufferSize);

	void Lock();
	void UnLock();

	UINT Write(const void* pSourceBuffer, const UINT iNumBytes);
	BOOL Read(void* pDestBuffer, const UINT _iBytesToRead, UINT* pbBytesRead);

	void Clone(CCycBuffer* pCircleBuffer);

	bool Read(void* pData, const UINT dataLen);

private:	
	BOOL m_bLock;
	BYTE* m_pBuffer;
	UINT m_iBufferSize;
	UINT m_iReadCursor;
	UINT m_iWriteCursor;
	BOOL m_bBufferIsFull;

	UINT m_iOldReadCursor;
	UINT m_iOldWriteCursor;
	BOOL m_bOldBufferIsFull;

	CLock m_cs;
};

#endif