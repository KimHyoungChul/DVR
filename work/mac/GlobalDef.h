//
//  GlobalDef.h
//  NetDVRSDK
//
//  Created by sz Kangtop on 5/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NetDVRSDK_GlobalDef_h
#define NetDVRSDK_GlobalDef_h


/*****************Version Define********************/
#define HAVE_USERRIGHT_VERSION	0x020400//DVR allow get user right
#define ADD_PPPOE_VERSION		0x020800//define DVX_NETSETUP_VER2
#define MODIFY_DDNS_VERSION		0x02080B//define DVX_NETSETUP_VER3
#define HAVE_LINKNUM_VERSION	0x020900//DVR allow get link number
#define NEW_VGA_SET_VERSION		0x030000//整合版VGA计算取基数为0-255，老版本为0-260
#define ADD_CDMA_VERSION		0x03000B//define DVX_NETSETUP_VER4 and DVX_PTZCRUISE
#define ADD_EMAILINTERVAL		0x030020//modify DVX_NETSETUP_VER4 for add EmailInterval, allow open filebackup dialog
#define ADD_NEWDDNS				0x030023//add new ddns servers and delete channel name view when play back record.



/*********************TYPE DEFINE********************/
typedef char				S8;//8 bits
typedef short				S16;//16 bits
typedef	int					S32;//32 bits
typedef long				SL32;//system dependent
typedef long				S64;//64 bits

typedef unsigned char		UNS8;//8 bits
typedef unsigned short		UNS16;//16 bits
typedef unsigned int		UNS32;//32 bits
typedef unsigned long		UNSL32;//system dependent
typedef unsigned long		UNS64;//64 bits

typedef float				SF32;//32 bits
typedef double				SF64;//64 bits
typedef void				*HANDLE;
typedef void				*LPVOID;
//typedef int                 uBOOL;
/*****************************************************/



#define DVS_MAX_SOLO_CHANNEL_16			16
#define DVS_MAX_SYSTEM_NAME_LEN			16		//œµÕ≥√˚≥∆◊Ó≥§◊÷ ˝
#define DVS_MAX_PTZ_NAME_LEN			16		//‘∆Ã®µƒ√˚◊÷≥§∂»
#define DVS_MAX_PTZ_NAME_SHOW			14		//‘∆Ã®√˚◊÷œ‘ æ≥§∂»
#define DVS_MAX_DEVICE_TYPE_NAME_LEN	8
#define MAX_NET_DAY_FILE_NUM			1024	//∑¢ÀÕƒ≥“ªÃÏµƒ¬ºœÒŒƒº˛√˚µƒ ±∫Ú◊Ó∂‡“ª¥Œ∑¢ÀÕµƒ ˝¡ø
#define DVS_MAX_WEEKDAYS				7
#define DVS_MAX_DAYHOURS				24
#define DVS_MAX_SENSOR_LEN			    16
#define DVS_MAX_VIDEOLINE				18
#define DVS_MAX_DDNSNAME				20		//DDNS”√ªß√˚≥§£¨÷Æ«∞∞Ê±æ≥§∂»16
#define DVS_MAX_DDNSNAME_OLD			16		//¿œ∞Ê±æ≥§∂»16
#define DVS_MAX_DNSPWD					20		//DDNS”√ªß√‹¬Î≥§∂»£¨÷Æ«∞∞Ê±æ≥§∂»8
#define DVS_MAX_DNSPWD_OLD				8		//¿œ∞Ê±æ≥§∂»8
#define DVS_MAX_DOMAINLEN				32
#define DVS_MAX_EMAIL_NAME				32
#define DVS_MAX_EMAIL_PWD				20		//Email√‹¬Î≥§∂»£¨÷Æ«∞∞Ê±æ≥§∂»16
#define DVS_MAX_EMAIL_PWD_OLD			16		//¿œ∞Ê±æ≥§∂»16
#define DVS_MAX_EMAIL_SSL				32
#define DVS_MAX_EMAIL_SMTP				32
#define DVS_MAX_USERNAME				8
#define DVS_MAX_USERPWD					8
#define DVS_MAX_WATERKEY				16
#define DVS_MAX_SYSNAME					16
#define DVS_MAX_DISK_NAME				128
#define DVS_MAX_DISK_MOUNTPATH			128
#define DVS_MAX_DISK_NODE				12
#define DVS_MAX_HARDDISK_NUM			4		//œµÕ≥◊Ó¥Û”≤≈Ã ˝¡ø
#define DVS_MAX_USERNUM					15
#define MAX_PTZ_PATTERN_POSITIONS		20
#define MAX_PTZ_PATTERN_POSITIONS_VER4	256
#define DVS_FILE_NAME_LEN				64
#define DVS_MOTION_ROWS					18		//∂ØÃ¨’Ï≤‚÷–£¨∆¡ƒª’Ï≤‚«¯”Úµƒ–– ˝
#define DVS_MOTION_COLS					22		//∂ØÃ¨’Ï≤‚÷–£¨∆¡ƒª’Ï≤‚«¯”Úµƒ¡– ˝


//¥ÌŒÛ¿‡–Õ¥˙∫≈,∂‘”¶DVS_GetLastError()∫Ø ˝∑µªÿ÷µ
#define DVS_NOERROR							0					//√ª”–¥ÌŒÛ
#define DVS_ERROR							1					//Œ¥÷™¥ÌŒÛ
#define DVS_NOINIT							2					//√ª”–≥ı ºªØ
#define DVS_INVALID_HANDLE					3					//Œﬁ–ßµƒæ‰±˙
#define DVS_PARAM_ERROR						4					//”√ªß≤Œ ˝∑«∑®
#define DVS_DEVINFO_DATAERROR				5					//¥´»Îµƒ…Ë±∏–≈œ¢¥ÌŒÛ
#define DVS_CONFIG_DATAERROR				6					//¥´»Îµƒ≈‰÷√≤Œ ˝¥ÌŒÛ
#define DVS_NETWORK_ERROR					7					//Õ¯¬Á¥ÌŒÛ
#define DVS_LOGIN_TIMEOUT					8					//µ«¬Ω≥¨ ±
#define DVS_LOGIN_PWD_ERROR					9					//µ«¬Ω√‹¬Î¥ÌŒÛ
#define DVS_LOGIN_USER_INVALID				10					//µ«¬Ω”√ªß√˚Œﬁ–ß
#define DVS_LOGIN_USER_EXIST				11					//∏√”√ªß“—µ«¬Ω
#define DVS_ADD_USER_EXIST					12					//ÃÌº””√ªß√˚“—¥Ê‘⁄
#define DVS_ADD_USER_FULL					13					//‘ˆº””√ªß“—¬˙
#define DVS_DEL_USER_NOTEXIST				14					//…æ≥˝µƒ”√ªß√˚≤¢≤ª¥Ê‘⁄
#define DVS_EDIT_USER_NOTEXIST				15					//–ﬁ∏ƒ”√ªßµƒ”√ªß√˚≤ª¥Ê‘⁄
#define DVS_CUR_STREAMCHANNEL_OPENED		16					//µ±«∞ µ ±¡˜Õ®µ¿“—±ª¥Úø™
#define DVS_CUR_STREAMCHANNEL_NOTEXIST		17					//µ±«∞ µ ±¡˜Õ®µ¿≤ª¥Ê‘⁄
#define DVS_GET_FILEINDEX_ERROR				18					//ªÒ»°≤•∑≈Œƒº˛À˜“˝ ß∞‹
#define DVS_OPEN_FILE_ERROR					19					//¥Úø™Œƒº˛ ß∞‹
#define DVS_REAL_ALREADY_SAVING				20					// µ ± ˝æ›“—æ≠¥¶”⁄±£¥Ê◊¥Ã¨
#define DVS_SOCKETINIT_ERROR				21					//socketø‚≥ı ºªØ≥ˆ¥Ì
#define DVS_SEND_PARAM_FAILED				22					//∑¢ÀÕ≤Œ ˝ ß∞‹
#define DVS_RECV_PARAM_FAILED				23					//Ω” ’≤Œ ˝ ß∞‹
#define	DVS_VERSION_NOT_MATCH				101					//DVR∞Ê±æ≤ª∆•≈‰


