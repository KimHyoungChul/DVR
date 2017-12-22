#ifndef PTHREAD_NETSYS_H
#define PTHREAD_NETSYS_H

/********************************************************
说明:
	本文件是网络系统管理模块的头文件

版本修改澹?	Version	 	Name 		Date
	V0.0.1		wangjm		2006.09.21
********************************************************/
/********************** 包含头文件 *******************************/
//#include <netinet/in.h>

#include "DvssDefine.h"
#include "NetDvrApi.h"


typedef struct{
	UNS32	H;
	UNS32	L;
}std64_net_t;

typedef struct{
	UNS32	ItemNum;	//本次发送的日期总数量
	UNS32 	Date[MAX_DB_DATE_NUM];
}NET_SYS_DB_DATE_INFO;
/*************** 发送系统数据库某一天的文件列表 Buffer结构 **********/
typedef struct{
	UNS32	Date;	//本次发送的日期
	UNS32	TotalNum;//文件的数量
	DB_BRIEF_FILE 	File[MAX_NET_DAY_FILE_NUM];
}NET_SYS_DB_DAY_FILE;

/*************** 系统日志和事件文件  Buffer结构******************/
typedef struct{
	UNS32	StartItem;	//传输的开始条目
	UNS32	EndItem;	//传输的结束条目
	LogFile 	Item[MAX_NET_LOG_ITEM];
}NET_SYS_LOG_INFO;
/*************** 回放状态改变  Buffer结构******************/

typedef struct
{
	MODE_PLAY PlayStatus;
	UNS32 SecondData;
}NET_PLAY_STATUS_CHANGE;

/*************** 云台控制  Buffer结构******************/
typedef struct
{
	UNS32				Channel;	//通道号
	DVS_PTZ_CTRL_MODE	ControlMode;//方向调整之类的 调用组类型 存储组类型
	UNS32				ControlInfo;//调用或者存储的组号
}NET_PTZ_CTL_INFO;

/*************** 网络录像文件备份  Buffer结构******************/
typedef struct
{
	UNS32	VideoChannel;				//需要备份的通道,单比特访问,LSB=Channel 1 1=需要备份
	UNS32 	AudioChannel;                        //按bit
	UNS32	FileStartTime;				//备份开始的时间(分钟)
	UNS32	FileEndTime;				//备份结束的时间(分钟)
	UNS8	FileFormat;				//备份文件的格式(BACKUP_FORMAT_EXE or BACKUP_FORMAT_PS)
	UNS8	reserved1;
	UNS16	reserved2;
	UNS32	FileSize_L;				//要求大小 C->S的时候,填写 0;要求备份开始的时候,填写1
	UNS32	FileSize_H;
	//S->C的时候，填写，单位KByte
}NET_FILE_BACKUP_SIZE;

typedef struct
{
	UNS32	Flag;  //bit[7..3]:通道号  0--31 -> 1--32
					//bit[2..1]:帧类型 0=I帧 1=P帧 2=reserve 3=音频
					//bit[0]:结束标志 1=备份的最后一帧 
	UNS32	TotalSize;	//本结构体的大小
	UNS8	PESPacket[MAX_BACKUP_PES_BUFFER_SIZE]; //PES包内容
}NET_FILE_BACKUP_PES;

typedef union
{
	PS_FILE_HDR  PSHdr;  //PS文件头信息
	UNS8	ExeHdr[MAX_FILE_HDR_SIZE_EXE]; //exe可执行文件头信息
	NET_FILE_BACKUP_PES PESPakcet; //要保存的PES包信息
}NET_FILE_BACKUP;

/*************** 用户多媒体接入认证  Buffer结构******************/
typedef enum
{
	MAIN_STREAM,//主码流
	SUB_STREAM,//子码流
	AUDIO_STREAM//音频流
}MEDIA_STREAM_TYPE;

typedef struct
{
	UNS32	AccessCode;	//用户随机数
	UNS32	UserID;		//用户组序号
	MEDIA_STREAM_TYPE StreamType;
}NET_MEDIA_ACCESS;	//用户通过网络监控或者网络回放时候的消息buffer结构

/*************** 用户接入认证  Buffer结构******************/
typedef struct{
	CLIENTACTION	AccessType;	//连接类型
	UNS8	UserName[MAX_USER_NAME_LEN]; //用户名
	UNS8	PassWord[MAX_PASSWORD_LEN];	//密码
} NET_REMOTE_ACCESS;
/*
Client发送日志(系统日志和事件日志)请求时的消息buffer内容
*/
typedef struct{
	UNS32 StartItem;
	UNS32 EndItem;
}NET_WANT_LOG;
/*
 Client关闭与 Server的TCP连接请求
*/
typedef struct{
	UNS32	AccessCode;	//随机数校验
}NET_TCP_CLOSE;

