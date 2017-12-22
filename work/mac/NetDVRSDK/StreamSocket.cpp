#include "StreamSocket.h"
#include "macro.h"
#include "Global.h"

#define MAX_RECV_VIDEO_DATALEN	(1024 * 1024 * 2)
#define MAX_RECV_FRAMESIZE		(128 * 1024)

UNSL32 StreamReceiver(LPVOID lpvoid);

StreamSocket::StreamSocket()
{
	m_dataSocket	= INVALID_SOCKET;
	m_bExit			= false;
	m_fifo			= NULL;
}

StreamSocket::~StreamSocket()
{
	UnInit();
}

bool StreamSocket::Start()
{
	m_bExit = false;
	if (!m_hDataRcv.CreateAThread(StreamReceiver, this))
	{
		return false;
	}

	return true;
}

//SendData: Success return 0; Failed return 1;
S32 StreamSocket::SendData(S8* pData, S32 len)
{
	timeval tv;
	fd_set fd;	 
	S32 left = len;
	S32 ret = 0;

	if (m_dataSocket == INVALID_SOCKET || len < 0)
	{
		return 1;
	}

	do
	{
		FD_ZERO(&fd);
		FD_SET(m_dataSocket, &fd);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		if (select(m_dataSocket+1, 0, &fd, 0, &tv) <= 0)
		{
			break;
		}

		ret = send(m_dataSocket, pData + len - left, left, 0);
		if (ret < 0)
		{
			break;
		}
		
		left -= ret;
		Sleep(1);
	} while (left > 0);

	return (left == 0) ? 0 : 1;
}

//BuildSocket: Success return 0; Failed return 1;
S32 StreamSocket::BuildSocket(UNSL32 ip, UNS16 port)
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

		//for(S32 i=0; i<12; i++)
		{
			iMode = SOCKET_RECV_BUFFER;//i*16*1024;
			setsockopt(m_dataSocket, SOL_SOCKET, SO_RCVBUF, (const S8*)&iMode, sizeof(iMode));
		}

		sockaddr_in addr; 
		addr.sin_family			= AF_INET;
		addr.sin_addr.s_addr	= ip;
		addr.sin_port			= htons(port);

		timeval tv;
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(m_dataSocket, &fd);

		tv.tv_sec = 3;
		tv.tv_usec = 0;
		connect(m_dataSocket, (struct sockaddr *)&addr, sizeof(sockaddr_in));
		if (select(m_dataSocket+1, 0, &fd, 0, &tv) == 1)
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

//Init: Success return 0; Failed return 1;
S32 StreamSocket::Init(UNSL32 ip, UNS16 port, StreamFifo *pFifo)
{
	S32 ret = BuildSocket(ip, port);
	m_fifo = pFifo;

	if (ret != 0)
	{
		UnInit();
	}

	return ret;
}

void StreamSocket::UnInit()
{
	m_bExit = true;
	m_hDataRcv.ExitAThread();
	if (m_dataSocket != INVALID_SOCKET)
	{
		closesocket(m_dataSocket);
		m_dataSocket = INVALID_SOCKET;
	}
}

UNSL32 StreamReceiver(LPVOID lpvoid)
{
	StreamSocket *pDataSocket = (StreamSocket*)lpvoid;
	timeval tv;
	fd_set fd;
	
	S32 ret = 0;
	UNS32 *pMarket = NULL;
	UNS32 frameLeft = 0;
	UNS32 dataLeft  = 0;
	UNS32 checkNum  = 0;
	UNS32 i = 0;
	UNS32 curpos = 0;
	
	PES_PACKET_HEADER dataInfo;
	bool bFindMarket = false;
	bool bFindDataInfo = false;
	
	S8 *buf = new S8[MAX_RECV_FRAMESIZE];
	S8 *pFrame = new S8[MAX_RECV_FRAMESIZE];
	S8 *pData = pFrame + sizeof(PES_PACKET_HEADER) + sizeof(S32);	
	if (buf == NULL || pFrame == NULL)
	{
		return 0;
	}

	do
	{
        FD_ZERO(&fd);
		FD_SET(pDataSocket->m_dataSocket, &fd);			
		tv.tv_sec = 0;
		tv.tv_usec = 100000 ;
        int len = select((int)(pDataSocket->m_dataSocket+1), &fd, NULL, NULL, &tv);
		if (len <= 0)
		{
            perror("select error:");
			continue;
		}

		ret = recv(pDataSocket->m_dataSocket, buf + dataLeft, MAX_RECV_FRAMESIZE - dataLeft, 0);
		if (ret <= 0)
		{
			Sleep(10);
			continue;
		}

		dataLeft += ret;
		curpos = 0;
		do
		{			
			if (bFindMarket == false)//获取前导符
			{	
				checkNum = (dataLeft - curpos) >> 2;///4;//to optimize alive view
				checkNum = checkNum << 2;//*= 4;//to optimize alive view					
				for (i = 0; i<checkNum; i++)
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
				memcpy(pData, buf + curpos, dataLeft - curpos);
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
					if (dataInfo.len > MAX_RECV_VIDEO_DATALEN)
					{
						bFindDataInfo = false;
						bFindMarket = false;
						curpos = 0;
						dataLeft = 0;
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
					//此处把文件写入FIFO
					if (pDataSocket->m_fifo != NULL)
					{
						dataInfo.len -= sizeof(MAGIC_END);
						S32 framelen = dataInfo.len + sizeof(dataInfo);
						memcpy(pFrame, &framelen, sizeof(framelen));
						memcpy(pFrame + sizeof(S32), &dataInfo, sizeof(dataInfo));
						framelen += sizeof(S32);

						while(!pDataSocket->m_bExit)
						{
                            printf("PushFrame.....\n");
							if (pDataSocket->m_fifo->PushFrame(pFrame, framelen) > 0)
							{
								break;
							}
							Sleep(10);
						}
					}
				}else
				{
					printf("pes packet lost data\n");
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
		delete[] buf;
		buf = NULL;
	}

	if (pFrame != NULL)
	{
		delete[] pFrame;
		pFrame = NULL;
	}

	return 0;
}

S32 StreamSocket::ReadData(S8* pBuffer, S32 nSize)
{
	S32 nCount = 0;
	//S32 nTrytimes = 3;
	S32 nTryTime  = 0;
	while (nCount < nSize)
	{
		timeval timeOut = {0, };
		timeOut.tv_sec = 20;//超时秒   
		timeOut.tv_usec = 0;//微秒精度 

		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(m_dataSocket, &fdset);
		S32 nRet = ::select(m_dataSocket+1, &fdset, 0, 0, &timeOut);
		if (0 == nRet)//两次，共40s超时
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
