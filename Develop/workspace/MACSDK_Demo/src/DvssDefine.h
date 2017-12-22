#ifndef DEFINE_H
#define DEFINE_H

#include <time.h>
/************************* 数据类型预定义***************************/
typedef unsigned char			UNS8;	/* 8-bit */
typedef unsigned short			UNS16;	/* 16-bit */
typedef unsigned int			UNS32;	/* 32-bit */ 
//typedef unsigned __int64		UNS64;	/* 64-bit */
typedef unsigned long			UNS64;	/* 64-bit *//*wuzy-12.4.16*/
typedef char				S8;	/* 8-bit */
typedef short				S16;	/* 16-bit */
typedef int				S32;	/* 32-bit */ 
//typedef __int64			S64;	/* 64-bit */
typedef long				S64;	/* 64-bit *//*wuzy-12.4.16*/

/************************* 常数 *********************************/
#define	MAX_MIN_IN_HOUR		60	//一小时最多60分钟
#define MAX_SOLO_CHANNEL_16	16

typedef enum {
	DT_SYSTEM = 1,
	DT_VIDEO,
	DT_AUDIO
}DATA_TYPE;

typedef enum
{
    I_PICTURE,
    P_PICTURE,
    A_PICTURE,	//音频帧
    B_PICTURE
}PICTURE_TYPE;

#define	RESOLUTION_D1	0
#define	RESOLUTION_CIF	1
#define RESOLUTION_HD1  2

/************************* SETUP模块 *********************************/
#define MAX_USER_NAME_LEN		8	//系统用户名最大长度
#define MAX_PASSWORD_LEN		8	//系统密码最大长度
#define MAX_HARDDISK_NUM		4	//系统最大硬盘数量
#define MAX_SYSTEM_NAME_LEN		16	//系统名称最长字数 
#define MAX_PTZ_NAME_LEN		16	//云台的名字长度

typedef enum{
	REC_OFF = 0,   // 关闭
	REC_MANUAL,	// 手动录像
	REC_SCHEDULE,	// 定时录像
	REC_MOTION_SENSOR,   //事件 : 动态侦测 + 报警录像
	REC_SCHEDULE_EVENT, // 定时+ 事件
	REC_SENSOR,	// 报警录像
	REC_MOTION,		// 动态侦测录像
	REC_ALL  //全部类型，仅搜索录像文件时使用
}RECORD_MODE;

typedef struct{
	time_t StartTime;	//开始时间
	time_t EndTime;		//结束时间
}SYS_FILE;

/************************* 磁盘管理模块 ************************************/
#define MAX_FILE_NAME_LEN		64	//文件名最大长度	包含绝对路径
#define MAX_DB_DATE_NUM			4096//文件数据库中，最多存放的日期数量

typedef enum{
	DS_NO =1,	//该硬盘不存在
	DS_ERROR,	//硬盘出现错误
	DS_ENOUGH,	//硬盘容量没有满，可用
	DS_FULL		//硬盘已满
}DISK_STATUS;

typedef struct{
	UNS8	filename[MAX_FILE_NAME_LEN];
	time_t	start_time;
	time_t	end_time;
	UNS32	conti;
	UNS32	manual;
	UNS32	schedule;
	UNS32	motion;
	UNS32	sensor;
	UNS32	schedule_event;
	UNS32	file_size;
}DB_BRIEF_FILE;

typedef struct{
	SYS_FILE File; //录像文件的属性
}DB_ATTRIBUTE;

/************************ 报警模块 ***********************************/
typedef enum{
	ALARM_WARNING = 1,	//外部输入报警
	ALARM_MOTION,		//动态侦测报警
	ALARM_DISKERROR,	//磁盘错误
	ALARM_VIDEOLOSS,	//视频丢失
	ALARM_PSERROR,		//密码错误
	ALARM_VIDEOCOVER,	//视频遮挡
	INTERCOM_REQUEST = 0xffff,//DVR端请求对讲
	INTERCOM_STOP = 0x5555,//DVR端停止对讲
}ALARM_TYPE;

typedef enum{
	SAMPLE_RATE_8KHZ = 0,
    SAMPLE_RATE_11025HZ,
	SAMPLE_RATE_16KHZ,
    SAMPLE_RATE_22050HZ,
	SAMPLE_RATE_32KHZ
}AUDIO_SAMPLE_RATE;

typedef enum{
	WORD_WIDTH_8BIT = 0,
	WORD_WIDTH_16BIT
}AUDIO_WORD_WIDTH;