#define MAX_INDEX_NUM_PER_BLOCK	(30 * 1024)
typedef struct
{
	UNS32 StartItemNum;
	UNS32 EndItemNum;
	PS_INDEX_RECORD Index[MAX_INDEX_NUM_PER_BLOCK];
}NET_FILE_INDEX;

typedef struct{
	SETUP_CMD_ID		SetupID;
	DVRSETUP_UNION		Setup;
}DVR_CONFIG_VER1;

typedef struct{
	DVS_SETUP			SetupID;
	UNS32				StructLen;//新增结构体长度，表示DVRSETUP_UNION的长度
	DVRSETUP_UNION		Setup;
}DVR_CONFIG_VER2;


/************************************** 普通消息 类型 ******************************/
typedef union
{
	DVR_CONFIG_VER1 SetupVer1;
	DVR_CONFIG_VER2 SetupVer2;
}NET_SYS_CONFIG_CHANGE;

typedef struct{
	UNS32	AccessCode;	//用户随机数
	UNS32	UserID;		//用户组序号
}NET_BACKUP_ACCESS;	//用户通过网络监控或者网络回放时候的消息buffer结构
/*
 网络监控或者网络回放时，Client端发送的连接信息
 Server发送给Client的验证通过信息
*/
typedef struct{
	UNS32	AccessCode;	//用户随机数
	UNS32	DeviceType; //系统型号
	UNS32	SysVersion;	//系统版本号
}NET_USER_ACCESS;
/*
 Server验证Client的时候，如果该用户名已经登录了，就发送该消息
*/
typedef struct{
	UNS32	IPAddr;	//IP地址
}NET_USER_HAS_ACCESSED;

typedef struct
{
	UNS32	PESPacketLength;//该pes包的长度
	UNS8	PESPacket[MAX_PES_PACKET_SIZE+24]; //PES包内容
}NET_FILE_PES_PACKET;

#pragma pack (1) //kk 07-18
typedef struct 
{
	UNS32 SetupID; //SETUP_CMD_ID
}NET_WANT_SYS_CONFIG_ITEM;
#pragma pack () //kk 07-18

/*************** 网络系统服务接口，类型 ******************/
//主要是服务端回复客户端请求的消息
typedef enum 
{//服务端回复消息，RecvNetTransPacket参数类型
	USER_ACCESS = 1,				//1->登录时客户端发送认证结构NET_REMOTE_ACCESS
	CLIENT_IS_FULL,					//2->认证回复消息，认证时等待返回，在DVS_Login中检测
	ACCESS_ACCEPT,					//3->USER_ACCESS的回复消息，认证成功，在DVS_Login中检测
	USER_PASSWORD_ERROR,			//4->USER_ACCESS的回复消息，密码错误，在DVS_Login中检测
	USER_NAME_INVALID,				//5->USER_ACCESS的回复消息，用户名无效，在DVS_Login中检测
	USER_HAS_ACCESSED,				//6->USER_ACCESS的回复消息，该用户已经存在，在DVS_Login中检测
	MIDEA_ACCESS,					//7->音视频请求时客户端发送认证结构NET_MEDIA_ACCESS
	MIDEA_ACCESS_ACCEPT,			//8->服务端回复的NET_MEDIA_ACCESS认证成功
	MIDEA_ACCESS_REFUSED,			//9->服务端回复的NET_MEDIA_ACCESS认证拒绝
	BACKUP_ACCESS,					//10->文件备份请求时客户端发送认证结构NET_BACKUP_ACCESS
	BACKUP_ACCESS_ACCEPT,			//11->BACKUP_ACCESS的回复消息，请求成功
	BACKUP_ACCESS_REFUSED,			//12->BACKUP_ACCESS的回复消息，下载被拒绝
	RIGHT_IS_INVALID,				//13->BACKUP_ACCESS的回复消息，没有权限
	VIEW_INFO,						//14->S_WANT_VIEW_INFO的消息类型，发送接收消息相同
	SYS_STATUS,						//15->未用到，获取系统状态S_WANT_SYS_STATUS，发送接收消息相同
	SYS_CONFIG,						//16->S_WANT_SYS_CONFIG的消息类型，用于GetConfig，发送和接收相同
	SETUP_CHANGED,					//17->SetConfig的消息类型
	CAM_NAME_CHANGED,				//18->S_CAM_NAME_CHANGED
	PTZ_CONTROL,					//19->S_PTZ_CONTROL
	SYS_DB_FILE_DATE,				//20->S_WANT_DB_DATE
	SYS_DB_DAY_FILE,				//21->S_WANT_DB_DAY_FILE
	PLAY_FILE_NAME,					//22->S_PLAY_FILE_NAME
	PLAY_FILE_PES_PACKET,			//23->S_WANT_PLAY_FILE_PES_PACKET
	PLAY_FILE_INDEX,				//24->S_WANT_PLAY_FILE_INDEX
	PLAY_FILE_INFO,					//25->S_WANT_PLAY_FILE_INFO
	PLAY_FILE_OFFSET,				//26->未用到，获取文件offset位置回复消息
	PLAY_STATUS_CHANGE,				//27->S_PLAY_STATUS_CHANGE
	PLAY_FILE_IS_END,				//28->接收消息
	BACKUP_SIZE,					//29->S_BACKUP_SIZE
	FILE_OPEN_SUCCESS,				//30->PLAY_FILE_NAME的回复消息，未用到
	FILE_OPEN_FAILED,				//31->PLAY_FILE_NAME的回复消息
	BACKUP_RIGHT_CHECK,				//32->S_WANT_FILE_RIGHT_CHANNEL
	FILE_DOWNLOAD_START,			//33->开始下载
	FILE_DOWNLOAD_STOP,				//34->结束下载，没有用到
	SYS_LOG_FILE,					//35->未用到
	SYS_EVENT_FILE,					//36->未用到
	LOG_NUM,						//37->未用到
	EVENT_NUM,						//38->未用到
	NOTICE,							//39->未用到，原来叫COMMON //屏幕上显示的消息
	CLIENT_TCP_CLOSE,				//40->客户端端口断开	----没有buffer内容 -----
	HEART_BEAT,						//41->心跳消息			----没有buffer内容 -----
	ALARM_IN,						//42->报警输入状态变化，接收DVR端对讲消息
	ALARM_OUT,						//43->报警输出状态变化
	RECORD_UPDATE,					//44->未用到，S_RECORD_UPDATE
	RESOLUTION_UPDATE,				//45->未用到
	HDD_FREE_SIZE_UPDATE,           //46->未用到，硬盘剩余容量发生变化
	TEMPERATURE_UPDATE,   			//47->未用到，温度发生变化
	RECORD_FILES_PERIOD_UPDATE,   	//48->未用到，录像文件的时间范围
	GET_USER_RIGHT,					//49->获取用户权限
	GET_LINK_NUM,					//50->获取网络资源连接数
	REQUEST_EMAIL_TESTING,			//51->客户端请求DVR发送测试邮件
    REQUEST_REBOOT,                 //52->客户端请求重启DVR设备
    REQUEST_SHUTDOWN                //53->客户端请求关闭DVR设备
}NET_TRANS_TYPE;

