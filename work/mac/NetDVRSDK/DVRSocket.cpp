#include "Event.h"
#include "DVRSocket.h"
#include "RemoteFilePlayer.h"
#include "macro.h"
extern "C"
{
	#include "NetDvrApi.h"
};

UNSL32 HeartBeat(LPVOID lpvoid);
UNSL32 RecvMessageThread(LPVOID lpvoid);

extern int errno;

DVRSocket::DVRSocket()
{
	m_CmdSocket     = INVALID_SOCKET;
	m_PlayTCPSocket = INVALID_SOCKET;
	m_DownTcpSocket = INVALID_SOCKET;

	m_nRecvMsgType = -1;
	m_nIDCode = -1;
	m_nWaitTime = 3000;  //默认超时时间为3秒
	m_IPAddr = 0;
	memset(&m_AccessCode, -1, sizeof(NET_USER_ACCESS));

	m_bExit = false;
    //设置心跳事件为自动重置，初始无信号
	//m_hHBEvent.CreateUnsignaledEvent();
    m_hHBEvent.CreateUnsignaledEvent("m_hHBEvent",1);
	memset(&m_LastViewInfo, 0, sizeof(DVS_DEV_INFO));

	m_bIsTreated = false;
	m_pRemotePlayer = NULL;
	m_pcbMessFunc = NULL;
	m_bGetViewInfo = false;
	//m_hWaitViewInfo.CreateUnsignaledEvent();	
    m_hHBEvent.CreateUnsignaledEvent("m_hHBEvent",1);
	m_bOnLine = true;
	m_bChnnlNameChanged = false;
}

DVRSocket::~DVRSocket()
{
	m_bExit = true;
	m_hHBEvent.SetSignaledEvent();
	m_hHeartBeat.ExitAThread();
	m_hRecvMsg.ExitAThread();
	if (m_CmdSocket != INVALID_SOCKET)
	{
		closesocket(m_CmdSocket);
		m_CmdSocket = INVALID_SOCKET;
	}

	if (m_PlayTCPSocket != INVALID_SOCKET)
	{
		closesocket(m_PlayTCPSocket);
		m_PlayTCPSocket = INVALID_SOCKET;
	}

	if (m_DownTcpSocket != INVALID_SOCKET)
	{
		closesocket(m_DownTcpSocket);
		m_DownTcpSocket = INVALID_SOCKET;
	}
	
	m_IPAddr = 0;
	memset(&m_AccessCode, -1, sizeof(NET_USER_ACCESS));
}

S32  DVRSocket::GetLastRecvMsgType()
{
	return m_nRecvMsgType;
}

S32  DVRSocket::GetOverTime()
{
	return m_nWaitTime;
}

