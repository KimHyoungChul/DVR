/*
 *  srv_vs_vmsdvr.h
 *  PlayerCore
 *
 *  Created by Kelvin on 2/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __SRV_VS_VMSDVR_HEADER
#define __SRV_VS_VMSDVR_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
	
#define CLR_BYTE_BIT(addr, index) 	(*(addr + (index>>3)) &= (~(((BYTE)0x01) << index % 8)))
#define SET_BYTE_BIT(addr, index) 	(*(addr + (index>>3)) |= 	(((BYTE)0x01) << index % 8))
	
#define MAX_CHN_NUMPARA 16 /*Dvr max channel*/
#define ASSIGN_FOUR_BYTES(number) (((number) + 3) / 4 * 4)
	
typedef struct _recordmode_{
	unsigned char ChnCloseOrOpen[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];	
	unsigned char quality;		/*0: high; 1: middle; 2: low*/
	unsigned char RecordMode;	/*0: sche timer; 1: start up; 2: manual*/
	unsigned char PackTime;		/*0: 15min 1: 30min 2: 45min 3: 60min*/
	unsigned char resolution;	/*0: D1; 1: HD1; 2: CIF*/
	unsigned char audio;		/*audio, 1-8bit map 1-8ch; 0: close; 1: open*/
	unsigned char reserve1[7];	/*reserve*/

}recordmode_t;
	
typedef struct _recordset_{
	unsigned char channel;		/*channel*/
	unsigned char quality;		/*0:high; 1: middle; 2: low*/
	unsigned char detail;		/*resolution, D1,HD1,CIF*/
	unsigned char fps;			/*pal fps*/
	unsigned char fpsN;			/*ntsc fps*/
	unsigned char audio;		/*audio switch*/
	unsigned char reserve[2];
	
}recordset_t;
	
typedef struct _recordschedule_{
	unsigned char hour[ASSIGN_FOUR_BYTES(7)][24]; /*0: no record; 1: normal record; 2: alarm record*/
	unsigned char reserve1[8];
	
}recordschedule_t;	
	
typedef struct _hddmanage_{
	unsigned char OverWrite; /*0: close; 1: open*/
	unsigned char reserve1[7];
	
}hddmanage_t;
	
typedef struct _systemlanguage_{
	unsigned char language; /*0: chinese; 1: english*/
	unsigned char reserve1[7];

}systemlanguage_t;
	
typedef struct _userpassword_{
	char UserPwd[8];	/*user name*/
	char UserPwd2[8];
	char AdminPwd[8];	/*admin password*/
	char AdminPwd2[8];	
	unsigned int DeviceID;		/*device id*/
	unsigned char HavePwd;		/*0: no; 1: used*/
	unsigned char reserve1[23];

}userpassword_t;

typedef struct _alarmsetting_{
	unsigned char IoAlarmSet[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];/*0: alway on; 1: alway off; 2: off*/
	unsigned char BuzzerMooTime;	/*0: 0s 1: 10s 2: 20s 3: 40s 4: 60s*/
	unsigned char AlarmOutTime;		/*0: 0s 1: 10s 2: 20s 3: 40s 4: 60s*/
	unsigned char RecordDelayTime;	/*0: 30s 1: 1Min 2: 2Min 3: 5Min*/
	unsigned char VideoLossAlarm;	/*0: close; 1: open*/
	unsigned char InvalidAlarm;		/*0: close; 1: open*/
	unsigned char NoSpaceAlarm;		/*0: close; 1: open*/
	unsigned char reserve1[6];
	
}alarmsetting_t;
	
typedef struct _showsetting_{
	/*0: left top ; 1: left down; 2: right top; 3: right down*/
	unsigned char Position[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];	
	unsigned char Preview[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];	
	/*channel name*/
	char ChnName[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)][20];
	/*0: close; 1: open*/
	unsigned char PreviewTimeSet;
	/*0: close; 1: open*/
	unsigned char RecordTimeSet;	
	/*0: close; 1: open*/
	unsigned char voice;
	unsigned char reserve1[9];

}showsetting_t;
	
