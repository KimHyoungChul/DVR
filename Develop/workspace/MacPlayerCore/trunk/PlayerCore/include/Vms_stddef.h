/***************************************************************************
                          rm_stddef.h  -  description
                             -------------------
    begin                : 四  5月 19 2005
    copyright            : (C) 2005 by xwjiao
    email                : xwjiao@sz-streaming.com
 ***************************************************************************/


#ifndef _RM_STDDEF_H_
#define _RM_STDDEF_H_

#include "Vms_SeesionList.h"
#include "../include/cms_dev_interface/CMSDevInterface.h"


/* trans_mode type */
#define TRANS_I_FRAME							0x00
#define TRANS_VIDEO_FRAME						0x01
#define TRANS_I_AUDIO_FRAME					0x02
#define TRANS_ALL_FRAME						0x03

#define TRANS_MAINSTREAM						0x01
#define TRANS_SUBSTREAM						0x00

/*multimedia cmd*/

#define CMD_START_REALSTREAM_REQ				0x0001
#define CMD_STOP_REALSTREAM_REQ				0x0002
//PC->DVR
#define CMD_START_DOWNLOAD_REQ				0x0001
#define CMD_START_DOWNINDEX_REQ				0x0002
#define CMD_STOP_DOWNLOAD_REQ				0x0003

//DVR->PC
#define CMD_START_VOD_REQ						0x0107
#define CMD_STOP_VOD_REQ						0x0108
#define CMD_START_BEGIN_REQ					0x0109
#define CMD_STOP_BEGIN_REQ					0x010A
#define CMD_SET_OFFSET_REQ					0x010E

/*jwj add*/
//media autodownload cmd
#define	CMD_CFG_VER_REQ						0x010F		// 请求配置文件版本dvr->pc
#define	CMD_CFG_GETCFGLIST_REQ				0x0110          //请求播放配置文件列表pc->dvr
#define	CMD_CFG_DOWNLIST_REQ				0x0111		// 发送下载文件列表内容dvr->pc
#define   CMD_GET_MEDIALENGTH					0x0112          //得到文件的长度DVR->PC
#define   CMD_SLEEP								0x0113          //媒体相关文件全部下载完毕，开始睡眠

/*jwj add*/
//auto upgrade
#define   CMD_UPGRADE_GETVERSION				0x0114         //服务器向v6获取升级程序的版本号码
#define   CMD_UPGADE_START						0x0115         //开始升级

/*jwj add*/
//auto delete media file
#define   CMD_DELETE_MEDIA						0x0116         //开始删除媒体文件

/*jwj add
2006/11/11*/
//auto download netlog file
//MDVR->PC
#define   CMD_GET_NETLOG						0x0117         //get netlog file

/*jwj add
2007/03/23
//media files have benn trans
*/
#define	CMD_MEDIA_OK							0x0118         //广告文件下载完毕


/*jwj add
2007/07/24
start remote format hdd func*/
#define	CMD_START_FORMAT						0x011B        //启动格式化功能

/*jwj add
2007/07/24
get format stat func*/
#define	CMD_GET_FORMAT_STAT					0x011C        //查询格式化状态

/*jwj add
2007/07/24
lock or unlock file on mdvr*/
#define	CMD_LOCKUNLOCK_FILE					0x011D        //查询格式化状态


/**jwj add
2007/05/07*/
#define	CMD_SHUTDOWNMACHINE					0x0001	//remote shutdown machine
#define	CMD_RESETMACHINE						0x0002	//remote reset machine

#define VMS_TEMP_BUFFER_SIZE  	500
#define TEMP_FILE_LIST_SIZE 64*1024
#define CMD_PART_LENTH  	500
#define VMS_BUFFER_SZE    64*1024
#define UPGRADE_FILEHEADER_LEN		1024

#define MAX_MEDIA_FILE_LEN  256
#define INVALID_SESS_ID (0xffffffff)

#define   THREAD_STACK_SIZE  8*1024
#define  per_sector 32*1024

#define MAX_THREAD_NUM 20//定义设备中最大有多少个VMS网络协议的线程　
#define MAX_BUFFER_CH_NUM 4//定义每个THREAD中有多少个通道数据
#define MAX_CH_SESSION_NUM 3//定义每个通道同时最大的连接数目

#define MAX_SESSION_COUNT        50

//定义线程池中功能使用的线程号
#define FILE_USER_WORKER_ID 		8
#define UPDATE_WORKER_ID              	9
#define LOG_WORKER_ID                 		10
#define ALARM_USER_WORKER_ID 	11
/*升级是独立的线程，不受管理*/