//≤•∑≈øÿ÷∆√¸¡Ó∫Í∂®“Â
#define	DVS_PLAYCTRL_BACKWARD			1                          //øÏÕÀ
#define	DVS_PLAYCTRL_BACKPLAY			2                          //∫ÛÕÀ
#define DVS_PLAYCTRL_PLAY				3                          //«∞Ω¯
#define	DVS_PLAYCTRL_FORWARD			4                          //øÏΩ¯
#define	DVS_PLAYCTRL_BACKSHIFT			5                          //µ•÷°∫ÛÕÀ
#define	DVS_PLAYCTRL_SHIFT				6                          //µ•÷°«∞Ω¯
#define	DVS_PLAYCTRL_PAUSE				7                          //µ±«∞≤•∑≈Œª÷√‘›Õ£
#define	DVS_PLAYCTRL_STOP				8                          //Õ£÷π£¨ªÿµΩ≤•∑≈∆ ºŒª÷√


typedef enum _DVXVIDEO_QUALITY
{
    VQUALITY_SUPER_LOW,		//≥¨µÕª≠÷ 
    VQUALITY_LOW,			//µÕª≠÷ 
    VQUALITY_NORMAL,		//∆’Õ®ª≠÷ 
    VQUALITY_HIGH,			//∏ﬂª≠÷ 
    VQUALITY_SUPER_HIGH,	//≥¨∏ﬂª≠÷ 
    VQUALITY_CBR_64K,		//CBR-64K
    VQUALITY_CBR_256K,		//CBR-256K
    VQUALITY_CBR_512K,		//CBR-512K
    VQUALITY_CBR_1024K,		//CBR-1024K
    VQUALITY_CBR_2048K,		//CBR-2048K	
    VQUALITY_MAX,
}DVXVIDEO_QUALITY;

typedef enum _DVXVIDEO_RATE//÷°ÀŸ¬ ƒ£ Ω
{
    VRATE_OFF,
    VRATE_LEVEL1,	//PAL: 1;	NTSC: 1
    VRATE_LEVEL2,	//PAL: 2.5;	NTSC: 2
    VRATE_LEVEL3,	//PAL: 6.25;NTSC: 6
    VRATE_LEVEL4,	//PAL: 12.5	NTSC:15
    VRATE_LEVEL5,	//PAL: 25	NTSC:30
    VRATE_BUFFER,
    VRATE_MAX,
}DVXVIDEO_RATE;

typedef enum _DNSSERVER
{
    DVS_CN3322,
    DVS_DYNDNS,
    DVS_VERSIONDEFINE,
    DVS_DDNSNOIP,
    DVS_DDNSORAY,
    DVS_DDNSLEANDINGDVR,
    MAX_DNSSERVER
    //DVS_NIGHTOWL,
    //DVS_BESTDVR,
    //DVS_MYQSEE,
    //MAX_DNSSERVER
}DNSSERVER;

typedef enum _DVS_OVERWRITE_STATE
{	
    DVS_OVERWRITE_OFF = 0,	      // πÿ±’
    DVS_OVERWRITE_AUTO,           // ◊‘∂Ø
    DVS_OVERWRITE_ONE_DAY,        // 1 ÃÏ
    DVS_OVERWRITE_SEVEN_DAYS,     // 7 ÃÏ
    DVS_OVERWRITE_THIRTY_DAYS,    //30 ÃÏ
    DVS_OVERWRITE_NINETY_DAYS,    //90 ÃÏ
    DVS_OVERWRITE_MAX
}DVS_OVERWRITE_STATE;//—≠ª∑∏≤∏«£¨∂‘”¶OVERWRITE_STATE

typedef enum _DVS_SUMMER_TIMES
{
    DVS_SUMMER_OFF,	//πÿ±’
    DVS_SUMMER_EUROPE,	//≈∑÷ﬁ
    DVS_SUMMER_AMERICA,	//√¿π˙
    DVS_SUMMER_AUSTRILIA//∞ƒ¥Û¿˚—«
}DVS_SUMMER_TIMES; //œƒ¡Ó ±

typedef enum _DVS_SYSTEM_LANGUAGE
{	
    DVS_LANGUAGE_CHINESE_SIMPLE = 0,	//ºÚÃÂ÷–Œƒ		
    DVS_LANGUAGE_ENGLISH,				//”¢Œƒ	
    DVS_LANGUAGE_RUSSIAN,               //∂Ì”Ô
    DVS_LANGUAGE_KEREAN,				//∫´Œƒ	
    DVS_LANGUAGE_JAPANESE,				//»’Œƒ	
    DVS_LANGUAGE_TAILAND,				//Ã©π˙Œƒ
    DVS_LANGUAGE_CHINESE_TRIDATIONAL,	//∑±ÃÂ÷–Œƒ
    DVS_LANGUAGE_FRANCH,                //∑®”Ô
    DVS_LANGUAGE_SPANISH,               //Œ˜∞‡—¿”Ô
    DVS_LANGUAGE_ETALIAN,               //“‚¥Û¿˚”Ô
    DVS_LANGUAGE_GERMAN,                //µ¬”Ô
    DVS_LANGUAGE_HUNGARIAN,				//–Ÿ—¿¿˚”Ô
    DVS_LANGUAGE_POLISH,				//≤®¿º”Ô
    DVS_LANGUAGE_PORTUGUESE, 			//∆œÃ——¿”Ô
    DVS_LANGUAGE_GREEK,					//œ£¿∞”Ô
    DVS_LANGUAGE_TURKISH,				//Õ¡∂˙∆‰”Ô
    DVS_LANGUAGE_HEGREW,				//œ£≤Æ¿¥”Ô
    DVS_LANGUAGE_FARSI,					//≤®Àπ”Ô
    DVS_LANGUAGE_ARABIC,				//∞¢¿≠≤Æ”Ô
    DVS_LANGUAGE_THAI,					//Ã©”Ô	
}DVS_SYSTEM_LANGUAGE;

typedef enum _DVS_DATE_FORMAT
{
    DVS_YEAR_MON_DAY =1,
    DVS_DAY_MON_YEAR,
    DVS_MON_DAY_YEAR
}DVS_DATE_FORMAT;

