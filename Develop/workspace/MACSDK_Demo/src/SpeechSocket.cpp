#include "SpeechSocket.h"
#include "DvrSpeech.h"

#define MAX_RECV_AUDIO_DATALEN		(50 * 1024)

UNSL32 __attribute__((stdcall)) AudioReceiver(LPVOID lpvoid);

SpeechSocket::SpeechSocket()
{
	m_dataSocket	= INVALID_SOCKET;
	m_bExit			= false;
}

SpeechSocket::~SpeechSocket()
{
	UnInit();
}

S32 SpeechSocket::SendData(S8* pData, S32 len)
{
	timeval tv;
	fd_set fd;	
	S32 left = len;
	if (m_dataSocket == INVALID_SOCKET || len < 0)
	{
		return 1;
	}
	
	S32 ret = 0;	
	do
	{
		FD_ZERO(&fd);
		FD_SET(m_dataSocket, &fd);		
		tv.tv_sec = 0;
		tv.tv_usec = 1000 * 1000 * 3;//3s
		if (select(1, 0, &fd, 0, &tv) <= 0)
		{
			break;
		}
		
		ret = send(m_dataSocket, pData+len-left, left, 0);		
		if (ret < 0)
		{
			break;
		}
		left -= ret;
		
		Sleep(1);
	} while (left > 0);
	
	return (left == 0) ? 0 : 1;
}

S32 SpeechSocket::BuildSocket(const S8* ip, UNS16 port)
{
	do
	{
		m_dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_dataSocket == INVALID_SOCKET)
		{
			break;
		}
		
		UNSL32 iMode = 1;
		if (ioctlsocket(m_dataSocket, FIONBIO, &iMode) != 0)
		{
			break;
		}
		
		iMode = AUDIOSTREAM_RECV_BUFFER;
		setsockopt(m_dataSocket, SOL_SOCKET, SO_RCVBUF, (const S8*)&iMode, sizeof(iMode));
		
		sockaddr_in addr; 
		addr.sin_family			= AF_INET;
		addr.sin_addr.s_addr	= inet_addr(ip);
		addr.sin_port			= htons(port);

		timeval tv;
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(m_dataSocket, &fd);
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		connect(m_dataSocket, (struct sockaddr *) &addr, sizeof(sockaddr_in));
		if (select(1, 0, &fd, 0, &tv) == 1)
		{
			return 0;
		}
	} while (0);
	
	if (m_dataSocket != INVALID_SOCKET)
	{
		closesocket(m_dataSocket);
		m_dataSocket = INVALID_SOCKET;
	}
	
	return 1;
}

S32 SpeechSocket::Init(const S8 *ip, UNS16 port, SpeechFifo *pFifo)
{
	S32 ret = BuildSocket(ip, port);	
	m_fifo = pFifo;

	if (ret != 0)
	{
		UnInit();
	}else
	{
		m_bExit = false;
		if (!m_hDataRcv.CreateAThread(AudioReceiver, this))
			return -1;
	}

	return ret;
}

void SpeechSocket::UnInit()
{
	m_bExit = true;
	m_hDataRcv.ExitAThread();
	if (m_dataSocket != INVALID_SOCKET)
	{
		closesocket(m_dataSocket);
		m_dataSocket = INVALID_SOCKET;
	}
}

