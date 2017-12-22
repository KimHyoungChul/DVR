//
//  DvrSettingControl.h
//  PlayerDVR
//
//  Created by Kelvin on 3/9/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GridSelectView.h"
#import "ScheduleView.h"
#include "kt_core_handler.h"
#include "kt_dvr_manage.h"


@interface DvrSettingRecord : NSObject {
	IBOutlet NSComboBox *Channel;
	IBOutlet NSButton *Enable;
	IBOutlet NSMatrix *Mode;
	IBOutlet NSMatrix *Audio;
	IBOutlet NSMatrix *Quality;
	IBOutlet NSMatrix *PackLen;
	IBOutlet NSMatrix *Resolution;
	IBOutlet ScheduleView *Schedule;
	
	IBOutlet NSTextField *labelChannel;
	IBOutlet NSTextField *labelMode;
	IBOutlet NSTextField *labelAudio;
	IBOutlet NSTextField *labelQuality;
	IBOutlet NSTextField *labelPacketlenth;
	IBOutlet NSTextField *labelResolution;
	IBOutlet NSBox *scheduleSetup;
	IBOutlet NSMatrix *labelRecTypeSelect;
	
	recordmode_t *RecMode;
	recordschedule_t *RecSche;
	recordset_t *RecSet;
	
	int selectCh;
	
}

- (void)setRecMode:(recordmode_t *)mode RecSet:(recordset_t*)set andRecSche:(recordschedule_t*)sche;
- (void)getRecMode;
- (void)SwitchChannel:(id)sender;

- (IBAction)MotionRecordMode:(id)sender;
- (IBAction)NormalRecordMode:(id)sender;
- (IBAction)NoRecordMode:(id)sender;
- (IBAction)ScheRecMode:(id)sender;
- (IBAction)PowerUpRecMode:(id)sender;

- (void)InitLanguage;

@end

@interface DvrSettingAlarm : NSObject {
	IBOutlet NSComboBox *Channel;
	IBOutlet NSMatrix *IOEnable;
	IBOutlet NSMatrix *MotionEnable;
	IBOutlet NSMatrix *Sensitivity;
	IBOutlet NSMatrix *VLossEnable;
	IBOutlet NSMatrix *HDDLossEnable;
	IBOutlet NSMatrix *HDDSpaceEnable;
	IBOutlet NSComboBox *VLOutput;
	IBOutlet NSComboBox *HLBuzzer;
	IBOutlet NSComboBox *HSPostRec;
	IBOutlet GridSelectView *Region;
	IBOutlet NSButton *RegionBtn;
	
	IBOutlet NSTextField *labelChannel;
	IBOutlet NSTextField *labelAllChannel;	
	IBOutlet NSTextField *labelIOAlarm;
	IBOutlet NSTextField *labelMotionDetect;
	IBOutlet NSTextField *labelSensitivity;
	IBOutlet NSTextField *labelVideoLoss;
	IBOutlet NSTextField *labelHDDLoss;
	IBOutlet NSTextField *labelHDDSpace;
	IBOutlet NSTextField *labelOutput;
	IBOutlet NSTextField *labelBuzzer;
	IBOutlet NSTextField *labelPostRec;
	IBOutlet NSBox *reginSetup;
	
	alarmsetting_t *Alarm;
	motiondetect_t *Motion;
	
	int selectCh;
	unsigned char tvSystem;
	
}

- (void)InitLanguage;

- (void)setAlarm:(alarmsetting_t*)alarm andMotionDetect:(motiondetect_t*)motion;
- (void)setTvSystem:(unsigned char)system;
- (void)getAlarmAndMDetect;
- (void)SwitchChannel:(id)sender;

- (IBAction)MotionAlarmOn:(id)sender;
- (IBAction)MotionAlarmOff:(id)sender;

- (IBAction)RegionClear:(id)sender;

@end

@interface DvrSettingEmail : NSObject {
	IBOutlet NSMatrix *Status;
	IBOutlet NSMatrix *SSL;
	IBOutlet NSTextField *SmtpPort;
	IBOutlet NSTextField *SmtpServer;
	IBOutlet NSTextField *SendMail;
	IBOutlet NSSecureTextField *SendPwd;
	IBOutlet NSTextField *RecvMail;
	
