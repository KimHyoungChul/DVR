#ifndef DVRSOCKET_H
#define DVRSOCKET_H

#include "Lock.h"
#include "Thread.h"
#include "Event.h"

#define PLAY_SYNC_CODE				0x24242424
#define MAX_SELECT_TIMEOUT			1	//1s
#define MAX_RETURN_TIMEOUT			40	//40s

//主要是客户端向服务器请求服务的一些消息类型
typedef enum{//客户端发起的消息类型，RequestOperation参数
	S_CREATCMDSOCKET = 1,		//1
	S_CREATVIEWTCPSOCKET,		//2
	S_CREATVIEWUDPSOCKET,		//3
	S_CREATPLAYTCPSOCKET,		//4
	S_CREATBROADCASTSOCKET,		//5
	S_PLAY_STATUS_CHANGE,		//6系统网络回放状态改变(c->s NET_PLAY_STATUS_CHANGE) SendPlayStatusChange(NET_PLAY_STATUS_CHANGE *PlayStatusChange)
	S_PTZ_CONTROL,				//7云台控制信息(c->s NET_PTZ_CTL_INFO) SendPTZControl(NET_PTZ_CTL_INFO *pPTZControl)
	S_MIDEA_ACCESS,				//8网络监控或者网络回放的认证消息(c->s NET_MEDIA_ACCESS)
	S_USER_ACCESS,				//9远程用户接入 (c->s NET_REMOTE_ACCESS)
	S_COMMON,					//10屏幕上显示的消息(c->s NetCommonMsg)SendCommon(S8 *pScreenMessage)
	S_RECORD_UPDATE,			//11开始或结束录像----C->S时，没有buffer内容 -----SendRecordStart()
								//				----S->C NET_RECORD_BUFFER	RecMode-----SendRecordEnd()
	S_SETUP_CHANGED,			//12配置信息修改(s->c无buffer)      SendSetupChange(NET_SYS_CONFIG_CHANGE *pSetupItem)
	S_CAM_NAME_CHANGED,			//13摄像枪标识修改(c->s==s->c NET_CAMNAME_CHANGE)SendCamNameChanged(S8 *pCamName,UNS32 Channel)
	S_WANT_SYS_CONFIG,			//14要求系统配置信息(c->s无buffer)-----SendWantSystemConfig(DVRSETUP *pdvrsetup)
	S_WANT_DB_DATE,				//15要求数据库中的文件日期(c->s无buffer)-----SendWantDBDate(NET_SYS_DB_DATE_INFO *pDateInfo)
	S_WANT_DB_DAY_FILE,			//16要求数据库某一天的文件列表(c->s UNS32 Day)SendWantDBDayFile(UNS32 day,NET_SYS_DB_DAY_FILE	*pDBDayFile)
	S_WANT_LOG_FILE,			//17要求日志文件(c->s NET_WANT_LOG)SendWantLogFile(NET_WANT_LOG *pLogBuffer,NET_SYS_LOG_INFO *pSyslogFile)
	S_WANT_EVENT_FILE,			//18要求事件日志文件(c->s NET_WANT_LOG)SendWantEventFile(NET_WANT_LOG *pEventBuffer,NET_SYS_LOG_INFO *pSysEventFile)		
	S_WANT_SYS_STATUS,			//19要求系统状态信息(c->s无buffer)----SendWantSystemStatus(SYS_STATUS_INFO *psysstatus)
	S_WANT_PLAY_FILE_INFO,		//20DVR调用的函数返回有问题，要求回放文件的信息(c->s无buffer) -----SendWantPlayFileInfo(PS_FILE_HDR *pPlayFileInfo)
	S_PLAY_FILE_NAME,			//21网络回放的文件名(c->s UNS8 PlayFileName[MAX_FILE_NAME_LEN])SendPlayFileName(S8 *pPlayFileName);
	S_CLIENT_TCP_CLOSE,			//22客户端端口断开(c->s NET_TCP_CLOSE)-----SendClientTCPClose()
	S_HEART_BEAT,				//23心跳消息(c->s无buffer)-----SendHeartBeat()
	S_ALARM_IN,					//24设备接收到外部报警输入(c->s NET_ALARM_IN
	S_ALARM_OUT,				//25报警取消(s->c NET_ALARM_OUT)
	S_LOG_NUM,					//26系统日志文件最大条目数量 C->S buffer空; S->C Buffer  SendWantLogNum(UNS32 *LogNum);
	S_EVENT_NUM,				//27系统事件日志最大条目数量 C->S buffer空; S->C Buffer  SendWantEventNum(UNS32 *EventNum)
	S_BACKUP_SIZE,				//28获取网络备份文件的大小或者开始备份		SendWantBackupSize()	
	S_WANT_PLAY_FILE_INDEX,		//29
	S_WANT_PLAY_FILE_PES_PACKET,//30
	S_WANT_BACKUP_EXE_HDR,		//31
	S_WANT_VIEW_INFO,			//32
	S_WANT_FILE_RIGHT_CHANNEL,	//33
	S_FILE_DOWNLOAD_START,		//34
	S_FILE_DOWNLOAD_STOP,		//35
	S_WANT_GET_USER_RIGHT,		//36获取用户权限
	S_WANT_GET_LINK_NUM,		//37获取网络资源连接数
	S_WANT_REQUEST_EMAIL_TESTING//38客户端请求DVR发送测试邮件
}NET_PACKET_TYPE;
//主要是服务器端效应客户端请求的回复消息类型
#if !defined(NET_MESSAGE_H)
#define NET_MESSAGE_H

