/***************************************************************************
                          rm_stddef.h  -  description
                             -------------------
    begin                : ��  5�� 19 2005
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
#define	CMD_CFG_VER_REQ						0x010F		// ���������ļ��汾dvr->pc
#define	CMD_CFG_GETCFGLIST_REQ				0x0110          //���󲥷������ļ��б�pc->dvr
#define	CMD_CFG_DOWNLIST_REQ				0x0111		// ���������ļ��б�����dvr->pc
#define   CMD_GET_MEDIALENGTH					0x0112          //�õ��ļ��ĳ���DVR->PC
#define   CMD_SLEEP								0x0113          //ý������ļ�ȫ��������ϣ���ʼ˯��

/*jwj add*/
//auto upgrade
#define   CMD_UPGRADE_GETVERSION				0x0114         //��������v6��ȡ��������İ汾����
#define   CMD_UPGADE_START						0x0115         //��ʼ����

/*jwj add*/
//auto delete media file
#define   CMD_DELETE_MEDIA						0x0116         //��ʼɾ��ý���ļ�

/*jwj add
2006/11/11*/
//auto download netlog file
//MDVR->PC
#define   CMD_GET_NETLOG						0x0117         //get netlog file

/*jwj add
2007/03/23
//media files have benn trans
*/
#define	CMD_MEDIA_OK							0x0118         //����ļ��������


/*jwj add
2007/07/24
start remote format hdd func*/
#define	CMD_START_FORMAT						0x011B        //������ʽ������

/*jwj add
2007/07/24
get format stat func*/
#define	CMD_GET_FORMAT_STAT					0x011C        //��ѯ��ʽ��״̬

/*jwj add
2007/07/24
lock or unlock file on mdvr*/
#define	CMD_LOCKUNLOCK_FILE					0x011D        //��ѯ��ʽ��״̬


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

#define MAX_THREAD_NUM 20//�����豸������ж��ٸ�VMS����Э����̡߳�
#define MAX_BUFFER_CH_NUM 4//����ÿ��THREAD���ж��ٸ�ͨ������
#define MAX_CH_SESSION_NUM 3//����ÿ��ͨ��ͬʱ����������Ŀ

#define MAX_SESSION_COUNT        50

//�����̳߳��й���ʹ�õ��̺߳�
#define FILE_USER_WORKER_ID 		8
#define UPDATE_WORKER_ID              	9
#define LOG_WORKER_ID                 		10
#define ALARM_USER_WORKER_ID 	11
/*�����Ƕ������̣߳����ܹ���*/

#define MAX_VMS_DOWNLOADFILE 8

#define MAKE_SESSION_ID( worker_id, trans_id,sessid )  ( ( worker_id ) << 24 |(trans_id)<< 16 | ( sessid ) )
#define GET_WORKER_ID( sessid )  ( (( sessid) >>24 )&0xff )
#define GET_TRANS_ID( sessid )   ( (( sessid) >>16 )&0xff )
#define GET_MESS_ID( sessid )  ( ( sessid)&0x000000ff )

typedef unsigned int  sess_id_t;
typedef unsigned int  req_id_t;
typedef int stream_id;


//һ��֡������
typedef struct tag_frame_buffer_work
{
	SeesionList   				 ch_seesio_list[MAX_BUFFER_CH_NUM];
	pthread_t            			  worker_thr;
   	pthread_mutex_t 			  mutex;
	int	               			  media_file;
	char                                      filename[128];
	int                                        Filelenth;//�ļ�����
	char                            	        workerInit;
	char                                     buffer_id;
	char					        bThreadRun;
	int                                        count;
	int                                        sock;
	StreamType			stream_type;				//����������������
//	int						send_fileheader[MAX_BUFFER_CH_NUM];		//	1:need send fileheader
	int                                    	has_newsession[MAX_BUFFER_CH_NUM];		// 1:new session
	int                                    	MainJumpToIFrame[MAX_BUFFER_CH_NUM];
	int                                    	SubJumpToIFrame[MAX_BUFFER_CH_NUM];
	int                                    	has_requestiframe[MAX_BUFFER_CH_NUM];		// 1:new session

}frame_buffer_work;