typedef struct _colorsetting_{
	/*0: sedu; 1: liangdu; 2: duibidu; 3: baohedu*/
	unsigned char Color[MAX_CHN_NUMPARA][4];
	unsigned char reserve1[8];
	
}colorsetting_t;
	
/*%20: 5; %30: 10; %40: 15; %50: 20*/
typedef struct _motiondetect_{
	/*0: high; 1: higher; 2: middle; 3: low*/
	unsigned char Sensitivity[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];	
	/*0: close; 1: open*/
	unsigned char ChnSwitch[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	/*area, 180grid, 32bytes, bit map one grid*/
	unsigned char RegionSetting[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)][32];
	/*buzzer timer*/	
	unsigned char reserve[8];	

}motiondetect_t;
	
typedef struct _devicemaintain_{
	/*time*/
	unsigned char time[4];
	/*0: close; 1: open*/
	unsigned char automaintain;
	/*0: every data; 1: every week; 2: every month*/
	unsigned char maintainperiod1;
	/*0-6: week data; 7-37: date in month*/
	unsigned char maintainperiod2;
	unsigned char reserve1[9];
	
}devicemaintain_t;	
	
typedef struct _PTZsetting_{
	/*0: Pelco-D; 1: Pelco-P*/
	unsigned char Protocol[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	/*0: 1200; 1: 2400; 2: 4800; 3: 9600*/
	unsigned char Baudrate[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	/*0: 8; 1: 7; 2: 6; 3: 5*/
	unsigned char DataBit[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	/*0: 1; 1: 2*/
	unsigned char StopBit[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	/*0: None; 1: Odd; 2: Even; 3: Mark; 4: Space*/
	unsigned char Check[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];
	/*in[1,63]*/
	unsigned char Number[ASSIGN_FOUR_BYTES(MAX_CHN_NUMPARA)];

}PTZsetting_t;
	
typedef struct _NetWorkSetting_{
	/*IP adress*/
	unsigned char IPAddr[4];
	/*subnet mark*/
	unsigned char NetMask[4];
	/*gateway*/
	unsigned char GateWay[4];
	/*port*/
	unsigned int Port;
	/*Web port*/
	unsigned int WebPort;
	/*DNS*/
	unsigned char dns[4];
	/*PPPOE username max 16bytes*/
	char PPPOEuser[36];
	/*PPPOE password max 8bytes⁄*/
	char Password[36];
	/*0: DHCP; 1: PPPOE; 2: manual*/
	unsigned char NetworkMode;
	
	unsigned char Upnp;	/*upnp switch*/

	unsigned char reserve1[14];

}NetWorkSetting_t;
	
typedef struct _ddnssetting_{
	/*HOST name max 16bytes*/
	char HostName[36];
	/*server address, max 16bytes*/
	unsigned char ServerAddr[36];
	/*user name, max 16bytes*/
	char UserName[36];
	/*password, max 8bytes*/
	char UserPW[36];
	/*DDNS, 0: close; 1: open*/
	unsigned char UseDDNS;
	unsigned char reserve1[23];

}DdnsSetting_t;
	
typedef struct _timesetting_{
	/*0: MM/DD/YY; 1: YY-MM-DD*/
	unsigned char DateMode;
	/*0: 24hours; 1: 12hours*/
	unsigned char TimeMode;
	/*xialingshi, 0: close; 1: open*/
	unsigned char Dst;
	/*xialingshi mode, 0: default; 1: custom*/
	unsigned char DstMode;
	/*0: 1st week; 1: 2nd week; 2: 3rd week; 3: 4th week*/
	unsigned char StartWeek;
	/*12 months, 0-11*/
	unsigned char StartMonth;
	/*0: 1st week; 1: 2nd week; 2: 3rd week;, 3: 4th week*/
	unsigned char EndWeek;
	/*12 months, 0-11*/
	unsigned char EndMonth;
	unsigned char reserve1[8];

}TimeSetting_t;	
	
typedef struct _videosetting_{
	unsigned char resolution;/*resulotion, 0:600x480; 1: 800x600; 2:1024x768*/	
	unsigned char cameramode;/*0: TS_PAL; 1: TS_NTSC*/	
	unsigned char reserve1[6];/*reserve*/

}videosetting_t;
	
typedef struct _ie_client_setting_
{
	/*bitrate, K*/
	unsigned int BitRate;
	char UserPwd[8];//normal user password
	char AdminPwd[8];/*admin passwrod*/	
	unsigned char HavePwd;/*0: not have; 1: have*/
	/*0: (CIF 6FPS); 1: (QCIF 25FPS)*/
	unsigned char QualityOrFrame;
	unsigned char reserve1[22];
	
}ieclientsetting_t;	
	
typedef struct _mobilesetting_{
	/*0: 2.5G; 1: 2.75G; 2: 3G*/
	unsigned char network;
	/*0-7*/
	unsigned char channel;
	unsigned char reservel[2];
	
	unsigned int port;
	char usename[20];
	char usepwd[20];
	unsigned char reserve[24];

}mobilesetting_t;
	
typedef struct _emailsetting_{
	/*SMTP server*/
	char SMTP[36];
	/*sender address*/
	char SendEmail[36];
	/*password*/
	char SendEmailPW[36];
	/*recever address*/
	char RecvEmail[36];
	/*port*/
	unsigned int Port;
	/*SSL, 0: close; 1: open*/
	unsigned char SSLSwitch;
	/*0: close; 1: open*/
	unsigned char EmailSwitch;
	char reserve[2];

}emailsetting_t;
	
typedef struct _keyparameter_{
	unsigned char MacAddr[6];
	unsigned char TvSystem;/*0: TS_PAL; 1: TS_NTSC*/
	unsigned char reserve1[1];
	unsigned int UpgradeAddr;/**/
	unsigned char TimeZone;
	unsigned char reserve[116 - 3];
	
	unsigned short EnablePtzCruise16;
	unsigned char EnablePtzCruise8;
	unsigned char AutoSEQDelay[(3*MAX_CHN_NUMPARA)/2 - 1];
}keyparameter_t;
	
#define SETUP_PARAMETER_MAGIC	0x13245768
typedef struct _setupparameter_{
	unsigned int magic;/*magic code*/
	keyparameter_t KeyPara;
	recordmode_t RecordMode;
	recordset_t RecordSet[MAX_CHN_NUMPARA];
	recordschedule_t RecordSchedule[MAX_CHN_NUMPARA];
	hddmanage_t HddManage;
	userpassword_t UserPassWord;
	alarmsetting_t AlarmSetting;
	showsetting_t ShowSetting;
	colorsetting_t ColorSetting;
	motiondetect_t MotionDetect;
	devicemaintain_t DeviceMaintain;
	PTZsetting_t PTZSetting;
	systemlanguage_t LanguageSetting;
	NetWorkSetting_t NetworkSetting;
	DdnsSetting_t DDNSSetting;
	TimeSetting_t TimeSetting;
	videosetting_t VideoSetting;
	mobilesetting_t mobile;
	ieclientsetting_t ieclient;
	emailsetting_t email;
	char StringEnd[8];
}setupparameter_t;
	
typedef struct _stateparameter_{
	char DeviceType[24];
	char AppVer[24];
	char UbootVer[24];
	char KernelVer[24];
	char RootfsVer[24];	

	//3515 add this param
	char System800600logoVer[24];
	char UpdatalogoVer[24];
	char System720576logoVer[24];

	char MacAddr[24];
	char SerialNum[24];
	/*0: no hd; 1: not format; 2: normal; 3: break*/
	unsigned long HddState;
	unsigned long HddTotalSize;/*byte*/
	unsigned long HddFreeSize;
	unsigned long HddFreeRecord;
	unsigned long ParaId;/*ID*/
		
}stateparameter_t;

/**/
typedef struct _systemparameter_{
	setupparameter_t setup;/*setup*/
	stateparameter_t state;/*status*/
	
	int nSize;
	unsigned char reserve[4];
}systemparameter_t;	
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRV_VS_VMSDVR_HEADER*/