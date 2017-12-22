/*
 *  srv_vms_tcp.h
 *  PlayerCore
 *
 *  Created by Kelvin on 2/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __SRV_VS_VMSTCP_HEADER
#define __SRV_VS_VMSTCP_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define VS_USRNAME_LEN		32/*login username length*/
#define VS_PASS_LEN			32/*login password length*/
	
#define CMD_START_REALSTREAM_REQ			0x0001
#define CMD_STOP_REALSTREAM_REQ				0x0002
	
#define CMD_START_DOWNLOAD_REQ				0x0001
#define CMD_START_DOWNINDEX_REQ				0x0002
#define CMD_STOP_DOWNLOAD_REQ				0x0003
	
/* trans_mode type */
#define TRANS_I_FRAME						0x00
#define TRANS_VIDEO_FRAME					0x01
#define TRANS_I_AUDIO_FRAME					0x02
#define TRANS_ALL_FRAME						0x03
	
//stream_select
#define TRANS_MAINSTREAM					0x01
#define TRANS_SUBSTREAM						0x00
	
// PTZ CMD
#define PTZCONTROL_UP						0x01	//
#define PTZCONTROL_DOWN						0x02	//
#define PTZCONTROL_LEFT						0x03	//
#define PTZCONTROL_RIGHT					0x04	//
#define PTZCONTROL_IRIS_OPEN				0x05	//
#define PTZCONTROL_IRIS_CLOSE				0x06	//
#define PTZCONTROL_ZOOM_IN					0x07	//
#define PTZCONTROL_ZOOM_OUT					0x08	//
#define PTZCONTROL_FOCUS_NEAR				0x09	//
#define PTZCONTROL_FOCUS_FAR				0x0a	//
#define PTZCONTROL_AUTOPAN					0x0b	//
	
//#if defined(PTZ_Cruise)
#define PTZCONTROL_PRESET					0x0d	//
#define PTZCONTROL_RECALL					0x0e	//
#define PTZCONTROL_CRUISE					0x0f	//
#define PTZCONTROL_CLEARPRESET				0x10	//
#define PTZCONTROL_LOAD						0x11	//
#define PTZCONTROL_SAVE						0x12	//
#define PTZACTION_CRUISE_STOP				0x13	//
//#endif /*PTZ_Cruise*/
	
#define PTZCONTROL_STOP						0x14	
	
/*VMS Protocol version*/
#define PROTOCOLVERSION					0x0001

#define VMS_PREVIEW						0x00000003/*video and audio preview*/
#define VMS_FILE_DOWNLOAD				0x00000007/*record file download*/
#define VMS_SEARCH_FILE					0x00000009/*search record file*/
#define VMS_GET_CH_SIGNAL				0x0000000A
#define VMS_PARAM_GET					0x0000000E//get Dvr param
#define VMS_PARAM_SET			        0x0000000F/*PC Set config file to MDVR*/
#define VMS_GET_CALENDA					0x00000011/*get current month record info*/
#define VMS_PTZ                         0x00000018/*PTZ control*/
#define VMS_USRPSW                      0x00000019/*username psw control*/
#define VMS_AUDIO_REQUIRE				0x0000001A/*audio require control*/	
/*kelvin@kangtop*/
#define VMS_SEARCH_FILE_NUM				0x0000001B/*how many files in dvr*/
#define VMS_SEARCH_FILE_GETPAGE			0x0000001C/*get one page files from dvr*/
	
/*VMS Login result*/
#define LOGIN_FAILED		0x04
#define NORMAL_USER			0x09
#define MANAGER				0x0a
	
#define DvrType(SIGNALSTATE)	ntohl(SIGNALSTATE.State)&0xffff
#define QueryChannelCount(SIGNALSTATE, CHCOUNT)		switch(DvrType(SIGNALSTATE)) { \
														case 0x9602:				\
														case 0x9612:				\
														case 0x7614:				\
														case 0x1504:				\
														case 0x1514:				\
															CHCOUNT = 4;	break;	\
														case 0x9603:				\
														case 0x9613:				\
														case 0x7618:				\
														case 0x1508:				\
														case 0x1518:				\
															CHCOUNT = 8;	break;	\
														case 0x1916:				\
															CHCOUNT = 16;	break;	\
														default:					\
															CHCOUNT = -1;	break;}
	