typedef struct  tag_Net_Protocol_Header
{
	unsigned long  	ProtoVersion;        	/* �汾*/
	unsigned long  	MsgType;           	/* ��Ϣ����*/
	unsigned long   	TransactionNo;      /* ���б��*/
        unsigned long   	SessionNo;         /* ����������*/
	unsigned long    	PacketLen;           /*������*/
	/* ����header�ͺ���data_info���ڵ�packet�ߴ�*/
}Net_Protocol_Header;

typedef struct tag_VMSRES_MSG_HEAD
{
 	int     packagelen;
	int     errorcode;

}vms_resp_head;

typedef enum service_type
{
    SERVICE_REALTIME,  // ʵʱ
    SERVICE_RELIABLE   // �ɿ�
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


//��������Ƶ������Ϣ
typedef struct VMS_Audio_Control
{
	PREVIEW_RESPONSE_MSG		Audio_msg;//ȡ�·�ʵʱԤ��ָ��ʱ�豸�ش���ֵ
	int		iFlag;//0��ʾ������Ƶ��1��ʾ��������Ƶ��ĿǰֵӦ����������
};


// �����ļ���Ϣ
typedef struct
{
	unsigned long 	        CmdID;			// ��Ϣ����
	unsigned long		Length;			// ��Ϣ����
	int 	flag;				/*ƫ������־, 0:ȫ���ļ� 1�����մ�Сƫ�������� 2������ʱ��ƫ��������*/
	int	start;			/*��ʼƫ����*/
	int	end;				/*����ƫ����*/
	int    offset; /*�ļ�ƫ���������˵ģ������ļ�ͷ���Ѿ����صĲ���*/
	char	file_name[128];	/*ȫ·���ļ���*/
}file_msg_vms;

//�����ṹ����
typedef struct
{
	unsigned char	year;//��־��ѯ��,0~255ȡֵ��Χ������2000~2255��
	unsigned char	month;//��־��ѯ�£�1~12Ϊȡֵ��Χ
	unsigned char	day;//��־��ѯ�죬����δ��
	unsigned char type;//��־����,0,¼���ļ���������־�ļ�

}calenda_setting_vms;

//Ӳ�̸�ʽ���ṹ����
typedef struct
{
	int disktype;//�������ͣ�1->hdd,2->sd
	int number;/*��Ҫ��ʽ�����̷���ÿ��bit�����Ӧ�����Ƿ���Ҫ��ʽ������
	0x05�������һ���͵�����Ӳ����Ҫ��ʽ��*/

}formathdd_vms;

//¼���ļ�������Ϣ
//¼���ļ���������
typedef struct
{
       int     type;/* ¼�����ͣ� 1������¼�� 2������¼��3 ������¼��*/
	int     channel;/* ͨ����, ���������ļ���ͨ���� bit0~bit15�ֱ����1~16ͨ��*/

	/*¼��ʱ��*/
	unsigned char 	start_year;	//���֣����������Ϊ2000+start_year���磸������2008
	unsigned char 	start_month;//���֣�����1~12
	unsigned char 	start_day;//���֣�����1~31
	unsigned char 	start_hour;//���֣�����0~23
	unsigned char 	start_min;//���֣�����0~59
	unsigned char	start_sec;//���֣�����0~59

	/*����ʱ��,����ͬ��ʼʱ��*/
	unsigned char	end_hour;
	unsigned char 	end_min;
	unsigned char	end_sec;
	unsigned char reserved[3];
}search_setting_vms;

// �ļ��ṹ��
typedef struct
{
	char 			filename[128];			// �ļ���
	unsigned int	filesize;				// �ļ���С

}file_info_vms;

// �����ļ���Ϣ
typedef struct
{
	int	filesize;				/*�ļ���ʵ��С*/
}file_return_vms;


// �����ļ���Ϣ
typedef struct
{
	char				filename[20];
	unsigned int		filesize;				/*�ļ���ʵ��С*/
}configfile_info_vms;


//�豸���ص��ļ���Ϣ
typedef struct
{
       int    filesize;              //�ü����ɵ��ļ���С�������ļ�ͷ
	int	start_offset;		// ��ʼƫ����
	int 	end_offset;		// ����ƫ����
}File_download_info;

typedef struct tag_CHSIGNALSTATE {
	int ch_id;
	int State;  //0xx(31-24)xx(23-16)xx(15-8)xx
	/*31-24:�����豸��������
	    23-16:�����豸��������
	    15-0:�����豸��ͨ����Ч��*/
}ChSignalState_vms;

//��ʼȡʵʱ����ֹͣ
//Net_Protocol_Header + AVMessageHead
typedef struct tag_AVMessageHead{
	unsigned long 	        CmdID;			// ��Ϣ����
	unsigned long		Length;			// ��Ϣ����
	unsigned long		channel;			// ͨ����
	unsigned long		trans_mode;		//����ģʽ
	unsigned long        stream_select;    //����������������
	char                     usrname[vs_usrname_len];       //��¼�����û���
	char                     password[vs_pass_len];      //��¼��������
}AVMessageHead_vms;


typedef struct tag_StartlogHead{
	unsigned char         type;/*������־���ͣ���ʱ����������û����*/
	unsigned char		search_year;		//���֣����������Ϊ2000+start_year���磸������2008
	unsigned char		search_month;	//���֣�����1~12
	unsigned char		search_day;		//���֣�����1~31
}Startlog_vms;

typedef struct {
	char                     usrname[vs_usrname_len];       //��¼�����û���
	char                     password[vs_pass_len];      //��¼��������
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

/*��̨*/
typedef struct
{
	unsigned long	channel;		//ͨ����
	unsigned long	cmd;  			//��̨��������
	unsigned long	para;			//�����û�������Ԥ�õ�ʱ��ΪԤԤ�õ��
	unsigned char	reserved[20];
} PTZAction;

#if 0
#define PTZCONTROL_UP					0x09//	0x01	//��ͷ��ת��
#define PTZCONTROL_DOWN					0x0a//0x02	//��ͷ��ת��
#define PTZCONTROL_LEFT					0x0b//0x03	//��ͷ��ת��
#define PTZCONTROL_RIGHT				0x0c//0x04	//��ͷ��ת��
#define PTZCONTROL_IRIS_OPEN				0x0d	//��Ȧ ��
#define PTZCONTROL_IRIS_CLOSE			    0x0e	//��Ȧ С
#define PTZCONTROL_ZOOM_IN				0x06	//	���� ��
#define PTZCONTROL_ZOOM_OUT				0x05	//	���� ��
#define PTZCONTROL_FOCUS_NEAR			0x07	//�۽� ��
#define PTZCONTROL_FOCUS_FAR			0x08	//�۽� Զ
#define PTZCONTROL_AUTOPAN				0x10	//�Զ�
#define PTZCONTROL_BRUSH_ON				0x0c
#define PTZCONTROL_BRUSH_OFF			0x0d
#define PTZCONTROL_PRESET					0x0e	//Ԥ�õ�
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
#define PTZCONTROL_UP						0x01	//��ͷ��ת��
#define PTZCONTROL_DOWN					0x02	//��ͷ��ת��
#define PTZCONTROL_LEFT						0x03	//��ͷ��ת��
#define PTZCONTROL_RIGHT					0x04	//��ͷ��ת��
#define PTZCONTROL_IRIS_OPEN				0x05	//��Ȧ ��
#define PTZCONTROL_IRIS_CLOSE			0x06	//��Ȧ С
#define PTZCONTROL_ZOOM_IN				0x07	//	���� ��
#define PTZCONTROL_ZOOM_OUT				0x08	//	���� ��
#define PTZCONTROL_FOCUS_NEAR			0x09	//�۽� ��
#define PTZCONTROL_FOCUS_FAR			0x0a	//�۽� Զ
#define PTZCONTROL_AUTOPAN				0x0b	//�Զ�
#define PTZCONTROL_BRUSH_ON				0x0c
#define PTZCONTROL_BRUSH_OFF			0x0d
#define PTZCONTROL_PRESET					0x0e	//Ԥ�õ�
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