typedef enum _DVS_DISK_FORMAT_TYPE
{
    DVS_FORMAT_NULL,	//Œﬁ≤Ÿ◊˜
    DVS_FORMAT_INNER,	//ƒ⁄÷√
    DVS_FORMAT_OUTER,	//Õ‚÷√
    DVS_FORMAT_SYSLOG,	//œµÕ≥»’÷æ
    DVS_FORMAT_EVENTLOG,// ¬º˛»’÷æ
}DVS_DISK_FORMAT_TYPE;//”≤≈Ã∏Ò ΩªØ¿‡–Õ

typedef enum _DVS_DISKTYPE
{
    DVXDS_HARDDISK =1,	//”≤≈Ã
    DVXDS_USB,			//Õ®π˝USBΩ”»Îµƒ”≤≈Ã
    DVXDS_CDROM,		//CDROM
    DVXDS_DVDROM,		//DVDROM
    DVXDS_CDRW			//CDRW øÃ¬ºª˙
}DVS_DISKTYPE;	//¥≈≈Ã÷÷¿‡

typedef enum _DVS_DISKSTATUS
{
    DVXDS_NO =1,	//∏√”≤≈Ã≤ª¥Ê‘⁄
    DVXDS_ERROR,	//”≤≈Ã≥ˆœ÷¥ÌŒÛ
    DVXDS_ENOUGH,	//”≤≈Ã»›¡ø√ª”–¬˙£¨ø…”√
    DVXDS_FULL		//”≤≈Ã“—¬˙
}DVS_DISKSTATUS;

typedef enum _SETUP_CMD_ID
{
    RECORD_CMD,
    CAMERA_CMD,
    EVENT_CMD,
    DISPLAY_CMD,
    NET_CMD,
    SYSTEM_CMD,
    USER_CMD,
    TIME_CMD,
    ALL_PARAM_CMD
}SETUP_CMD_ID;

typedef enum _DVS_SETUP
{
    SETUP_RECORD,
    SETUP_CAMERA,
    SETUP_EVENT,
    SETUP_DISPLAY,
    SETUP_NET,
    SETUP_SYS,
    SETUP_USER,
    SETUP_PTZ,
    SETUP_SYSTIME,
    SETUP_MAX = 1000,
    
    //“‘œ¬√¸¡ÓŒ™÷ª∂¡√¸¡Ó
    CMD_SYSINFO,
}DVS_SETUP;

typedef enum _RESOLUTION_TYPE
{
    CIF_ONLY = 1,
    CIF_HD1 = 3,
    D1_ONLY = 4,
    CIF_D1 = 5,
    CIF_HD1_D1 = 7
}RESOLUTION_TYPE;

//‘∆Ã®øÿ÷∆√¸¡Ó∂®“Â
typedef enum _DVS_PTZ_CTRL_MODE
{
    DVS_PTZ_CMD_NULL = 0,
    DVS_PTZ_CTRL_STOP,                                             //Õ£÷πµ±«∞’˝‘⁄Ω¯––µƒ‘∆Ã®≤Ÿ◊˜
    DVS_PTZ_CTRL_UP,                                               //…œ
    DVS_PTZ_CTRL_DOWN,                                             //œ¬
    DVS_PTZ_CTRL_LEFT,                                             //◊Û
    DVS_PTZ_CTRL_RIGHT,                                            //”“
    DVS_PTZ_CTRL_ZOOM_IN,                                          //Ωπæ‡±‰¥Û
    DVS_PTZ_CTRL_ZOOM_OUT,                                         //Ωπæ‡±‰–°
    DVS_PTZ_CTRL_FOCUS_NEAR,                                       //Ωπµ„«∞µ˜
    DVS_PTZ_CTRL_FOCUS_FAR,                                        //Ωπµ„∫Ûµ˜
    DVS_PTZ_CTRL_PRESET_SET,                                       //…Ë÷√‘§…Ëµ„£¨‘§…Ëµ„µƒ…Ë÷√ ˝”Î‘∆Ã®µƒ…Ë÷√ƒ‹¡¶”–πÿ
    DVS_PTZ_CTRL_PRESET_GOTO,                                      //µ˜”√‘§…Ëµ„
    DVS_PTZ_CTRL_CLEARPRESET,
    DVS_PTZ_CTRL_IRIS_OPEN,                                        //π‚»¶¿©¥Û
    DVS_PTZ_CTRL_IRIS_CLOSE,                                       //π‚»¶Àı–°
    DVS_PTZ_CTRL_RESETCAMERA,
    DVS_PTZ_CTRL_AUTOPAN,                                          //◊‘∂Ø—≤∫Ω
    DVS_PTZ_CTRL_UPLEFT,
    DVS_PTZ_CTRL_UPRIGHT,
    DVS_PTZ_CTRL_DOWNLEFT,
    DVS_PTZ_CTRL_DOWNRIGHT,
    DVS_PTZ_CTRL_SETZOOMSPEED,
    DVS_PTZ_CTRL_SETFOCUSSPEED,
    DVS_PTZ_CTRL_OSDMENU,                                           //≤Àµ•π¶ƒ‹       
    DVS_PTZ_CTRL_MENUENTER,
    DVS_PTZ_CTRL_SETPATTERNSTART,
    DVS_PTZ_CTRL_SETPATTERNSTOP,
    DVS_PTZ_CTRL_RUNPATTERN,
    DVS_PTZ_CTRL_STOPPATTERN,										//Õ£÷ππÏº£—≤∫Ω
    DVS_PTZ_CTRL_MAX_PTZ_CMD,
    DVS_PTZ_CTRL_WRITE_CHAR  
}DVS_PTZ_CTRL_MODE;


typedef struct _DVXRECBASICSETUP//∂‘”¶REC_BASIC_SETUP
{
    DVXVIDEO_QUALITY	RecordQuality;                                  //ª≠÷ : ≥¨µÕ°¢µÕ°¢∆’Õ®°¢∏ﬂ°¢≥¨∏ﬂ
    DVXVIDEO_RATE	    RecordRate;	                                    //¬ºœÒ÷°ÀŸ¬ £¨¥˝∂® 2°¢6°¢12°¢25
    DVXVIDEO_RATE	    PreRecOn;		                                //‘§¬ºœÒ…Ë÷√£¨¥˝∂® 2°¢6
    S8				RecordPlan[DVS_MAX_WEEKDAYS][DVS_MAX_DAYHOURS]; //¬ºœÒº∆ªÆ, µº ≤…”√RECORD_MODE√∂æŸ–Õ,”√∆‰÷–µƒ0,2,3,4
    S8	            Resolution;	                                    //»˝÷÷:D1,HD1, CIF£¨ µº ≤…”√VIDEO_SIZE√∂æŸ–Õ
    S8	            AudioChannel;	                                //¬ºœÒµƒ“Ù∆µÕ®µ¿,0=πÿ±’,ªÚ1~4
    UNS16				reserved;
}DVXRECBASICSETUP;

