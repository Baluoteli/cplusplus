#include "CycBuffer.h"

CCycBuffer::CCycBuffer(const UINT iBufferSize)
{
	m_iBufferSize = iBufferSize;
	m_iReadCursor = 0;
	m_iWriteCursor = 0;
	m_bBufferIsFull = FALSE;
	m_bLock = FALSE;

	m_iOldReadCursor = 0;
	m_iOldWriteCursor = 0;
	m_bOldBufferIsFull = FALSE;

	m_pBuffer = (BYTE*)malloc(m_iBufferSize);
}

CCycBuffer::~CCycBuffer()
{
	free(m_pBuffer);
}

void CCycBuffer::Clone(CCycBuffer* pCircleBuffer)
{
	INSYNC(pCircleBuffer->GetLocker());
	m_bBufferIsFull = pCircleBuffer->m_bBufferIsFull;
	m_iWriteCursor = pCircleBuffer->m_iWriteCursor;
	m_iReadCursor = pCircleBuffer->m_iReadCursor;
	m_iOldWriteCursor = pCircleBuffer->m_iOldWriteCursor;
	m_iOldReadCursor = pCircleBuffer->m_iOldReadCursor;
	memcpy(m_pBuffer, pCircleBuffer->m_pBuffer, m_iBufferSize);
}

UINT CCycBuffer::GetFreeSize()
{
	INSYNC(m_cs);
	unsigned int iNumBytesFree;

	if (m_bLock)
	{
		iNumBytesFree = 0;
	}
	else
	{
		if(m_iWriteCursor < m_iReadCursor)
		{
			iNumBytesFree = m_iReadCursor - m_iWriteCursor;
		}
		else if(m_iWriteCursor == m_iReadCursor)
		{
			if (m_bBufferIsFull)
				iNumBytesFree = 0;
			else
				iNumBytesFree = m_iBufferSize;
		}
		else
		{
			iNumBytesFree = m_iReadCursor + (m_iBufferSize - m_iWriteCursor);
		}
	}

	return iNumBytesFree;
}

UINT CCycBuffer::GetUsedSize()
{
	INSYNC(m_cs);
	return GetBufferSize() - GetFreeSize();
}

UINT CCycBuffer::GetBufferSize()
{
	INSYNC(m_cs);
	return m_iBufferSize;
}

void CCycBuffer::Flush()
{
	INSYNC(m_cs);
	m_iReadCursor = 0;
	m_iWriteCursor = 0;
	m_bBufferIsFull = FALSE;
}

bool CCycBuffer::Resize(const UINT iBufferSize)
{
	INSYNC(m_cs);
	bool bResult = false;
	UINT nBytesRead = 0;
	UINT nCanReadSize = GetUsedSize();
	if (nCanReadSize < iBufferSize && iBufferSize > 0)
	{
		BYTE* pBuf = (BYTE*)malloc(iBufferSize);
		Read(pBuf, nCanReadSize, &nBytesRead);
		m_iReadCursor = 0;		
		m_iWriteCursor = nBytesRead;		
		m_iOldReadCursor = 0;
		m_iOldWriteCursor = nBytesRead;
		free(m_pBuffer);
		m_pBuffer = pBuf;
		m_iBufferSize = iBufferSize;
		m_bBufferIsFull = false;
		m_bOldBufferIsFull = false;
	}
	return bResult;
}

BOOL CCycBuffer::IsFull()
{
	INSYNC(m_cs);
	return m_bBufferIsFull;
}