#define MAX_VMS_DOWNLOADFILE 8

#define MAKE_SESSION_ID( worker_id, trans_id,sessid )  ( ( worker_id ) << 24 |(trans_id)<< 16 | ( sessid ) )
#define GET_WORKER_ID( sessid )  ( (( sessid) >>24 )&0xff )
#define GET_TRANS_ID( sessid )   ( (( sessid) >>16 )&0xff )
#define GET_MESS_ID( sessid )  ( ( sessid)&0x000000ff )

typedef unsigned int  sess_id_t;
typedef unsigned int  req_id_t;
typedef int stream_id;


//一个帧缓存区
typedef struct tag_frame_buffer_work
{
	SeesionList   				 ch_seesio_list[MAX_BUFFER_CH_NUM];
	pthread_t            			  worker_thr;
   	pthread_mutex_t 			  mutex;
	int	               			  media_file;
	char                                      filename[128];
	int                                        Filelenth;//文件长度
	char                            	        workerInit;
	char                                     buffer_id;
	char					        bThreadRun;
	int                                        count;
	int                                        sock;
	StreamType			stream_type;				//子码流还是主码流
//	int						send_fileheader[MAX_BUFFER_CH_NUM];		//	1:need send fileheader
	int                                    	has_newsession[MAX_BUFFER_CH_NUM];		// 1:new session
	int                                    	MainJumpToIFrame[MAX_BUFFER_CH_NUM];
	int                                    	SubJumpToIFrame[MAX_BUFFER_CH_NUM];
	int                                    	has_requestiframe[MAX_BUFFER_CH_NUM];		// 1:new session

}frame_buffer_work;

typedef struct  tag_Net_Protocol_Header
{
	unsigned long  	ProtoVersion;        	/* 版本*/
	unsigned long  	MsgType;           	/* 消息类型*/
	unsigned long   	TransactionNo;      /* 序列编号*/
        unsigned long   	SessionNo;         /* 连接任务编号*/
	unsigned long    	PacketLen;           /*包长度*/
	/* 包括header和后面data_info在内的packet尺寸*/
}Net_Protocol_Header;

typedef struct tag_VMSRES_MSG_HEAD
{
 	int     packagelen;
	int     errorcode;

}vms_resp_head;

typedef enum service_type
{
    SERVICE_REALTIME,  // 实时
    SERVICE_RELIABLE   // 可靠
} service_type_t;

typedef enum stream_type
{
   STREAM_FILE,
   STREAM_PREVIEW,
   STREAM_DEV_PLAY
} stream_type_t;

typedef enum
{
   channel_ok=0,
   channel_invalid,
   session_invalid,
   thread_get_failed,
   login_failed,
   not_encoder,
   start_failed,
   not_support_mainstream,
   not_support_substream,
   normal_user,
   manager,
   superuser,
} Preview_Response_RetCode;

typedef struct tag_VMSRES_MSG
{
 	int            req_id;
       sess_id_t  sid;

}PREVIEW_RESPONSE_MSG;


//子码流音频控制信息
typedef struct VMS_Audio_Control
{
	PREVIEW_RESPONSE_MSG		Audio_msg;//取下发实时预览指令时设备回传的值
	int		iFlag;//0表示传输音频，1表示不传输音频，目前值应用于子码流
};


// 下载文件信息
typedef struct
{
	unsigned long 	        CmdID;			// 消息代号
	unsigned long		Length;			// 消息长度
	int 	flag;				/*偏移量标志, 0:全部文件 1：按照大小偏移量下载 2：按照时间偏移量下载*/
	int	start;			/*起始偏移量*/
	int	end;				/*结束偏移量*/
	int    offset; /*文件偏移量，ｐｃ端的，包括文件头和已经下载的部分*/
	char	file_name[128];	/*全路径文件名*/
}file_msg_vms;

//月历结构定义
typedef struct
{
	unsigned char	year;//日志查询年,0~255取值范围，代表2000~2255年
	unsigned char	month;//日志查询月，1~12为取值范围
	unsigned char	day;//日志查询天，现在未用
	unsigned char type;//日志类型,0,录像文件，１，日志文件

}calenda_setting_vms;

//硬盘格式化结构定义
typedef struct
{
	int disktype;//磁盘类型，1->hdd,2->sd
	int number;/*需要格式化的盘符，每个bit代表对应的盘是否需要格式化．如
	0x05　代表第一个和第三个硬盘需要格式化*/

}formathdd_vms;

