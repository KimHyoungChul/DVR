#include "SpeechFifo.h"

SpeechFifo::SpeechFifo(S32 size)
{
	m_maxSize = size;
	m_buf = (S8*)malloc(size);
	if (m_buf == NULL)
	{
		exit(1);
	}

	m_totalData    = 0;
	m_readPointer  = 0;
	m_writePointer = 0;
}

SpeechFifo::~SpeechFifo()
{
	if (m_buf != NULL)
	{
		free(m_buf);
		m_buf = NULL;
	}
}

S32 SpeechFifo::AddData(S8 *pData, UNS32 length)
{	
	m_section.Lock();	
	if (m_totalData + length > m_maxSize)
	{
		m_section.Unlock();
		return 0;
	}

	if (m_writePointer + length <= m_maxSize)
	{
		memcpy(m_buf + m_writePointer, pData, length);
		m_writePointer += length;

	}else
	{
		memcpy(m_buf + m_writePointer, pData, m_maxSize - m_writePointer);
		memcpy(m_buf, (pData + m_maxSize - m_writePointer), length -(m_maxSize - m_writePointer));
		m_writePointer = length - (m_maxSize - m_writePointer);
	}

	m_totalData += length;
	m_section.Unlock();

	return length;

}

S32 SpeechFifo::GetData(S8 *buf, UNS32 max)
{
	do{
		m_section.Lock();
		if (m_totalData < 1)
		{
			break;
		}
		
		UNS32 length = max;
		if (length > m_totalData)
		{
			length = m_totalData;
			break;
		}
		
		if (length > m_totalData || length > m_maxSize)
		{
			//Error accured and i reset all pointers.
			m_readPointer	= 0;
			m_writePointer	= 0;
			m_totalData		= 0;
			break;
		}

		if (m_readPointer + length <= m_maxSize)
		{
			memcpy(buf, m_buf + m_readPointer, length);
			m_readPointer += length;
		}else
		{
			memcpy(buf, m_buf + m_readPointer, m_maxSize - m_readPointer);
			memcpy(buf + m_maxSize - m_readPointer,	m_buf, length + m_readPointer - m_maxSize);
			m_readPointer = length + m_readPointer - m_maxSize;
		}

		m_totalData -= length;
		m_section.Unlock();

		return length;
	} while (0);
	m_section.Unlock();

	return 0;
}

void SpeechFifo::Reset()
{
	m_section.Lock();
	m_totalData		= 0;
	m_writePointer	= 0;
	m_readPointer	= 0;
	m_section.Unlock();
}