UNSL32 __attribute__((stdcall)) AudioReceiver(LPVOID lpvoid)
{
	SpeechSocket *pDataSocket = (SpeechSocket*)lpvoid;
	timeval tv;
	fd_set fd;

	S32 ret = 0;
	UNS32 *pMarket = NULL;
	UNS32 frameLeft = 0;
	UNS32 dataLeft  = 0;
	UNS32 checkNum  = 0;
	UNS32 i = 0;
	UNS32 curpos = 0;	
	//S16 waveLen = 0;
	
	PES_PACKET_HEADER dataInfo;
	bool bFindMarket = false;
	bool bFindDataInfo = false;
	
	S8 *buf = (S8*)malloc(MAX_RECV_AUDIO_DATALEN);
	S8 *pFrame = (S8*)malloc(MAX_RECV_AUDIO_DATALEN + sizeof(PES_PACKET_HEADER));
	S8 *pData = pFrame + sizeof(PES_PACKET_HEADER);	
	
	if (buf == NULL || pFrame == NULL)
	{
		return 0;
	}

	do{
		FD_ZERO(&fd);
		FD_SET(pDataSocket->m_dataSocket, &fd);
		tv.tv_sec = 0;
		tv.tv_usec = 1000 * 1000;
		if (select(1, &fd, 0, 0, &tv) <= 0)
		{
			continue;
		}

		ret = recv(pDataSocket->m_dataSocket, buf + dataLeft, MAX_RECV_AUDIO_DATALEN - dataLeft, 0);
		if (ret <= 0)
		{
			Sleep(10);
			continue;
		}

		dataLeft += ret;
		curpos = 0;
		do{
			//获取前导符
			if (bFindMarket == false)
			{
				checkNum = (dataLeft - curpos) >> 2;///4;//to optimize alive view
				checkNum = checkNum << 2;//*= 4;//to optimize alive view
				for (i = 0; i< checkNum; i++)
				{
					pMarket = (UNS32*)(buf + curpos);
					if (*pMarket == MAGIC_START)
					{
						bFindMarket = true;
						curpos += 4;
						break;
					}	
					curpos += 1;
				}
			}

			if (bFindMarket == false)
			{
				memcpy(pData, buf+curpos, dataLeft - curpos);
				memcpy(buf, pData, dataLeft - curpos);
				dataLeft = dataLeft - curpos;
				break;
			}

			//获取数据结构信息
			if (bFindDataInfo == false)
			{
				if (dataLeft >= (curpos + sizeof(PES_PACKET_HEADER)))
				{
					memcpy(&dataInfo, buf + curpos, sizeof(dataInfo));
					bFindDataInfo = true;
					dataInfo.len += sizeof(MAGIC_END);//此处多接收4个字节，用于判断音视频帧的结束标识
					frameLeft = dataInfo.len;
					curpos += sizeof(dataInfo);
					//Here maybe a fatal error
					if (dataInfo.len > MAX_RECV_AUDIO_DATALEN)
					{
						bFindDataInfo	= false;
						bFindMarket		= false;
						curpos			= 0;
						dataLeft		= 0;
						break;
					}
				}else
				{
					memcpy(pData, buf + curpos, dataLeft - curpos);
					memcpy(buf, pData, dataLeft - curpos);
					dataLeft = dataLeft - curpos;
					break;
				}
			}

			if (dataLeft - curpos >= frameLeft)
			{
				memcpy(pData + dataInfo.len - frameLeft, buf + curpos, frameLeft);//Get Full frame
				pMarket = (UNS32*)(pData + dataInfo.len - sizeof(MAGIC_END));
				if (*pMarket == MAGIC_END) //判断是否是一帧的结尾
				{
					if (pDataSocket->m_fifo != NULL)
					{
						dataInfo.len -= sizeof(MAGIC_END);
						S32 framelen = dataInfo.len + sizeof(dataInfo);
						memcpy(pFrame, &framelen, sizeof(framelen));
						memcpy(pFrame + sizeof(S32), &dataInfo, sizeof(dataInfo));
						framelen += sizeof(S32);

						while(!pDataSocket->m_bExit)
						{
							if (pDataSocket->m_fifo->AddData(pFrame, framelen) > 0)
							{
								break;
							}
							Sleep(10);
						}
					}
				}

				bFindDataInfo = false;
				bFindMarket   = false;
				curpos += frameLeft;
			}else
			{
				memcpy(pData + dataInfo.len - frameLeft, buf + curpos, dataLeft - curpos);
				frameLeft -= (dataLeft - curpos);
				dataLeft = 0;
				break;
			}
		} while (!pDataSocket->m_bExit);
	} while (!pDataSocket->m_bExit);

	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}

	if (pFrame != NULL)
	{
		free(pFrame);
		pFrame = NULL;
	}

	return 0;
}

S32 SpeechSocket::ReadData(S8* pBuffer, S32 nSize)
{
	S32 nCount = 0;
	//S32 nTrytimes = 3;
	S32 nTryTime  = 0;
	while (nCount < nSize)
	{
		timeval timeOut = {0};
		timeOut.tv_sec = 2;		//超时秒   
		timeOut.tv_usec = 0;	//微秒精度
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(m_dataSocket, &fdset);
		S32 nRet = ::select(0, &fdset, 0, 0, &timeOut);
		if (0 == nRet)		//超时
		{
			if (nTryTime >= 2)
			{
				break;
			}

			nTryTime++;
			continue;
		}

		nRet = ::recv(m_dataSocket, (S8*)pBuffer + nCount, nSize - nCount, 0);
		if (SOCKET_ERROR == nRet)
		{
			return -1;
		}
		else if (0 == nRet)	//对方SOCKET已关闭
		{
			return -2;
		}
		else if (nRet > 0)
		{
			nCount += nRet;
		}
	}

	return nCount;
}
