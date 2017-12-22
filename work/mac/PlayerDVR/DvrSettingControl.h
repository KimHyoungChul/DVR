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
- (void)defaultSetting;

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
	videosetting_t *Video;
	
	int selectCh;
	unsigned char tvSystem;
	
}

- (void)InitLanguage;
- (void)defaultSetting;

- (void)setAlarm:(alarmsetting_t*)alarm andMotionDetect:(motiondetect_t*)motion andVideoSet:(videosetting_t*)videoSet;
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
- (void)defaultSetting;

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
- (void)defaultSetting;

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
	IBOutlet NSTextField *DNS2;
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
	IBOutlet NSTextField *MobileUserName;
	IBOutlet NSTextField *MobilePassword;
	IBOutlet NSMatrix *K3GType;
	IBOutlet NSTextField *K3GTelNum;
	IBOutlet NSTextField *K3GUserName;
	IBOutlet NSTextField *K3GPassword;
	IBOutlet NSTextField *K3GAPN;
	
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
	IBOutlet NSTextField *labelMobileUserName;
	IBOutlet NSTextField *labelMobilePassword;
	IBOutlet NSTextField *label3GType;
	IBOutlet NSTextField *label3GTelNum;
	IBOutlet NSTextField *label3GUserName;
	IBOutlet NSTextField *label3GPassword;
	IBOutlet NSTextField *label3GAPN;
	
	NetWorkSetting_t *Network;
	DdnsSetting_t *DDNS;
	ieclientsetting_t *IEClient;
	mobilesetting_t *Mobile;
	keyparameter_t *KeyParam;
	
}
- (void)InitLanguage;
- (void)defaultSetting;

- (void)setNetwork:(NetWorkSetting_t*)network andDdns:(DdnsSetting_t*)ddns andIE:(ieclientsetting_t*)ie andMobile:(mobilesetting_t*)mobile andKeypara:(keyparameter_t*)keyPara;
- (void)getNetwork;

- (IBAction)DDNSEnable:(id)sender;
- (IBAction)DDNSDisable:(id)sender;
- (IBAction)UseDHCP:(id)sender;
- (IBAction)UsePPPOE:(id)sender;
- (IBAction)UseStatic:(id)sender;
- (IBAction)Use3G:(id)sender;

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
	
	IBOutlet NSTextField *labelDeviceID;
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
    
    
    IBOutlet NSButton			*add;
	IBOutlet NSButton			*del;
    IBOutlet NSButton           *modify;
    	
}
- (void)InitLanguage;
- (void)defaultSetting;

- (void)setUserManage:(userpassword_t*)user andIEClient:(ieclientsetting_t*)ie;
- (void)getUserManage;

- (IBAction)PasswordVerifyEnable:(id)sender;
- (IBAction)PasswordVerifyDisable:(id)sender;
- (IBAction)RemoteVerifyEnable:(id)sender;
- (IBAction)RemoteVerifyDisable:(id)sender;


- (IBAction)add:(id)sender;
- (IBAction)del:(id)sender;
- (IBAction)modify:(id)sender;

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
- (void)defaultSetting;

- (void)setDvrState:(stateparameter_t*)state andKeyParam:(keyparameter_t*)key;

@end

@interface DvrSettingDisplay : NSObject {    
    IBOutlet NSComboBox *mainInterval;
    IBOutlet NSComboBox *spotInterval;
    IBOutlet NSComboBox *ignoreVideoloss;
    IBOutlet NSComboBox *vgaResolution;
}
- (void)InitLanguage;
- (void)defaultSetting;

- (void)setDisplay:(DVS_VGASETUP*)display;

@end