//录像文件搜索信息
//录像文件搜索参数
typedef struct
{
       int     type;/* 录像类型， 1：常规录像 2：报警录像，3 ：所有录像*/
	int     channel;/* 通道号, 具体搜索哪几个通道。 bit0~bit15分别代表1~16通道*/

	/*录像时间*/
	unsigned char 	start_year;	//数字，代表的年数为2000+start_year，如８，代表2008
	unsigned char 	start_month;//数字，代表1~12
	unsigned char 	start_day;//数字，代表1~31
	unsigned char 	start_hour;//数字，代表0~23
	unsigned char 	start_min;//数字，代表0~59
	unsigned char	start_sec;//数字，代表0~59

	/*结束时间,含义同开始时间*/
	unsigned char	end_hour;
	unsigned char 	end_min;
	unsigned char	end_sec;
	unsigned char reserved[3];
}search_setting_vms;

// 文件结构体
typedef struct
{
	char 			filename[128];			// 文件名
	unsigned int	filesize;				// 文件大小

}file_info_vms;

// 下载文件信息
typedef struct
{
	int	filesize;				/*文件真实大小*/
}file_return_vms;


// 下载文件信息
typedef struct
{
	char				filename[20];
	unsigned int		filesize;				/*文件真实大小*/
}configfile_info_vms;


//设备传回的文件信息
typedef struct
{
       int    filesize;              //裁减生成的文件大小，包括文件头
	int	start_offset;		// 起始偏移量
	int 	end_offset;		// 结束偏移量
}File_download_info;

typedef struct tag_CHSIGNALSTATE {
	int ch_id;
	int State;  //0xx(31-24)xx(23-16)xx(15-8)xx
	/*31-24:返回设备的子类型
	    23-16:返回设备的主类型
	    15-0:返回设备的通道有效性*/
}ChSignalState_vms;

//开始取实时流或停止
//Net_Protocol_Header + AVMessageHead
typedef struct tag_AVMessageHead{
	unsigned long 	        CmdID;			// 消息代号
	unsigned long		Length;			// 消息长度
	unsigned long		channel;			// 通道号
	unsigned long		trans_mode;		//传输模式
	unsigned long        stream_select;    //主码流还是子码流
	char                     usrname[vs_usrname_len];       //登录主机用户名
	char                     password[vs_pass_len];      //登录主机密码
}AVMessageHead_vms;


typedef struct tag_StartlogHead{
	unsigned char         type;/*下载日志类型，暂时保留，现在没有用*/
	unsigned char		search_year;		//数字，代表的年数为2000+start_year，如８，代表2008
	unsigned char		search_month;	//数字，代表1~12
	unsigned char		search_day;		//数字，代表1~31
}Startlog_vms;

typedef struct {
	char                     usrname[vs_usrname_len];       //登录主机用户名
	char                     password[vs_pass_len];      //登录主机密码
}USRPSW_vms;

#define INVALID_STREAM_ID           (-1)


/*jwj add
01/19/2006
this structure is used for statement of current transporting frame.
        int channel;0-3   which channel is used now
	int type;     0:I Frame;   1:P Frame; 2:Audio Frame;other;
	int frame_length;total length of current frame including of frame head and data
	int bytes_totrans;how many bytes will be left to transport on current frame
*/

typedef struct  tag_CurFrameInfo
{
        int channel;//channel number between one buffer
	int channelid;//real channel id from 0-17
	frame_type_t type;
	unsigned int frame_length;
	int has_frame;//if 1,has frame,else no
	char *buffer;//data point
}CurFrameInfo,*pCurFrameInfo;

/*2006/4/20
jwj add for traffic control
*/

typedef enum tag_limit_type
{
   NO_LIMIT,
   SPEED_384K,
   SPEED_512K,
   SPEED_1M,
   SPEED_2M,
   SPEED_OTHER
}speed_type_t;

/*云台*/
typedef struct
{
	unsigned long	channel;		//通道号
	unsigned long	cmd;  			//云台控制命令
	unsigned long	para;			//当调用或者设置预置点时，为预预置点号
	unsigned char	reserved[20];
} PTZAction;