/************************ 网络模块 ***********************************/
#define MAX_NET_COMMON_MSG_LEN	1024//网路发送给Server的最大屏幕显示消息 单位:Byte
#define MAX_NET_STARTCODE_LEN	4	//网络传送的开始码长度	单位：字节 RUI
#define MAX_NET_ENDCODE_LEN		4	//网络传送的结束码长度	单位：字节 XUN
#define NET_TRANS_START_CODE	"KTSY"
#define NET_TRANS_END_CODE		"ktsy"
#define MAX_NET_LOG_ITEM		20	//网络传输日志文件条目的最大数量

typedef enum{
	NET_CLIENT_CONFIG = 1,	//网络配置
	NET_CLIENT_VIEW_TCP,	//网络以TCP方式监控
	NET_CLIENT_VIEW_UDP,	//网络以UDP方式监控
	NET_CLIENT_PLAY			//网络回放
}CLIENTACTION;

typedef struct{
	DISK_STATUS	status;
	UNS32		FreeSize;
}HDD_INFO;

typedef struct {
	UNS32 VideoChannel;	//video channel
	UNS32 AudioChannel;	//audio channel
	UNS32 RecMode;	//录像模式
}NET_RECORD_BUFFER; //used for network broadcast

typedef struct{
	ALARM_TYPE	AlarmType;//报警类型
	UNS32		Channel;//warning,motion,video loss,直接给出通道号，disk err直接给出磁盘号，密码错误该项无效
	UNS32		Value;	// 1: 有；0: 无;  disk err 和密码错误时该项无效
}NET_ALARM_IN;

typedef struct{
     UNS32	Channel;//channel number START FROM CHANNEL_1
     UNS8	CamName[MAX_PTZ_NAME_LEN];//é????1±êê?DT??
}NET_CAMNAME_CHANGE;

typedef struct {
	UNS8	Resolution[MAX_SOLO_CHANNEL_16];
}NET_RESOLUTION_BUFFER;

typedef struct{
	UNS32	Channel;//报警输出通道号0~3
	UNS32   Value;	// 1: set; 0:clear
}NET_ALARM_OUT;

//HDD_FREE_SIZE_UPDATE命令发送结构体
typedef struct{
	HDD_INFO DiskInfo[MAX_HARDDISK_NUM];
}HDD_FREE_SIZE_INFO;

//TEMPERATURE_UPDATE命令发送结构体：
typedef struct{
	UNS32 TemperId; // 目前只有一个测试点,该值设为0
	UNS32 Temperature;
}TEMPERATURE_INFO;

/************************ 系统日志模块 ***********************************/
#pragma pack(1)
typedef struct{
	time_t	FileStartTime;
	UNS8	DiskNum;//磁盘号
}LOG_DISK_FILE_STATUS;
#pragma pack()

typedef struct{
	UNS32	IPAddr; //登录者的IP地址
	UNS8	UserName[MAX_USER_NAME_LEN]; //用户名称
	UNS8	UserNum;	 //登录用户的网络序号，从1开始
}LOG_NET_LOGIN;

typedef struct{
	UNS8	UserName[MAX_USER_NAME_LEN]; //用户名称
	UNS8	Message[MAX_NET_COMMON_MSG_LEN];//消息内容
}LOG_NET_CMN_MSG;

typedef struct{
	UNS8	FileName[MAX_FILE_NAME_LEN]; //文件名称
	UNS8	UserNum;	 //登录用户的网络序号，从1开始
}LOG_NET_DOWNLOAD;

typedef union{
	UNS16	Channel;	//通道号，包括视频丢失ID,联动报警通道号，动态侦测通道号，
						//内部/外部磁盘号
	UNS8	OK;			//操作结果，0=Fail,1=OK,2=TimeOut。
						//包括事件日志文件创建,系统日志文件创建,时间同步
	UNS8	Disk;		//磁盘操作结果 高5bit=磁盘号 低3bit=操作结果 1=OK 0=Fail 
						//包括：内部，外部磁盘检测
	UNS32	Version;	//系统版本信息 高8bit=MagicNum,后面依次为主版本号，次版本号，第三版本号
	UNS8	UserName[MAX_USER_NAME_LEN];//用户名
	UNS8	RecStart;	//1=Start Record; 0=End Record
	LOG_DISK_FILE_STATUS	File;//磁盘文件操作结果 ，删除文件时使用
	LOG_NET_LOGIN			NetUser;//网络用户简要信息
							//包括:网络用户登录和注销
	LOG_NET_CMN_MSG			Message;//网络屏显消息的日志记录
	LOG_NET_DOWNLOAD		NetDownload;//网络下载时的日志内容
	UNS8					Dummy;//对于没有BUffer的项目的填充位。包括：Display Freezed	
}LOG_BUFFER;