typedef struct _DVS_RECSETUP//∂‘”¶RECORDSETUP
{
    S8				TailOverWrite;	                      //”≤≈Ã¬˙∏≤∏«£¨DVS_OVERWRITE_STATE
    S8				OverlayTime;                         //µ˛º” ±º‰–≈œ¢ true=µ˛º”£¨ false = ≤ªµ˛º”
    UNS16				res;
    DVXRECBASICSETUP	RecBasic[DVS_MAX_SOLO_CHANNEL_16 + 1]; //¬ºœÒ…Ë÷√(0~15,all)
}DVS_RECSETUP;

/******遮挡隐私功能*********/
typedef struct{
    UNS16 x;
    UNS16 y;
    UNS16 w;
    UNS16 h;
    UNS32 reserved;
}AreaAttr_t;

typedef struct{
    UNS8 ChanelState; //1:开 0:关
    UNS8 reseved1;
    UNS8 reseved2;
    UNS8 reseved3;
    UNS8 SelectArea[4]; //1:hook 0:no
    AreaAttr_t AreaAttr[4];
}privacyzone_t;

typedef struct _DVS_CAMERAOPTS//∂‘”¶CAMERASETUP
{
    UNS8	    PTZName[DVS_MAX_PTZ_NAME_LEN];	//‘∆Ã®µƒ√˚≥∆
    UNS32	    PTZModel;	//–≠“È¿‡–Õ:
    UNS32		BaudRate;    //≤®Ãÿ¬ :1200,2400,4800,9600
    UNS32	    PTZAddr;	//√ø∏ˆ‘∆Ã®µƒµÿ÷∑¬Î
    S16			HUE;	//…´µ˜   -128~127
    S16			SAT;	//±•∫Õ∂»  0~255
    S16			CONT;	//∂‘±»∂»  0~255
    S16			BRT;	//¡¡∂»   -128~127
    UNS8	    PanSpeed;	//‘∆Ã®øÿ÷∆ÀŸ¬ º∂±
    UNS8	    TiltSpeed;	//‘∆Ã®π‚»¶ÀŸ¬ º∂±	
    UNS8	    HDelay; //horizontal delay -8~8,”√”⁄»•µÙª≠√Ê◊Û≤‡ªÚ”“≤‡µƒ∫⁄±ﬂ
    UNS8	    VDelay; //vertical delay ¥π÷±∫⁄±ﬂ
    
    privacyzone_t   PRIVACYZONE;
    
}DVS_CAMERAOPTS;

typedef struct _DVS_CAMERASETUP
{
    DVS_CAMERAOPTS opts[DVS_MAX_SOLO_CHANNEL_16 + 1];
}DVS_CAMERASETUP;

typedef struct _DVS_ES_WARNING//∂‘”¶ES_WARNING
{
    S8	EventName[DVS_MAX_SENSOR_LEN];	//±®æØ∆˜√˚≥∆
    UNS32	SensorOut;	//±®æØ ‰≥ˆ ¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆ
    UNS32	RecChannel;	//¬ºœÒÕ®µ¿∫≈ ¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆÕ®µ¿
    UNS32	ContinueTime;//≥÷–¯ ±º‰,µ•Œª «£∫√Î, 3,5,10,20,30,60,120,180,300,600,900,1200
    S8	InOpenSensor;//≥£ø™–Õ ≥£πÿ–Õ
    S8	OutOpenSensor;	//±®æØ ‰≥ˆ «≥£ø™–Õ ±æ≤ø∑÷√ª”–Õ®π˝ΩÁ√Ê≈‰÷√
    S8	MainSwitch;	//÷˜œ‘ æ∆˜«–ªª 0=πÿ±’,ªÚ1~16
    S8	SpotSwitch;	//SPOTœ‘ æ∆˜«–ªª	0=πÿ±’£¨ªÚ1~16
    S8	PTZGroup;	//≤Ÿ◊˜‘∆Ã®∫≈ 0=πÿ±’ Õ®µ¿¥”1ø™ º 
    S8	PTZCall;	//µ˜”√‘∆Ã®‘§÷√Œª∫≈
    S8	PTZCancel;	//’˝≥£µ˜”√‘∆Ã®‘§÷√Œª∫≈
    S8	BeepOn;	//∑‰√˘∆˜ø™πÿ£¨0=πÿ±’,1=¥Úø™
    S8	SendMail; //∑¢ÀÕ” º˛£ª
    S8	res1;
    UNS16	res2;
}DVS_ES_WARNING;

typedef struct _DVS_ES_MOTION//∂‘”¶ES_MOTION
{
    UNS32	MotionArea[DVS_MAX_VIDEOLINE];	//√ø“ª––”√“ª∏ˆ ˝◊÷±Ì æ£¨ ˝◊÷√ø“ªbit±Ì æ“ª∏ˆ«¯”Ú£¨◊ÓµÕŒª±Ì æ◊Ó”“±ﬂøÈ
    //»Áπ˚’˚∆¡£¨ƒ«æÕ∂‘”¶∂ºŒ™true£¨»Áπ˚«Â∆¡£¨ƒ«æÕ∂ºŒ™false.
    UNS32	RecChannel;	//¬ºœÒÕ®µ¿∫≈ ¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆÕ®µ¿
    UNS32	ContinueTime;//≥÷–¯¬ºœÒ ±º‰	µ•Œª «£∫√Î
    UNS32	SensorOut;	//±®æØ ‰≥ˆ ¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆ
    UNS8	Threshold;	//¡È√Ù∂»
    UNS8	MainSwitch;	//÷˜œ‘ æ∆˜«–ªª
    UNS8	SpotSwitch;	//SPOTœ‘ æ∆˜«–ªª	0=off
    UNS8	BeepOn;	//∑‰√˘∆˜ø™πÿ
    UNS8	SendMail;//∑¢ÀÕ” º˛ 1 = ∑¢ÀÕ£¨ 0 = ≤ª∑¢ÀÕ
    UNS8	res1;
    UNS16	res2;
}DVS_ES_MOTION;

typedef struct _DVS_ES_HARDDISK//∂‘”¶ES_HARDDISK
{
    UNS32	ContinueTime;//≥÷–¯¬ºœÒ ±º‰	µ•Œª «£∫√Î
    UNS32	SensorOut;	//¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆ ‰≥ˆ±®æØ
    UNS8	BeepOn;	//∑‰√˘∆˜ø™πÿ
    UNS8	SendMail;  //∑¢ÀÕ” º˛£ª
    UNS16	res;
}DVS_ES_HARDDISK;

typedef struct _DVS_ES_VIDEOLOSS//∂‘”¶ES_VIDEOLOSS
{
    UNS32	ContinueTime;//≥÷–¯¬ºœÒ ±º‰	µ•Œª «£∫√Î
    UNS32	SensorOut;	//±®æØ ‰≥ˆ ¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆ ‰≥ˆ±®æØ
    UNS8	BeepOn;	//∑‰√˘∆˜ø™πÿ
    UNS8	SendMail;  //∑¢ÀÕ” º˛
    UNS16	res;
}DVS_ES_VIDEOLOSS;