UINT CCycBuffer::Write(const void* pSourceBuffer, const UINT iNumBytes)
{
	INSYNC(m_cs);
	UINT iBytesToWrite = iNumBytes;
	BYTE* pReadCursor = (BYTE*)pSourceBuffer;

	if ((iBytesToWrite > GetFreeSize()) || (iBytesToWrite == 0))
		return 0;

	if (!m_bLock)
	{
		if(m_iWriteCursor >= m_iReadCursor)
		{
			UINT iChunkSize = m_iBufferSize - m_iWriteCursor;
			if(iChunkSize > iBytesToWrite)
			{
				iChunkSize = iBytesToWrite;
			}

			// Ð´
			memcpy(m_pBuffer + m_iWriteCursor, pReadCursor, iChunkSize);
			pReadCursor += iChunkSize;
			iBytesToWrite -= iChunkSize;

			// ¸üÐÂÐ´Î»ÖÃ
			m_iWriteCursor += iChunkSize;
			if(m_iWriteCursor >= m_iBufferSize)
				m_iWriteCursor -= m_iBufferSize;
		}

		if(iBytesToWrite)
		{
			memcpy(m_pBuffer + m_iWriteCursor, pReadCursor, iBytesToWrite);
			m_iWriteCursor += iBytesToWrite;
			iBytesToWrite = 0;
			if(m_iWriteCursor >= m_iBufferSize)
				m_iWriteCursor -= m_iBufferSize;
		}

		m_iOldReadCursor = m_iReadCursor;
		m_iOldWriteCursor = m_iWriteCursor;
		m_bOldBufferIsFull = m_bBufferIsFull;
	}

	if (m_iWriteCursor == m_iReadCursor)
		m_bBufferIsFull = TRUE;

	return iNumBytes - iBytesToWrite;
}

BOOL CCycBuffer::Read(void* pDestBuffer, const UINT _iBytesToRead, UINT* pbBytesRead)
{
	INSYNC(m_cs);
	UINT iBytesToRead = _iBytesToRead;
	UINT iBytesRead = 0;

	while(iBytesToRead > 0)
	{
		if ((m_iReadCursor >= m_iWriteCursor) || m_bBufferIsFull) 
		{
			if ((m_iReadCursor == m_iWriteCursor) && !m_bBufferIsFull)
			{
				break;
			}

			UINT iChunkSize = m_iBufferSize - m_iReadCursor;
			if (iChunkSize > iBytesToRead)
			{
				iChunkSize = iBytesToRead;
			}

			//¶Á
			if (pDestBuffer != NULL)
			{
				memcpy((BYTE*)pDestBuffer + iBytesRead, m_pBuffer + m_iReadCursor, iChunkSize);	
			}

			iBytesRead += iChunkSize;
			iBytesToRead -= iChunkSize;

			m_iReadCursor += iChunkSize;
			if(m_iReadCursor >= m_iBufferSize)
				m_iReadCursor -= m_iBufferSize;
		}

		if ((iBytesToRead > 0) && (m_iReadCursor < m_iWriteCursor))
		{
			UINT iChunkSize = m_iWriteCursor - m_iReadCursor;
			if(iChunkSize > iBytesToRead)
			{
				iChunkSize = iBytesToRead;
			}

			//¶Á
			if (pDestBuffer != NULL)
			{
				memcpy((BYTE*)pDestBuffer + iBytesRead, m_pBuffer + m_iReadCursor, iChunkSize);	
			}

			iBytesRead += iChunkSize;
			iBytesToRead -= iChunkSize;
			m_iReadCursor += iChunkSize;
		}
	}

	*pbBytesRead = iBytesRead;
	if (m_iWriteCursor == m_iReadCursor)
		m_bBufferIsFull = FALSE;
	return TRUE;
}

UINT CCycBuffer::GetPosition()
{
	INSYNC(m_cs);
	return ((m_iReadCursor - m_iOldReadCursor) + m_iBufferSize) % m_iBufferSize;
}

void CCycBuffer::SetPosition(UINT nPos)
{
	INSYNC(m_cs);
	UINT nUsedSize = GetUsedSize();
	if (nPos < nUsedSize)
	{
		m_iReadCursor = (m_iOldReadCursor + nPos) % m_iBufferSize;
	}
}

void CCycBuffer::Lock()
{
	INSYNC(m_cs);
	m_bLock = TRUE;
}

void CCycBuffer::UnLock()
{
	INSYNC(m_cs);
	m_bLock = FALSE;
}

bool CCycBuffer::Read(void* pData, const UINT dataLen)
{
	if (GetUsedSize() >= dataLen)
	{		
		UINT bytesRead = 0;
		Read(pData, dataLen, &bytesRead);
		return true;
	}
	return false;
}