#define WM_NET_MESSAGE			WM_USER+138

typedef enum{//经客户端转换的服务端回复消息，RecvNetTransPacket的接收类型
	NM_SYS_CONFIG = 1,			//1->SYS_CONFIG，S_WANT_SYS_CONFIG的回复消息
	NM_SYS_DB_FILE_DATE,		//2->SYS_DB_FILE_DATE，S_WANT_DB_DATE的回复消息
	NM_SYS_DB_DAY_FILE,			//3->SYS_DB_DAY_FILE，S_WANT_DB_DAY_FILE的回复消息
	NM_SYS_LOG_FILE,			//4->SYS_LOG_FILE，S_WANT_LOG_FILE的回复消息
	NM_SYS_EVENT_FILE,			//5->SYS_EVENT_FILE，S_WANT_EVENT_FILE的回复消息
	NM_SYS_STATUS,				//6->SYS_STATUS，S_WANT_SYS_STATUS的回复消息
	NM_FILE_BACKUP,				//7->
	NM_PLAY_FILE_INFO,			//8->PLAY_FILE_INFO
	NM_VIEW_INFO,				//9->VIEW_INFO，登录成功即发送给客户端
	NM_PLAY_STATUS_CHANGE,		//10->PLAY_STATUS_CHANGE，S_PLAY_STATUS_CHANGE的回复消息
	NM_PTZ_CONTROL,				//11->PTZ_CONTROL，S_PTZ_CONTROL的回复消息
	NM_MIDEA_ACCESS,			//12->
	NM_USER_ACCESS,				//13->
	NM_COMMON,					//14->
	NM_RECORD_UPDATE,			//15->
	NM_SETUP_CHANGED,			//16->
	NM_CAM_NAME_CHANGED,		//17->
	NM_WANT_SYS_CONFIG,			//18->
	NM_WANT_DB_DATE,			//19->
	NM_WANT_DB_DAY_FILE,		//20->
	NM_WANT_LOG_FILE,			//21->
	NM_WANT_EVENT_FILE,			//22->
	NM_WANT_SYS_STATUS,			//23->
	NM_WANT_PLAY_FILE_INFO,		//24->
	NM_PLAY_FILE_NAME,			//25->
	NM_CLIENT_TCP_CLOSE,		//26->
	NM_CLIENT_IS_FULL,			//27->
	NM_ACCESS_ACCEPT,			//28->ACCESS_ACCEPT
	NM_USER_PASSWORD_ERROR,		//29->
	NM_USER_NAME_INVALID,		//30->
	NM_USER_HAS_ACCESSED,		//31->
	NM_RIGHT_IS_INVALID,		//32->
	NM_HEART_BEAT,				//33->
	NM_ALARM_IN,				//34->
	NM_ALARM_OUT,				//35->
	NM_HDD_FREE_SIZE_UPDATE,	//36->
	NM_TEMPERATURE_UPDATE,		//37->
	NM_RECORD_FILES_PERIOD_UPDATE,//38->
	NM_LOG_NUM,					//39->LOG_NUM
	NM_EVENT_NUM,				//40->EVENT_NUM
	NM_BACKUP_SIZE,				//41->
	NM_SOCKT_MAYBE_COLSE,		//42->
	NM_RESOLUTION_UPDATE,		//43->
	DM_PLAYFILE_SECOND,			//44->
	DM_INDEX_SECOND,			//45->
	DM_PLAYFILE_EOF,			//46->
	DM_SEGMENTFILE_PERCENT,		//47->
	DM_OPENFILE_SUCCEED,		//48->
	DM_FLUX_INFO,				//49->
	NM_CONNECTION_ERR,			//50->
	NM_PLAY_FILE_INDEX,			//51->PLAY_FILE_INDEX，S_WANT_PLAY_FILE_INDEX的回复消息
	NM_PLAY_FILE_PES_PACKET,	//52->PLAY_FILE_PES_PACKET，S_WANT_PLAY_FILE_PES_PACKET的回复消息
	NM_PLAY_FILE_OFFSET,		//53->PLAY_FILE_OFFSET
	NM_GET_USER_RIGHT,			//54->GET_USER_RIGHT，S_WANT_GET_USER_RIGHT获取用户权限的回复消息
	NM_GET_LINK_NUM				//55->GET_LINK_NUM，S_WANT_GET_LINK_NUM获取网络资源连接数的回复消息
}NET_MESSAGE_TYPE;
#endif // !defined(NET_MESSAGE_H)