typedef struct _DVS_ES_VIDEOMASK
{
    UNS32	ContinueTime;//≥÷–¯¬ºœÒ ±º‰	µ•Œª «£∫√Î
    UNS32	SensorOut;	//±®æØ ‰≥ˆ ¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆ ‰≥ˆ±®æØ
    UNS8	BeepOn;	//∑‰√˘∆˜ø™πÿ
    UNS8	SendMail;  //∑¢ÀÕ” º˛
    UNS16	res;
}DVS_ES_VIDEOMASK;

typedef struct _DVS_ES_PWD//∂‘”¶ES_PASSWD
{
    UNS32	ContinueTime;//≥÷–¯¬ºœÒ ±º‰	µ•Œª «£∫√Î
    UNS32	SensorOut;	//±®æØ ‰≥ˆ ¥”µÕŒªÀ„∆£¨√ø1bitŒ™“ª∏ˆ ‰≥ˆ±®æØ
    UNS8	BeepOn;	//∑‰√˘∆˜ø™πÿ
    UNS8	SendMail; //∑¢ÀÕ” º˛
    UNS16	res;
}DVS_ES_PWD;

typedef struct _DVS_EVENTSETUP//∂‘”¶EVENTSETUP
{
    DVS_ES_WARNING		WARNING[DVS_MAX_SOLO_CHANNEL_16 + 1];	//±®æØ…Ë÷√
    DVS_ES_MOTION		Motion[DVS_MAX_SOLO_CHANNEL_16 + 1];	//∂ØÃ¨’Ï≤‚…Ë÷√
    DVS_ES_HARDDISK		Harddisk;	              //¥≈≈Ã…Ë÷√
    DVS_ES_VIDEOLOSS	VideoLoss;	              // ”∆µ∂™ ß¥ÌŒÛ
    DVS_ES_VIDEOMASK	VideoMask;
    DVS_ES_PWD			Pwd;	                    //√‹¬Î¥ÌŒÛ
}DVS_EVENTSETUP;

typedef struct _DVS_VGASETUP//∂‘”¶VGASETUP
{
    UNS32	MainInterval;	//÷˜œ‘ æ¬÷ªªÕ®µ¿º‰∏Ù ±º‰,1,2,3,5,10,15,20,30,40,50,60,120,180,300,600
    UNS32	SpotInterval;	//SPOTº‰∏Ù,Õ¨…œ	
    UNS8	IgnoreVideoLoss;	//∫ˆ¬‘ ”∆µ∂™ ß,true= «,false=∑Ò	
    UNS8	VGAMode;	//VGA¿‡–Õ,800x600@60=7,1024x768@60=8,1280x1024@60=9
    S16		reserved;
    S16		UpSide;            //…œ≤‡±ﬂæ‡
    S16		DownSide;          //œ¬≤‡±ﬂæ‡
    S16		LeftSide;         //◊Û≤‡±ﬂæ‡
    S16		RightSide;        //”“≤‡±ﬂæ‡
}DVS_VGASETUP;

typedef struct _DVS_NETSETUP_VER1//∂‘”¶NETSETUP
{
    UNS32	IP; //IP µÿ÷∑
    UNS32	NetMask;	//◊”Õ¯—⁄¬Î
    UNS32	Gateway;	//Õ¯πÿµÿ÷∑
    UNS8	mac[20];
    UNS32	NTP; //NTP Serverµÿ÷∑
    UNS32	HttpPort; 
    UNS32	ViewPort;	// ”∆µº‡øÿ∂Àø⁄
    UNS32	BackupPort;	//Õ¯¬Á±∏∑›∂Àø⁄
    UNS32	SysPort;	//œµÕ≥≈‰÷√∂Àø⁄
    UNS32	RemoteAudioPort;	//‘∂≥Ã“Ù∆µ∂Àø⁄
    UNS8	NetAccess;//Õ¯¬ÁΩ”»Î∑Ω Ω
    UNS8	UPNPSwitch;
    UNS8	NetEncSwitch;	//ø™∆ÙÀ´¬Î¡˜ø™πÿ
    UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF, µº ≤…”√VIDEO_SIZE√∂æŸ–Õ
    DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //ª≠÷ 
    DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//÷°ÀŸ¬ 
    DNSSERVER			DDNSServer;	//DDNS∑˛ŒÒÃ·π©…Ã
    UNS8	DDNSName[DVS_MAX_DDNSNAME_OLD];
    UNS8	DDNSPass[DVS_MAX_DNSPWD_OLD];
    UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
    UNS8	DDNSSwitch;	//DDNSø™πÿ±Í÷æ
    UNS8	EMAILSwitch;    //EMAILø™πÿ±Í÷æ
    UNS8	reserved;
    UNS8 	PPPOESwitch;  //PPPOE ø™πÿ±Í÷æ
    UNS8	PPPOEUserName[DVS_MAX_EMAIL_NAME];
    UNS8	PPPOEPass[DVS_MAX_EMAIL_PWD_OLD];
    UNS8	EMAILSName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILPass[DVS_MAX_EMAIL_PWD_OLD];
    UNS8	EMAIL_SSL[DVS_MAX_EMAIL_SSL];
    UNS8	EMAILRName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILSMTP[DVS_MAX_EMAIL_SMTP];	
    UNS32	EMAILSMTP_Port;
    UNS32	MobilePort;	
    UNS32	DNS1;
    UNS32	DNS2;
}DVS_NETSETUP_VER1;

typedef struct _DVS_NETSETUP_VER2//∂‘”¶NETSETUP
{
    UNS32	IP; //IP µÿ÷∑
    UNS32	NetMask;	//◊”Õ¯—⁄¬Î
    UNS32	Gateway;	//Õ¯πÿµÿ÷∑
    UNS8	mac[20];
    UNS32	NTP; //NTP Serverµÿ÷∑
    UNS32	HttpPort; 
    UNS32	ViewPort;	// ”∆µº‡øÿ∂Àø⁄
    UNS32	BackupPort;	//Õ¯¬Á±∏∑›∂Àø⁄
    UNS32	SysPort;	//œµÕ≥≈‰÷√∂Àø⁄
    UNS32	RemoteAudioPort;	//‘∂≥Ã“Ù∆µ∂Àø⁄
    UNS8	NetAccess;//Õ¯¬ÁΩ”»Î∑Ω Ω
    UNS8	UPNPSwitch;
    UNS8	NetEncSwitch;	//ø™∆ÙÀ´¬Î¡˜ø™πÿ
    UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF, µº ≤…”√VIDEO_SIZE√∂æŸ–Õ
    DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //ª≠÷ 
    DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//÷°ÀŸ¬ 
    DNSSERVER			DDNSServer;	//DDNS∑˛ŒÒÃ·π©…Ã
    UNS8	DDNSName[DVS_MAX_DDNSNAME_OLD];
    UNS8	DDNSPass[DVS_MAX_DNSPWD_OLD];
    UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
    UNS8	DDNSSwitch;	//DDNSø™πÿ±Í÷æ
    UNS8	EMAILSwitch;    //EMAILø™πÿ±Í÷æ
    UNS8 	PPPOESwitch;  //PPPOE ø™πÿ±Í÷æ
    UNS8	PPPOETeleTial;  //0: ”–œﬂ; 1: 3G    //–¬‘ˆ
    UNS8	PPPOETelephone[DVS_MAX_EMAIL_NAME];  //–¬‘ˆ
    UNS8	PPPOEUserName[2][DVS_MAX_EMAIL_NAME];//–ﬁ∏ƒ
    UNS8	PPPOEPass[2][DVS_MAX_EMAIL_PWD_OLD];//–ﬁ∏ƒ
    UNS8	EMAILSName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILPass[DVS_MAX_EMAIL_PWD_OLD];
    UNS8	EMAIL_SSL[DVS_MAX_EMAIL_SSL];
    UNS8	EMAILRName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILSMTP[DVS_MAX_EMAIL_SMTP];	
    UNS32	EMAILSMTP_Port;
    UNS32	MobilePort;	
    UNS32	DNS1;
    UNS32	DNS2;
}DVS_NETSETUP_VER2;