typedef union
{
	NET_SYS_DB_DATE_INFO	DBFileDate;//请求时不发送该结构体，接收时解析
	NET_SYS_DB_DAY_FILE		DBDayFile;//请求时不发送该结构体，接收时解析
	NET_PLAY_STATUS_CHANGE	PlayStatus;
	PS_FILE_HDR				PlayFileInfo;//请求时不发送该结构体，接收时解析
	NET_PTZ_CTL_INFO		PTZControl;
	NET_BACKUP_ACCESS		BackupAccess;
	DVS_DEV_INFO			NetViewInfo;
	USER_NETRIGHT			NetUserRight;//请求时不发送该结构体，接收时解析
	NET_LINK_INFO			LinkNum;//请求时不发送该结构体，接收时解析

	UNS8	NetCommonMsg[MAX_NET_COMMON_MSG_LEN];//普通网络传输的字符串消息
	NET_SYS_CONFIG_CHANGE		SetupItem;
	NET_WANT_SYS_CONFIG_ITEM	SysconfigItem;
	NET_CAMNAME_CHANGE			CamName;
	UNS32	Day;	//需要哪一天的播放文件名称列表
	UNS8	PlayFileName[MAX_FILE_NAME_LEN];//回放文件名字
	NET_FILE_BACKUP_SIZE	WantFileSize;//C->S 要求计算文件大小,文件大小为空，其余填满；//S->C 回复备份文件大小，文件大小填上，其余为来的时候的数值。										
	NET_WANT_LOG			LogBuffer; //要求日志文件的时候的buffer
	NET_TCP_CLOSE			TcpClose;	//关闭TCP连接的请求
	NET_ALARM_IN			AlarmIn;	//报警输入
	NET_ALARM_OUT			AlarmOut;
	NET_FILE_INDEX			NetFileIndex;
	std64_net_t				offset;//2222
	NET_FILE_PES_PACKET		Net_File_PES_Packet;
}NET_MSG_BUFFER;

/*************** 网络系统服务接口，网络传输包结构 ******************/
typedef struct{
	UNS8	StartCode[MAX_NET_STARTCODE_LEN];
	UNS32	TotalSize;	//该数据包的大小 单位：Byte
	UNS32	IDCode;	//网络系统服务区别用户的标识，也就是网络的组号
	NET_TRANS_TYPE	InfoType;
	NET_MSG_BUFFER	Buffer;
	UNS8	EndCode[MAX_NET_ENDCODE_LEN];
}NET_TRANS_PACKET;


#endif