UNS32 DVRSocket::CreateNetThread()
{
	m_bExit = true;
	m_hHBEvent.SetSignaledEvent();
	m_hHeartBeat.ExitAThread();
	m_hRecvMsg.ExitAThread();

	m_bExit = false;
	//创建心跳线程，定时给服务器端发送消息，实际上是确认自己在线
	m_hHeartBeat.CreateAThread(HeartBeat, this);//create heat beat thread
	//创建一个接收线程，准备接收音视频数据
	m_hRecvMsg.CreateAThread(RecvMessageThread, this);
	if (m_hRecvMsg.ThreadIsNULL())
	{
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::DeleteCMDSocket()
{				
	RequestOperation(S_CLIENT_TCP_CLOSE, 0, m_CmdSocket);
	m_bExit = true; //kuku 05-23
	m_hHBEvent.SetSignaledEvent();
	m_hHeartBeat.ExitAThread();
	m_hRecvMsg.ExitAThread();

	return true;
}

bool DVRSocket::CreateDownTCPSocket()
{
	if (m_DownTcpSocket != INVALID_SOCKET)
	{
		closesocket(m_DownTcpSocket);
		m_DownTcpSocket = INVALID_SOCKET;
	}

	m_DownTcpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_DownTcpSocket == INVALID_SOCKET)
	{
		return false;
	}

	//创建TCP连接通道
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = m_IPAddr;
	sockaddr.sin_port = htons(m_wCmdPort - 1);

	for (S32 i=0; i<8; i++)
	{
		S32 nRecvBufSize = 16 * 1024 * i;
		setsockopt(m_DownTcpSocket, SOL_SOCKET, SO_RCVBUF, (const S8*)&nRecvBufSize, sizeof(nRecvBufSize));
	}

	if (connect(m_DownTcpSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		closesocket(m_DownTcpSocket);
		m_DownTcpSocket = INVALID_SOCKET;
		return false;
	}

	UNSL32 lret = 1;// 非阻塞模式
	ioctlsocket(m_DownTcpSocket, FIONBIO, &lret);
	//判断是否可以进入
	NET_TRANS_PACKET sPacket;
	strncpy((S8*)sPacket.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	sPacket.InfoType = BACKUP_ACCESS;
	sPacket.TotalSize = Net_GetMsgLength(&sPacket);
	sPacket.IDCode = m_nIDCode;
	sPacket.Buffer.BackupAccess.AccessCode = m_AccessCode.AccessCode;
	sPacket.Buffer.BackupAccess.UserID = m_nIDCode;
	strncpy((S8*)(&sPacket) + sPacket.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);
	
	if (sendn(m_DownTcpSocket, (S8*)&sPacket, sPacket.TotalSize, 0) != (S32)sPacket.TotalSize)
	{
		closesocket(m_DownTcpSocket);
		m_DownTcpSocket = INVALID_SOCKET;
		return false;
	}

	return true;
}

bool DVRSocket::DeleteDownTCPSocket()
{
	if (m_DownTcpSocket != INVALID_SOCKET)
	{
		RequestOperation(S_CLIENT_TCP_CLOSE, 0, m_DownTcpSocket);
		closesocket(m_DownTcpSocket);
		m_DownTcpSocket = INVALID_SOCKET;
	}

	return true;
}

UNS32 DVRSocket::RequestOperation(NET_PACKET_TYPE PacketType, LPVOID lpPacketBuf, SOCKET s, S32 nVer)
{
	m_DVRSocketSection.Lock();
	this->m_nRecvMsgType = -1;
    S32 nRet = 0;
	switch(PacketType)
	{
	case S_CREATCMDSOCKET://该分支未用到
		break;
	case S_CREATVIEWTCPSOCKET://该分支未用到	
		break;
	case S_CREATVIEWUDPSOCKET://该分支未用到
		break;
	case S_CREATPLAYTCPSOCKET://该分支未用到
		break;
	case S_CREATBROADCASTSOCKET://该分支未用到
		break;
	case S_PLAY_STATUS_CHANGE://远程文件回放播放状态改变请求消息，类型：音视频，参数：通道号，实时消息由PackCommand组合
		if (lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(NET_PLAY_STATUS_CHANGE));
			nRet = SendPlayStatusChange(s); 
		}
		break;
	case S_PTZ_CONTROL://云台控制请求
		if (lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(NET_PTZ_CTL_INFO));
			nRet = SendPTZControl();
		}
		break;
	case S_MIDEA_ACCESS://媒体请求消息，由PackCommand组合，该分支未用到
		break;
	case S_USER_ACCESS://认证消息，由PackCommand组合，该分支未用到
		break;
	case S_COMMON://该分支未用到
		if (lpPacketBuf)
		{
           memcpy(SendBuffer, lpPacketBuf, MAX_NET_COMMON_MSG_LEN);
           nRet = SendCommon();
		}
		break; 
	case S_RECORD_UPDATE://该分支未用到
	    nRet = SendRecordUpdate();
		break; 
	case S_SETUP_CHANGED://SETUP_CHANGED消息由SetConfig完成，该分支没用到
		break; 
	case S_CAM_NAME_CHANGED://该分支未用到
        nRet = SendCamNameChanged(); 
		break; 
	case S_WANT_SYS_CONFIG://获取DVR配置信息
		if (lpPacketBuf)
		{
            memcpy(SendBuffer, lpPacketBuf, sizeof(NET_WANT_SYS_CONFIG_ITEM));
            nRet = SendWantSystemConfig(nVer);
		}      
		break;
    case S_WANT_DB_DATE://文件检索，日期匹配
		nRet = SendWantDBDate();
		break; 
	case S_WANT_DB_DAY_FILE://文件检索，当日文件匹配
		if (lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(UNS32));
            nRet = SendWantDBDayFile();
		}
		break;
	case S_WANT_LOG_FILE://该分支未用到
		if (lpPacketBuf)
		{
            memcpy(SendBuffer, lpPacketBuf, sizeof(NET_WANT_LOG));
            nRet = SendWantLogFile();
		}
		break;
	case S_WANT_EVENT_FILE://该分支未用到
		if (lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(NET_WANT_LOG));
			nRet = SendWantEventFile();
		}
		break;
	case S_WANT_SYS_STATUS://获取系统状态信息，该分支未用到
		nRet = SendWantSystemStatus(); 
		break; 
	case S_WANT_PLAY_FILE_INFO://获取远程文件信息，远程回放或备份时发送
		if (lpPacketBuf)
		{
            memcpy(SendBuffer, lpPacketBuf, DVS_FILE_NAME_LEN);
            nRet = SendWantPlayFileInfo(s);
		}
		break;
	case S_PLAY_FILE_NAME://打开文件
		if (lpPacketBuf)
		{
            memcpy(SendBuffer, lpPacketBuf, DVS_FILE_NAME_LEN);
            nRet = SendPlayFileName(s);
		}
		 break; 
	case S_CLIENT_TCP_CLOSE://关闭下载Socket
		if (s != INVALID_SOCKET)
		{
            nRet = SendClientTCPClose(s);
		}
		break; 
	case S_HEART_BEAT://心跳
		nRet = SendHeartBeat(); 
		break;
	case S_ALARM_IN://报警输入状态变化消息，该分支未用到
		if (lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(NET_ALARM_IN));
            nRet = SendAlarmIn();
		}
		break;
	case S_ALARM_OUT://报警输出状态变化，该分支未用到
		if (lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(NET_ALARM_OUT));
			nRet = SendAlarmOut();
		}
		break;
	case S_LOG_NUM://日志文件数目，该分支未用到
		nRet = SendWantLogNum();
		break;
	case S_EVENT_NUM://事件文件数目，该分支未用到
		nRet = SendWantEventNum();
		break;
	case S_BACKUP_SIZE://获取下载字节数
		if(lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(NET_FILE_BACKUP_SIZE));
			nRet = SendWantBackupSize();
		}
		break;
	case S_WANT_PLAY_FILE_INDEX://获取回放文件索引信息
		nRet = SendWantPlayFileIndex(); 
		break;
	case S_WANT_PLAY_FILE_PES_PACKET://获取回放文件的PES包
		if (lpPacketBuf)
		{
			memcpy(SendBuffer, lpPacketBuf, sizeof(std64_net_t));
			nRet = SendWantPlayFilePESPacket();
		}
		break;
	case S_WANT_BACKUP_EXE_HDR://该分支未用到
		break;
	case S_WANT_VIEW_INFO://获取设备信息，在改变通道名称时重新获取
		m_bGetViewInfo = false;
		nRet = SendWantViewInfo();
		break;
	case S_WANT_FILE_RIGHT_CHANNEL://备份权限检测，在打开备份文件时检测
		nRet = SendWantFileRightChannel();
		break;
	case S_FILE_DOWNLOAD_START://开始备份请求，该分支未用到
		break;
	case S_FILE_DOWNLOAD_STOP://文件备份结束，该分支未用到
		break;
	case S_WANT_GET_USER_RIGHT://获取用户权限
		nRet = SendWantUserRight();      
		break;
	case S_WANT_GET_LINK_NUM://获取当前资源连接数
		nRet = SendWantLinkNum();
		break;
	case S_WANT_REQUEST_EMAIL_TESTING://客户端请求DVR发送测试邮件
		nRet = RequestEmailTesting();
		break;
	default:
		nRet = FAILURE;
		break;
	}
	m_DVRSocketSection.Unlock();

	return nRet;
}