typedef struct _DVS_NETSETUP_VER3//∂‘”¶NETSETUP
{
    UNS32	IP; //IP µÿ÷∑
    UNS32	NetMask;	//◊”Õ¯—⁄¬Î
    UNS32	Gateway;	//Õ¯πÿµÿ÷∑
    UNS8	mac[20];
    UNS32	NTP; //NTP Serverµÿ÷∑
    UNS32	HttpPort; 
    UNS32	ViewPort;	// ”∆µº‡øÿ∂Àø⁄
    UNS32	BackupPort;	//Õ¯¬Á±∏∑›∂Àø⁄
    UNS32	SysPort;	//œµÕ≥≈‰÷√∂Àø⁄
    UNS32	RemoteAudioPort;	//‘∂≥Ã“Ù∆µ∂Àø⁄
    UNS8	NetAccess;//Õ¯¬ÁΩ”»Î∑Ω Ω
    UNS8	UPNPSwitch;
    UNS8	NetEncSwitch;	//ø™∆ÙÀ´¬Î¡˜ø™πÿ
    UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF, µº ≤…”√VIDEO_SIZE√∂æŸ–Õ
    DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //ª≠÷ 
    DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//÷°ÀŸ¬ 
    DNSSERVER			DDNSServer;	//DDNS∑˛ŒÒÃ·π©…Ã
    UNS8	DDNSName[DVS_MAX_DDNSNAME];
    UNS8	DDNSPass[DVS_MAX_DNSPWD];
    UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
    UNS8	DDNSSwitch;	//DDNSø™πÿ±Í÷æ
    UNS8	EMAILSwitch;    //EMAILø™πÿ±Í÷æ
    UNS8 	PPPOESwitch;  //PPPOE ø™πÿ±Í÷æ
    UNS8	PPPOETeleTial;  //0: ”–œﬂ; 1: 3G    //–¬‘ˆ
    UNS8	PPPOETelephone[DVS_MAX_EMAIL_NAME];  //–¬‘ˆ
    UNS8	PPPOEUserName[2][DVS_MAX_EMAIL_NAME];//–ﬁ∏ƒ
    UNS8	PPPOEPass[2][DVS_MAX_EMAIL_PWD];//–ﬁ∏ƒ
    UNS8	EMAILSName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILPass[DVS_MAX_EMAIL_PWD];
    UNS8	EMAIL_SSL[DVS_MAX_EMAIL_SSL];
    UNS8	EMAILRName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILSMTP[DVS_MAX_EMAIL_SMTP];	
    UNS32	EMAILSMTP_Port;
    UNS32	MobilePort;	
    UNS32	DNS1;
    UNS32	DNS2;
}DVS_NETSETUP_VER3;

typedef struct _DVS_NETSETUP_VER4//∂‘”¶NETSETUP
{
    UNS32	IP; //IP µÿ÷∑
    //UNS8    IP[4];
    UNS32	NetMask;	//◊”Õ¯—⁄¬Î
    //UNS8    NetMask[4];
    UNS32	Gateway;	//Õ¯πÿµÿ÷∑
    //UNS8    Gateway[4];
    UNS8	MAC[20];
    UNS32	NTP; //NTP Serverµÿ÷∑
    UNS32	HttpPort; 
    UNS32	ViewPort;	// ”∆µº‡øÿ∂Àø⁄
    UNS32	BackupPort;	//Õ¯¬Á±∏∑›∂Àø⁄
    UNS32	SysPort;	//œµÕ≥≈‰÷√∂Àø⁄
    UNS32	RemoteAudioPort;	//‘∂≥Ã“Ù∆µ∂Àø⁄
    UNS8	NetAccess;//Õ¯¬ÁΩ”»Î∑Ω Ω,  0: static IP; 1:DHCP
    UNS8	UPNPSwitch;
    UNS8	NetEncSwitch;	//ø™∆ÙÀ´¬Î¡˜ø™πÿ
    UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF, µº ≤…”√VIDEO_SIZE√∂æŸ–Õ
    DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //ª≠÷ 
    DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//÷°ÀŸ¬ 
    DNSSERVER			DDNSServer;	//DDNS∑˛ŒÒÃ·π©…Ã
    UNS8	DDNSName[DVS_MAX_DDNSNAME];
    UNS8	DDNSPass[DVS_MAX_DNSPWD];
    UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
    UNS8	DDNSSwitch;	//DDNSø™πÿ±Í÷æ
    UNS8	EMAILSwitch;    //EMAILø™πÿ±Í÷æ
    UNS8 	PPPOESwitch;  //PPPOE ø™πÿ±Í÷æ
    UNS8	PPPOETeleTial;  //PPPOE_TYPE, 0: ”–œﬂ; 1: 3G CDMA; 2: 3G WCDMA;
    UNS8	PPPOETelephone[3][DVS_MAX_EMAIL_NAME]; //”…1∏ˆ∏ƒŒ™3∏ˆ£¨//µ´ «”–œﬂµƒ ±∫Ú[0]≤ª”√,//÷ª”√[1]∫Õ[2]
    UNS8	PPPOEUserName[3][DVS_MAX_EMAIL_NAME]; //”…2∏ˆ∏ƒŒ™3∏ˆ
    UNS8	PPPOEPass[3][DVS_MAX_EMAIL_PWD];  //”…2∏ˆ∏ƒŒ™3∏ˆ
    UNS8	PPPOE_APN[DVS_MAX_EMAIL_NAME]; //–¬‘ˆ
    UNS8	EMAILSName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILPass[DVS_MAX_EMAIL_PWD];
    UNS8	EMAIL_SSL[DVS_MAX_EMAIL_SSL];
    UNS8	EMAILRName[DVS_MAX_EMAIL_NAME];
    UNS8	EMAILRName2[DVS_MAX_EMAIL_NAME]; //–¬‘ˆ£¨≤Àµ•‘›≤ª–ﬁ∏ƒ
    UNS8	EMAILRName3[DVS_MAX_EMAIL_NAME]; ////–¬‘ˆ£¨≤Àµ•‘›≤ª–ﬁ∏ƒ
    UNS8	EMAILSMTP[DVS_MAX_EMAIL_SMTP];
    UNS32	EMAILSMTP_Port;
    UNS32	MobilePort;
    UNS32	DNS1;
    UNS32	DNS2;
    UNS16	EmailInterval;//Email∑¢ÀÕº‰∏Ù£¨µ•Œª «√Î
    UNS16	reserved;
}DVS_NETSETUP_VER4;