#if 0
#define PTZCONTROL_UP					0x09//	0x01	//镜头旋转上
#define PTZCONTROL_DOWN					0x0a//0x02	//镜头旋转下
#define PTZCONTROL_LEFT					0x0b//0x03	//镜头旋转左
#define PTZCONTROL_RIGHT				0x0c//0x04	//镜头旋转右
#define PTZCONTROL_IRIS_OPEN				0x0d	//光圈 大
#define PTZCONTROL_IRIS_CLOSE			    0x0e	//光圈 小
#define PTZCONTROL_ZOOM_IN				0x06	//	调焦 缩
#define PTZCONTROL_ZOOM_OUT				0x05	//	调焦 伸
#define PTZCONTROL_FOCUS_NEAR			0x07	//聚焦 进
#define PTZCONTROL_FOCUS_FAR			0x08	//聚焦 远
#define PTZCONTROL_AUTOPAN				0x10	//自动
#define PTZCONTROL_BRUSH_ON				0x0c
#define PTZCONTROL_BRUSH_OFF			0x0d
#define PTZCONTROL_PRESET					0x0e	//预置点
#define PTZCONTROL_RECALL					0x0f
#define PTZCONTROL_CRUISE					0x10
#define PTZCONTROL_CLEARPRESET			0x11
#define PTZCONTROL_SPEED					0x12
#define PTZCONTROL_EX_ZOOM_IN			0x13
#define PTZCONTROL_STOP						0x0 //LiuHB test 0x14
#define PTZCONTROL_CLEARALLPRESET		0x15
#define PTZCONTROL_SETGROUP				0x16
#define PTZCONTROL_PATTERNSTART		0x17
#define PTZCONTROL_PATTERNSTOP		0x18
#define PTZCONTROL_RUNPATTERN			0x19
#endif
#define PTZCONTROL_UP						0x01	//镜头旋转上
#define PTZCONTROL_DOWN					0x02	//镜头旋转下
#define PTZCONTROL_LEFT						0x03	//镜头旋转左
#define PTZCONTROL_RIGHT					0x04	//镜头旋转右
#define PTZCONTROL_IRIS_OPEN				0x05	//光圈 大
#define PTZCONTROL_IRIS_CLOSE			0x06	//光圈 小
#define PTZCONTROL_ZOOM_IN				0x07	//	调焦 缩
#define PTZCONTROL_ZOOM_OUT				0x08	//	调焦 伸
#define PTZCONTROL_FOCUS_NEAR			0x09	//聚焦 进
#define PTZCONTROL_FOCUS_FAR			0x0a	//聚焦 远
#define PTZCONTROL_AUTOPAN				0x0b	//自动
#define PTZCONTROL_BRUSH_ON				0x0c
#define PTZCONTROL_BRUSH_OFF			0x0d
#define PTZCONTROL_PRESET					0x0e	//预置点
#define PTZCONTROL_RECALL					0x0f
#define PTZCONTROL_CRUISE					0x10
#define PTZCONTROL_CLEARPRESET			0x11	
#define PTZCONTROL_SPEED					0x12
#define PTZCONTROL_EX_ZOOM_IN			0x13
#define PTZCONTROL_STOP						0x14
#define PTZCONTROL_CLEARALLPRESET		0x15
#define PTZCONTROL_SETGROUP				0x16
#define PTZCONTROL_PATTERNSTART		0x17
#define PTZCONTROL_PATTERNSTOP		0x18
#define PTZCONTROL_RUNPATTERN			0x19





#ifndef RM_UINT32
#define RM_UINT32  unsigned int
#endif

#ifndef  RM_UINT16
#define RM_UINT16  unsigned short
#endif

#ifndef RM_UINT8
#define RM_UINT8  unsigned char
#endif

#ifndef RM_UCHAR
#define RM_UCHAR   unsigned char
#endif

#ifndef RM_CHAR
#define RM_CHAR   char
#endif

#ifndef RM_SINT32
#define RM_SINT32   int
#endif

#ifndef  RM_SINT16
#define RM_SINT16   short
#endif

#ifndef RM_SINT8
#define RM_SINT8   char
#endif

typedef  void * Rm_Handle_t;

#define  EXTERN  extern

//@{




//#define BOOL        RM_SINT32
#define RM_BOOL     RM_SINT32
#define RM_FALSE    0
#define RM_TRUE     (!RM_FALSE)

//#define TRACE_FUNC(s)  printf(#s"\r\n");

#define RM_ASSERT(s)  assert(#s);

#define RM_RESULT  RM_SINT32


#ifndef __cplusplus

#define		XINLINE inline

#endif // __cplusplus



#endif /*the end of file*/
