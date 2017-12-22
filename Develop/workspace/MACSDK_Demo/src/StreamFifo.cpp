#include "StreamFifo.h"
#include "macro.h"

StreamFifo::StreamFifo(S32 size)
{
	m_maxSize = size;
	m_buf = new S8[size];
	m_totalData    = 0;;
	m_readPointer  = 0;
	m_writePointer = 0;
}

StreamFifo::~StreamFifo()
{
	if (m_buf != NULL)
	{
		delete[] m_buf;
		m_buf = NULL;
	}
}

UNS32 StreamFifo::PushFrame(S8 *pData, UNS32 length)
{
	m_section.Lock();
	if (length == 0)
	{
		printf("my gold\n");
	}
	
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

UNS32 StreamFifo::PopFrame(S8 *buf, UNS32 max)
{
	m_section.Lock();
	UNS32 length = sizeof(S32);
	UNS32 oldreader = m_readPointer;

	do{   
		//如果当前视频数据长度小于4，说明当前缓存没有视频数据可取
		if (m_totalData < sizeof(S32))
		{
			length = 0;
			break;
		}

		//如果当前可读位置+4个字节（存储包长的空间 ）<= 缓存空间
		if (m_readPointer + length <= m_maxSize)
		{
			//取出包的前四个字节，获取包的长度
			memcpy(buf, m_buf + m_readPointer, length);
			//移动可读位置
			m_readPointer += length;

		}else
		{//如果当前可读位置+4个字节（存储包长的空间 ）> 缓存空间
			//取出末尾的字节
			memcpy(buf, m_buf + m_readPointer, m_maxSize - m_readPointer);
			//从开头取出长度四个字节还没有取完的字节
			memcpy(buf + m_maxSize - m_readPointer, m_buf, length + m_readPointer - m_maxSize);
			m_readPointer = length + m_readPointer - m_maxSize;
		}
		//获取还剩余可取的有效视频字节数
		m_totalData -= length;
		//获取包长
		memcpy(&length, buf, 4);
		//如果当前要取的包长大于可取的有效视频字节数，将所有的数字清零
		//从头写进数据包，从头取
		if (length > m_totalData || length > m_maxSize)
		{
			m_readPointer    = 0;
			m_writePointer   = 0;
			m_totalData      = 0;
			length = 0;
			break;
		}
		
		if (m_maxSize < length)
		{
			m_readPointer = oldreader;
			m_totalData  += sizeof(S32);
			length = 0;
			break;
		}

		//开始读取数据，读取方法可参考写入缓存数据方法的注释
		if (m_readPointer + length <= m_maxSize)
		{
			memcpy(buf, m_buf + m_readPointer, length);
			m_readPointer += length;
		}else
		{
			memcpy(buf, m_buf + m_readPointer, m_maxSize - m_readPointer);
			memcpy(buf + m_maxSize - m_readPointer, m_buf, length + m_readPointer - m_maxSize);
			m_readPointer = length + m_readPointer - m_maxSize;
		}

		//对可读有效视频进行重新赋值
		m_totalData  -= length;
		m_section.Unlock();

		return length;
	} while (0);

	m_section.Unlock();
	return 0;
}

void StreamFifo::Reset()
{
	m_section.Lock();
	m_totalData    = 0;
	m_writePointer = 0;
	m_readPointer  = 0;
	m_section.Unlock();
}