typedef struct _DVS_SYSSETUP//∂‘”¶SYSSETUP
{
    UNS32	            MagicNum;	                    //–£—È÷µ
    UNS8	            PowerPwd[DVS_MAX_USERPWD];	  //ø™πÿª˙√‹¬Î,±£¡Ù
    DVS_SUMMER_TIMES	SummerTimes;	        //œƒ¡Ó ±
    DVS_DATE_FORMAT	    DateFormat;	          //»’∆⁄∏Ò Ω
    UNS8	            SystemName[DVS_MAX_SYSNAME];  //œµÕ≥√˚≥∆
    DVS_SYSTEM_LANGUAGE	Language;	          //œµÕ≥”Ô—‘
    DVS_DISK_FORMAT_TYPE FormatType;	      //”≤≈Ã∏Ò ΩªØ¿‡–Õ
    
    UNS32	            AutoLock;		                  //◊‘∂ØÀ¯∂® ±º‰ 0=πÿ±’
    UNS16	            Zone;	                        // ±«¯ ƒ⁄≤ø¡ÌÕ‚µÿ∑Ω¥Ê∑≈“ª∏ˆ√ø∏ˆ ±«¯µƒ’˝∏∫∆´“∆¡ø
    UNS8	            SmartDetect;	                //”≤≈ÃºÏ≤‚ø™πÿ	true false
    UNS8	            SystemID;	                    //œµÕ≥ID
    UNS8	            SysUpdate;	                  //œµÕ≥…˝º∂:0œµÕ≥BIOS,1:œµÕ≥”≥œÒ2:œµÕ≥APP
    
    //----------- πÃ∂®≈‰÷√ -------------
    UNS8	PlayContinue;	//ªÿ∑≈µƒ ±∫Ú£¨¡¨–¯øÁŒƒº˛≤•∑≈=true
    UNS8	PlayAddTime;	//ªÿ∑≈µƒ ±∫Ú£¨–Ë“™º” ±º‰t=true
    UNS8	VideoMode;    //NTSC, PAL
    UNS32	WaterStrength;	//ÀÆ”°µƒ¡¶∂»
    UNS8	WaterKey[DVS_MAX_WATERKEY];		//ÀÆ”°µƒ√‹¬Î
}DVS_SYSSETUP;

typedef struct _USER_LOCALRIGHT//∂‘”¶LOCALACCESS
{
    UNS32	Search;		//±æµÿÀ—À˜»®œﬁ£¨µ•bitøÿ÷∆≤∫LSB=Channel 1£¨”––ßŒª£∫[15...0]£¨bit31Œ™◊‹ø™πÿ±Í÷æŒª
    UNS8	SysSetup;	//±æµÿ≈‰÷√œµÕ≥»®œﬁ
    UNS8	Record;		//±æµÿ¬ºœÒ»®œﬁ
    UNS8	Log;		//±æµÿ≤Èø¥»’÷æ»®œﬁ
    UNS8	PTZ;		//±æµÿµ˜’˚PTZøÿ÷∆»®œﬁ
    UNS8	Backup;		//±æµÿ±∏∑›»®œﬁ
    UNS8	res11;
    UNS16	res12;
}USER_LOCALRIGHT;

typedef struct _USER_NETRIGHT//∂‘”¶NETACCESS
{
    UNS32	Search;		  //Õ¯¬ÁÀ—À˜»®œﬁ
    UNS32	NetView;	     //Õ¯¬Áº‡øÿ»®œﬁ µ•bitøÿ÷∆ LSB=Channel 1£¨”––ßŒª£∫[15...0]£¨bit31Œ™◊‹ø™πÿ±Í÷æŒª
    UNS8	SysSetup;	  //Õ¯¬Á≈‰÷√œµÕ≥»®œﬁ
    UNS8	Log;		     //Õ¯¬Á≤Èø¥»’÷æ»®œﬁ
    UNS8	PTZ;		     //Õ¯¬Áµ˜’˚PTZøÿ÷∆»®œﬁ
    UNS8	SysInfo;	     //Õ¯¬Á≤Èø¥œµÕ≥–≈œ¢»®œﬁ
    UNS8	MobileAccess; // ÷ª˙∑√Œ 
    UNS8	reserved1;
    UNS16	reserved2;
}USER_NETRIGHT;

typedef struct _NET_LINK_INFO
{
    UNS8	MainStreamNum; //∞¸¿® µ ±º‡ ”µƒ÷˜¬Î¡˜°¢ªÿ∑≈°¢œ¬‘ÿ¡¨Ω” ˝
    UNS8	SubStreamNum; // µ ±º‡ ”µƒ◊”¬Î¡˜¡¨Ω” ˝
    UNS8	UserNum; // µ±«∞“—µ«¬ºµƒÕ¯¬Á”√ªß ˝£¨∞¸¿®÷ÿ∏¥µ«¬ºµƒ”√ªß
    UNS8	reserved;
}NET_LINK_INFO;

typedef struct _DVS_USEROPTS//∂‘”¶USERSETUP
{
    UNS8			ValidUser;	//∏√”√ªß «∑Ò”––ß
    UNS8			reserved1;
    UNS16			reserved2;
    UNS8			UserName[DVS_MAX_USERNAME];	//”√ªß√˚
    UNS8			Pwd[DVS_MAX_USERPWD];		//√‹¬Î	
    USER_LOCALRIGHT	LocalAccess;
    USER_NETRIGHT	NetAccess;	
}DVS_USEROPTS;

typedef struct _DVS_USERSETUP
{
    DVS_USEROPTS opts[DVS_MAX_USERNUM];
}DVS_USERSETUP;

typedef struct _DVS_PRESETSETUP
{
    UNS8	presetName;     //‘§÷√µ„√˚≥∆
    UNS8	residenceTime;  //‘§÷√µ„Õ£¡Ù ±º‰
    UNS8	presetIsValid;  //‘§÷√µ„ «∑Ò”––ß£¨ µº ∏≥÷µ∞¥bool£∫trueªÚfalse
    UNS8	reserved;       //‘§¡Ù¿©’πŒª
}DVS_PRESETSETUP;

typedef struct _DVS_PTZSETUP
{
    DVS_PRESETSETUP	gp_preset_param[MAX_PTZ_PATTERN_POSITIONS];
    UNS32  			presetNumber;         //◊‹µƒ‘§÷√µ„ ˝£¨0~MAX_PTZ_PATTERN_POSITIONS
}DVS_PTZSETUP;

typedef struct _DVS_PTZCRUISE
{
    UNS32			PtzChannel;
    DVS_PRESETSETUP	gp_preset_param[MAX_PTZ_PATTERN_POSITIONS_VER4];
    UNS32  			presetNumber;         //◊‹µƒ‘§÷√µ„ ˝£¨0~MAX_PTZ_PATTERN_POSITIONS_VER4
}DVS_PTZCRUISE;