	IBOutlet NSTextField *labelStatus;
	IBOutlet NSTextField *labelSSL;
	IBOutlet NSTextField *labelSmtpPort;
	IBOutlet NSTextField *labelSmtpServer;
	IBOutlet NSTextField *labelSendEmail;
	IBOutlet NSTextField *labelSendPassword;
	IBOutlet NSTextField *labelRecvEmail;
	
	emailsetting_t *Email;
	
}

- (void)InitLanguage;

- (void)setEmail:(emailsetting_t*)mail;
- (void)getEmail;

- (IBAction)emailEnable:(id)sender;
- (IBAction)emailDisable:(id)sender;

@end

@interface DvrSettingPtz : NSObject {
	IBOutlet NSComboBox *Channel;
	IBOutlet NSMatrix *DvrProtocol;
	IBOutlet NSComboBox *Address;
	IBOutlet NSMatrix *BaudRate;
	IBOutlet NSMatrix *DataBit;
	IBOutlet NSMatrix *StopBit;
	IBOutlet NSMatrix *Verify;
	IBOutlet NSMatrix *AutoCruise;
	
	IBOutlet NSTextField *labelChannel;
	IBOutlet NSTextField *labelProtocol;
	IBOutlet NSTextField *labelAddress;
	IBOutlet NSTextField *labelBaudRate;
	IBOutlet NSTextField *labelDataBit;
	IBOutlet NSTextField *labelStopBit;
	IBOutlet NSTextField *labelVerity;
	IBOutlet NSTextField *labelAutoCruise;
	
	PTZsetting_t *PTZ;
	keyparameter_t *KeyParam;
	int selectCh;
	
}
- (void)InitLanguage;

- (void)setPTZ:(PTZsetting_t*)ptz andKeyParam:(keyparameter_t*)para;
- (void)getPTZ;
- (void)SwitchChannel:(id)sender;

@end

@interface DvrSettingNetwork : NSObject {
	IBOutlet NSMatrix *Type;
	IBOutlet NSTextField *IPAddress;
	IBOutlet NSTextField *SubNetMask;
	IBOutlet NSTextField *GateWay;
	IBOutlet NSTextField *DNS;
	IBOutlet NSComboBox *InternetBitrate;
	IBOutlet NSTextField *MediaPort;
	IBOutlet NSTextField *WebPort;
	IBOutlet NSTextField *MobilePort;
	IBOutlet NSTextField *PPPOEName;
	IBOutlet NSSecureTextField *PPPOEPwd;
	IBOutlet NSMatrix *DDNSEnable;
	IBOutlet NSComboBox *DDNSService;
	IBOutlet NSTextField *DDNSHost;
	IBOutlet NSTextField *DDNSUserName;
	IBOutlet NSSecureTextField *DDNSUserPwd;
	
	IBOutlet NSTextField *labelType;
	IBOutlet NSTextField *labelIPAddress;
	IBOutlet NSTextField *labelNetMask;
	IBOutlet NSTextField *labelGateway;
	IBOutlet NSTextField *labelDNS;
	IBOutlet NSTextField *labelInternetBitrate;
	IBOutlet NSTextField *labelMediaPort;
	IBOutlet NSTextField *labelWebPort;
	IBOutlet NSTextField *labelMobilePort;
	IBOutlet NSTextField *labelPPPOEName;
	IBOutlet NSTextField *labelPPPOEPassword;
	IBOutlet NSTextField *labelDDNS;
	IBOutlet NSTextField *labelDDNSService;
	IBOutlet NSTextField *labelDDNSHostName;
	IBOutlet NSTextField *labelDDNSUserName;
	IBOutlet NSTextField *labelDDNSPassword;
	
	NetWorkSetting_t *Network;
	DdnsSetting_t *DDNS;
	ieclientsetting_t *IEClient;
	mobilesetting_t *Mobile;
	
}
- (void)InitLanguage;