UNS32 DVRSocket::SetConfig(LPVOID lpConfig, S32 size, S32 nVer)
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= SETUP_CHANGED;
	memcpy((S8*)&s_Packet.Buffer.SetupItem, (S8*)lpConfig, size);
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet, nVer);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"SETUP_CHANGED\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendPlayStatusChange(SOCKET s)
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= PLAY_STATUS_CHANGE;	
	memcpy(&s_Packet.Buffer.PlayStatus, (S8*)SendBuffer, sizeof(NET_PLAY_STATUS_CHANGE));
	s_Packet.IDCode		= m_nIDCode;
	
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);
	
	if (s == INVALID_SOCKET)
	{
		return FAILURE;
	}

	if (sendn(s, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{	
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendPTZControl()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= PTZ_CONTROL;
	memcpy((S8*)&s_Packet.Buffer.PTZControl, (S8*)SendBuffer, sizeof(NET_PTZ_CTL_INFO));
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendCommon()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= NOTICE;
	s_Packet.IDCode		= m_nIDCode;
	memcpy((S8*)s_Packet.Buffer.NetCommonMsg, (S8*)SendBuffer, MAX_NET_COMMON_MSG_LEN);
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendRecordUpdate()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= RECORD_UPDATE;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"COMMON\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendCamNameChanged()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= CAM_NAME_CHANGED;	
	memcpy((S8*)&s_Packet.Buffer.CamName, (S8*)SendBuffer, sizeof(NET_CAMNAME_CHANGE));
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet,s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"CAM_NAME_CHANGED\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantSystemConfig(S32 nVer)
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType				= SYS_CONFIG;
	s_Packet.Buffer.SysconfigItem	= *(NET_WANT_SYS_CONFIG_ITEM*)SendBuffer;
	s_Packet.IDCode					= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet, nVer);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}	

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0 ) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"WANT_SYS_CONFIG\" to the server command port\n");
		return FAILURE;
	}
	
	return SUCCESS;
}

UNS32 DVRSocket::SendWantDBDate()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= SYS_DB_FILE_DATE;
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"WANT_DB_DATE\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantDBDayFile()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= SYS_DB_DAY_FILE;	
	s_Packet.Buffer.Day	= *(UNS32*)SendBuffer;
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"WANT_DB_DAY_FILE\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantLogFile()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= SYS_LOG_FILE;	
	memcpy(&s_Packet.Buffer.LogBuffer, (S8*)SendBuffer, sizeof(NET_WANT_LOG));
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"WANT_LOG_FILE\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantEventFile()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= SYS_EVENT_FILE;	
	memcpy(&s_Packet.Buffer.LogBuffer, SendBuffer, sizeof(NET_WANT_LOG));
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"WANT_EVENT_FILE\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;	
}

UNS32 DVRSocket::SendWantSystemStatus()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));

	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= SYS_STATUS;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"WANT_SYS_STATUS\" to the server command port\n");
		return FAILURE;
	}	

	return SUCCESS;
}

UNS32 DVRSocket::SendWantPlayFileInfo(SOCKET s)
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= PLAY_FILE_INFO;	
	memcpy((S8*)s_Packet.Buffer.PlayFileName, (S8*)SendBuffer, MAX_FILE_NAME_LEN);
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize	= Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (s == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if(sendn(s, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendPlayFileName(SOCKET s)
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= PLAY_FILE_NAME;	
	memcpy((S8*)s_Packet.Buffer.PlayFileName, (S8*)SendBuffer, MAX_FILE_NAME_LEN);
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (s == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(s, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"PLAY_FILE_NAME\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendClientTCPClose(SOCKET s)
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));

	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= CLIENT_TCP_CLOSE;
	s_Packet.IDCode		= m_nIDCode;
	
	s_Packet.Buffer.TcpClose.AccessCode = m_AccessCode.AccessCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (s == INVALID_SOCKET)
	{
		return FAILURE;
	}

	if (sendn(s, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"CLIENT_TCP_CLOSE\" to the server command port\n");
	}

	if (m_CmdSocket == s)
	{
		closesocket(m_CmdSocket);
		m_CmdSocket = INVALID_SOCKET;	
		m_IPAddr = 0;
		memset(&m_AccessCode, -1, sizeof(NET_USER_ACCESS));
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendHeartBeat()
{
	//此处判断一下m_AccessCode.UserID
	if (m_nIDCode == -1)
	{
		return SUCCESS;
	}
	
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= HEART_BEAT;
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize	= Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}
	
	S32 sendlen = sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0);
	if (sendlen != s_Packet.TotalSize)
	{
		//printf_s("=====socket send error = %d=========\n", GetLastError());
		printf("=====socket send error = %s=========\n", strerror(errno));
		return FAILURE;
	}
	
	return SUCCESS;
}

UNS32 DVRSocket::SendAlarmIn()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= ALARM_IN;	
	memcpy(&s_Packet.Buffer.AlarmIn, SendBuffer, sizeof(NET_ALARM_IN));
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"ALARM_IN\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendAlarmOut()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= ALARM_OUT;
	memcpy(&s_Packet.Buffer.AlarmOut, SendBuffer, sizeof(NET_ALARM_OUT));
    s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		return FAILURE;
	}

	if (sendn(m_CmdSocket,(S8*)&s_Packet,s_Packet.TotalSize,0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"WANT_PLAY_FILE_INFO\" to the server command port\n");
		return FAILURE;
	}

    return SUCCESS;
}