@interface DvrSettingNetwork1 : NSObject {
    IBOutlet NSComboBox *bootProtocol;
    IBOutlet NSTextField *ipAddress;
    IBOutlet NSTextField *netMask;
    IBOutlet NSTextField *gateWay;
    IBOutlet NSTextField *ntpServerAddr;
    IBOutlet NSTextField *macAddress;
    IBOutlet NSComboBox *channel;
    IBOutlet NSComboBox *subStream;
    IBOutlet NSComboBox *resolution;
    IBOutlet NSTextField *httpPort;
    IBOutlet NSTextField *mediaPort;
    IBOutlet NSTextField *intercomPort;
    IBOutlet NSTextField *mobileServicePort;
    IBOutlet NSTextField *dns1;
    IBOutlet NSTextField *dns2;
    IBOutlet NSButton *nupn;
    IBOutlet NSComboBox *quality;
    IBOutlet NSComboBox *frameRate;
    /*Email*/
    IBOutlet NSTabView *netTableView;
    IBOutlet NSButton *email;
    IBOutlet NSTextField *esmtpServer;
    IBOutlet NSTextField *from;
    IBOutlet NSTextField *emailPassword;
    IBOutlet NSTextField *smtpPort;
    IBOutlet NSTextField *to1;
    IBOutlet NSTextField *to2;
    IBOutlet NSTextField *to3;
    IBOutlet NSComboBox *emailInterval;
    IBOutlet NSButton *test;
    /*DDNS*/
    IBOutlet NSButton *ddns;    
    IBOutlet NSComboBox *ddnsType;
    IBOutlet NSTextField *ddnsName;
    IBOutlet NSTextField *ddnsDomain;
    IBOutlet NSTextField *ddnsUserName;
    IBOutlet NSTextField *ddnsPassword;
    /*PPPOE*/
    IBOutlet NSButton *pppoe;
    IBOutlet NSComboBox *dialMode;
    IBOutlet NSTextField *telephone;
    IBOutlet NSTextField *pppoeUserName;
    IBOutlet NSTextField *pppoePassword;
    IBOutlet NSTextField *apn;  
    
    DVS_NETSETUP_VER4 m_dvxNetPara;
}
- (void)InitLanguage;
- (void)defaultSetting;

- (void)setNetwork1:(char*)network1;

@end

@interface DvrSettingPtz1 : NSObject{
    
    IBOutlet NSComboBox *channelSetup;
    IBOutlet NSTextField *channelName;
    IBOutlet NSComboBox *ptzProtocol;
    IBOutlet NSComboBox *ptzBaudRate;
    IBOutlet NSTextField *ptzAddress;
    IBOutlet NSSlider *brightness;
    IBOutlet NSSlider *contrast;
    IBOutlet NSSlider *saturation;
    IBOutlet NSSlider *hue;

}
- (void)InitLanguage;
- (void)defaultSetting;

- (void)setPtz1:(DVS_CAMERASETUP*)ptz1;
- (IBAction)test:(id)sender;

@end

@interface DvrSettingSystem : NSObject{
    
    IBOutlet NSComboBox *dateFormat;
    IBOutlet NSComboBox *timeZone;
    IBOutlet NSButton *ajstWithLTime;
    IBOutlet NSTextField *deviceID;
    IBOutlet NSComboBox *language;
    IBOutlet NSComboBox *ntscPal;
    IBOutlet NSComboBox *autoLock;
    
}
- (void)InitLanguage;
- (void)defaultSetting;

- (void)setSystem:(DVS_SYSSETUP*)system;

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

    IBOutlet DvrSettingDisplay *display;
    IBOutlet DvrSettingNetwork1 *network1;
    IBOutlet DvrSettingPtz1 *ptz1;
    IBOutlet DvrSettingSystem *system;


    
	IBOutlet NSTextField *result;
	
	IBOutlet NSTabView *tabView;
	IBOutlet NSButton *applyBtn;
	IBOutlet NSButton *defaultBtn;
	
	
	systemparameter_t *systemParameters;
    dvrparameter_t    *dvrParameters;
}

- (void)InitLanguage;

- (IBAction)applySetupOption:(id)sender;
- (IBAction)defaultSetupOption:(id)sender;
- (void)updateSetting;
- (void)setSystemParameter:(systemparameter_t*)sysParam;
@end

@interface SettingPanelControl : NSObject {
	DvrSettingControl *setupWin;	

}

- (IBAction)showSettingPanel:(id)sender;

@end

