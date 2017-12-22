// DeviceStream.cpp: implementation of the CDeviceStream class.
//
//////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------------------
// 2007-04-19 为了改成异步模式kuku对这个类进行较大改动
//
// 增加一个列表 vector<PLAYCHNINFO> m_PlayChnInfoList 用来保存本设备中那个通道播放，
// 以及映射在主界面的那个playwin上
// 把InitDVRSocket()提升成共有成员函数 需要使用现单独调用
// 改写了OpenDevice()函数，
// AddPlayChnInfo()函数
// 增加了Play和Stop函数
// 本类的使用规则为，首先调用InitDVRSocket和AddPlayChnInfo进行通道信息设置和请求登录
// 登录成功后可以调用OpenDevice函数进行初始化
// 然后可以使用Play和Stop函数进行控制
// 又设计了一个CDeviceList对本类进行了二次管理，原则上主界面不需要调用本类的函数
// 这里面可能有一个隐患，就是调用Play和Stop函数可能不一定在登录成功后，
// 需要问问李工也没有对StartRender和StopRender加保护
//
//-----------------------------------------------------------------------------------------------------------------

#include "DeviceStream.h"
#include "macro.h"
#include "PSWriter.h"
#include "../PlayerDVR/kt_util.h"

#define VIDEO_MBSIZE    16
#define MAX_VIDEOWIDTH  704
#define MAX_VIDEOHEIGHT 576

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PSWriter *m_pShareWriter = NULL;//for record audio

UNSL32 LivePlayer(LPVOID lpvoid)
{
	CDeviceStream *pStream = (CDeviceStream*)lpvoid;
	if (pStream != NULL)
	{
		pStream->RenderTask();
	}

	return 0;
}

CDeviceStream::CDeviceStream()
{
	m_pRealWriter	= NULL;
	m_pStreamer		= NULL;	
	m_pFifo = new StreamFifo(MAX_DECODE_FRAMESIZE);
	
	m_bExit			= false;
	m_bFindIFrame	= false;	
	m_bSaveRealData = false;
	m_bSubStream	= false;
	
	m_videoSignalType	= 0;
	m_videoRes			= 0;
	m_chnnl				= 0;
	m_biAudio			= 0;

	memset(m_chnnlName, 0, DVS_MAX_PTZ_NAME_LEN + 1);
	memset(m_cDvrAddr, 0, DVS_MAX_DOMAINLEN + 1);
	memset(&m_cbRealVideoData, 0, sizeof(CbRealVideoStruct));
}

CDeviceStream::~CDeviceStream()
{
	CloseRealStream();	
	if (m_pRealWriter != NULL)
	{
		delete m_pRealWriter;
		m_pRealWriter = NULL;
	}

	if (m_pFifo != NULL)
	{
		delete m_pFifo;
	}
	m_pFifo = NULL;

	if (m_pStreamer != NULL)
	{
		delete m_pStreamer;
		m_pStreamer = NULL;
	}
}

void CDeviceStream::SetUserInfo(UNS32 userId, UNS32 accessId)
{
	m_userId = userId;
	m_accessId = accessId;
}

void CDeviceStream::SetChnnlName(S8 *chnnlName)
{
	snprintf(m_chnnlName, DVS_MAX_PTZ_NAME_LEN, "%s", chnnlName);
}

bool CDeviceStream::GetDomainIP(S8 *pDomain, S8 *ip, bool bFromIP)
{
	if (pDomain[0] == '\0' || ip == NULL)
	{
		return false;
	}


	if (!bFromIP)
	{
		struct hostent *host = NULL;
		host = gethostbyname((const char *)pDomain);
		if (host == NULL || host->h_addr == NULL)
		{
			//sprintf_s("Error code = %d\n", GetLastError());
			//printf("Error code = %s\n", strerror(errno));
			return false;
		}
		sprintf(ip, "%s", (S8*)inet_ntoa(*(struct in_addr*)(host->h_addr)));
	}
	else
		strncpy(ip, pDomain, 16);

	return true;
}

