/*
 *  kt_language_str.cpp
 *  DVRPlayer
 *
 *  Created by Kelvin on 5/16/11.
 *  Copyright 2011 KangTop. All rights reserved.
 *
 */

//#include "kt_language_str.h"
#include <stdio.h>

/*login page*/
char *loginTitle	= NULL;
char *loginServer	= NULL;
char *loginPort		= NULL;
char *loginPassword	= NULL;
char *loginNetwork	= NULL;
char *loginInternet	= NULL;
char *loginLan		= NULL;
char *loginSigned	= NULL;
char *loginLanguage	= NULL;

/*preview page*/
char *previewTitle			= NULL;
char *previewVideoControl	= NULL;
char *previewPTZControl		= NULL;
char *previewPTZCtlZoom		= NULL;
char *previewPTZCtlFocus	= NULL;
char *previewPTZCtlIris		= NULL;
char *previewSetup			= NULL;
char *previewPlayback		= NULL;
char *previewLogout			= NULL;

/*playback page*/
char *playbackTitle		= NULL;
char *playbackRefresh	= NULL;
char *playbackSearch	= NULL;
char *playbackDelete	= NULL;
char *playbackOpenFS	= NULL;
char *playbackOnline	= NULL;
char *playbackHistroy	= NULL;
char *playbackLocal		= NULL;
char *playbackStartTimeStr	= NULL;
char *playbackStop		= NULL;
char *playbackNormal	= NULL;
char *playbackPause		= NULL;
char *playbackFinish	= NULL;
char *playbackStep		= NULL;
char *playbackNoFile	= NULL;
char *playbackRecDay	= NULL;
char *playbackBackup	= NULL;

/*setting page*/
char *settingMainTitle	= NULL;
char *settingMainApply	= NULL;
char *settingMainRecord	= NULL;
char *settingMainAlarm	= NULL;
char *settingMainEmail	= NULL;
char *settingMainPTZ	= NULL;
char *settingMainNetwork= NULL;
char *settingMainManage	= NULL;
char *settingMainInfo	= NULL;

/*setting record page*/
char *settingRecordMode			= NULL;
char *settingRecordAudio		= NULL;
char *settingRecordQuality		= NULL;
char *settingRecordPacketLen	= NULL;
char *settingRecordResolution	= NULL;
char *settingRecordChannel   	= NULL;
char *settingRecordScheSetup	= NULL;
char *settingRecordScheMotion	= NULL;
char *settingRecordScheRecord	= NULL;
char *settingRecordScheNoRec	= NULL;
char *settingRecordModeSche		= NULL;
char *settingRecordModePowerOn	= NULL;
char *settingRecordQualityNor	= NULL;
char *settingRecordQualityMid	= NULL;
char *settingRecordQualityBest	= NULL;
char *settingRecordPacketLen15	= NULL;
char *settingRecordPacketLen30	= NULL;
char *settingRecordPacketLen45	= NULL;
char *settingRecordPacketLen60	= NULL;
char *settingRecordResolD1		= NULL;
char *settingRecordResolHD1		= NULL;
char *settingRecordResolCIF		= NULL;

/*setting alarm page*/
char *settingAlarmChannel	= NULL;
char *settingAlarmIOSwitch	= NULL;
char *settingAlarmMotion	= NULL;
char *settingAlarmSensitivity= NULL;
char *settingAlarmVideoLose	= NULL;
char *settingAlarmHDDLose	= NULL;
char *settingAlarmHDDSpace	= NULL;
char *settingAlarmOutput	= NULL;
char *settingAlarmBuzzer	= NULL;
char *settingAlarmPostRec	= NULL;
char *settingAlarmRegionSetup= NULL;

/*setting email page*/
char *settingEmailStatus	= NULL;
char *settingEmailSSL		= NULL;
char *settingEmailSmtpPort	= NULL;
char *settingEmailSmtpServer= NULL;
char *settingEmailSendEmail	= NULL;
char *settingEmailSendPw	= NULL;
char *settingEmailRecvEmail	= NULL;

/*setting ptz page*/
char *settingPTZProtocol	= NULL;
char *settingPTZAddress		= NULL;
char *settingPTZBaudrate	= NULL;
char *settingPTZDataBit		= NULL;
char *settingPTZStopBit		= NULL;
char *settingPTZVerity		= NULL;

/*setting network page*/
char *settingNetworkType	= NULL;
char *settingNetworkTypeStatic=NULL;
char *settingNetworkIPAdd	= NULL;
char *settingNetworkSubnetM	= NULL;
char *settingNetworkGateway	= NULL;
char *settingNetworkDNS		= NULL;
char *settingNetworkInternetBt	= NULL;
char *settingNetworkMediaPort	= NULL;
char *settingNetworkWebPort		= NULL;
char *settingNetworkMobilePort	= NULL;
char *settingNetworkUserName	= NULL;
char *settingNetworkPassword	= NULL;
char *settingNetworkDDNSService	= NULL;
char *settingNetworkDDNSHostName= NULL;

/*setting manage page*/
char *settingManageDVRPassword	= NULL;
char *settingManageIEPassword	= NULL;
char *settingManageUserPassword	= NULL;
char *settingManageAdminPassword= NULL;
char *settingManageReEnter		= NULL;

/*setting information*/
char *settingInfoSerialNum	= NULL;
char *settingInfoMACAddress	= NULL;
char *settingInfoVersion	= NULL;
char *settingInfoHDDState	= NULL;
char *settingInfoHDD		= NULL;

/*notify information*/
char *loginSuccessInfo		= NULL;
char *loginFailInfo			= NULL;
char *setParamSuccessInfo	= NULL;
char *setParamFailInfo		= NULL;
char *noRecordAtMonth		= NULL;

/*golbal string*/
char *golbalAllChannel		= NULL;
char *golbalStartTime		= NULL;
char *golbalChannel			= NULL;
char *golbalSize			= NULL;
char *golbalCommonRec		= NULL;
char *golbalAlarmRec		= NULL;
char *golbalAllTypeRec		= NULL;
char *golbalOpen			= NULL;
char *golbalClose			= NULL;






