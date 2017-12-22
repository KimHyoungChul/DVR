#ifndef _NETDVRAPI_H_
#define _NETDVRAPI_H_

#include "../GlobalDef.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
//*********************TYPE DEFINE********************
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
****************************************************

#define DVS_MAX_SOLO_CHANNEL_16			16
#define DVS_MAX_SYSTEM_NAME_LEN			16		//ϵͳ���������
#define DVS_MAX_PTZ_NAME_LEN			16		//��̨�����ֳ���
#define DVS_MAX_PTZ_NAME_SHOW			14		//��̨������ʾ����
#define DVS_MAX_DEVICE_TYPE_NAME_LEN	8
#define MAX_NET_DAY_FILE_NUM			1024	//����ĳһ���¼���ļ�����ʱ�����һ�η��͵�����
#define DVS_MAX_WEEKDAYS				7
#define DVS_MAX_DAYHOURS				24
#define DVS_MAX_SENSOR_LEN			    16
#define DVS_MAX_VIDEOLINE				18
#define DVS_MAX_DDNSNAME				20		//DDNS�û�������֮ǰ�汾����16
#define DVS_MAX_DDNSNAME_OLD			16		//�ϰ汾����16
#define DVS_MAX_DNSPWD					20		//DDNS�û����볤�ȣ�֮ǰ�汾����8
#define DVS_MAX_DNSPWD_OLD				8		//�ϰ汾����8
#define DVS_MAX_DOMAINLEN				32
#define DVS_MAX_EMAIL_NAME				32
#define DVS_MAX_EMAIL_PWD				20		//Email���볤�ȣ�֮ǰ�汾����16
#define DVS_MAX_EMAIL_PWD_OLD			16		//�ϰ汾����16
#define DVS_MAX_EMAIL_SSL				32
#define DVS_MAX_EMAIL_SMTP				32
#define DVS_MAX_USERNAME				8
#define DVS_MAX_USERPWD					8
#define DVS_MAX_WATERKEY				16
#define DVS_MAX_SYSNAME					16
#define DVS_MAX_DISK_NAME				128
#define DVS_MAX_DISK_MOUNTPATH			128
#define DVS_MAX_DISK_NODE				12
#define DVS_MAX_HARDDISK_NUM			4		//ϵͳ���Ӳ������
#define DVS_MAX_USERNUM					15
#define MAX_PTZ_PATTERN_POSITIONS		20
#define MAX_PTZ_PATTERN_POSITIONS_VER4	256
#define DVS_FILE_NAME_LEN				64
#define DVS_MOTION_ROWS					18		//��̬����У���Ļ������������
#define DVS_MOTION_COLS					22		//��̬����У���Ļ������������


//�������ʹ���,��ӦDVS_GetLastError()��������ֵ
#define DVS_NOERROR							0					//û�д���
#define DVS_ERROR							1					//δ֪����
#define DVS_NOINIT							2					//û�г�ʼ��
#define DVS_INVALID_HANDLE					3					//��Ч�ľ��
#define DVS_PARAM_ERROR						4					//�û������Ƿ�
#define DVS_DEVINFO_DATAERROR				5					//������豸��Ϣ����
#define DVS_CONFIG_DATAERROR				6					//��������ò�������
#define DVS_NETWORK_ERROR					7					//�������
#define DVS_LOGIN_TIMEOUT					8					//��½��ʱ
#define DVS_LOGIN_PWD_ERROR					9					//��½�������
#define DVS_LOGIN_USER_INVALID				10					//��½�û�����Ч
#define DVS_LOGIN_USER_EXIST				11					//���û��ѵ�½
#define DVS_ADD_USER_EXIST					12					//����û����Ѵ���
#define DVS_ADD_USER_FULL					13					//�����û�����
#define DVS_DEL_USER_NOTEXIST				14					//ɾ�����û�����������
#define DVS_EDIT_USER_NOTEXIST				15					//�޸��û����û���������
#define DVS_CUR_STREAMCHANNEL_OPENED		16					//��ǰʵʱ��ͨ���ѱ���
#define DVS_CUR_STREAMCHANNEL_NOTEXIST		17					//��ǰʵʱ��ͨ��������
#define DVS_GET_FILEINDEX_ERROR				18					//��ȡ�����ļ�����ʧ��
#define DVS_OPEN_FILE_ERROR					19					//���ļ�ʧ��
#define DVS_REAL_ALREADY_SAVING				20					//ʵʱ�����Ѿ����ڱ���״̬
#define DVS_SOCKETINIT_ERROR				21					//socket���ʼ������
#define DVS_SEND_PARAM_FAILED				22					//���Ͳ���ʧ��
#define DVS_RECV_PARAM_FAILED				23					//���ղ���ʧ��
#define	DVS_VERSION_NOT_MATCH				101					//DVR�汾��ƥ��


//���ſ�������궨��
#define	DVS_PLAYCTRL_BACKWARD			1                          //����
#define	DVS_PLAYCTRL_BACKPLAY			2                          //����
#define DVS_PLAYCTRL_PLAY				3                          //ǰ��
#define	DVS_PLAYCTRL_FORWARD			4                          //���
#define	DVS_PLAYCTRL_BACKSHIFT			5                          //��֡����
#define	DVS_PLAYCTRL_SHIFT				6                          //��֡ǰ��
#define	DVS_PLAYCTRL_PAUSE				7                          //��ǰ����λ����ͣ
#define	DVS_PLAYCTRL_STOP				8                          //ֹͣ���ص�������ʼλ��


typedef enum _DVXVIDEO_QUALITY
{
	VQUALITY_SUPER_LOW,		//���ͻ���
	VQUALITY_LOW,			//�ͻ���
	VQUALITY_NORMAL,		//��ͨ����
	VQUALITY_HIGH,			//�߻���
	VQUALITY_SUPER_HIGH,	//���߻���
	VQUALITY_CBR_64K,		//CBR-64K
	VQUALITY_CBR_256K,		//CBR-256K
	VQUALITY_CBR_512K,		//CBR-512K
	VQUALITY_CBR_1024K,		//CBR-1024K
	VQUALITY_CBR_2048K,		//CBR-2048K	
	VQUALITY_MAX,
}DVXVIDEO_QUALITY;