UNS32 DVRSocket::SendWantLogNum()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= LOG_NUM;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"LOG_NUM\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantEventNum()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= EVENT_NUM;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"EVENT_NUM\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantBackupSize()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= BACKUP_SIZE;	
	memcpy((S8*)&s_Packet.Buffer.WantFileSize, (S8*)SendBuffer, sizeof(NET_FILE_BACKUP_SIZE));
    s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_DownTcpSocket == INVALID_SOCKET)
	{
		return FAILURE;
	}

	if (sendn(m_DownTcpSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantPlayFileIndex()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= PLAY_FILE_INDEX;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"PLAY_FILE_INDEX\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantPlayFilePESPacket()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= PLAY_FILE_PES_PACKET;	
	memcpy(&s_Packet.Buffer.offset, SendBuffer, sizeof(std64_net_t));
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"PLAY_FILE_PES_PACKET\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantViewInfo()
{
    NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= VIEW_INFO;
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize	= Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		return FAILURE;
	}

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		return FAILURE;
	}

	return SUCCESS;	
}

UNS32 DVRSocket::SendWantFileRightChannel()
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= BACKUP_RIGHT_CHECK;
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_DownTcpSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(m_DownTcpSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"BACKUP_RIGHT_CHECK\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantUserRight()//获取用户权限
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= GET_USER_RIGHT;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}	

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"GET_USER_RIGHT\" to the server command port\n");
		return FAILURE;
	}
	
	return SUCCESS;
}