void CDeviceStream::ReConnect()
{
	if (m_pFifo != NULL)
	{
		m_pFifo->Reset();
	}else
	{
		printf("ReConnect m_pFifo is 0!\n");
		return;
	}

	S8 ip[DVS_MAX_DOMAINLEN + 1] = {0};
	if (!GetDomainIP(m_cDvrAddr, ip))
	{
		printf("ReConnect Get domain ip failed!\n");
		return;
	}

	UNSL32 addr = inet_addr(ip);		
	if (!CreateStreamer(addr, m_port))
	{
		printf("ReConnect CreateStreamer failed, address is %x, port is %d\n",addr,m_port);
		return;
	}

	if (m_chnnl > 0)
	{
		if (!ChangeStream(m_chnnl, VIDEO_CHANGE))
		{
			printf("Reconnect ChangeStream failed!\n");
		}
	}
}

bool CDeviceStream::OpenRealStream(S8 *destIP, UNS16 port, UNS8 chnnl, UNS8 biAudio, bool bSubStream)
{
	do
	{
		if (destIP == NULL)
		{
			break;
		}
		
		if (m_pFifo != NULL)
		{
			m_pFifo->Reset();
		}else
		{
			break;
		}

		m_biAudio = biAudio;
		S8 ip[DVS_MAX_DOMAINLEN + 1] = {0};
		if (!GetDomainIP(destIP, ip))
		{
			printf("Get domain ip failed!\n");
			break;
		}
		UNSL32 addr = inet_addr(ip);		
		m_bSubStream = bSubStream;
		if (!CreateStreamer(addr, port))
		{
			break;
		}
		
		m_chnnl = chnnl;
		m_port = port;
		snprintf(m_cDvrAddr, DVS_MAX_DOMAINLEN, "%s", destIP);
		
		m_bExit = false;
		if (!m_hVideoThread.CreateAThread(LivePlayer, this, STACK_SIZE_PARAM_IS_A_RESERVATION))
			break;
		
		return true;
	} while (0);
	
	return false;
}

void CDeviceStream::CloseRealStream()
{
	ChangeStream(0, VIDEO_CHANGE);
	m_bExit = true;
	m_hVideoThread.ExitAThread();
	DestroyStreamer();
}

bool CDeviceStream::PlayRealStream()
{
	m_bFindIFrame = false;

	if (m_chnnl > 0)
		return ChangeStream(m_chnnl, VIDEO_CHANGE);
	else
		return false;
}

void CDeviceStream::StopRealStream()
{
	if (!m_bSaveRealData)
	{
		//如果不存储则关闭视频流.
		ChangeStream(0, VIDEO_CHANGE);
	}
}

//////////////////////////////////////////////////////////////
//////////
//函数名: SaveRealData
//作者:   czg
//日期:   2009-5-21
//功能:   按指定的文件名创建实时文件，开始保存
//参数:   nChannel    通道号
//　　　　sFileName   保存文件名
//返回值: 0表示成功
//        1文件创建失败
//        2该通道实时数据已经处于保存状态
//////////
///////////////////////////////////////////////////////////////
S32  CDeviceStream::SaveRealData(S8 *sFileName)
{
	//该通道已经处于保存状态
	S32 ret = 2;
	if (m_bSaveRealData)
	{
		return ret;
	}
	
	do
	{
		m_writeSection.Lock();
		if (m_pRealWriter != NULL)
		{
			delete m_pRealWriter;
			m_pRealWriter = NULL;
		}

		m_pRealWriter = new PSWriter();
		if (m_pRealWriter == NULL)
		{
			break;
		}

		PS_FILE_HDR fileHdr;
		GenerateFileInfo(m_chnnl - 1, fileHdr);
		if (!m_pRealWriter->Create(sFileName, (void*)&fileHdr, sizeof(fileHdr)))
		{
			delete m_pRealWriter;
			m_pRealWriter = NULL;
			break;
		}
	   
		//设置标志值
		m_bSaveRealData = true;
		m_writeSection.Unlock();
		
		//if (!m_bRender)
		{
			ret = ChangeStream(m_chnnl, VIDEO_CHANGE);
		}

		//for record audio
		m_pShareWriter = m_pRealWriter;

		ret = 0;
	} while (0);
    
	return ret;
}