#pragma pack (1) //kk 07-18
typedef struct {
	UNS32		Number;	//记录号
	UNS16		PrevSize;//上一条记录大小 0=文件开始,单位:Byte
	UNS16		CurSize;//当前记录大小	单位:Byte
    time_t		Time;
	UNS8		Level;	//事件级别
	UNS16		LogType;//LOG_TYPE类型
	LOG_BUFFER	Buffer;
}LogFile;
#pragma pack ()  //kk 07-18

/************************ 系统备份和网路备份模块 *********************/
#define	MAX_FILE_HDR_SIZE_EXE		0x1234	//生成的exe文件头的大小，需要测试
#define MAX_BACKUP_PES_BUFFER_SIZE	(200 * 1024)//网络备份的本地开辟的最大PES包部分内存区域
#define MAX_PES_PACKET_SIZE			(200 * 1024)

typedef enum{
	BACKUP_FORMAT_EXE=1,//备份成exe文件格式
	BACKUP_FORMAT_PS,	//备份成PS文件格式
	BACKUP_FORMAT_ERASE,//擦除磁盘
	BACKUP_FORMAT_PES,	//备份数据Buffer是PES包
	BACKUP_FORMAT_MAX
}BACKUP_FORMAT;

/************************ 录像模块 *****************************/
#pragma pack (1)
typedef struct
{
    time_t	time;		//时间值
    UNS8	ch_type;	//低4位表示通道号，中2位表示帧类型，第7bit表示本通道是否motion，第8bit表示本通道是否sensor
    UNS8	H_offset;	//从文件头的偏移量高8位
    UNS32	L_offset;	//从文件头的偏移量低32位
}PS_INDEX_RECORD;
#pragma pack ()/*取消指定对齐，恢复缺省对齐*/

//修改后的十六路PS文件结构
#pragma pack (1) /*指定按1字节对齐*/
typedef struct{
   UNS32 	FileID;					//use this field to id our ps file
   UNS32	RecordChannel;	//录像通道号 单比特记录 LSB=Channel 1
   UNS32	SysVersion;				//系统版本
   UNS8		SysLanguage;			//系统语言
   UNS8		VGAMode;				//PAL  NTSC
   UNS8		SysMAC[6];				//MAC 地址
   UNS32	AVNum;					//系统音频/视频数量 便于与8路/16路兼容 高16bit=音频 低16bit=视频 LSB=通道1
   UNS8		SysName[MAX_SYSTEM_NAME_LEN];							//系统名称
   UNS8		CamName[MAX_SOLO_CHANNEL_16][MAX_PTZ_NAME_LEN];	//每一路云台的名称
   RECORD_MODE ChEvent[MAX_SOLO_CHANNEL_16];					//event record mode
   UNS8		ChResolution[MAX_SOLO_CHANNEL_16];				//D1 or CIF	?
   UNS8		ChComp[MAX_SOLO_CHANNEL_16];						//压缩比
   UNS8		ChFPS[MAX_SOLO_CHANNEL_16];						//帧速率
   UNS8		PreRecOn[MAX_SOLO_CHANNEL_16];
   UNS8		Recording;  // 1: 本文件正在录像
   UNS8		SysMagicNum;//产品系列号SYS_MAGICNUM
   UNS8		Reserved1;
   UNS8		Reserved2;
   UNS8		AudioChannel[MAX_SOLO_CHANNEL_16];				//每一视频通道对应的音频通道号，不是按bit , (0=无音频)
   UNS8		RecMode[MAX_SOLO_CHANNEL_16][MAX_MIN_IN_HOUR];	//每一分钟各通道的录像模式 REC_NO,REC_MANUAL,REC_SCHEDULE,REC_SCHEDULE_EVENT,REC_SENSOR,REC_MOTION
   time_t	TimeStart;  //文件创建时间
   time_t	TimeEnd;    //文件结束时间
   UNS32	FileSize;      //文件大小，MB
   UNS32	TotalIndex;    //总索引条目数，也是文件中视频帧总个数
   UNS64	EffectiveOffset;		//最后一次写入文件头的有效偏移，文件大小，bytes
}PS_FILE_HDR;
#pragma pack () /*取消指定对齐，恢复缺省对齐*/