UNS32 DVRSocket::Net_GetMsgLength(NET_TRANS_PACKET *pMsg, S32 nVer)//只用于发包长度的计算
{
	UNS32 Size;
	Size = sizeof(MAX_NET_STARTCODE_LEN) + //start code
		sizeof(MAX_NET_ENDCODE_LEN) +	//end code
		sizeof(UNS32) +	//Total Size
		sizeof(UNS32) +	//IDCode
		sizeof(NET_TRANS_TYPE);	//InfoType
	//calculate the buffer size
    switch (pMsg->InfoType)
	{
	case USER_ACCESS://认证消息，由PackCommand组合，该分支未用到
		Size += sizeof(NET_REMOTE_ACCESS);
		break;
	case MIDEA_ACCESS://媒体请求消息，由PackCommand组合，该分支未用到
		Size += sizeof(NET_MEDIA_ACCESS);
		break;
	case BACKUP_ACCESS://下载请求消息，由CreateDownTCPSocket完成
		Size += sizeof(NET_BACKUP_ACCESS);
		break;
	case VIEW_INFO://获取设备信息，由SendWantViewInfo完成
		break;
	case SYS_STATUS://获取系统状态信息，由SendWantSystemStatus完成
		break;
	case SYS_CONFIG://获取DVR配置信息，由SendWantSystemConfig完成
		Size += sizeof(UNS32);
		break;
	case SETUP_CHANGED://远程设置请求，由SetConfig完成
		if (nVer == 4)
		{
			if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_RECORD)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_RECSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_CAMERA)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_CAMERASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_EVENT)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_EVENTSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_DISPLAY)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_VGASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_NET)
			{
				if (DVS_GetDvrVersion() >= ADD_EMAILINTERVAL)
					Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_NETSETUP_VER4);
				else
					Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_NETSETUP_VER4) - sizeof(UNS16) * 2;
			}
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_SYS)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_SYSSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_USER)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_USERSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_PTZ)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_PTZCRUISE);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_SYSTIME)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(UNS32);
		}
		else if (nVer == 3)
		{
			if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_RECORD)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_RECSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_CAMERA)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_CAMERASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_EVENT)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_EVENTSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_DISPLAY)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_VGASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_NET)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_NETSETUP_VER3);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_SYS)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_SYSSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_USER)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_USERSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_PTZ)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_PTZSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_SYSTIME)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(UNS32);
		}
		else if (nVer == 2)
		{
			if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_RECORD)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_RECSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_CAMERA)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_CAMERASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_EVENT)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_EVENTSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_DISPLAY)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_VGASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_NET)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_NETSETUP_VER2);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_SYS)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_SYSSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_USER)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_USERSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_PTZ)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(DVS_PTZSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SETUP_SYSTIME)
				Size += sizeof(DVS_SETUP) + sizeof(UNS32) + sizeof(UNS32);
		}
		else if (nVer == 1)
		{
			if(pMsg->Buffer.SysconfigItem.SetupID == RECORD_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(DVS_RECSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == CAMERA_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(DVS_CAMERASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == EVENT_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(DVS_EVENTSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == DISPLAY_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(DVS_VGASETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == NET_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(DVS_NETSETUP_VER1);
			else if(pMsg->Buffer.SysconfigItem.SetupID == SYSTEM_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(DVS_SYSSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == USER_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(DVS_USERSETUP);
			else if(pMsg->Buffer.SysconfigItem.SetupID == TIME_CMD)
				Size += sizeof(SETUP_CMD_ID) + sizeof(UNS32);
		}
		break;
	case CAM_NAME_CHANGED://由SendCamNameChanged完成，客户端未发送该消息请求
		Size += sizeof(NET_CAMNAME_CHANGE);
		break;
	case PTZ_CONTROL://云台控制请求，由SendPTZControl完成
		Size += sizeof(NET_PTZ_CTL_INFO);
		break;
	case SYS_DB_FILE_DATE://文件检索，由SendWantDBDate完成
		break;
	case SYS_DB_DAY_FILE://文件检索，由SendWantDBDayFile完成
		Size += sizeof(UNS32);
		break;
	case PLAY_FILE_NAME://打开文件，由SendPlayFileName完成
		Size += MAX_FILE_NAME_LEN;
		break;
	case PLAY_FILE_PES_PACKET:////获取回放文件的PES包，由SendWantPlayFilePESPacket完成
		Size += sizeof(UNS64);
		break;
    case PLAY_FILE_INDEX://获取回放文件索引信息，由SendWantPlayFileIndex完成
		break;
	case PLAY_FILE_INFO://获取远程文件信息，由SendWantPlayFileInfo完成
		Size += MAX_FILE_NAME_LEN;//获取fileinfo时，发送文件名
		break;
	case PLAY_FILE_OFFSET://获取文件offset位置，客户端未发送该消息
		break;
	case PLAY_STATUS_CHANGE://改变播放状态，类型：音视频，参数：通道号，实时消息由PackCommand组合，回放消息由SendPlayStatusChange完成
		Size += sizeof(NET_PLAY_STATUS_CHANGE);
		break;
	case BACKUP_SIZE://获取下载字节数，由SendWantBackupSize完成
		Size += sizeof(NET_FILE_BACKUP_SIZE);
		break;
	case BACKUP_RIGHT_CHECK://备份权限检测，由SendWantFileRightChannel完成
		break;
	case FILE_DOWNLOAD_START://开始备份请求，由SendStartDownload完成
		Size += sizeof(NET_FILE_BACKUP_SIZE);
		break;
	case FILE_DOWNLOAD_STOP://文件备份结束请求由SendStopDownload完成，客户端未发送该请求
		break;
	case SYS_LOG_FILE://日志文件请求，客户端未发送该请求
		break;
	case SYS_EVENT_FILE://事件文件请求，客户端未发送该请求
		break;
	case LOG_NUM://日志文件数目，客户单未发送该请求
		break;
	case EVENT_NUM://事件文件数目，客户端未发送该请求
		break;
	case NOTICE://屏幕显示消息，客户端未发送该请求
		break;
	case CLIENT_TCP_CLOSE://关闭下载Socket，由SendClientTCPClose完成
		Size += sizeof(NET_TCP_CLOSE);
		break;
	case HEART_BEAT://心跳，由SendHeartBeat完成
		break;
	case ALARM_IN://报警输入状态变化，该分支未用到
		break;
	case ALARM_OUT://报警输出状态变化，该分支未用到
		break;
	case RECORD_UPDATE://该分支未用到
		break;
	case RESOLUTION_UPDATE://该分支未用到
		break;
	case HDD_FREE_SIZE_UPDATE://硬盘剩余容量发生变化，该分支未用到
		break;
	case TEMPERATURE_UPDATE://温度发生变化，该分支未用到
		break;
	case RECORD_FILES_PERIOD_UPDATE://录像文件的时间范围，该分支未用到
		break;
	case GET_USER_RIGHT://获取用户权限，由SendWantUserRight完成
		break;
	case GET_LINK_NUM://获取网络资源连接数，由SendWantLinkNum完成
		break;
	case REQUEST_EMAIL_TESTING://客户端请求DVR发送测试邮件，由RequestEmailTesting完成
		break;
	default:
		return FAILURE;
	}
	return Size;
}

S32 DVRSocket::recvn(SOCKET fd, const void *vptr, S32 n, S32 flag, S32 waitTime)
{
	S32 nleft, counter = 0;
	S32 nread;
    S8 *ptr = NULL;
	//建立可读套接字
	fd_set rfd;
	struct timeval timeout;
	ptr = (S8*)vptr;
	//赋值需要接收的字节数
	nleft = n;
	while (nleft > 0 && (fd > 0))
	{ 
		FD_ZERO(&rfd);
		FD_SET(fd, &rfd); 
		timeout.tv_sec  = waitTime;
		timeout.tv_usec = 0;
		S32 nCount = select(fd + 1, &rfd, 0, 0, &timeout);		
        if (nCount > 0)
		{
			if (FD_ISSET(fd, &rfd)) 
			{
				if ((nread = recv(fd, ptr, nleft, flag)) < 0)
				{
					//if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError()== WSAEINTR || WSAGetLastError()== WSAEINPROGRESS)
					if(errno >= 0 && errno <= 132)
					{
						nread = 0;
						counter++;
						if (counter > MAX_RETURN_TIMEOUT / MAX_SELECT_TIMEOUT)
						{
							return -1;
						}
					}else
					{
						printf("Unknown error\n");
						return 0;
					}
				} else if (nread == 0)
				{
					return 0;
				}
				//还剩有多少个字节需要接收
				nleft -= nread;
				ptr += nread;
				counter = 0;
			}
		}else
		{
			counter++;
			if (counter > MAX_RETURN_TIMEOUT / MAX_SELECT_TIMEOUT)
			{
				return -1;
			}
		}
	}
    //返回接收了多少个字节
	return (n - nleft); 
}

S32 DVRSocket::sendn(SOCKET fd, const void *vptr, S32 n, S32 flag)
{
	S32 nleft;
	S32 nwritten;
	const S8 *ptr = NULL;
	fd_set wfd;
	S32 counter = 0;
	struct timeval play_timeout;
	
	ptr = (S8*)vptr;
	nleft = n;
	while (nleft > 0 && (fd > 0)) 
	{
		//设置SELECT模式
		FD_ZERO(&wfd);
		FD_SET(fd, &wfd);
		//超时设置为1秒
		play_timeout.tv_sec = MAX_SELECT_TIMEOUT;
		play_timeout.tv_usec = 0;
		//注意函数中fd+1没有实际意义，这里不起任何作用
		if (select(fd + 1, 0, &wfd, 0, &play_timeout) > 0)
		{
			if (FD_ISSET(fd, &wfd)) 
			{
				if ((nwritten = send(fd, ptr, nleft,flag)) <= 0) 
				{ 
					//if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR || WSAGetLastError() == WSAEINPROGRESS)
					if(errno >= 0 && errno <= 132)
					{
						//出现以上几个错误（暂时还没有搞明白各个错误的标识），只是说明发送缓冲区已满
						//可以等待进入下一次发送，并不是说明该SOCKET发送失败
						//此时一个字节的数据都没有发送出去，需要重发，故 nwritten = 0
						nwritten = 0;
						counter++;
						//如果重发次数超过了指定次数，可以退出
						if (counter > MAX_RETURN_TIMEOUT / MAX_SELECT_TIMEOUT)
						{
							return -1;
						}
					}else
					{
						return 0;
					}
				}

				//还剩多少个字节需要发送
				nleft -= nwritten;
				//将应用程序的发送缓冲去前移
				ptr += nwritten;
				counter = 0;
			}
		}else
		{
			counter++;
			//超过发送等待时间，发送失败，退出程序
			if (counter > MAX_RETURN_TIMEOUT / MAX_SELECT_TIMEOUT)
			{
				return -1;
			}
		}
	}
	//返回发送了多少个字节，此处有点多余
	return n;
}

bool DVRSocket::RecvNetTransPacket(NET_TRANS_PACKET *pNetPacket)//用户认证成功后接收的消息
{
	CRemoteFilePlayer *pRemotePlayer = (CRemoteFilePlayer*)m_pRemotePlayer;
	switch (pNetPacket->InfoType)
	{
	case MIDEA_ACCESS_ACCEPT://MIDEA_ACCESS的回复消息，MIDEA_ACCESS后即等待回复，该分支未用到
		break;
	case MIDEA_ACCESS_REFUSED://MIDEA_ACCESS的回复消息，MIDEA_ACCESS后即等待回复，该分支未用到
		break;
	case BACKUP_ACCESS_ACCEPT://BACKUP_ACCESS的回复消息，在CreateDownTCPSocket里立即等待回复，该分支未用到
		break;
	case BACKUP_ACCESS_REFUSED://BACKUP_ACCESS的回复消息，在CreateDownTCPSocket里立即等待回复，该分支未用到
		break;
	case RIGHT_IS_INVALID://BACKUP_ACCESS的回复消息，在DumpTask中检测，该分支未用到
		break;
	case VIEW_INFO://获取设备信息回复消息，在认证成功后或发送设备信息请求S_WANT_VIEW_INFO后获取
		memcpy(&m_LastViewInfo, &pNetPacket->Buffer.NetViewInfo, sizeof(m_LastViewInfo));
		m_nRecvMsgType = NM_VIEW_INFO;
		m_hWaitViewInfo.SetSignaledEvent();
        m_bGetViewInfo = true;
		m_bChnnlNameChanged = true;
		break;
	case SYS_STATUS://获取设备状态信息的回复，跟请求消息相同，客户端未发送该请求
		break;
	case SYS_CONFIG://获取DVR配置信息的回复，跟请求消息相同
		m_nRecvMsgType = NM_SYS_CONFIG;
		break;
	case SETUP_CHANGED://远程设置请求的回复消息，由SetConfig完成，或者不发送任何数据也会接收到该消息
		break;
	case CAM_NAME_CHANGED://由SendCamNameChanged完成，客户端未发送该消息请求，或者不发送任何数据也会接收到该消息
		break;
	case PTZ_CONTROL://云台控制请求的回复消息，没有回复
		break;
	case SYS_DB_FILE_DATE://文件检索，日期匹配的回复消息
		m_nRecvMsgType = NM_SYS_DB_FILE_DATE;
		break;
	case SYS_DB_DAY_FILE://文件检索，当日文件匹配的回复消息
		m_nRecvMsgType = NM_SYS_DB_DAY_FILE;
		break;
	case PLAY_FILE_NAME://打开文件的回复消息，下载时由Net_GetTcpPacket检测
		break;
	case PLAY_FILE_PES_PACKET://获取回放文件的PES包的回复
		m_nRecvMsgType = NM_PLAY_FILE_PES_PACKET;
		pRemotePlayer->m_RemotePSFile.RecvRemoteFilePESPacket();
		break;
	case PLAY_FILE_INDEX://获取回放文件索引信息的回复
		if (pRemotePlayer->m_RemotePSFile.RecvRemoteFileIndex())
		{
			m_nRecvMsgType = NM_PLAY_FILE_INDEX;
		}
		break;
	case PLAY_FILE_INFO://获取远程文件信息的回复
		m_nRecvMsgType = NM_PLAY_FILE_INFO;
		break;
	case PLAY_FILE_OFFSET://获取文件offset位置的回复，客户端未发送该消息
		m_nRecvMsgType = NM_PLAY_FILE_OFFSET;
		pRemotePlayer->m_RemotePSFile.RecvRemoteFileOffset();
		break;
	case PLAY_STATUS_CHANGE://改变播放状态回复消息，没有该类型回复
		break;
	case PLAY_FILE_IS_END://文件播放结束回复消息，没有该类型回复
		pRemotePlayer->m_RemotePSFile.SetEof(true);
		break;
	case BACKUP_SIZE://获取下载字节数回复，该分支未用到，使用Net_GetTcpPacket检测
		break;
	case FILE_OPEN_SUCCESS://打开文件的回复消息，打开成功
		break;
	case FILE_OPEN_FAILED://打开文件的回复消息，打开失败，在打开备份文件时检测，该分支未用到
		break;
	case BACKUP_RIGHT_CHECK://备份权限检测消息回复，该分支未用到，使用Net_GetTcpPacket检测
		break;
	case FILE_DOWNLOAD_START://备份请求回复，没有该类型回复
		break;
	case FILE_DOWNLOAD_STOP://文件备份结束回复消息，没有该类型回复
		break;
	case SYS_LOG_FILE://日志文件请求回复，客户端未发送该请求
		m_nRecvMsgType = NM_SYS_LOG_FILE;
		break;
	case SYS_EVENT_FILE://事件文件请求回复，客户端未发送该请求
		m_nRecvMsgType = NM_SYS_EVENT_FILE;
		break;
	case LOG_NUM://日志文件数目回复，客户单未发送该请求
		m_nRecvMsgType = NM_LOG_NUM;
		break;
	case EVENT_NUM://事件文件数目回复，客户端未发送该请求
		m_nRecvMsgType = NM_EVENT_NUM;
		break;
	case NOTICE://屏幕显示消息回复，客户端未发送该请求
		break;
	case CLIENT_TCP_CLOSE://关闭下载Socket回复
		break;
	case HEART_BEAT://心跳回复，发送和接收消息相同
		m_nRecvMsgType = NM_HEART_BEAT;
		break;
	case ALARM_IN://报警输入状态变化，处理DVR端发出的对讲请求
		if (this->m_pcbMessFunc)
		{
			DealAlarmInfo();
		}
		break;
	case ALARM_OUT://报警输出状态变化，该分支未用到
		break;
	case RECORD_UPDATE://S:RECORD_UPDATE或者不发送任何数据也会接收到该消息
		break;
	case RESOLUTION_UPDATE://该分支未用到
		break;	
	case HDD_FREE_SIZE_UPDATE://硬盘剩余容量发生变化
		break;
	case TEMPERATURE_UPDATE://温度发生变化
		break;
	case RECORD_FILES_PERIOD_UPDATE://录像文件的时间范围
		break;
	case GET_USER_RIGHT://用户权限回复，发送和接收消息相同
		m_nRecvMsgType = NM_GET_USER_RIGHT;
		break;
	case GET_LINK_NUM://获取网络资源连接数
		m_nRecvMsgType = NM_GET_LINK_NUM;
		break;
	default:
		printf("Invalid InfoType:%d\n", pNetPacket->InfoType);
		return false;
	}
	
	return true;
}

#define HEARTBEAT_TIMEVAL	(30 * 1000)
UNSL32 HeartBeat(LPVOID lpvoid)
{
	DVRSocket *pSocket = (DVRSocket*)lpvoid;
	do{
		//每隔一分钟发送发送一次心跳信息
		if (pSocket)
		{
			//pSocket->m_hHBEvent.WaitForMilliSeconds(HEARTBEAT_TIMEVAL);
            sleep(30);
			S32 nRet = pSocket->RequestOperation(S_HEART_BEAT, 0);
			if (nRet == 0)
			{
				pSocket->m_bOnLine = false;//设备不在线
			}
			else
			{
				pSocket->m_bOnLine = true;//设备在线
			}
		}
	} while (pSocket && !pSocket->m_bExit);

	return 0;
}

UNSL32 RecvMessageThread(LPVOID lpvoid)//UNS32 DVRSocket::RecvMessageThread()
{
	DVRSocket *pSocket = (DVRSocket*)lpvoid;
	
	NET_TRANS_PACKET r_Packet;
	memset((void*)&r_Packet, 0, sizeof(NET_TRANS_PACKET));

	UNS32 recvsize = 0;	
	UNS32 headersize = MAX_NET_STARTCODE_LEN + sizeof(UNS32) * 2 + sizeof(NET_TRANS_TYPE);//该头大小为NET_TRANS_PACKET的前4个变量
	UNS32 packetsize;

	while(pSocket && (!pSocket->m_bExit)) 
	{
		//注意第一个要接收的包为对客户端登陆的回应，该回应是附加数据的，但接收函数是先接收
		//附加数据之前的包部分，得到包长后，继续根据包长获取数据部分内容，但有一点需要注意
		//尽管是分两次接收，但服务端只是用了一次发送，此处两次接收只是从TCP的数据缓冲区两次取出已经
		//收到的数据
		//返回消息有以下几种：
        /************************************************************************/
        /* CLIENT_IS_FULL
		   ACCESS_ACCEPT
		   USER_PASSWORD_ERROR
		   USER_NAME_INVALID
		   USER_HAS_ACCESSED*/
		/************************************************************************/
		if (pSocket->m_CmdSocket == INVALID_SOCKET)
		{
			printf("INVALID_SOCKET\n");
			Sleep(10);
			continue;
		}

		if ((recvsize = pSocket->recvn(pSocket->m_CmdSocket, (S8*)&r_Packet, headersize, 0)) != headersize)
		{
			Sleep(10);
	    }

		packetsize = r_Packet.TotalSize;		
		if (packetsize > sizeof(NET_TRANS_PACKET))
		{
			printf("too large %d[%lu]\n", packetsize, sizeof(NET_TRANS_PACKET));
			Sleep(10);
			continue;
		}
        //开始接收包的数据部分
		if ((recvsize = pSocket->recvn(pSocket->m_CmdSocket, (S8*)&r_Packet+headersize, packetsize - headersize, 0)) != (S32)(packetsize - headersize))
		{
			Sleep(10);
		}
		
		if (strncmp((S8*)r_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN) 
			|| strncmp((S8*)(&r_Packet) + r_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN))
		{
			printf("wrong message received,start code:\"%s\" or end code:\"%s\" not matched\n", r_Packet.StartCode, r_Packet.EndCode);
			Sleep(10);
			continue;
		}

		memcpy((S8*)(pSocket->ReceiveBuffer), (S8*)&r_Packet + headersize, r_Packet.TotalSize - headersize - MAX_NET_ENDCODE_LEN);
		
		//处理接收到的各种消息
		if (pSocket->RecvNetTransPacket(&r_Packet) == false) 
		{
			Sleep(10);
		}
	}
	pSocket->m_bExit = true;

	return SUCCESS;
}

void DVRSocket::StartDVRMessCallBack(fMessCallBack cbMessFunc, HANDLE hDvs, UNS32 dwUser)
{
    m_pcbMessFunc = cbMessFunc;
	this->m_struCbMessFuncInfo.hDvs = hDvs;
	this->m_struCbMessFuncInfo.dwUserData = dwUser;
}

void DVRSocket::StopDVRMessCallBack()
{
    m_pcbMessFunc = NULL;
}

void DVRSocket::DealAlarmInfo()
{
    NET_ALARM_IN struAlarmInfo;
	DVS_ALARM_INFO struDvsAlarmInfo;	
	memcpy(&struAlarmInfo, ReceiveBuffer, sizeof(NET_ALARM_IN));
	struDvsAlarmInfo.dwSize = sizeof(DVS_ALARM_INFO);
	struDvsAlarmInfo.dwAlarmType = struAlarmInfo.AlarmType - 1;
	struDvsAlarmInfo.dwAlarmData = struAlarmInfo.Channel;
	struDvsAlarmInfo.dwValue = struAlarmInfo.Value;

	UNS32 dwCommand = struDvsAlarmInfo.dwAlarmType;
	S8 sDeviceIP[DVS_MAX_DOMAINLEN + 1] = {0,};
	sprintf(sDeviceIP, "%d.%d.%d.%d", m_IPAddr&0xff, (m_IPAddr>>8)&0xff, (m_IPAddr>>16)&0xff, (m_IPAddr>>24)&0xff);
	UNS32 wPort = m_wCmdPort;
	HANDLE hDvs = m_struCbMessFuncInfo.hDvs;
	UNSL32 dwUserData = m_struCbMessFuncInfo.dwUserData;    
	//调用回调函数
	m_pcbMessFunc(dwCommand, hDvs, (S8*)&struDvsAlarmInfo, sizeof(struDvsAlarmInfo), sDeviceIP, wPort, dwUserData);
}

bool  DVRSocket::Net_GetTcpPacket(NET_TRANS_PACKET *pPacket, SOCKET hSocket, S32 timeout)
{
	do
	{
		if (pPacket == NULL || hSocket == INVALID_SOCKET)
		{
			break;
		}

		memset((void*)pPacket, 0, sizeof(NET_TRANS_PACKET));
		UNS32 recvsize = 0;
		//该头大小为NET_TRANS_PACKET的前4个变量
		UNS32 headersize = MAX_NET_STARTCODE_LEN + sizeof(UNS32) * 2 + sizeof(NET_TRANS_TYPE);
		UNS32 packetsize;

		if ((recvsize = recvn(hSocket, (S8*)pPacket, headersize, 0, timeout)) != headersize)
		{
			break;
		}

		packetsize = pPacket->TotalSize;
		if (packetsize > sizeof(NET_TRANS_PACKET))
		{
			printf("too large %d[%lu]\n", packetsize, sizeof(NET_TRANS_PACKET));
			Sleep(100);
			break;
		}

		//开始接收包的数据部分
		if ((recvsize = recvn(hSocket, (S8*)pPacket + headersize, packetsize - headersize, 0, timeout)) != (S32)(packetsize - headersize))
		{
			Sleep(100);
			break;
		}

		if (strncmp((S8*)pPacket->StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN) 
			|| strncmp((S8*)(pPacket) + pPacket->TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN))
		{
			printf("wrong message received,start code:\"%s\" or end code:\"%s\" not matched\n", pPacket->StartCode, pPacket->EndCode);
			Sleep(1);
			break;
		}

		//处理接收到的各种消息
		return true;	  
	} while (0);

	return false;
}

UNS32 DVRSocket::SendStartDownload(SOCKET s, LPVOID lpvoid, S32 size)
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= FILE_DOWNLOAD_START;	
	memcpy((S8*)&s_Packet.Buffer.WantFileSize, (S8*)lpvoid, size);
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (s == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}

	if (sendn(s, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"FILE_DOWNLOAD_START\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}

UNS32 DVRSocket::SendWantLinkNum()//获取网络资源连接数
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= GET_LINK_NUM;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}	

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"GET_LINK_NUM\" to the server command port\n");
		return FAILURE;
	}
	
	return SUCCESS;
}

UNS32 DVRSocket::RequestEmailTesting()//客户端请求DVR发送测试邮件
{
	NET_TRANS_PACKET s_Packet;
	memset((void*)&s_Packet, 0, sizeof(NET_TRANS_PACKET));
	strncpy((S8*)s_Packet.StartCode, NET_TRANS_START_CODE, MAX_NET_STARTCODE_LEN);
	s_Packet.InfoType	= REQUEST_EMAIL_TESTING;	
	s_Packet.IDCode		= m_nIDCode;
	s_Packet.TotalSize = Net_GetMsgLength(&s_Packet);
	strncpy((S8*)(&s_Packet) + s_Packet.TotalSize - MAX_NET_ENDCODE_LEN, NET_TRANS_END_CODE, MAX_NET_ENDCODE_LEN);

	if (m_CmdSocket == INVALID_SOCKET)
	{
		printf("not connected,login first!\n");
		return FAILURE;
	}	

	if (sendn(m_CmdSocket, (S8*)&s_Packet, s_Packet.TotalSize, 0) != (S32)s_Packet.TotalSize)
	{
		printf("failure when send \"GET_LINK_NUM\" to the server command port\n");
		return FAILURE;
	}

	return SUCCESS;
}