typedef union _DVRSETUP_UNION
{
    DVS_RECSETUP		RecSet;
    DVS_CAMERASETUP		CameraSetup;
    DVS_EVENTSETUP		EventSetup;
    DVS_VGASETUP		VgaSetup;
    DVS_NETSETUP_VER1	NetSetupVer1;
    DVS_NETSETUP_VER2	NetSetupVer2;
    DVS_NETSETUP_VER3	NetSetupVer3;
    DVS_NETSETUP_VER4	NetSetupVer4;
    DVS_SYSSETUP		SysSetup;
    DVS_USERSETUP		UserSetup;
    DVS_PTZSETUP		PtzSetup;
    DVS_PTZCRUISE		PtzCruise;
    UNS32				sysTime;
}DVRSETUP_UNION;

typedef struct
{
    UNS8	SysName[DVS_MAX_SYSTEM_NAME_LEN];	//œµÕ≥√˚≥∆
    UNS8	PTZName[DVS_MAX_SOLO_CHANNEL_16][DVS_MAX_PTZ_NAME_LEN];	//‘∆Ã®µƒ√˚≥∆
    UNS32	PTZModel[DVS_MAX_SOLO_CHANNEL_16]; //‘∆Ã®–Õ∫≈ PTZ_MODEL_NULL=Œﬁ–ß
    UNS8	RecResolution[DVS_MAX_SOLO_CHANNEL_16]; //D1=0;CIF=1
    UNS8	NetResolution[DVS_MAX_SOLO_CHANNEL_16]; //
    UNS8	AudioChannel[DVS_MAX_SOLO_CHANNEL_16]; //√ø∏ˆ ”∆µÕ®µ¿∂‘”¶µƒ“Ù∆µÕ®µ¿∫≈(1~16)≤ª «∞¥bitµƒ
    UNS8	DevTypeName[DVS_MAX_DEVICE_TYPE_NAME_LEN]; // »Á"F"°¢"L" 
    RESOLUTION_TYPE  ResType;     //À˘÷ß≥÷µƒ∑÷±Ê¬ 
    UNS16	Total_RecFrameLimit;       //÷˜¬Î¡˜◊‹÷° ˝
    UNS16	Total_NetFrameLimit;       //¥Œ¬Î¡˜◊‹÷° ˝
    UNS8	VideoType; // 0---NTSC, 1---PAL
    UNS8	VideoInNum;   //* ”∆µ ‰»ÎÕ®µ¿
    UNS8	AudioInNum;	 //*“Ù∆µ ‰»ÎÕ®µ¿ ˝
    UNS8	SensorInNum;	 //*±®æØ ‰»ÎÕ®µ¿ ˝
    UNS8	SensorOutNum;	 //*±®æØ ‰≥ˆÕ®µ¿ ˝
    UNS8	HardDiskNum;      //*ƒ⁄÷√SATA”≤≈Ã ˝¡ø
    UNS8	SpotOutNum;       //* SPOT ‰≥ˆÕ®µ¿ ˝
    UNS8	DevType;
    UNS8	SysMAC[6];	//œµÕ≥MACµÿ÷∑
    UNS16	res;
    UNS32	langSupport;
}DVS_DEV_INFO,*LPDVS_DEV_INFO;

typedef void (*process_cb_ex2)(void* param, void* data, int len, int dropFrame, int fps);
typedef struct
{
    process_cb_ex2	cbRealVideoFunc;
    void* dwUser;
}cb_Struct;


typedef struct
{
    UNS32	dwYear;
    UNS32	dwMonth;
    UNS32	dwDay;
    UNS32	dwHour;
    UNS32	dwMinute;
    UNS32	dwSecond;
}DVS_TIME,*LPDVS_TIME;

//¬ºœÒŒƒº˛–≈œ¢
typedef struct
{
    UNS32		dwSize;
    UNS8		sFileName[DVS_FILE_NAME_LEN];          //¬ºœÒŒƒº˛√˚
    DVS_TIME	dtStartTime;                           //¬ºœÒŒƒº˛∆ º ±º‰
    DVS_TIME	dtEndTime;                             //¬ºœÒŒƒº˛Ω· ¯ ±º‰
    UNS32		dwFileSize;                            //¬ºœÒŒƒº˛¥Û–°,¥À¥¶“‘1MŒ™µ•Œª
    S32			type;                         
}DVS_RECORDFILE_INFO,*LPDVS_RECORDFILE_INFO;

typedef struct
{
    UNS32				dwSize;
    UNS32				dwRecordfileListSize;                  //∏√±Ìƒ‹±£¥Ê¬ºœÒŒƒº˛µƒ◊Ó¥Û ˝ƒø
    UNS32				dwRecordfileCount;                     //∏√±Ì µº ±£¥Ê¬ºœÒŒƒº˛ ˝ƒø
    DVS_RECORDFILE_INFO	*pRecordfileList;                 
}DVS_RECORDFILE_LIST,*LPDVS_RECORDFILE_LIST;

//±®æØœ˚œ¢∂®“Â
typedef struct
{
    UNS32	dwSize;
    UNS32	dwAlarmType;	//±®æØœ˚œ¢¿‡–Õ,0-Õ‚≤ø ‰»Î±®æØ£¨1-∂ØÃ¨’Ï≤‚±®æØ£¨2-¥≈≈Ã¥ÌŒÛ£¨3- ”∆µ∂™ ß£¨4-√‹¬Î¥ÌŒÛ
    UNS32	dwAlarmData;	//dwAlarmType=0,1,3 ±±Ì æÕ®µ¿∫≈£¨dwAlarmType=2 ±±Ì æ¥≈≈Ã∫≈£¨dwAlarmType=4 ±∏√œÓŒﬁ–ß
    UNS32	dwValue;		//dwAlarmType=0 ±£¨dwValue=1±Ì æÕ‚≤ø ‰»Î±®æØ¥Ê‘⁄£¨dwValue=0±Ì æÕ‚≤ø ‰»Î±®æØ»°œ˚
    //dwAlarmType=1 ±£¨dwValue=1±Ì æ”–∂ØÃ¨’Ï≤‚±®æØ£¨dwValue=0±Ì æ∂ØÃ¨’Ï≤‚±®æØ»°œ˚
    //dwAlarmType=3 ±£¨dwValue=1±Ì æ”– ”∆µ∂™ ß£¨dwValue=0±Ì ædwAlarmDataÕ®µ¿ ”∆µ”…ŒﬁµΩ”–
    //dwAlarmType=2,4 ±£¨∏√÷µŒﬁ“‚“Â
}DVS_ALARM_INFO;

typedef enum _CallBackDataType
{
    REAL_VIDEO,// µ ± ”∆µ
    REAL_AUDIO,// µ ±“Ù∆µ
    REMOTE_VIDEO,//‘∂≥Ãªÿ∑≈ ”∆µ
    REMOTE_AUDIO,//‘∂≥Ãªÿ∑≈“Ù∆µ
    LOCAL_VIDEO,//±æµÿªÿ∑≈ ”∆µ
    LOCAL_AUDIO,//±æµÿªÿ∑≈“Ù∆µ
    INTERCOM_RECV,//∂‘Ω≤
}CallBackDataType;

#endif