typedef struct{
	UNS8 mono_stereo;  //0: MONO; 1: STEREO
	UNS8 sampleRate; //音频采样率SAMPLE_RATE_8KHZ,SAMPLE_RATE_16KHZ,...
	UNS8 reserved;
	UNS8 bitsPerSample;	//每次采样的bit数，WORD_WIDTH_8BIT  ，WORD_WIDTH_16BIT             
}AUDIO_INFO;

typedef struct{
	UNS8 BindAudioChn;  //绑定的音频通道号
	UNS8 resolution; //编码分辨率
	UNS8 frameRate; //帧速率
	UNS8 vop_type;	//i帧=0 p帧=1                  
}VIDEO_INFO;

typedef union{
    VIDEO_INFO VideoInfo;
    AUDIO_INFO AudioInfo;
}AV_INFO;

typedef struct{
	UNS32	channel_id;//通道0~15, MAGIC_START:index start magic  
	UNS8	deinterlace;// 是否需要做deinterlace; 1: 是，0: 否
	UNS8	enc_format;//编码格式，MPEG4, H264, G723等
	UNS8	frame_num_inGOP;//	当前帧在GOP中的序号 0--frame_nm-1
	UNS8	av_type;// DT_VIDEO, DT_AUDIO , 0xff: index  
	AV_INFO	avInfo;	//音频或视频各自特有的信息                       
	UNS32	pts;	//帧时间戳，精确到毫秒             
	UNS32	ltime;	//帧时间戳，精确到秒
	UNS32	len;	//solo包长度,不包括本包头		                   
}PES_PACKET_HEADER;

/************************ 回放模块 *****************************/
typedef enum{
	PAUSE = 1,	//播放暂停
	STOP,		//停止播放
	PLAY_MINUS_5,	//第五级向前慢放 最慢
	PLAY_MINUS_4,	//第四级向前慢放
	PLAY_MINUS_3,	//第三级向前慢放
	PLAY_MINUS_2,	//第二级向前慢放
	PLAY_MINUS_1,	//第一级向前慢放
	PLAY_PLUS_0,	//正常速度播放
	PLAY_PLUS_1,	//2倍速度逐帧播放
	PLAY_PLUS_2,	//4倍速度逐帧播放
	PLAY_PLUS_3,	//8倍速度逐帧播放
	PLAY_PLUS_4,	//16倍速度逐帧播放
	FFX1,
	FFX2,	//两倍速向前快进
	FFX4,	//4倍速向前快进
	FFX8,	//8倍速向前快进	
	FFX16,	//16倍速向前快进
	FFX32,	//32倍速向前快进
	REWX1,	//正常速度向后播放
	REWX2,	//两倍速向后快退
	REWX4,	//4倍速向后快退
	REWX8,	//8倍速向后快退	
	REWX16,	//16倍速向后快退
	REWX32,	//32倍速向后快退
	STEP_F,	//单帧向前播放
	STEP_B,	//单帧向后播放
	TIME_POINT,	//定点播放
	AUDIO_CHANGE,	//音频通道修改
	VIDEO_CHANGE	//视频通道修改
}MODE_PLAY;

typedef struct  
{
	UNS32	start_code;
	UNS32	av_type;
	UNS32	chn_id;
	UNS32	packet_length;
	UNS32	data_length;
	UNS32	pts;
	UNS32	dts;
	UNS32	prev_pes_packet_length;
	UNS32	cur_pes_packet_length;
	UNS32	vop_type;
	UNS32	time_code;
	UNS32	cur_time;
	UNS32   cur_date;	
	UNS32   nAudioChannels;
	UNS32   nSample;
	UNS32   nDepth;
	UNS32   interlace;//用来判断该包是否是逐行扫描
	UNS32   bdeinterlace; //该帧是否需要解交织，0-不需要，1-需要
	UNS32   vsize;    //标志该包x轴大小，实际大小需乘以16
	UNS32   hsize;    //标志该包y轴大小
	//由于solo芯片出来的的I帧包头少了一些有用的解码信息，在送入解码器解码
	//解码之前，必须把这些信息添加进去，上述两参数目的就是为此
    time_t  ltime;
}PES_PACKET_HEAD;//ku 01-05

/************************* 函数返回值 *********************************/
#define FAILURE 			0
#define SUCCESS				1

/*************************SOCKET接收缓冲区大小*************************/
#define SOCKET_RECV_BUFFER			(512 * 1024)
#define AUDIOSTREAM_RECV_BUFFER		(32 * 1024)

#endif