typedef unsigned int  sess_id_t;
typedef unsigned int  req_id_t;
typedef int stream_id;
	
typedef struct tag_VMSRES_MSG
{
	int req_id;
	sess_id_t sid;

}PREVIEW_RESPONSE_MSG;	
	
typedef struct  tag_Net_Protocol_Header
{
	unsigned int   ProtoVersion;	//version
	unsigned int   MsgType;		//message type
	unsigned int   TransactionNo;	//sequence
    unsigned int   SessionNo;		//session number
	unsigned int   PacketLen;		//packet length(header and data)

}Net_Protocol_Header;

typedef struct tag_AVMessageHead{
	unsigned int 	    CmdID;					//message ID
	unsigned int		Length;					//message length
	unsigned int		channel;				//channel num
	unsigned int		trans_mode;				//translate mode
	unsigned int		stream_select;			//main stream or sub stream
	char                usrname[VS_USRNAME_LEN];//user name
	char                password[VS_PASS_LEN];	//password
}AVMessageHead_vms;
	
typedef struct tag_CHSIGNALSTATE
{
	int ch_id;
	int State;	/*0xx(31-24)xx(23-16)xx(15-8)xx*/
				/*31-24: device sub id; 23-16: device main id; 15-8: device ch num*/
}ChSignalState_vms;
	
typedef struct tag_Calenda_setting_vms
{
	unsigned char year;		/*0-255 map 2000-2255*/
	unsigned char month;	/*1-12*/
	unsigned char day;		/**/
	unsigned char type;		/*0: rec file; 1: log file*/
	
}calenda_setting_vms;
	
typedef struct tag_Search_setting
{
	int type;		/*normal rec, alarm rec, all rec*/
	int channel;	/*bit0~bit15 map 1~16 channel*/
	
	unsigned char start_year;
	unsigned char start_month;
	unsigned char start_day;
	unsigned char start_hour;
	unsigned char start_min;
	unsigned char start_sec;
	
	/*end*/
	unsigned char end_hour;
	unsigned char end_min;
	unsigned char end_sec;
	
}search_setting;
	
typedef struct tag_Search_file_setting
{
	int file_size;		/*Bytes*/
	int rec_type;		/*1: normal rec; 2: alarm rec; 3: all rec*/
	int channel;		/*bit0~bit15: 1~16channel*/
	
	char file_name[128]; /*absolutely path*/
	//rec start time
	unsigned char start_hour;
	unsigned char start_min;
	unsigned char start_sec;
	//rec stop time
	unsigned char end_hour;
	unsigned char end_min;
	unsigned char end_sec;
	unsigned char reserve[2];
	
}search_file_setting;
	
typedef struct tag_Search_Result_Num
{
	int fileNum;
	
}search_result_num;
	
typedef struct tag_Search_Page
{
	int page;
}Search_Page;
	
typedef struct tag_File_msg_vms
{
	unsigned int CmdID;		/*message id*/
	
	unsigned int Length;	/*message length*/
	int flag;				/*0: all file; 1: size offset; 2: time offset;*/
	int start;				/*start offset*/
	int end;				/*end offset*/
	int offset;				/*file offset, in client; include header and record data*/
	char file_name[128];	/*path*/
	
}file_msg_vms;
	
typedef struct tag_PTZAction
{
	unsigned int channel;	/*channel num*/
	unsigned int cmd;		/*command id*/
	unsigned int para;		/**/
	unsigned char reserved[20];
}PTZAction;

typedef struct {
	char usrname[VS_USRNAME_LEN];	//user name
	char password[VS_PASS_LEN];		//password
}USRPSW_vms;

typedef struct
{
	char filename[20];
	unsigned int filesize;	//file real size
}configfile_info_vms;

typedef struct tag_VMSRES_MSG_HEAD
{
 	int packagelen;
	int errorcode;
}vms_resp_head; //DVR response header 
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRV_VS_VMSTCP_HEADER*/