void CDeviceStream::StopRealData()
{
	if (!m_bSaveRealData)
	{
		return;
	}

	//for record audio
	m_pShareWriter = NULL;
	m_writeSection.Lock();
	//设置标志值
	m_bSaveRealData = false;
	if (m_pRealWriter != NULL)
	{
		delete m_pRealWriter;
		m_pRealWriter = NULL;
	}	
	m_writeSection.Unlock();
	
	//if (!m_bRender)
	{
		//ChangeStream(0, VIDEO_CHANGE);
	}
}

void CDeviceStream::GenerateFileInfo(S32 chnnl, PS_FILE_HDR &fileHdr)
{
	memset(&fileHdr, 0, sizeof(fileHdr));
	fileHdr.FileID    = 0x22;
	fileHdr.VGAMode   = m_videoSignalType;	
	memcpy(fileHdr.CamName[chnnl], m_chnnlName, sizeof(fileHdr.CamName[chnnl]));
	fileHdr.RecordChannel = 1 << chnnl;
	fileHdr.ChResolution[chnnl] = m_videoRes;
	fileHdr.ChFPS[chnnl] = 5;
    
    struct tm *curTime = kt_util_getLocalTime();
    printf("data: %d%d%d time: %d:%d:%d \n", (1900 + curTime->tm_year), (1 + curTime->tm_mon), 
           curTime->tm_mday, (curTime->tm_hour + 8), curTime->tm_min, curTime->tm_sec);
    
    //fileHdr.TimeStart = 
	//time(&fileHdr.TimeStart);
	memset(fileHdr.RecMode[chnnl], REC_MANUAL, sizeof(fileHdr.RecMode[chnnl]));
	fileHdr.AVNum = 0xffff;
}

/*保存实时流*/
void CDeviceStream::SaveRealData(S8 *ps, UNS32 len)
{	
	m_writeSection.Lock();
	if (m_pRealWriter != NULL)
	{
		m_pRealWriter->PushPacket(ps, len);
	}
	m_writeSection.Unlock();
}

bool CDeviceStream::IsRecording()
{
	return m_bSaveRealData;
}

bool CDeviceStream::ChangeStream(UNS8 chnnl, MODE_PLAY playMode)
{
	NET_PLAY_STATUS_CHANGE StatusChange;
	memset(&StatusChange, 0, sizeof(NET_PLAY_STATUS_CHANGE));
	StatusChange.PlayStatus = playMode;
	
	if (chnnl > 0)
	{
		if (playMode == VIDEO_CHANGE)
			StatusChange.SecondData = (0x01 << (chnnl - 1 ));
		else
			StatusChange.SecondData = chnnl;
	}
	
	S8 cmd[MAX_CMD_LEN + 1] = {0, };
	S32 cmdlen = PackCommand(&StatusChange, sizeof(StatusChange), PLAY_STATUS_CHANGE, cmd, sizeof(cmd), m_userId);	
	if (m_pStreamer != NULL)
	{
		if (0 == m_pStreamer->SendData(cmd, cmdlen))
		{
			return true;
		}
	}
	
	return false;
}

bool CDeviceStream::CreateStreamer(UNSL32 ip, UNS16 port)
{
	NET_MEDIA_ACCESS media; 
	media.UserID		= m_userId;
	media.AccessCode	= m_accessId;

	if (m_bSubStream)
		media.StreamType = SUB_STREAM;
	else
		media.StreamType = MAIN_STREAM;

	S32	ret = 0;
	S32 cmdlen = 0;
	S8 cmd[MAX_CMD_LEN + 1] = {0, };
	cmdlen = PackCommand(&media, sizeof(media), MIDEA_ACCESS, cmd, sizeof(cmd), m_userId);

	do
	{
		if (cmdlen == 0)
		{
			break;
		}

		if (m_pStreamer != NULL)
		{
			m_pStreamer->UnInit();
		}

		if (m_pStreamer == NULL)
		{
			m_pStreamer = new StreamSocket();
		}

		if (m_pStreamer != NULL)
		{
			ret = m_pStreamer->Init(ip, port, m_pFifo);
		}

		if (ret != 0)
		{
			break;
		}
		
		if (0 != m_pStreamer->SendData(cmd, cmdlen))
		{
			break;
		}

		KTCMDHDR hdr = { 0, };
		cmdlen = m_pStreamer->ReadData((S8*)&hdr, sizeof(hdr));
		if (cmdlen != sizeof(hdr))
		{
			break;
		}

		if (hdr.startCode != MAGIC_START)
		{
			break;
		}

		cmdlen = m_pStreamer->ReadData(cmd, hdr.totalSize - sizeof(hdr));
		if (cmdlen != hdr.totalSize - sizeof(hdr))
		{
			break;
		}
		
		if (hdr.infoType != MIDEA_ACCESS_ACCEPT)
		{
			break;
		}
		
		if (m_pStreamer->Start())
		{
			return true;
		}
	} while (0);

	DestroyStreamer();
	return false;
}