typedef struct{
    HANDLE hDvs;
	UNSL32 dwUserData;
}CbMessFuncInfo;

class DVRSocket  
{
public:
	DVRSocket();
	virtual ~DVRSocket();

	UNS32 RequestOperation(NET_PACKET_TYPE PacketType, LPVOID lpPacketBuf, SOCKET s = INVALID_SOCKET, S32 nVer = 1);
	UNS32 SetConfig(LPVOID lpConfig, S32 size, S32 nVer = 1);//远程设置接口，实际发送的消息类型为SETUP_CHANGED(17)
	UNS32 DeleteCMDSocket();
	UNS32 SendPlayStatusChange(SOCKET s);//S:PLAY_STATUS_CHANGE   R:RIGHT_IS_INVALID
	UNS32 SendPTZControl();//S:PTZ_CONTROL
	UNS32 SendCommon();//S:COMMON
	UNS32 SendRecordUpdate();//S:RECORD_UPDATE
	UNS32 SendCamNameChanged();//S:CAM_NAME_CHANGED
	UNS32 SendWantSystemConfig(S32 nVer = 1);	//S:WANT_SYS_CONFIG		R:SYS_CONFIG || RIGHT_IS_INVALID
	UNS32 SendWantDBDate();//S:WANT_DB_DATE		R:SYS_DB_FILE_DATE
	UNS32 SendWantDBDayFile();//S:WANT_DB_DAY_FILE	R:SYS_DB_DAY_FILE
	UNS32 SendWantLogFile();//S:WANT_LOG_FILE	R:SYS_LOG_FILE || RIGHT_IS_INVALID
	UNS32 SendWantEventFile();//S:WANT_EVENT_FILE	R:SYS_EVENT_FILE || RIGHT_IS_INVALID
	UNS32 SendWantSystemStatus();	//S:WANT_SYS_STATUS		R:SYS_STATUS || RIGHT_IS_INVALID
	UNS32 SendWantPlayFileInfo(SOCKET s);//S:WANT_PLAY_FILE_INFO	R:PLAY_FILE_INFO
	UNS32 SendPlayFileName(SOCKET s);//S:PLAY_FILE_NAME
	UNS32 SendClientTCPClose(SOCKET s);//S:CLIENT_TCP_CLOSE
	UNS32 SendHeartBeat();//S:HEART_BEAT(MAX_NET_HEART_BEAT_TIME)
	UNS32 SendAlarmIn();//S:ALARM_IN
    UNS32 SendAlarmOut();
	UNS32 SendWantLogNum();	//S:LOG_NUM		R:LOG_NUM || RIGHT_IS_INVALID
	UNS32 SendWantEventNum();//S:EVENT_NUM	R:EVENT_NUM || RIGHT_IS_INVALID
	UNS32 SendWantBackupSize();//S:NET_FILE_BACKUP_SIZE	R:NET_FILE_BACKUP_SIZE  ||  RIGHT_IS_INVALID  || 数据流
	UNS32 SendWantPlayFileIndex();//S_WANT_PLAY_FILE_INDEX
	UNS32 SendWantPlayFilePESPacket();//S_WANT_PLAY_FILE_PES_PACKET
    UNS32 SendWantViewInfo();//S_WANT_VIEW_INFO
	UNS32 SendWantFileRightChannel();
	UNS32 SendWantUserRight();//获取用户权限
	UNS32 SendWantLinkNum();//获取网络资源连接数
	UNS32 RequestEmailTesting();//客户端请求DVR发送测试邮件

