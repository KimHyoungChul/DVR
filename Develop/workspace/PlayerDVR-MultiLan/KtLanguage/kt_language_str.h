/*
 *  kt_language_str.h
 *  DVRPlayer
 *
 *  Created by Kelvin on 5/16/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */
#ifndef __KT_LANGUAGE_STR_H
#define __KT_LANGUAGE_STR_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
	
/*login page*/
extern char *loginTitle;
extern char *loginServer;
extern char *loginPort;
extern char *loginPassword;
extern char *loginNetwork;
extern char *loginInternet;
extern char *loginLan;
extern char *loginSigned;
extern char *loginLanguage;
	
/*preview page*/
extern char *previewTitle;
extern char *previewVideoControl;
extern char *previewPTZControl;
extern char *previewPTZCtlZoom;
extern char *previewPTZCtlFocus;
extern char *previewPTZCtlIris;
extern char *previewSetup;
extern char *previewPlayback;
extern char *previewLogout;
extern char *previewCruiseSet;
extern char *previewCruiseStart;
extern char *previewCruiseStop;
extern char *previewCruiseRecall;
extern char *previewCruiseLoad;
extern char *previewCruiseSave;
extern char *previewCruiseClear;

/*playback page*/
extern char *playbackTitle;
extern char *playbackRefresh;
extern char *playbackSearch;
extern char *playbackDelete;
extern char *playbackOpenFS;
extern char *playbackOnline;
extern char *playbackHistroy;
extern char *playbackLocal;
extern char *playbackStartTimeStr;
extern char *playbackStop;
extern char *playbackNormal;
extern char *playbackPause;
extern char *playbackFinish;
extern char *playbackStep;
extern char *playbackNoFile;
extern char *playbackRecDay;
extern char *playbackBackup;

/*setting page*/
extern char *settingMainTitle;
extern char *settingMainApply;
extern char *settingMainRecord;
extern char *settingMainAlarm;
extern char *settingMainEmail;
extern char *settingMainPTZ;
extern char *settingMainNetwork;
extern char *settingMainManage;
extern char *settingMainInfo;
	
/*setting record page*/
extern char *settingRecordMode;
extern char *settingRecordAudio;
extern char *settingRecordQuality;
extern char *settingRecordPacketLen;
extern char *settingRecordResolution;
extern char *settingRecordChannel;
extern char *settingRecordScheSetup;
extern char *settingRecordScheMotion;
extern char *settingRecordScheRecord;
extern char *settingRecordScheNoRec;
extern char *settingRecordModeSche;
extern char *settingRecordModePowerOn;
extern char *settingRecordQualityNor;
extern char *settingRecordQualityMid;
extern char *settingRecordQualityBest;
extern char *settingRecordPacketLen15;
extern char *settingRecordPacketLen30;
extern char *settingRecordPacketLen45;
extern char *settingRecordPacketLen60;
extern char *settingRecordResolD1;
extern char *settingRecordResolHD1;
extern char *settingRecordResolCIF;

/*setting alarm page*/
extern char *settingAlarmChannel;
extern char *settingAlarmIOSwitch;
extern char *settingAlarmMotion;
extern char *settingAlarmSensitivity;
extern char *settingAlarmVideoLose;
extern char *settingAlarmHDDLose;
extern char *settingAlarmHDDSpace;
extern char *settingAlarmOutput;
extern char *settingAlarmBuzzer;
extern char *settingAlarmPostRec;
extern char *settingAlarmRegionSetup;
	
/*setting email page*/
extern char *settingEmailStatus;
extern char *settingEmailSSL;
extern char *settingEmailSmtpPort;
extern char *settingEmailSmtpServer;
extern char *settingEmailSendEmail;
extern char *settingEmailSendPw;
extern char *settingEmailRecvEmail;
	
/*setting ptz page*/
extern char *settingPTZProtocol;
extern char *settingPTZAddress;
extern char *settingPTZBaudrate;
extern char *settingPTZDataBit;
extern char *settingPTZStopBit;
extern char *settingPTZVerity;
	
/*setting network page*/
extern char *settingNetworkType;
extern char *settingNetworkTypeStatic;
extern char *settingNetworkIPAdd;
extern char *settingNetworkSubnetM;
extern char *settingNetworkGateway;
extern char *settingNetworkDNS;
extern char *settingNetworkInternetBt;
extern char *settingNetworkMediaPort;
extern char *settingNetworkWebPort;
extern char *settingNetworkMobilePort;
extern char *settingNetworkUserName;
extern char *settingNetworkPassword;
extern char *settingNetworkDDNSService;
extern char *settingNetworkDDNSHostName;
	
/*setting manage page*/
extern char *settingManageDVRPassword;
extern char *settingManageIEPassword;
extern char *settingManageUserPassword;
extern char *settingManageAdminPassword;
extern char *settingManageReEnter;
	
/*setting information*/
extern char *settingInfoSerialNum;
extern char *settingInfoMACAddress;
extern char *settingInfoVersion;
extern char *settingInfoHDDState;
extern char *settingInfoHDD;
	
/*notify information*/
extern char *loginSuccessInfo;
extern char *loginFailInfo;
extern char *setParamSuccessInfo;
extern char *setParamFailInfo;
extern char *noRecordAtMonth;
	
/*golbal string*/
extern char *golbalAllChannel;
extern char *golbalStartTime;
extern char *golbalChannel;
extern char *golbalSize;
extern char *golbalCommonRec;
extern char *golbalAlarmRec;
extern char *golbalAllTypeRec;
extern char *golbalOpen;
extern char *golbalClose;
	
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__KT_LANGUAGE_STR_H*/