- (void)setNetwork:(NetWorkSetting_t*)network andDdns:(DdnsSetting_t*)ddns andIE:(ieclientsetting_t*)ie andMobile:(mobilesetting_t*)mobile;
- (void)getNetwork;

- (IBAction)DDNSEnable:(id)sender;
- (IBAction)DDNSDisable:(id)sender;
- (IBAction)UseDHCP:(id)sender;
- (IBAction)UsePPPOE:(id)sender;
- (IBAction)UseStatic:(id)sender;

@end

@interface DvrSettingManage : NSObject {
	IBOutlet NSTextField *DeviceID;
	IBOutlet NSMatrix *PasswordEnable;
	IBOutlet NSSecureTextField *UserPassword;
	IBOutlet NSSecureTextField *UserPassword2;
	IBOutlet NSSecureTextField *AdminPassword;
	IBOutlet NSSecureTextField *AdminPassword2;
	IBOutlet NSMatrix *RemoteEnable;
	IBOutlet NSSecureTextField *RemoteUserPwd;
	IBOutlet NSSecureTextField *RemoteUserPwd2;
	IBOutlet NSSecureTextField *RemoteAdminPwd;
	IBOutlet NSSecureTextField *RemoteAdminPwd2;
	
	IBOutlet NSTextField *labelPassword;
	IBOutlet NSTextField *labelUserPassword1;
	IBOutlet NSTextField *labelAdminPassword1;
	IBOutlet NSTextField *labelReEnter1;
	IBOutlet NSTextField *labelReEnter2;
	IBOutlet NSTextField *labelRemote;
	IBOutlet NSTextField *labelUserPassword2;
	IBOutlet NSTextField *labelAdminPassword2;
	IBOutlet NSTextField *labelReEnter3;
	IBOutlet NSTextField *labelReEnter4;
	
	userpassword_t *UserPwd;
	ieclientsetting_t *IEClient;
	
}
- (void)InitLanguage;

- (void)setUserManage:(userpassword_t*)user andIEClient:(ieclientsetting_t*)ie;
- (void)getUserManage;

- (IBAction)PasswordVerifyEnable:(id)sender;
- (IBAction)PasswordVerifyDisable:(id)sender;
- (IBAction)RemoteVerifyEnable:(id)sender;
- (IBAction)RemoteVerifyDisable:(id)sender;

@end

@interface DvrSettingInfo : NSObject {
	IBOutlet NSTextField *AppVersion;
	IBOutlet NSTextField *SerialNum;
	IBOutlet NSTextField *MACAddress;
	IBOutlet NSTextField *DeviceType;
	IBOutlet NSTextField *HddState;
	IBOutlet NSTextField *HddSize;
	
	IBOutlet NSTextField *labelSerialNum;
	IBOutlet NSTextField *labelMACAddress;
	IBOutlet NSTextField *labelSoftwareVersion;
	IBOutlet NSTextField *labelHDDState;
	IBOutlet NSTextField *labelHDD;
	
	stateparameter_t *DvrState;
	keyparameter_t *KeyParam;
	
}
- (void)InitLanguage;

- (void)setDvrState:(stateparameter_t*)state andKeyParam:(keyparameter_t*)key;

@end

@interface DvrSettingControl : NSWindowController {
	IBOutlet NSWindow *settingWin;
	IBOutlet DvrSettingRecord *record;
	IBOutlet DvrSettingAlarm *alarm;
	IBOutlet DvrSettingEmail *email;
	IBOutlet DvrSettingPtz *ptz;
	IBOutlet DvrSettingNetwork *network;
	IBOutlet DvrSettingManage *manage;
	IBOutlet DvrSettingInfo *info;
	IBOutlet NSTextField *result;
	
	IBOutlet NSTabView *tabView;
	IBOutlet NSButton *applyBtn;
	
	
	systemparameter_t *systemParameters;
}

- (void)InitLanguage;

- (IBAction)applySetupOption:(id)sender;
- (void)updateSetting;
- (void)setSystemParameter:(systemparameter_t*)sysParam;

@end

@interface SettingPanelControl : NSObject {
	DvrSettingControl *setupWin;	

}

- (IBAction)showSettingPanel:(id)sender;

@end