void CDeviceStream::DestroyStreamer()
{
	if (m_pStreamer != NULL)
	{
		delete m_pStreamer;
		m_pStreamer = NULL;
	}
}

void CDeviceStream::RenderTask()
{
	S8 *buf = new S8[MAX_DECODE_FRAMESIZE];
	UNS32 length	= 0;
	PES_PACKET_HEADER *PESHead = NULL;
	S32 vopType			= 0;
	S32 avtype			= VIDEO_FRAME;
	S32 chID			= 0;
	S8 *curMP4Buf		= NULL;	
	S32 curMp4peslen	= 0;
	UNS32 lostDataTime = GetTickCount();

	while (!m_bExit)
	{
        //printf("RenderTask 处理。。。\n");
		if (m_pFifo != NULL)
		{
			length = m_pFifo->PopFrame((S8*)buf, MAX_DECODE_FRAMESIZE);
		}
		
		if (length <= 0)
		{			
			if (GetTickCount() - lostDataTime > 40000)//40s接收不到有效数据则重新连接
			{
                printf("ReConnect();//重连....。。。\n");
				//if (m_bRender)
				{
					ReConnect();//重连
				}				
				lostDataTime = GetTickCount();
			}
			
			Sleep(10);
			continue;
		}		
		lostDataTime = GetTickCount();//to optimize alive view
		
		if (length > MAX_DECODE_FRAMESIZE)
		{
			//fatal error
			m_bFindIFrame = false;
			continue;
		}
		
		PESHead = (PES_PACKET_HEADER*)buf;
		vopType = PESHead->avInfo.VideoInfo.vop_type;
		chID	= PESHead->channel_id;
		avtype  = PESHead->av_type;
		PESHead->avInfo.VideoInfo.BindAudioChn = m_biAudio;
		
		if ((vopType == I_PICTURE) && (m_bFindIFrame == false))
		{
			m_bFindIFrame = true;
		}
		
		if (avtype == VIDEO_FRAME && m_bFindIFrame)
		{
			if (PESHead->channel_id + 1 == m_chnnl)//如果保存
			{
				SaveRealData((S8*)buf, length);
			}
			curMP4Buf = (S8*)(buf + PESHEADER_SIZE);
			curMp4peslen = PESHead->len;
			if (PESHead->channel_id + 1 != m_chnnl)
			{
				//判断是否是该通道的视频
				continue;
			}
			//Real video call back
            printf("chnnl = %d,Mp4len = %d\n", m_chnnl,curMp4peslen);
			if (m_cbRealVideoData.cbRealVideoFunc)
			{
				//UNS32 type = REAL_VIDEO;
				//m_cbRealVideoData.cbRealVideoFunc(type, m_chnnl, curMP4Buf, curMp4peslen, m_cbRealVideoData.dwUser);
                //void* param, void* data, int len, int dropFrame, int fps
                m_cbRealVideoData.cbRealVideoFunc((void*)m_cbRealVideoData.dwUser, curMP4Buf, curMp4peslen, 0, 1);
			}
		}
	}

	if (buf != NULL)
	{
		delete[] buf;
		buf = NULL;
	}
}

void CDeviceStream::SetRealVideoCallBack(process_cb_ex cbRealVideoFunc, void* dwUser)
{
	if(cbRealVideoFunc)
	{
		m_cbRealVideoData.cbRealVideoFunc = cbRealVideoFunc;
		m_cbRealVideoData.dwUser = dwUser;
	}
	else
	{
		memset(&m_cbRealVideoData, 0, sizeof(CbRealVideoStruct));
	}
}