typedef enum _DVXVIDEO_RATE//֡����ģʽ
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
	DVS_NIGHTOWL,
	DVS_BESTDVR,
	DVS_MYQSEE,
	MAX_DNSSERVER
}DNSSERVER;

typedef enum _DVS_OVERWRITE_STATE
{	
	DVS_OVERWRITE_OFF = 0,	      // �ر�
	DVS_OVERWRITE_AUTO,           // �Զ�
	DVS_OVERWRITE_ONE_DAY,        // 1 ��
	DVS_OVERWRITE_SEVEN_DAYS,     // 7 ��
	DVS_OVERWRITE_THIRTY_DAYS,    //30 ��
	DVS_OVERWRITE_NINETY_DAYS,    //90 ��
	DVS_OVERWRITE_MAX
}DVS_OVERWRITE_STATE;//ѭ�����ǣ���ӦOVERWRITE_STATE

typedef enum _DVS_SUMMER_TIMES
{
	DVS_SUMMER_OFF,	//�ر�
	DVS_SUMMER_EUROPE,	//ŷ��
	DVS_SUMMER_AMERICA,	//����
	DVS_SUMMER_AUSTRILIA//�Ĵ�����
}DVS_SUMMER_TIMES; //����ʱ

typedef enum _DVS_SYSTEM_LANGUAGE
{	
	DVS_LANGUAGE_CHINESE_SIMPLE = 0,	//��������		
	DVS_LANGUAGE_ENGLISH,				//Ӣ��	
	DVS_LANGUAGE_RUSSIAN,               //����
	DVS_LANGUAGE_KEREAN,				//����	
	DVS_LANGUAGE_JAPANESE,				//����	
	DVS_LANGUAGE_TAILAND,				//̩����
	DVS_LANGUAGE_CHINESE_TRIDATIONAL,	//��������
	DVS_LANGUAGE_FRANCH,                //����
	DVS_LANGUAGE_SPANISH,               //��������
	DVS_LANGUAGE_ETALIAN,               //�������
	DVS_LANGUAGE_GERMAN,                //����
	DVS_LANGUAGE_HUNGARIAN,				//��������
	DVS_LANGUAGE_POLISH,				//������
	DVS_LANGUAGE_PORTUGUESE, 			//��������
	DVS_LANGUAGE_GREEK,					//ϣ����
	DVS_LANGUAGE_TURKISH,				//��������
	DVS_LANGUAGE_HEGREW,				//ϣ������
	DVS_LANGUAGE_FARSI,					//��˹��
	DVS_LANGUAGE_ARABIC,				//��������
	DVS_LANGUAGE_THAI,					//̩��	
}DVS_SYSTEM_LANGUAGE;

typedef enum _DVS_DATE_FORMAT
{
	DVS_YEAR_MON_DAY =1,
	DVS_DAY_MON_YEAR,
	DVS_MON_DAY_YEAR
}DVS_DATE_FORMAT;

typedef enum _DVS_DISK_FORMAT_TYPE
{
	DVS_FORMAT_NULL,	//�޲���
	DVS_FORMAT_INNER,	//����
	DVS_FORMAT_OUTER,	//����
	DVS_FORMAT_SYSLOG,	//ϵͳ��־
	DVS_FORMAT_EVENTLOG,//�¼���־
}DVS_DISK_FORMAT_TYPE;//Ӳ�̸�ʽ������

typedef enum _DVS_DISKTYPE
{
	DVXDS_HARDDISK =1,	//Ӳ��
	DVXDS_USB,			//ͨ��USB�����Ӳ��
	DVXDS_CDROM,		//CDROM
	DVXDS_DVDROM,		//DVDROM
	DVXDS_CDRW			//CDRW ��¼��
}DVS_DISKTYPE;	//��������

typedef enum _DVS_DISKSTATUS
{
	DVXDS_NO =1,	//��Ӳ�̲�����
	DVXDS_ERROR,	//Ӳ�̳��ִ���
	DVXDS_ENOUGH,	//Ӳ������û����������
	DVXDS_FULL		//Ӳ������
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

	//��������Ϊֻ������
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
/*
//��̨���������
typedef enum _DVS_PTZ_CTRL_MODE
{
	DVS_PTZ_CMD_NULL = 0,
	DVS_PTZ_CTRL_STOP,                                             //ֹͣ��ǰ���ڽ��е���̨����
	DVS_PTZ_CTRL_UP,                                               //��
	DVS_PTZ_CTRL_DOWN,                                             //��
	DVS_PTZ_CTRL_LEFT,                                             //��
	DVS_PTZ_CTRL_RIGHT,                                            //��
	DVS_PTZ_CTRL_ZOOM_IN,                                          //������
	DVS_PTZ_CTRL_ZOOM_OUT,                                         //�����С
	DVS_PTZ_CTRL_FOCUS_NEAR,                                       //����ǰ��
	DVS_PTZ_CTRL_FOCUS_FAR,                                        //������
	DVS_PTZ_CTRL_PRESET_SET,                                       //����Ԥ��㣬Ԥ��������������̨�����������й�
	DVS_PTZ_CTRL_PRESET_GOTO,                                      //����Ԥ���
	DVS_PTZ_CTRL_CLEARPRESET,
	DVS_PTZ_CTRL_IRIS_OPEN,                                        //��Ȧ����
	DVS_PTZ_CTRL_IRIS_CLOSE,                                       //��Ȧ��С
	DVS_PTZ_CTRL_RESETCAMERA,
	DVS_PTZ_CTRL_AUTOPAN,                                          //�Զ�Ѳ��
	DVS_PTZ_CTRL_UPLEFT,
	DVS_PTZ_CTRL_UPRIGHT,
	DVS_PTZ_CTRL_DOWNLEFT,
	DVS_PTZ_CTRL_DOWNRIGHT,
	DVS_PTZ_CTRL_SETZOOMSPEED,
	DVS_PTZ_CTRL_SETFOCUSSPEED,
	DVS_PTZ_CTRL_OSDMENU,                                           //�˵�����       
	DVS_PTZ_CTRL_MENUENTER,
	DVS_PTZ_CTRL_SETPATTERNSTART,
	DVS_PTZ_CTRL_SETPATTERNSTOP,
	DVS_PTZ_CTRL_RUNPATTERN,
	DVS_PTZ_CTRL_STOPPATTERN,										//ֹͣ�켣Ѳ��
	DVS_PTZ_CTRL_MAX_PTZ_CMD,
	DVS_PTZ_CTRL_WRITE_CHAR  
}DVS_PTZ_CTRL_MODE;


typedef struct _DVXRECBASICSETUP//��ӦREC_BASIC_SETUP
{
    DVXVIDEO_QUALITY	RecordQuality;                                  //����: ���͡��͡���ͨ���ߡ�����
	DVXVIDEO_RATE	    RecordRate;	                                    //¼��֡���ʣ����� 2��6��12��25
	DVXVIDEO_RATE	    PreRecOn;		                                //Ԥ¼�����ã����� 2��6
	UNS8				RecordPlan[DVS_MAX_WEEKDAYS][DVS_MAX_DAYHOURS]; //¼��ƻ�,ʵ�ʲ���RECORD_MODEö����,�����е�0,2,3,4
	UNS8	            Resolution;	                                    //����:D1,HD1, CIF��ʵ�ʲ���VIDEO_SIZEö����
	UNS8	            AudioChannel;	                                //¼�����Ƶͨ��,0=�ر�,��1~4
	UNS16				reserved;
}DVXRECBASICSETUP;

typedef struct _DVS_RECSETUP//��ӦRECORDSETUP
{
	UNS8				TailOverWrite;	                      //Ӳ�������ǣ�DVS_OVERWRITE_STATE
	UNS8				OverlayTime;                         //����ʱ����Ϣ true=���ӣ� false = ������
	UNS16				res;
	DVXRECBASICSETUP	RecBasic[DVS_MAX_SOLO_CHANNEL_16 + 1]; //¼������(0~15,all)
}DVS_RECSETUP;

typedef struct _DVS_CAMERAOPTS//��ӦCAMERASETUP
{
	UNS8	    PTZName[DVS_MAX_PTZ_NAME_LEN];	//��̨������
	UNS32	    PTZModel;	//Э������:
	UNS32		BaudRate;    //������:1200,2400,4800,9600
	UNS32	    PTZAddr;	//ÿ����̨�ĵ�ַ��
	S16			HUE;	//ɫ��   -128~127
	S16			SAT;	//���Ͷ�  0~255
	S16			CONT;	//�Աȶ�  0~255
	S16			BRT;	//����   -128~127
	UNS8	    PanSpeed;	//��̨�������ʼ���
	UNS8	    TiltSpeed;	//��̨��Ȧ���ʼ���	
	UNS8	    HDelay; //horizontal delay -8~8,����ȥ�����������Ҳ�ĺڱ�
	UNS8	    VDelay; //vertical delay ��ֱ�ڱ�
}DVS_CAMERAOPTS;

typedef struct _DVS_CAMERASETUP
{
	DVS_CAMERAOPTS opts[DVS_MAX_SOLO_CHANNEL_16 + 1];
}DVS_CAMERASETUP;

typedef struct _DVS_ES_WARNING//��ӦES_WARNING
{
	UNS8	EventName[DVS_MAX_SENSOR_LEN];	//����������
	UNS32	SensorOut;	//������� �ӵ�λ����ÿ1bitΪһ��
	UNS32	RecChannel;	//¼��ͨ���� �ӵ�λ����ÿ1bitΪһ��ͨ��
	UNS32	ContinueTime;//����ʱ��,��λ�ǣ���, 3,5,10,20,30,60,120,180,300,600,900,1200
	UNS8	InOpenSensor;//������ ������
	UNS8	OutOpenSensor;	//��������ǳ����� ������û��ͨ����������
	UNS8	MainSwitch;	//����ʾ���л� 0=�ر�,��1~16
	UNS8	SpotSwitch;	//SPOT��ʾ���л�	0=�رգ���1~16
	UNS8	PTZGroup;	//������̨�� 0=�ر� ͨ����1��ʼ 
	UNS8	PTZCall;	//������̨Ԥ��λ��
	UNS8	PTZCancel;	//����������̨Ԥ��λ��
	UNS8	BeepOn;	//���������أ�0=�ر�,1=��
	UNS8	SendMail; //�����ʼ���
	UNS8	res1;
	UNS16	res2;
}DVS_ES_WARNING;

typedef struct _DVS_ES_MOTION//��ӦES_MOTION
{
	UNS32	MotionArea[DVS_MAX_VIDEOLINE];	//ÿһ����һ�����ֱ�ʾ������ÿһbit��ʾһ���������λ��ʾ���ұ߿�
	//����������ǾͶ�Ӧ��Ϊtrue������������ǾͶ�Ϊfalse.
	UNS32	RecChannel;	//¼��ͨ���� �ӵ�λ����ÿ1bitΪһ��ͨ��
	UNS32	ContinueTime;//����¼��ʱ��	��λ�ǣ���
	UNS32	SensorOut;	//������� �ӵ�λ����ÿ1bitΪһ��
	UNS8	Threshold;	//������
	UNS8	MainSwitch;	//����ʾ���л�
	UNS8	SpotSwitch;	//SPOT��ʾ���л�	0=off
	UNS8	BeepOn;	//����������
	UNS8	SendMail;//�����ʼ� 1 = ���ͣ� 0 = ������
	UNS8	res1;
	UNS16	res2;
}DVS_ES_MOTION;

typedef struct _DVS_ES_HARDDISK//��ӦES_HARDDISK
{
	UNS32	ContinueTime;//����¼��ʱ��	��λ�ǣ���
	UNS32	SensorOut;	//�ӵ�λ����ÿ1bitΪһ���������
	UNS8	BeepOn;	//����������
	UNS8	SendMail;  //�����ʼ���
	UNS16	res;
}DVS_ES_HARDDISK;

typedef struct _DVS_ES_VIDEOLOSS//��ӦES_VIDEOLOSS
{
	UNS32	ContinueTime;//����¼��ʱ��	��λ�ǣ���
	UNS32	SensorOut;	//������� �ӵ�λ����ÿ1bitΪһ���������
	UNS8	BeepOn;	//����������
	UNS8	SendMail;  //�����ʼ�
	UNS16	res;
}DVS_ES_VIDEOLOSS;

typedef struct _DVS_ES_VIDEOMASK
{
	UNS32	ContinueTime;//����¼��ʱ��	��λ�ǣ���
	UNS32	SensorOut;	//������� �ӵ�λ����ÿ1bitΪһ���������
	UNS8	BeepOn;	//����������
	UNS8	SendMail;  //�����ʼ�
	UNS16	res;
}DVS_ES_VIDEOMASK;

typedef struct _DVS_ES_PWD//��ӦES_PASSWD
{
	UNS32	ContinueTime;//����¼��ʱ��	��λ�ǣ���
	UNS32	SensorOut;	//������� �ӵ�λ����ÿ1bitΪһ���������
	UNS8	BeepOn;	//����������
	UNS8	SendMail; //�����ʼ�
	UNS16	res;
}DVS_ES_PWD;

typedef struct _DVS_EVENTSETUP//��ӦEVENTSETUP
{
	DVS_ES_WARNING		WARNING[DVS_MAX_SOLO_CHANNEL_16 + 1];	//��������
	DVS_ES_MOTION		Motion[DVS_MAX_SOLO_CHANNEL_16 + 1];	//��̬�������
	DVS_ES_HARDDISK		Harddisk;	              //��������
	DVS_ES_VIDEOLOSS	VideoLoss;	              //��Ƶ��ʧ����
	DVS_ES_VIDEOMASK	VideoMask;
	DVS_ES_PWD			Pwd;	                    //�������
}DVS_EVENTSETUP;

typedef struct _DVS_VGASETUP//��ӦVGASETUP
{
	UNS32	MainInterval;	//����ʾ�ֻ�ͨ�����ʱ��,1,2,3,5,10,15,20,30,40,50,60,120,180,300,600
	UNS32	SpotInterval;	//SPOT���,ͬ��	
	UNS8	IgnoreVideoLoss;	//������Ƶ��ʧ,true=��,false=��	
	UNS8	VGAMode;	//VGA����,800x600@60=7,1024x768@60=8,1280x1024@60=9
	S16		reserved;
	S16		UpSide;            //�ϲ�߾�
	S16		DownSide;          //�²�߾�
	S16		LeftSide;         //���߾�
	S16		RightSide;        //�Ҳ�߾�
}DVS_VGASETUP;

typedef struct _DVS_NETSETUP_VER1//��ӦNETSETUP
{
	UNS32	IP; //IP ��ַ
	UNS32	NetMask;	//��������
	UNS32	Gateway;	//���ص�ַ
	UNS8	mac[20];
	UNS32	NTP; //NTP Server��ַ
	UNS32	HttpPort; 
	UNS32	ViewPort;	//��Ƶ��ض˿�
	UNS32	BackupPort;	//���籸�ݶ˿�
	UNS32	SysPort;	//ϵͳ���ö˿�
	UNS32	RemoteAudioPort;	//Զ����Ƶ�˿�
	UNS8	NetAccess;//������뷽ʽ
	UNS8	UPNPSwitch;
	UNS8	NetEncSwitch;	//����˫��������
	UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF,ʵ�ʲ���VIDEO_SIZEö����
	DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //����
	DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//֡����
	DNSSERVER			DDNSServer;	//DDNS�����ṩ��
	UNS8	DDNSName[DVS_MAX_DDNSNAME_OLD];
	UNS8	DDNSPass[DVS_MAX_DNSPWD_OLD];
	UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
	UNS8	DDNSSwitch;	//DDNS���ر�־
	UNS8	EMAILSwitch;    //EMAIL���ر�־
	UNS8	reserved;
	UNS8 	PPPOESwitch;  //PPPOE ���ر�־
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

typedef struct _DVS_NETSETUP_VER2//��ӦNETSETUP
{
	UNS32	IP; //IP ��ַ
	UNS32	NetMask;	//��������
	UNS32	Gateway;	//���ص�ַ
	UNS8	mac[20];
	UNS32	NTP; //NTP Server��ַ
	UNS32	HttpPort; 
	UNS32	ViewPort;	//��Ƶ��ض˿�
	UNS32	BackupPort;	//���籸�ݶ˿�
	UNS32	SysPort;	//ϵͳ���ö˿�
	UNS32	RemoteAudioPort;	//Զ����Ƶ�˿�
	UNS8	NetAccess;//������뷽ʽ
	UNS8	UPNPSwitch;
	UNS8	NetEncSwitch;	//����˫��������
	UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF,ʵ�ʲ���VIDEO_SIZEö����
	DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //����
	DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//֡����
	DNSSERVER			DDNSServer;	//DDNS�����ṩ��
	UNS8	DDNSName[DVS_MAX_DDNSNAME_OLD];
	UNS8	DDNSPass[DVS_MAX_DNSPWD_OLD];
	UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
	UNS8	DDNSSwitch;	//DDNS���ر�־
	UNS8	EMAILSwitch;    //EMAIL���ر�־
	UNS8 	PPPOESwitch;  //PPPOE ���ر�־
	UNS8	PPPOETeleTial;  //0: ����; 1: 3G    //����
	UNS8	PPPOETelephone[DVS_MAX_EMAIL_NAME];  //����
	UNS8	PPPOEUserName[2][DVS_MAX_EMAIL_NAME];//�޸�
	UNS8	PPPOEPass[2][DVS_MAX_EMAIL_PWD_OLD];//�޸�
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

typedef struct _DVS_NETSETUP_VER3//��ӦNETSETUP
{
	UNS32	IP; //IP ��ַ
	UNS32	NetMask;	//��������
	UNS32	Gateway;	//���ص�ַ
	UNS8	mac[20];
	UNS32	NTP; //NTP Server��ַ
	UNS32	HttpPort; 
	UNS32	ViewPort;	//��Ƶ��ض˿�
	UNS32	BackupPort;	//���籸�ݶ˿�
	UNS32	SysPort;	//ϵͳ���ö˿�
	UNS32	RemoteAudioPort;	//Զ����Ƶ�˿�
	UNS8	NetAccess;//������뷽ʽ
	UNS8	UPNPSwitch;
	UNS8	NetEncSwitch;	//����˫��������
	UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF,ʵ�ʲ���VIDEO_SIZEö����
	DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //����
	DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//֡����
	DNSSERVER			DDNSServer;	//DDNS�����ṩ��
	UNS8	DDNSName[DVS_MAX_DDNSNAME];
	UNS8	DDNSPass[DVS_MAX_DNSPWD];
	UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
	UNS8	DDNSSwitch;	//DDNS���ر�־
	UNS8	EMAILSwitch;    //EMAIL���ر�־
	UNS8 	PPPOESwitch;  //PPPOE ���ر�־
	UNS8	PPPOETeleTial;  //0: ����; 1: 3G    //����
	UNS8	PPPOETelephone[DVS_MAX_EMAIL_NAME];  //����
	UNS8	PPPOEUserName[2][DVS_MAX_EMAIL_NAME];//�޸�
	UNS8	PPPOEPass[2][DVS_MAX_EMAIL_PWD];//�޸�
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

typedef struct _DVS_NETSETUP_VER4//��ӦNETSETUP
{
	UNS32	IP; //IP ��ַ
	UNS32	NetMask;	//��������
	UNS32	Gateway;	//���ص�ַ
	UNS8	MAC[20];
	UNS32	NTP; //NTP Server��ַ
	UNS32	HttpPort; 
	UNS32	ViewPort;	//��Ƶ��ض˿�
	UNS32	BackupPort;	//���籸�ݶ˿�
	UNS32	SysPort;	//ϵͳ���ö˿�
	UNS32	RemoteAudioPort;	//Զ����Ƶ�˿�
	UNS8	NetAccess;//������뷽ʽ,  0: static IP; 1:DHCP
	UNS8	UPNPSwitch;
	UNS8	NetEncSwitch;	//����˫��������
	UNS8	EncResolution[DVS_MAX_SOLO_CHANNEL_16 + 1]; //CIF, QCIF,ʵ�ʲ���VIDEO_SIZEö����
	DVXVIDEO_QUALITY	NetEncQuality[DVS_MAX_SOLO_CHANNEL_16 + 1]; //����
	DVXVIDEO_RATE		NetEncRate[DVS_MAX_SOLO_CHANNEL_16 + 1];	//֡����
	DNSSERVER			DDNSServer;	//DDNS�����ṩ��
	UNS8	DDNSName[DVS_MAX_DDNSNAME];
	UNS8	DDNSPass[DVS_MAX_DNSPWD];
	UNS8	DDNSDomain[DVS_MAX_DOMAINLEN];
	UNS8	DDNSSwitch;	//DDNS���ر�־
	UNS8	EMAILSwitch;    //EMAIL���ر�־
	UNS8 	PPPOESwitch;  //PPPOE ���ر�־
	UNS8	PPPOETeleTial;  //PPPOE_TYPE, 0: ����; 1: 3G CDMA; 2: 3G WCDMA;
	UNS8	PPPOETelephone[3][DVS_MAX_EMAIL_NAME]; //��1����Ϊ3����//�������ߵ�ʱ��[0]����,//ֻ��[1]��[2]
	UNS8	PPPOEUserName[3][DVS_MAX_EMAIL_NAME]; //��2����Ϊ3��
	UNS8	PPPOEPass[3][DVS_MAX_EMAIL_PWD];  //��2����Ϊ3��
	UNS8	PPPOE_APN[DVS_MAX_EMAIL_NAME]; //����
	UNS8	EMAILSName[DVS_MAX_EMAIL_NAME];
	UNS8	EMAILPass[DVS_MAX_EMAIL_PWD];
	UNS8	EMAIL_SSL[DVS_MAX_EMAIL_SSL];
	UNS8	EMAILRName[DVS_MAX_EMAIL_NAME];
	UNS8	EMAILRName2[DVS_MAX_EMAIL_NAME]; //�������˵��ݲ��޸�
	UNS8	EMAILRName3[DVS_MAX_EMAIL_NAME]; ////�������˵��ݲ��޸�
	UNS8	EMAILSMTP[DVS_MAX_EMAIL_SMTP];
	UNS32	EMAILSMTP_Port;
	UNS32	MobilePort;
	UNS32	DNS1;
	UNS32	DNS2;
	UNS16	EmailInterval;//Email���ͼ������λ����
	UNS16	reserved;
}DVS_NETSETUP_VER4;

typedef struct _DVS_SYSSETUP//��ӦSYSSETUP
{
	UNS32	            MagicNum;	                    /*У��ֵ*
	UNS8	            PowerPwd[DVS_MAX_USERPWD];	  //���ػ�����,����
	DVS_SUMMER_TIMES	SummerTimes;	        //����ʱ
	DVS_DATE_FORMAT	    DateFormat;	          //���ڸ�ʽ
	UNS8	            SystemName[DVS_MAX_SYSNAME];  //ϵͳ����
	DVS_SYSTEM_LANGUAGE	Language;	          //ϵͳ����
	DVS_DISK_FORMAT_TYPE FormatType;	      //Ӳ�̸�ʽ������

	UNS32	            AutoLock;		                  //�Զ�����ʱ�� 0=�ر�
	UNS16	            Zone;	                        //ʱ�� �ڲ�����ط����һ��ÿ��ʱ��������ƫ����
	UNS8	            SmartDetect;	                //Ӳ�̼�⿪��	true false
	UNS8	            SystemID;	                    //ϵͳID
	UNS8	            SysUpdate;	                  //ϵͳ����:0ϵͳBIOS,1:ϵͳӳ��2:ϵͳAPP

	/*----------- �̶����� -------------*
	UNS8	PlayContinue;	//�طŵ�ʱ���������ļ�����=true
	UNS8	PlayAddTime;	//�طŵ�ʱ����Ҫ��ʱ��t=true
	UNS8	VideoMode;    //NTSC, PAL
	UNS32	WaterStrength;	//ˮӡ������
	UNS8	WaterKey[DVS_MAX_WATERKEY];		//ˮӡ������
}DVS_SYSSETUP;

typedef struct _USER_LOCALRIGHT//��ӦLOCALACCESS
{
	UNS32	Search;		//��������Ȩ�ޣ���bit���Ʋ�LSB=Channel 1����Чλ��[15...0]��bit31Ϊ�ܿ��ر�־λ
	UNS8	SysSetup;	//��������ϵͳȨ��
	UNS8	Record;		//����¼��Ȩ��
	UNS8	Log;		//���ز鿴��־Ȩ��
	UNS8	PTZ;		//���ص���PTZ����Ȩ��
	UNS8	Backup;		//���ر���Ȩ��
	UNS8	res11;
	UNS16	res12;
}USER_LOCALRIGHT;

typedef struct _USER_NETRIGHT//��ӦNETACCESS
{
	UNS32	Search;		  //��������Ȩ��
	UNS32	NetView;	     //������Ȩ�� ��bit���� LSB=Channel 1����Чλ��[15...0]��bit31Ϊ�ܿ��ر�־λ
	UNS8	SysSetup;	  //��������ϵͳȨ��
	UNS8	Log;		     //����鿴��־Ȩ��
	UNS8	PTZ;		     //�������PTZ����Ȩ��
	UNS8	SysInfo;	     //����鿴ϵͳ��ϢȨ��
	UNS8	MobileAccess; //�ֻ�����
	UNS8	reserved1;
	UNS16	reserved2;
}USER_NETRIGHT;

typedef struct _NET_LINK_INFO
{
	UNS8	MainStreamNum; //����ʵʱ���ӵ����������طš�����������
	UNS8	SubStreamNum; //ʵʱ���ӵ�������������
	UNS8	UserNum; // ��ǰ�ѵ�¼�������û����������ظ���¼���û�
	UNS8	reserved;
}NET_LINK_INFO;

typedef struct _DVS_USEROPTS//��ӦUSERSETUP
{
	UNS8			ValidUser;	//���û��Ƿ���Ч
	UNS8			reserved1;
	UNS16			reserved2;
	UNS8			UserName[DVS_MAX_USERNAME];	//�û���
	UNS8			Pwd[DVS_MAX_USERPWD];		//����	
	USER_LOCALRIGHT	LocalAccess;
	USER_NETRIGHT	NetAccess;	
}DVS_USEROPTS;

typedef struct _DVS_USERSETUP
{
	DVS_USEROPTS opts[DVS_MAX_USERNUM];
}DVS_USERSETUP;

typedef struct _DVS_PRESETSETUP
{
	UNS8	presetName;     //Ԥ�õ�����
	UNS8	residenceTime;  //Ԥ�õ�ͣ��ʱ��
	UNS8	presetIsValid;  //Ԥ�õ��Ƿ���Ч��ʵ�ʸ�ֵ��bool��true��false
	UNS8	reserved;       //Ԥ����չλ
}DVS_PRESETSETUP;

typedef struct _DVS_PTZSETUP
{
	DVS_PRESETSETUP	gp_preset_param[MAX_PTZ_PATTERN_POSITIONS];
	UNS32  			presetNumber;         //�ܵ�Ԥ�õ�����0~MAX_PTZ_PATTERN_POSITIONS
}DVS_PTZSETUP;

typedef struct _DVS_PTZCRUISE
{
	UNS32			PtzChannel;
	DVS_PRESETSETUP	gp_preset_param[MAX_PTZ_PATTERN_POSITIONS_VER4];
	UNS32  			presetNumber;         //�ܵ�Ԥ�õ�����0~MAX_PTZ_PATTERN_POSITIONS_VER4
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
	UNS8	SysName[DVS_MAX_SYSTEM_NAME_LEN];	//ϵͳ����
	UNS8	PTZName[DVS_MAX_SOLO_CHANNEL_16][DVS_MAX_PTZ_NAME_LEN];	//��̨������
	UNS32	PTZModel[DVS_MAX_SOLO_CHANNEL_16]; //��̨�ͺ� PTZ_MODEL_NULL=��Ч
	UNS8	RecResolution[DVS_MAX_SOLO_CHANNEL_16]; //D1=0;CIF=1
	UNS8	NetResolution[DVS_MAX_SOLO_CHANNEL_16]; //
	UNS8	AudioChannel[DVS_MAX_SOLO_CHANNEL_16]; /*ÿ����Ƶͨ����Ӧ����Ƶͨ����(1~16)���ǰ�bit��*
	UNS8	DevTypeName[DVS_MAX_DEVICE_TYPE_NAME_LEN]; /* ��"F"��"L" *
	RESOLUTION_TYPE  ResType;     //��֧�ֵķֱ���
	UNS16	Total_RecFrameLimit;       //��������֡��
	UNS16	Total_NetFrameLimit;       //��������֡��
	UNS8	VideoType; // 0---NTSC, 1---PAL
	UNS8	VideoInNum;   /*��Ƶ����ͨ��*
	UNS8	AudioInNum;	 /*��Ƶ����ͨ����*
	UNS8	SensorInNum;	 /*��������ͨ����*
	UNS8	SensorOutNum;	 /*�������ͨ����*
	UNS8	HardDiskNum;      /*����SATAӲ������*
	UNS8	SpotOutNum;       /* SPOT���ͨ����*
	UNS8	DevType;
	UNS8	SysMAC[6];	//ϵͳMAC��ַ
	UNS16	res;
	UNS32	langSupport;
}DVS_DEV_INFO,*LPDVS_DEV_INFO;

typedef struct
{
	UNS32	dwYear;
	UNS32	dwMonth;
	UNS32	dwDay;
	UNS32	dwHour;
	UNS32	dwMinute;
	UNS32	dwSecond;
}DVS_TIME,*LPDVS_TIME;

//¼���ļ���Ϣ
typedef struct
{
	UNS32		dwSize;
	UNS8		sFileName[DVS_FILE_NAME_LEN];          //¼���ļ���
	DVS_TIME	dtStartTime;                           //¼���ļ���ʼʱ��
	DVS_TIME	dtEndTime;                             //¼���ļ�����ʱ��
	UNS32		dwFileSize;                            //¼���ļ���С,�˴���1MΪ��λ
	S32			type;                         
}DVS_RECORDFILE_INFO,*LPDVS_RECORDFILE_INFO;

typedef struct
{
	UNS32				dwSize;
	UNS32				dwRecordfileListSize;                  //�ñ��ܱ���¼���ļ��������Ŀ
	UNS32				dwRecordfileCount;                     //�ñ�ʵ�ʱ���¼���ļ���Ŀ
	DVS_RECORDFILE_INFO	*pRecordfileList;                 
}DVS_RECORDFILE_LIST,*LPDVS_RECORDFILE_LIST;

//������Ϣ����
typedef struct
{
	UNS32	dwSize;
	UNS32	dwAlarmType;	//������Ϣ����,0-�ⲿ���뱨����1-��̬��ⱨ����2-���̴���3-��Ƶ��ʧ��4-�������
	UNS32	dwAlarmData;	//dwAlarmType=0,1,3ʱ��ʾͨ���ţ�dwAlarmType=2ʱ��ʾ���̺ţ�dwAlarmType=4ʱ������Ч
	UNS32	dwValue;		//dwAlarmType=0ʱ��dwValue=1��ʾ�ⲿ���뱨�����ڣ�dwValue=0��ʾ�ⲿ���뱨��ȡ��
	//dwAlarmType=1ʱ��dwValue=1��ʾ�ж�̬��ⱨ����dwValue=0��ʾ��̬��ⱨ��ȡ��
	//dwAlarmType=3ʱ��dwValue=1��ʾ����Ƶ��ʧ��dwValue=0��ʾdwAlarmDataͨ����Ƶ���޵���
	//dwAlarmType=2,4ʱ����ֵ������
}DVS_ALARM_INFO;

typedef enum _CallBackDataType
{
	REAL_VIDEO,//ʵʱ��Ƶ
	REAL_AUDIO,//ʵʱ��Ƶ
	REMOTE_VIDEO,//Զ�̻ط���Ƶ
	REMOTE_AUDIO,//Զ�̻ط���Ƶ
	LOCAL_VIDEO,//���ػط���Ƶ
	LOCAL_AUDIO,//���ػط���Ƶ
	INTERCOM_RECV,//�Խ�
}CallBackDataType;
*/    
    
//    uBOOL DVX_SetConfig(HANDLE hDvx,UNS32 cmd,LPVOID lpvoid,UNS32 optSize);
//    UNS32 DVX_GetConfig(HANDLE hDvx,UNS32 cmd,LPVOID lpvoid,UNS32 bufSize);
    
/*wuzy-12.4.16*/
//��Ϣ�ص���������dwCommand�������ͣ�hDvs--DVS_Login���صľ����pBuf--���ݣ�dwBufLen--pBuf���ȣ�pchDeviceIP--DVR��IP��wDevicePort--������Ϣ�Ķ˿ڣ�dwUser--�û�����
typedef void (*fMessCallBack)(UNS32 dwCommand, HANDLE hDvs, S8 *pBuf, UNS32 dwBufLen, S8 *pchDeviceIP, UNS32 wDevicePort, UNS32 dwUser);
//����Ƶ�ص���������type�������Ͷ�ӦCallBackDataType��channel��Ƶͨ���Ż����ͨ���ţ�1--16����pBuf--��Ƶ���ݣ�dwBufLen--pBuf���ȣ�dwUser--�û�����
typedef void (*fDataCallBack)(UNS32 type, UNS8 channel, S8 *pBuf, UNS32 dwBufLen, UNS32 dwUser);
typedef void (*process_cb_ex)(void* param, void* data, int len, int dropFrame, int fps);

//SDK��ĳ�ʼ����ע��
UNS32	DVS_Init();
void	DVS_Cleanup();
UNS32	DVS_GetLastError();//���ش�����

//�������ӳ�ʱʱ������Ӵ�����Ŀǰ�ڶ����������Ӵ����ݲ�ʹ�ã���0
void	DVS_SetConnectTime(UNS32 dwWaitTime, UNS32 dwTryTimes);

//��¼��ע��
HANDLE	DVS_Login(S8 *pchDeviceIP, UNS16 wDevicePort, S8 *pchUserName, S8 *pchPwd);
UNS32	DVS_Logout(HANDLE hDvs);

//�����豸��Ϣ�ص�����
void		 DVS_SetDVRMessCallBack(fMessCallBack cbMessFunc, UNS32 dwUser);
UNS32	 DVS_StartListen(HANDLE hDvs);
UNS32	 DVS_StopListen(HANDLE hDvs);
//��ȡ�豸��Ϣ
UNS32	 DVS_GetDeviceInfo(HANDLE hDvs, LPDVS_DEV_INFO lpDeviceInfo);

//�Խ�
UNS32	 DVS_SpeechStart(HANDLE hDvs);
void		 DVS_SpeechStop(HANDLE hDvs);
UNS32	 DVS_IsOnSpeech(HANDLE hDvs);//��0--���ڶԽ���0--û�жԽ�
 
//ʵʱ��Ԥ������
UNS32	 DVS_SetStreamType(HANDLE hDvs, S32 type);//0=������,1=������
HANDLE	 DVS_OpenRealStream(HANDLE hDvs, S8 *pchHostIP, UNS16 wHostPort, UNS8 channel, HANDLE hRealHandle,void* instance);
UNS32	 DVS_CloseRealStream(HANDLE hRealHandle);
UNS32	 DVS_PlayRealStream(HANDLE hRealHandle);
UNS32	 DVS_StopRealStream(HANDLE hRealHandle);
UNS32	 DVS_OpenAudioStream(HANDLE hDvs, S8 *pchHostIP, UNS16 wHostPort, void** hRealHandle);
UNS32	 DVS_PlayRealAudio(HANDLE hDvs, HANDLE hRealHandle, UNS8 channel);
UNS32	 DVS_StopRealAudio(HANDLE hRealHandle);
UNS32	 DVS_GetRealBiAudio(HANDLE hRealHandle, unsigned char** biAudio);
UNS32	 DVS_CloseAudioStream(HANDLE hRealHandle);
UNS8	 DVS_GetRealChnnl(HANDLE hRealHandle);
void	 DVS_SetRealVideoCallBack(fDataCallBack cbRealVideoFunc, HANDLE hRealHandle, void* dwUser);
void	 DVS_SetRealAudioCallBack(fDataCallBack cbRealAudioFunc, HANDLE hRealHandle, UNS32 dwUser);

//��ʼ��ֹͣ����ʵʱ�������ļ�
UNS32	 DVS_SaveRealDataStart(HANDLE hRealHandle, S8 *sFileName);
UNS32	 DVS_SaveRealDataStop(HANDLE hRealHandle);
UNS32	 DVS_IsRecording(HANDLE hRealHandle);//��0--����¼��0--û��¼��

//��̨���ƺ���
UNS32	 DVS_PTZControl(HANDLE hDvs, UNS8 channel, UNS32 dwPTZCommand, UNS32 dwParam);

//Զ���ļ��ط�
UNS32	 DVS_QueryRecordFile(HANDLE hDvs, DVS_TIME dtStartTime, DVS_TIME dtEndTime, S32 type, LPDVS_RECORDFILE_LIST lpRecordfileList);
UNS32	 DVS_OpenRemotePlay(HANDLE hDvs, S8 *pchRemoteFileName, UNS8 playCounts, HANDLE* hPlayHandle);
UNS32	 DVS_CloseRemotePlay(HANDLE hPlayHandle);
UNS32	 DVS_SetRemoteChannelMask(HANDLE hPlayHandle, UNS32 dwBitMaskChn);
UNS32	 DVS_RemotePlayControl(HANDLE hPlayHandle, UNS8 dwControlCode);
UNS32	 DVS_GetRemotePlayTime(HANDLE hPlayHandle, LPDVS_TIME lpCurPlayTime);
UNS32	 DVS_StartRemotePlayAudio(HANDLE hPlayHandle, UNS8 channel);
UNS32	 DVS_StopRemotePlayAudio(HANDLE hPlayHandle, UNS8 channel);
UNS32	 DVS_SeekRemotePlay(HANDLE hPlayHandle, UNS8 type, UNS64 dwArg);//type��1����ʱ�䶨λ��2�����ֽڶ�λ����ʱֻ֧��1
void	 DVS_SetRemoteDataCallBack(process_cb_ex cbRemoteDataFunc, HANDLE hPlayHandle, void* dwUser);

//Զ���ļ�����
UNS32	 DVS_DumpOpen(HANDLE hDvs, S8 *pDumpFile, S8 *pSaveAs, UNS32 startTime, UNS32 endTime, UNS32 chnnlMask);
void	 DVS_DumpClose(HANDLE hDvs);
S32		 DVS_DumpProgress(HANDLE hDvs);

//�����ļ��ط�
UNS32	 DVS_OpenLocalPlay(S8 *pchLocalFileName, UNS8 playCounts, HANDLE* hPlayHandle);
UNS32	 DVS_GetDuration(HANDLE hPlay, void** startTime, void** endTime);
UNS32	 DVS_GetLocalPlayCurTime(HANDLE hPlayHandle, UNS32 curTime);
UNS32	 DVS_CloseLocalPlay(HANDLE hPlayHandle);
UNS32	 DVS_SetLocalChannelMask(HANDLE hPlayHandle, UNS32 dwBitMaskChn);
UNS32	 DVS_LocalPlayControl(HANDLE hPlayHandle, UNS8 dwControlCode);
UNS32	 DVS_StartLocalPlayAudio(HANDLE hPlayHandle, UNS8 channel);
UNS32	 DVS_StopLocalPlayAudio(HANDLE hPlayHandle, UNS8 channel);
UNS32	 DVS_SeekLocalPlay(HANDLE hPlayHandle, UNS8 type, UNS64 dwArg);
void	 DVS_SetLocalDataCallBack(process_cb_ex cbLocalDataFunc, HANDLE hPlayHandle, void* dwUser);

//�ж��ļ��Ƿ񲥷Ž���
UNS32	 DVS_PlayIsEnd(HANDLE hPlayHandle);
UNS32	 DVS_IsOnline(HANDLE hDvs);//DVS����״̬����0--���ߣ�0--������
UNS32	 DVS_GetUserNetRight(HANDLE hDvs, USER_NETRIGHT *pNetAccess);//��ȡ�û�Ȩ��
UNS32	 DVS_GetDvrVersion();//��ȡDVR�汾
UNS32	 DVS_SetChName(HANDLE hRealHandle, S8 *cChName);//����ͨ������
UNS32	 DVS_GetLinkNum(HANDLE hDvs, NET_LINK_INFO *pNetLinkInfo);//��ȡ������Դ������
UNS32	 DVS_ChnnlNameChanged(HANDLE hDvs);//ͨ�������Ƿ�ı䣬��0--�Ѹı䣬0--δ�ı�
UNS32	 DVS_RequestEmailTest(HANDLE hDvs);//����DVR���Ͳ����ʼ�

//��������������ȡ
UNS32	 DVS_SetConfig(HANDLE hDvs, UNS32 cmd, LPVOID lpvoid, UNS32 optSize);
UNS32	 DVS_GetConfig(HANDLE hDvs, UNS32 cmd, LPVOID lpvoid, UNS32 bufSize);

	
/****************add by wuzy***************/
UNS32 DVS_GetChnNumber(HANDLE hDvs);


#ifdef __cplusplus
}
#endif


#endif