	//下载相关
	bool CreateDownTCPSocket();
	bool DeleteDownTCPSocket();
	UNS32 SendStartDownload(SOCKET s, LPVOID lpvoid, S32 size);

	UNS32 Net_GetMsgLength(NET_TRANS_PACKET *pMsg, S32 nVer = 1);
	S32 recvn(SOCKET fd, const void *vptr, S32 n, S32 flag, S32 timeout = MAX_SELECT_TIMEOUT);
	S32 sendn(SOCKET fd, const void *vptr, S32 n, S32 flag);
	bool  Net_GetTcpPacket(NET_TRANS_PACKET *pPacket, SOCKET hSocket, S32 timeout = 3);

	//设置回调函数信息
	void StartDVRMessCallBack(fMessCallBack cbMessFunc, HANDLE hDvs, UNS32 dwUser);
	void StopDVRMessCallBack();
	void DealAlarmInfo();
	UNS32 CreateNetThread();
	bool RecvNetTransPacket(NET_TRANS_PACKET *pNetPacket);
	S32  GetLastRecvMsgType();                  //返回最近一次接收到的消息类型,返回-1表示没有接收到过消息
	S32  GetOverTime();                         //获取网络的超时时间
	
	MyThread m_hHeartBeat;
	MyThread m_hRecvMsg;
	MyEvent	m_hHBEvent;//heart beat event
	MyEvent	m_hWaitViewInfo;

	bool	m_bExit;
	bool	m_bIsTreated;    //该消息内容是否被处理
	bool	m_bGetViewInfo;
	bool	m_bOnLine;
	bool	m_bChnnlNameChanged;

	S32		m_nRecvMsgType;   //定义接收消息类型
	S32		m_nWaitTime;      //定义接收超时时间
	S32		m_nIDCode;
	UNS32	m_wCmdPort;
	UNS32	m_IPAddr;

	NET_USER_ACCESS m_AccessCode;
	DVS_DEV_INFO m_LastViewInfo;
	CMyCriticalSection m_DVRSocketSection;
	void *m_pRemotePlayer;
	fMessCallBack m_pcbMessFunc;
	CbMessFuncInfo m_struCbMessFuncInfo;
	SOCKET m_CmdSocket;
	SOCKET m_PlayTCPSocket;
	SOCKET m_DownTcpSocket;

	S8 ReceiveBuffer[sizeof(NET_TRANS_PACKET)];
	S8 SendBuffer[sizeof(NET_TRANS_PACKET)];
};

#endif

