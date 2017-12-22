//
//  DvrSettingControl.m
//  PlayerDVR
//
//  Created by Kelvin on 3/9/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "DvrSettingControl.h"
#import "DvrManage.h"
#import "CustomButton.h"
#include "kt_dvr_manage.h"
#include "kt_language_str.h"

DvrManage* getDvrManage();
void dumpDvrConfig(DvrConfig *cfg);

@implementation DvrSettingControl

- (void)awakeFromNib {
	NSLog(@"awakeFromNib DvrSettingControl");
	systemParameters = NULL;
	
	systemparameter_t *pSysParam = kt_dvrMgr_getCurDvrParam();
	
	if(pSysParam)
	{
		if(systemParameters == NULL)
		{
			systemParameters = (systemparameter_t*)malloc(sizeof(systemparameter_t));
			if(systemParameters == NULL)
			{
				NSLog(@"xxxxxxxxxxxxxxxxxx not enought memory");
				return;
			}
		}
		memcpy(systemParameters, pSysParam, sizeof(systemparameter_t));
		NSLog(@"-===============->>>mobile port: %d", systemParameters->setup.mobile.port);
		[result setStringValue:@" "];
		[record setRecMode:&(systemParameters->setup.RecordMode) RecSet:systemParameters->setup.RecordSet andRecSche:systemParameters->setup.RecordSchedule];
		[alarm setAlarm:&(systemParameters->setup.AlarmSetting) andMotionDetect:&(systemParameters->setup.MotionDetect)];
		[alarm setTvSystem:systemParameters->setup.KeyPara.TvSystem];
		[email setEmail:&(systemParameters->setup.email)];
		[ptz setPTZ:&(systemParameters->setup.PTZSetting) andKeyParam:&(systemParameters->setup.KeyPara)];
		[network setNetwork:&(systemParameters->setup.NetworkSetting) andDdns:&(systemParameters->setup.DDNSSetting) andIE:&(systemParameters->setup.ieclient) andMobile:&(systemParameters->setup.mobile)];
		[manage setUserManage:&(systemParameters->setup.UserPassWord) andIEClient:&(systemParameters->setup.ieclient)];
		[info setDvrState:&(systemParameters->state) andKeyParam:&(systemParameters->setup.KeyPara)];
		
	}
	
	[self InitLanguage];
	
}

- (IBAction)applySetupOption:(id)sender {
	int retTmp;
	DvrConfig *cfg;
	NSLog(@"apply setup option");
	
	cfg = kt_dvrMgr_getCurDvr();

	if(systemParameters)
	{
		[record getRecMode];
		[alarm getAlarmAndMDetect];
		[email getEmail];
		[ptz getPTZ];
		[network getNetwork];
		[manage getUserManage];

		retTmp = kt_dvrMgr_setCurDvrParam(systemParameters);
		if(retTmp == 0)
		{
			/*show dialog*/
			NSAlert* alert = [[NSAlert alloc] init];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert setMessageText:@"Alert"];
			if(setParamSuccessInfo)
			{
				NSString *str = [[NSString alloc] initWithUTF8String:setParamSuccessInfo];
				[alert setInformativeText:str];
				[str release];
			}
			else
			{
				[alert setInformativeText:@"Set DVR parameters success!"];
			}
			[alert addButtonWithTitle:@"OK"];
			[alert runModal];
			[alert release];
			systemParameters->state.ParaId += 1;
		}
		else if(retTmp < 0)
		{
			/*show dialog*/
			NSAlert* alert = [[NSAlert alloc] init];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert setMessageText:@"Alert"];
			if(setParamFailInfo)
			{
				NSString *str = [[NSString alloc] initWithUTF8String:setParamFailInfo];
				[alert setInformativeText:str];
				[str release];
			}
			else
			{
				[alert setInformativeText:@"Set DVR parameters error!"];
			}
			[alert addButtonWithTitle:@"OK"];
			NSInteger ret = [alert runModal];
			if(ret == NSAlertFirstButtonReturn)
			{
				[alert release];
				return;
			}
			[alert release];
			return;
		}
	}
}

- (void)setSystemParameter:(systemparameter_t*)sysParam {
	if(sysParam == NULL)
	{
		return;
	}
	
	if(systemParameters)
	{
		/*maybe release the resource*/
		free(systemParameters);
		systemParameters = NULL;
	}
	
	systemParameters = sysParam;
}

- (void)updateSetting {
	systemparameter_t *pSysParam = NULL;
	
	pSysParam = kt_dvrMgr_getCurDvrParam();
	if(systemParameters)
	{
		memcpy(systemParameters, pSysParam, sizeof(systemparameter_t));
		//NSLog(@"get parameter success");
		//NSLog(@"mobile port: %d", systemParameters->setup.mobile.port);
		[result setStringValue:@" "];
		[record setRecMode:&(systemParameters->setup.RecordMode) RecSet:systemParameters->setup.RecordSet andRecSche:systemParameters->setup.RecordSchedule];
		[alarm setAlarm:&(systemParameters->setup.AlarmSetting) andMotionDetect:&(systemParameters->setup.MotionDetect)];
		[alarm setTvSystem:systemParameters->setup.KeyPara.TvSystem];
		[email setEmail:&(systemParameters->setup.email)];
		[ptz setPTZ:&(systemParameters->setup.PTZSetting) andKeyParam:&(systemParameters->setup.KeyPara)];
		[network setNetwork:&(systemParameters->setup.NetworkSetting) andDdns:&(systemParameters->setup.DDNSSetting) andIE:&(systemParameters->setup.ieclient) andMobile:&(systemParameters->setup.mobile)];
		[manage setUserManage:&(systemParameters->setup.UserPassWord) andIEClient:&(systemParameters->setup.ieclient)];
		[info setDvrState:&(systemParameters->state) andKeyParam:&(systemParameters->setup.KeyPara)];
	}
	else
	{
		NSLog(@"get parameter error");
	}	
}

- (void)InitLanguage {
	NSLog(@"VideoController InitLanguage");
	NSString *str = nil;
	
	if(settingMainTitle)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainTitle];
		[applyBtn.window setTitle:str];
		[str release];
	}	
	
	if(settingMainRecord)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainRecord];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:0];
		[item setLabel:str];		
		[str release];
	}
	if(settingMainAlarm)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainAlarm];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:1];
		[item setLabel:str];		
		[str release];
	}	
	if(settingMainEmail)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainEmail];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:2];
		[item setLabel:str];		
		[str release];
	}	
	if(settingMainPTZ)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainPTZ];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:3];
		[item setLabel:str];		
		[str release];
	}	
	if(settingMainNetwork)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainNetwork];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:4];
		[item setLabel:str];		
		[str release];
	}	
	if(settingMainManage)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainManage];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:5];
		[item setLabel:str];		
		[str release];
	}
	if(settingMainInfo)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainInfo];
		NSTabViewItem *item = [tabView tabViewItemAtIndex:6];
		[item setLabel:str];		
		[str release];
	}	
	if(settingMainApply)
	{
		str = [[NSString alloc] initWithUTF8String:settingMainApply];
		[applyBtn setTitle:str];	
		[str release];
	}	
	
}

- (void)dealloc {
	NSLog(@"xxxxxxxxxxxxxxxx>>>>>>DvrSettingControl dealloc");
	if(systemParameters)
	{
		free(systemParameters);
		systemParameters = NULL;
	}
	
	[super dealloc];
}

@end

@implementation DvrSettingRecord

- (void)awakeFromNib {
	NSLog(@"DvrSettingRecord awakeFromNib");

	RecMode = NULL;
	RecSche = NULL;
	
	[self InitLanguage];
}

- (void)InitLanguage {
	NSString *str = nil;
		
	if(settingRecordMode)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordMode];
		[labelMode setStringValue:str];	
		[str release];
	}		
	if(settingRecordAudio)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordAudio];
		[labelAudio setStringValue:str];	
		[str release];
	}		
	if(settingRecordQuality)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordQuality];
		[labelQuality setStringValue:str];	
		[str release];
	}
	if(settingRecordPacketLen)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordPacketLen];
		[labelPacketlenth setStringValue:str];	
		[str release];
	}
	if(settingRecordResolution)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordResolution];
		[labelResolution setStringValue:str];	
		[str release];
	}
	if(settingRecordChannel)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordChannel];
		[labelChannel setStringValue:str];	
		[str release];
	}
	if(settingRecordScheSetup)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordScheSetup];
		[scheduleSetup setTitle:str];	
		[str release];
	}
	if(settingRecordScheMotion)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordScheMotion];
		NSCell *cell = (NSCell*)[labelRecTypeSelect cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordScheRecord)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordScheRecord];
		NSCell *cell = (NSCell*)[labelRecTypeSelect cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordScheNoRec)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordScheNoRec];
		NSCell *cell = (NSCell*)[labelRecTypeSelect cellAtRow:0 column:2];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordModeSche)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordModeSche];
		NSCell *cell = (NSCell*)[Mode cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordModePowerOn)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordModePowerOn];
		NSCell *cell = (NSCell*)[Mode cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(golbalOpen)
	{
		str = [[NSString alloc] initWithUTF8String:golbalOpen];
		NSCell *cell = (NSCell*)[Audio cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(golbalClose)
	{
		str = [[NSString alloc] initWithUTF8String:golbalClose];
		NSCell *cell = (NSCell*)[Audio cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordQualityNor)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordQualityNor];
		NSCell *cell = (NSCell*)[Quality cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordQualityMid)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordQualityMid];
		NSCell *cell = (NSCell*)[Quality cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordQualityBest)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordQualityBest];
		NSCell *cell = (NSCell*)[Quality cellAtRow:0 column:2];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordPacketLen15)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordPacketLen15];
		NSCell *cell = (NSCell*)[PackLen cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordPacketLen30)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordPacketLen30];
		NSCell *cell = (NSCell*)[PackLen cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordPacketLen45)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordPacketLen45];
		NSCell *cell = (NSCell*)[PackLen cellAtRow:1 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(settingRecordPacketLen60)
	{
		str = [[NSString alloc] initWithUTF8String:settingRecordPacketLen60];
		NSCell *cell = (NSCell*)[PackLen cellAtRow:1 column:1];
		[cell setTitle:str];
		[str release];
	}
	
}

- (void)setRecMode:(recordmode_t *)mode RecSet:(recordset_t*)set andRecSche:(recordschedule_t*)sche {
	int i;
	DvrConfig *cfg = kt_dvrMgr_getCurDvr();
	
	RecMode = mode;
	RecSche = sche;
	RecSet = set;
	selectCh = 0;

	/*update UI*/
	NSLog(@"Record number of items: %d", cfg->channelNum);
	if(([Channel numberOfItems]) == 16)
	{
		for(i = 0; i < 16 - cfg->channelNum; i++)
		{
			[Channel removeItemAtIndex:cfg->channelNum];
		}	
	}
	[Channel selectItemAtIndex:selectCh];	
	if(RecMode->ChnCloseOrOpen[selectCh] == 1)
	{
		[Enable setState:NSOnState];
	}
	else
	{
		[Enable setState:NSOffState];
	}
	
	//[Schedule setSchedule:(unsigned char*)RecSche[selectCh].hour];
	
	/*record quality*/
	if(RecSet[selectCh].quality == 0) /*high*/
	{
		[Quality selectCellAtRow:0 column:2];
	}
	else if(RecSet[selectCh].quality == 1) /*middle*/
	{
		[Quality selectCellAtRow:0 column:1];
	}
	else if(RecSet[selectCh].quality == 2) /*low*/
	{
		[Quality selectCellAtRow:0 column:0];
	}
	
	/*Record mode*/
	if(RecMode->RecordMode == 0) /*sche*/
	{
		[Mode selectCellAtRow:0 column:0];
		NSLog(@"xxxxxxxxxxxxxxxxxxxxxxxxxxxxx>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		[Schedule setIsEnable:YES];
		[Schedule setSchedule:(unsigned char*)RecSche[selectCh].hour];
	}
	else if(RecMode->RecordMode == 1) /*power up*/
	{
		[Mode selectCellAtRow:0 column:1];
		[Schedule setIsEnable:NO];
	}
	else if(RecMode->RecordMode == 2) /*manual*/
	{
		/**/
		NSLog(@"unsupport");
	}
	
	/*packet time*/
	if(RecMode->PackTime == 0) /*15min*/
	{
		[PackLen selectCellAtRow:0 column:0];
	}
	else if(RecMode->PackTime == 1) /*30min*/
	{
		[PackLen selectCellAtRow:0 column:1];
	}
	else if(RecMode->PackTime == 2) /*45min*/
	{
		[PackLen selectCellAtRow:1 column:0];
	}
	else if(RecMode->PackTime == 3) /*60min*/
	{
		[PackLen selectCellAtRow:1 column:1];
	}
	
	/*audio*/
	if(RecMode->audio == 0) /*Off*/
	{
		[Audio selectCellAtRow:0 column:1];
	}
	else if(RecMode->audio == 1) /*On*/
	{
		[Audio selectCellAtRow:0 column:0];
	}
	
	/*resolution*/
	if(RecSet[selectCh].detail == 0) /*D1*/
	{
		[Resolution selectCellAtRow:0 column:0];
	}
	else if(RecSet[selectCh].detail == 1) /*HD1*/
	{
		[Resolution selectCellAtRow:0 column:1];
	}
	else if(RecSet[selectCh].detail == 2) /*D1*/
	{
		[Resolution selectCellAtRow:0 column:2];
	}

}

- (void)getRecMode {
	int column, row;
	unsigned char* tSche;
	NSLog(@"getRecMode");
	
//	if([Enable state] == NSOnState)
//	{
//		RecMode->ChnCloseOrOpen[selectCh] = 1;
//	}
//	else if([Enable state] == NSOffState)
//	{
//		RecMode->ChnCloseOrOpen[selectCh] = 0;
//	}
	
	column = [Resolution selectedColumn];
	row = [Resolution selectedRow];
	RecMode->resolution = column;
	
	tSche = [Schedule getSchedule];
	memcpy(RecSche[selectCh].hour, tSche, 7*24);
	
	column = [Audio selectedColumn];
	row = [Audio selectedRow];
	if(column == 1)
	{
		RecMode->audio = 0;
	}
	else if(column == 0)
	{
		RecMode->audio = 1;
	}
	
	column = [PackLen selectedColumn];
	row = [PackLen selectedRow];
	if(row == 0 && column == 0)
	{
		RecMode->PackTime = 0;
	}
	else if(row == 0 && column == 1)
	{
		RecMode->PackTime = 1;
	}
	else if(row == 1 && column == 0)
	{
		RecMode->PackTime = 2;
	}
	else if(row == 1 && column == 1)
	{
		RecMode->PackTime = 3;
	}
	
	column = [Mode selectedColumn];
	row = [Mode selectedRow];
	RecMode->RecordMode = column;
	
	column = [Quality selectedColumn];
	row = [Quality selectedRow];
	if(row == 0 && column == 0)
	{
		RecSet[selectCh].quality = 2;
	}
	else if(row == 0 && column == 1)
	{
		RecSet[selectCh].quality = 1;
	}
	else if(row == 0 && column == 2)
	{
		RecSet[selectCh].quality = 0;
	}
	
}

- (void)SwitchChannel:(id)sender {
	int idx;
	NSLog(@"switch channel");

	idx = [Channel indexOfSelectedItem];
	NSLog(@"index of selected item: %d", idx);
	if(idx != selectCh)
	{
		[self getRecMode];
	
		selectCh = idx;
	
		/*update ui*/
		if(RecMode->ChnCloseOrOpen[selectCh] == 1)
		{
			[Enable setState:NSOnState];
		}
		else
		{
			[Enable setState:NSOffState];
		}
		
		[Schedule setSchedule:(unsigned char*)RecSche[selectCh].hour];
		
		/*resolution*/
		if(RecSet[selectCh].detail == 0) /*D1*/
		{
			[Resolution selectCellAtRow:0 column:0];
		}
		else if(RecSet[selectCh].detail == 1) /*HD1*/
		{
			[Resolution selectCellAtRow:0 column:1];
		}
		else if(RecSet[selectCh].detail == 2) /*D1*/
		{
			[Resolution selectCellAtRow:0 column:2];
		}
		
		/*record quality*/
		if(RecSet[selectCh].quality == 0) /*high*/
		{
			[Quality selectCellAtRow:0 column:2];
		}
		else if(RecSet[selectCh].quality == 1) /*middle*/
		{
			[Quality selectCellAtRow:0 column:1];
		}
		else if(RecSet[selectCh].quality == 2) /*low*/
		{
			[Quality selectCellAtRow:0 column:0];
		}
	}
	
}

- (IBAction)MotionRecordMode:(id)sender {
	NSLog(@"Motion Record Mode");
	[Schedule setMotionRecMode];
}

- (IBAction)NormalRecordMode:(id)sender {
	NSLog(@"Normal Record Mode");
	[Schedule setNormalRecMode];
}

- (IBAction)NoRecordMode:(id)sender {
	NSLog(@"No Record Mode");
	[Schedule setNoRecordMode];
}

- (IBAction)ScheRecMode:(id)sender {
	NSLog(@"Schedule Record Mode");
	[Schedule setIsEnable:YES];
	[Schedule setSchedule:(unsigned char*)RecSche[selectCh].hour];
}

- (IBAction)PowerUpRecMode:(id)sender {
	NSLog(@"PowerUp Record Mode");
	[Schedule setIsEnable:NO];
}

- (void)dealloc {
	NSLog(@"DvrSettingRecord dealloc");

	[super dealloc];
}

@end

@implementation DvrSettingAlarm

- (void)awakeFromNib {
	NSLog(@"awakeFromNib DvrSettingAlarm");
	
	Alarm = NULL;
	Motion = NULL;
	selectCh = 0;
	
	[self InitLanguage];
}

- (void)InitLanguage {
	NSLog(@"DvrSettingAlarm InitLanguage");
	NSString *str = nil;
	
	if(golbalOpen)
	{
		str = [[NSString alloc] initWithUTF8String:golbalOpen];
		NSCell *cell = (NSCell*)[IOEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		cell = (NSCell*)[MotionEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		cell = (NSCell*)[VLossEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		cell = (NSCell*)[HDDLossEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		cell = (NSCell*)[HDDSpaceEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(golbalClose)
	{
		str = [[NSString alloc] initWithUTF8String:golbalClose];
		NSCell *cell = (NSCell*)[IOEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		cell = (NSCell*)[MotionEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		cell = (NSCell*)[VLossEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		cell = (NSCell*)[HDDLossEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		cell = (NSCell*)[HDDSpaceEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	
	if(settingAlarmChannel)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmChannel];
		[labelChannel setStringValue:str];	
		[str release];
	}
	if(golbalAllChannel)
	{
		str = [[NSString alloc] initWithUTF8String:golbalAllChannel];
		[labelAllChannel setStringValue:str];
		[str release];
	}
	if(settingAlarmIOSwitch)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmIOSwitch];
		[labelIOAlarm setStringValue:str];	
		[str release];
	}
	if(settingAlarmMotion)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmMotion];
		[labelMotionDetect setStringValue:str];	
		[str release];
	}
	if(settingAlarmSensitivity)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmSensitivity];
		[labelSensitivity setStringValue:str];	
		[str release];
	}
	if(settingAlarmVideoLose)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmVideoLose];
		[labelVideoLoss setStringValue:str];	
		[str release];
	}
	if(settingAlarmHDDLose)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmHDDLose];
		[labelHDDLoss setStringValue:str];	
		[str release];
	}
	if(settingAlarmHDDSpace)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmHDDSpace];
		[labelHDDSpace setStringValue:str];	
		[str release];
	}
	if(settingAlarmOutput)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmOutput];
		[labelOutput setStringValue:str];	
		[str release];
	}
	if(settingAlarmBuzzer)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmBuzzer];
		[labelBuzzer setStringValue:str];	
		[str release];
	}
	if(settingAlarmPostRec)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmPostRec];
		[labelPostRec setStringValue:str];	
		[str release];
	}
	if(settingAlarmRegionSetup)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmRegionSetup];
		[reginSetup setTitle:str];	
		[str release];
	}
}

- (void)setAlarm:(alarmsetting_t*)alarm andMotionDetect:(motiondetect_t*)motion {
	int i;
	DvrConfig *cfg = kt_dvrMgr_getCurDvr();
	
	Alarm = alarm;
	Motion = motion;
	
	/*update ui*/
	if(([Channel numberOfItems]) == 16)
	{
		for(i = 0; i < 16 - cfg->channelNum; i++)
		{
			[Channel removeItemAtIndex:cfg->channelNum];
		}
	}
	[Channel selectItemAtIndex:selectCh];
	
	if(Motion->Sensitivity[selectCh] == 0) /*higher*/
	{
		[Sensitivity selectCellAtRow:1 column:1];
	}
	else if(Motion->Sensitivity[selectCh] == 1) /*high*/
	{
		[Sensitivity selectCellAtRow:1 column:0];
	}
	else if(Motion->Sensitivity[selectCh] == 2) /*middle*/
	{
		[Sensitivity selectCellAtRow:0 column:1];
	}
	else if(Motion->Sensitivity[selectCh] == 3) /*low*/
	{
		[Sensitivity selectCellAtRow:0 column:0];
	}
	
	if(Motion->ChnSwitch[selectCh] == 0) /*off*/
	{
		[MotionEnable selectCellAtRow:0 column:1];
	}
	else if(Motion->ChnSwitch[selectCh] == 1) /*on*/
	{
		[MotionEnable selectCellAtRow:0 column:0];
	}
	
	//[Region setRegionMap:Motion->RegionSetting[selectCh]];
	
	if(Alarm->IoAlarmSet[selectCh] == 0) /*alway on*/
	{
		[IOEnable selectCellAtRow:0 column:0];
		[Region setIsEnable:YES];
		[Region setRegionMap:Motion->RegionSetting[selectCh]];
	}
	else if(Alarm->IoAlarmSet[selectCh] == 1) /*alway off*/
	{
		[IOEnable selectCellAtRow:0 column:1];
		[Region setIsEnable:NO];
	}
	else if(Alarm->IoAlarmSet[selectCh] == 2) /*off*/
	{
		[IOEnable selectCellAtRow:0 column:1];
	}
	
	if(Alarm->VideoLossAlarm == 0) /*Off*/
	{
		[VLossEnable selectCellAtRow:0 column:1];
	}
	else if(Alarm->VideoLossAlarm == 1) /*On*/
	{
		[VLossEnable selectCellAtRow:0 column:0];
	}
	
	if(Alarm->InvalidAlarm == 0) /*Off*/
	{
		[HDDLossEnable selectCellAtRow:0 column:1];
	}
	else if(Alarm->InvalidAlarm == 1) /*On*/
	{
		[HDDLossEnable selectCellAtRow:0 column:0];
	}
	
	if(Alarm->NoSpaceAlarm == 0) /*Off*/
	{
		[HDDSpaceEnable selectCellAtRow:0 column:1];
	}
	else if(Alarm->NoSpaceAlarm == 1) /*On*/
	{
		[HDDSpaceEnable selectCellAtRow:0 column:0];
	}

	if(Alarm->BuzzerMooTime == 0) /*0s*/
	{
		[HLBuzzer selectItemAtIndex:0];
	}
	else if(Alarm->BuzzerMooTime == 1) /*10s*/
	{
		[HLBuzzer selectItemAtIndex:1];
	}
	else if(Alarm->BuzzerMooTime == 2) /*20s*/
	{
		[HLBuzzer selectItemAtIndex:2];
	}
	else if(Alarm->BuzzerMooTime == 3) /*40s*/
	{
		[HLBuzzer selectItemAtIndex:3];
	}
	else if(Alarm->BuzzerMooTime == 4) /*60s*/
	{
		[HLBuzzer selectItemAtIndex:4];
	}
	
	if(Alarm->AlarmOutTime == 0) /*0s*/
	{
		[VLOutput selectItemAtIndex:0];
	}
	else if(Alarm->AlarmOutTime == 1) /*10s*/
	{
		[VLOutput selectItemAtIndex:1];
	}
	else if(Alarm->AlarmOutTime == 2) /*20s*/
	{
		[VLOutput selectItemAtIndex:2];
	}
	else if(Alarm->AlarmOutTime == 3) /*40s*/
	{
		[VLOutput selectItemAtIndex:3];
	}
	else if(Alarm->AlarmOutTime == 4) /*60s*/
	{
		[VLOutput selectItemAtIndex:4];
	}
	
	if(Alarm->RecordDelayTime == 0) /*30s*/
	{
		[HSPostRec selectItemAtIndex:0];
	}
	else if(Alarm->RecordDelayTime == 1) /*1Min*/
	{
		[HSPostRec selectItemAtIndex:1];
	}
	else if(Alarm->RecordDelayTime == 2) /*2Min*/
	{
		[HSPostRec selectItemAtIndex:2];
	}
	else if(Alarm->RecordDelayTime == 3) /*5Min*/
	{
		[HSPostRec selectItemAtIndex:3];
	}
	
}

- (void)getAlarmAndMDetect {
	int idx;
	int column, row;
	unsigned char *regMap;
	NSLog(@"getAlarmAndMDetect");
	
	idx = [HSPostRec indexOfSelectedItem];
	Alarm->RecordDelayTime = idx;
	idx = [VLOutput indexOfSelectedItem];
	Alarm->AlarmOutTime = idx;
	idx = [HLBuzzer indexOfSelectedItem];
	Alarm->BuzzerMooTime = idx;
	
	column = [HDDSpaceEnable selectedColumn];
	row = [HDDSpaceEnable selectedRow];
	if(column == 0)
	{
		Alarm->NoSpaceAlarm = 1;
	}
	else if(column == 1)
	{
		Alarm->NoSpaceAlarm = 0;
	}
	column = [HDDLossEnable selectedColumn];
	row = [HDDLossEnable selectedRow];
	if(column == 0)
	{
		Alarm->InvalidAlarm = 1;
	}
	else if(column == 1)
	{
		Alarm->InvalidAlarm = 0;
	}
	column = [VLossEnable selectedColumn];
	row = [VLossEnable selectedRow];
	if(column == 0)
	{
		Alarm->VideoLossAlarm = 1;
	}
	else if(column == 1)
	{
		Alarm->VideoLossAlarm = 0;
	}
	
	column = [Sensitivity selectedColumn];
	row = [Sensitivity selectedRow];
	if(row == 0 && column == 0)
	{
		Motion->Sensitivity[selectCh] = 3;
	}
	else if(row == 0 && column == 1)
	{
		Motion->Sensitivity[selectCh] = 2;
	}
	else if(row == 1 && column == 0)
	{
		Motion->Sensitivity[selectCh] = 1;
	}
	else if(row == 1 && column == 1)
	{
		Motion->Sensitivity[selectCh] = 0;
	}
	
	column = [MotionEnable selectedColumn];
	row = [MotionEnable selectedRow];
	if(column == 0)
	{
		Motion->ChnSwitch[selectCh] = 1;
	}
	else if(column == 1)
	{
		Motion->ChnSwitch[selectCh] = 0;
	}
	
	regMap = [Region getRegionMap];
	memcpy(Motion->RegionSetting[selectCh], regMap, 32);
	
	column = [IOEnable selectedColumn];
	row = [IOEnable selectedRow];
	if(row == 0 && column == 0)
	{
		Alarm->IoAlarmSet[selectCh] = 0;
	}
	else if(row == 0 && column == 1)
	{
		Alarm->IoAlarmSet[selectCh] = 1;
	}
	else if(row == 0 && column == 2)
	{
	
	}
}

- (void)SwitchChannel:(id)sender {
	int idx;
	NSLog(@"switch channel");
	
	idx = [Channel indexOfSelectedItem];
	NSLog(@"index of selected item: %d", idx);
	if(idx != selectCh)
	{
		[self getAlarmAndMDetect];
		
		selectCh = idx;
		
		/*update ui*/
		[Region setRegionMap:Motion->RegionSetting[selectCh]];		
		if(Motion->Sensitivity[selectCh] == 0) /*higher*/
		{
			[Sensitivity selectCellAtRow:1 column:1];
		}
		else if(Motion->Sensitivity[selectCh] == 1) /*high*/
		{
			[Sensitivity selectCellAtRow:1 column:0];
		}
		else if(Motion->Sensitivity[selectCh] == 2) /*middle*/
		{
			[Sensitivity selectCellAtRow:0 column:1];
		}
		else if(Motion->Sensitivity[selectCh] == 3) /*low*/
		{
			[Sensitivity selectCellAtRow:0 column:0];
		}
		
		if(Motion->ChnSwitch[selectCh] == 0) /*off*/
		{
			[MotionEnable selectCellAtRow:0 column:1];
		}
		else if(Motion->ChnSwitch[selectCh] == 1) /*on*/
		{
			[MotionEnable selectCellAtRow:0 column:0];
		}
		
		if(Alarm->IoAlarmSet[selectCh] == 0) /*alway on*/
		{
			[IOEnable selectCellAtRow:0 column:0];
		}
		else if(Alarm->IoAlarmSet[selectCh] == 1) /*alway off*/
		{
			[IOEnable selectCellAtRow:0 column:1];
		}
		else if(Alarm->IoAlarmSet[selectCh] == 2) /*off*/
		{
			[IOEnable selectCellAtRow:0 column:1];
		}
		
	}

}

- (IBAction)RegionClear:(id)sender {
	[Region clearRegoin];
}

- (IBAction)MotionAlarmOn:(id)sender {
	NSLog(@"DvrSettingAlarm MotionAlarmOn");
	
	[Region setIsEnable:YES];
	[Region setRegionMap:Motion->RegionSetting[selectCh]];
}

- (IBAction)MotionAlarmOff:(id)sender {
	NSLog(@"DvrSettingAlarm MotionAlarmOff");
	
	[Region setIsEnable:NO];
}

- (void)setTvSystem:(unsigned char)system {
	tvSystem = system;
	
	if(tvSystem == 1)
	{
		[Region setColumn:15 andRow:10];
	}
	else
	{
		[Region setColumn:15 andRow:12];
	}
}

- (void)dealloc {
	NSLog(@"DvrSettingAlarm dealloc");
	
	[super dealloc];
}

@end

@implementation DvrSettingEmail

- (void)awakeFromNib {
	NSLog(@"DvrSettingEmail awakeFromNib");
	Email = NULL;
	[self InitLanguage];
	
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(golbalOpen)
	{
		str = [[NSString alloc] initWithUTF8String:golbalOpen];
		NSCell *cell = (NSCell*)[Status cellAtRow:0 column:0];
		[cell setTitle:str];
		cell = (NSCell*)[SSL cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(golbalClose)
	{
		str = [[NSString alloc] initWithUTF8String:golbalClose];
		NSCell *cell = (NSCell*)[Status cellAtRow:0 column:1];
		[cell setTitle:str];
		cell = (NSCell*)[SSL cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(settingEmailStatus)
	{
		str = [[NSString alloc] initWithUTF8String:settingEmailStatus];
		[labelStatus setStringValue:str];	
		[str release];
	}
	if(settingEmailSSL)
	{
		str = [[NSString alloc] initWithUTF8String:settingEmailSSL];
		[labelSSL setStringValue:str];	
		[str release];
	}
	if(settingEmailSmtpPort)
	{
		str = [[NSString alloc] initWithUTF8String:settingEmailSmtpPort];
		[labelSmtpPort setStringValue:str];	
		[str release];
	}
	if(settingEmailSmtpServer)
	{
		str = [[NSString alloc] initWithUTF8String:settingEmailSmtpServer];
		[labelSmtpServer setStringValue:str];	
		[str release];
	}
	if(settingEmailSendEmail)
	{
		str = [[NSString alloc] initWithUTF8String:settingEmailSendEmail];
		[labelSendEmail setStringValue:str];	
		[str release];
	}
	if(settingEmailSendPw)
	{
		str = [[NSString alloc] initWithUTF8String:settingEmailSendPw];
		[labelSendPassword setStringValue:str];	
		[str release];
	}
	if(settingEmailRecvEmail)
	{
		str = [[NSString alloc] initWithUTF8String:settingEmailRecvEmail];
		[labelRecvEmail setStringValue:str];	
		[str release];
	}
}

- (void)setEmail:(emailsetting_t*)mail {
	Email = mail;
	
	/*update ui*/
	if(Email->EmailSwitch == 0) /*Off*/
	{
		[Status selectCellAtRow:0 column:1];
	}
	else if(Email->EmailSwitch == 1) /*On*/
	{
		[Status selectCellAtRow:0 column:0];
	}
	
	if(Email->SSLSwitch == 0) /*Off*/
	{
		[SSL selectCellAtRow:0 column:1];
	}
	else if(Email->SSLSwitch == 1) /*On*/
	{
		[SSL selectCellAtRow:0 column:0];
	}
	[SmtpPort setIntValue:(int)Email->Port];
	NSString *tmp = [[NSString alloc] initWithUTF8String:Email->SMTP];
	[SmtpServer setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:Email->SendEmail];
	[SendMail setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:Email->SendEmailPW];
	[SendPwd setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:Email->RecvEmail];
	[RecvMail setStringValue:tmp];
	[tmp release];
	
	if(Email->EmailSwitch == 0)
	{
		[SSL setEnabled:NO];
		[SmtpPort setEnabled:NO];
		[SmtpServer setEnabled:NO];
		[SendMail setEnabled:NO];
		[SendPwd setEnabled:NO];
		[RecvMail setEnabled:NO];
	}
}

- (void)getEmail {
	NSLog(@"get Email");
	int column, row;
	NSString *tmp;
	char *tmpUTF8;
	
	column = [Status selectedColumn];
	row = [Status selectedRow];
	if(column == 0 && row == 0)
	{
		Email->EmailSwitch = 1;
	}
	else if(column == 1 && row == 0)
	{
		Email->EmailSwitch = 0;
	}
	
	column = [SSL selectedColumn];
	row = [SSL selectedRow];
	if(column == 0 && row == 0)
	{
		Email->SSLSwitch = 1;
	}
	else if(column == 1 && row == 0)
	{
		Email->SSLSwitch = 0;
	}
	
	Email->Port = [SmtpPort intValue];
	
	tmp = [SmtpServer stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(Email->SMTP, tmpUTF8);
	
	tmp = [SendMail stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(Email->SendEmail, tmpUTF8);
	
	tmp = [RecvMail stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(Email->RecvEmail, tmpUTF8);
	
	tmp = [SendPwd stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(Email->SendEmailPW, tmpUTF8);
}

- (IBAction)emailEnable:(id)sender {
	NSLog(@"Email enable");
	Email->EmailSwitch = 1;
	[SSL setEnabled:YES];
	[SmtpPort setEnabled:YES];
	[SmtpServer setEnabled:YES];
	[SendMail setEnabled:YES];
	[SendPwd setEnabled:YES];
	[RecvMail setEnabled:YES];	
}

- (IBAction)emailDisable:(id)sender {
	NSLog(@"Email disable");
	Email->EmailSwitch = 0;
	[SSL setEnabled:NO];
	[SmtpPort setEnabled:NO];
	[SmtpServer setEnabled:NO];
	[SendMail setEnabled:NO];
	[SendPwd setEnabled:NO];
	[RecvMail setEnabled:NO];	
}

- (void)dealloc {
	NSLog(@"DvrSettingEmail dealloc");
	
	[super dealloc];
}

@end

@implementation DvrSettingPtz

- (void)awakeFromNib {
	NSLog(@"DvrSettingPtz awakeFromNib");
	PTZ = NULL;
	selectCh = 0;

	[self InitLanguage];

}

- (void)InitLanguage {
	NSString *str = nil;

	if(settingAlarmChannel)
	{
		str = [[NSString alloc] initWithUTF8String:settingAlarmChannel];
		[labelChannel setStringValue:str];
		[str release];
	}
	if(settingPTZProtocol)
	{
		str = [[NSString alloc] initWithUTF8String:settingPTZProtocol];
		[labelProtocol setStringValue:str];	
		[str release];
	}
	if(settingPTZAddress)
	{
		str = [[NSString alloc] initWithUTF8String:settingPTZAddress];
		[labelAddress setStringValue:str];	
		[str release];
	}
	if(settingPTZBaudrate)
	{
		str = [[NSString alloc] initWithUTF8String:settingPTZBaudrate];
		[labelBaudRate setStringValue:str];	
		[str release];
	}
	if(settingPTZDataBit)
	{
		str = [[NSString alloc] initWithUTF8String:settingPTZDataBit];
		[labelDataBit setStringValue:str];	
		[str release];
	}
	if(settingPTZStopBit)
	{
		str = [[NSString alloc] initWithUTF8String:settingPTZStopBit];
		[labelStopBit setStringValue:str];	
		[str release];
	}
	if(settingPTZVerity)
	{
		str = [[NSString alloc] initWithUTF8String:settingPTZVerity];
		[labelVerity setStringValue:str];	
		[str release];
	}
}

- (void)setPTZ:(PTZsetting_t*)ptz andKeyParam:(keyparameter_t*)para {
	int selected = 0;
	DvrConfig *cfg = kt_dvrMgr_getCurDvr();
	
	int i;
	PTZ = ptz;
	KeyParam = para;
	selectCh = selected;
	
	/*update ui*/
	[Channel selectItemAtIndex:selected];
	
	if(([Channel numberOfItems]) == 16)
	{
		for(i = 0; i < 16 - cfg->channelNum; i++)
		{
			[Channel removeItemAtIndex:cfg->channelNum];
		}
	}
	
	if(cfg->channelNum == 16)
	{
		NSLog(@"1xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if(KeyParam->EnablePtzCruise16 & (1<<selected)) /*On*/
		{
			[AutoCruise selectCellAtRow:0 column:0];
		}
		else
		{
			[AutoCruise selectCellAtRow:0 column:1];
		}

	}
	else
	{
		NSLog(@"2xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if(KeyParam->EnablePtzCruise8 & (1 << selected))
		{
			[AutoCruise selectCellAtRow:0 column:0];
		}
		else
		{
			[AutoCruise selectCellAtRow:0 column:1];
		}

	}

	
	if(PTZ->Protocol[selected] == 0) /*PELCO-D*/
	{
		[DvrProtocol selectCellAtRow:0 column:0];
	}
	else if(PTZ->Protocol[selected] == 1) /*PELCO-P*/
	{
		[DvrProtocol selectCellAtRow:0 column:1];
	}
	
	if(PTZ->Baudrate[selected] == 0) /*1200*/
	{
		[BaudRate selectCellAtRow:0 column:0];
	}
	else if(PTZ->Baudrate[selected] == 1) /*2400*/
	{
		[BaudRate selectCellAtRow:0 column:1];
	}
	else if(PTZ->Baudrate[selected] == 2) /*4800*/
	{
		[BaudRate selectCellAtRow:0 column:2];
	}
	else if(PTZ->Baudrate[selected] == 3) /*9600*/
	{
		[BaudRate selectCellAtRow:0 column:3];
	}
	
	if(PTZ->DataBit[selected] == 0) /*8bit*/
	{
		[DataBit selectCellAtRow:0 column:3];
	}
	else if(PTZ->DataBit[selected] == 1) /*7bit*/
	{
		[DataBit selectCellAtRow:0 column:2];
	}
	else if(PTZ->DataBit[selected] == 2) /*6bit*/
	{
		[DataBit selectCellAtRow:0 column:1];
	}
	else if(PTZ->DataBit[selected] == 3) /*5bit*/
	{
		[DataBit selectCellAtRow:0 column:0];
	}
	
	if(PTZ->StopBit[selected] == 0) /*1bit*/
	{
		[StopBit selectCellAtRow:0 column:0];
	}
	else if(PTZ->StopBit[selected] == 1) /*2bit*/
	{
		[StopBit selectCellAtRow:0 column:1];
	}
	
	if(PTZ->Check[selected] == 0) /*None*/
	{
		[Verify selectCellAtRow:0 column:0];
	}
	else if(PTZ->Check[selected] == 1) /*ODD*/
	{
		[Verify selectCellAtRow:0 column:1];
	}
	else if(PTZ->Check[selected] == 2) /*EVEN*/
	{
		[Verify selectCellAtRow:0 column:2];
	}
	else if(PTZ->Check[selected] == 3) /*Mark*/
	{
		[Verify selectCellAtRow:0 column:3];
	}
	else if(PTZ->Check[selected] == 4) /*SPACE*/
	{
		[Verify selectCellAtRow:0 column:4];
	}
	
	/*handle address*/
	[Address selectItemAtIndex:(PTZ->Number[selected] - 1)];
}

- (void)getPTZ {
	int column, row;
	DvrConfig *cfg = kt_dvrMgr_getCurDvr();
	NSLog(@"get PTZ");
	
	column = [AutoCruise selectedColumn];
	row = [AutoCruise selectedRow];
	if(cfg->channelNum == 16)
	{
		if(column == 0 && row == 0)
		{
			KeyParam->EnablePtzCruise16 = (KeyParam->EnablePtzCruise16 | (1 << selectCh));
		}
		else if(row == 0 && column == 1)
		{
			KeyParam->EnablePtzCruise16 = (KeyParam->EnablePtzCruise16 & (~(1 << selectCh)));
		}

	}
	else
	{
		if(column == 0 && row == 0)
		{
			KeyParam->EnablePtzCruise8 = (KeyParam->EnablePtzCruise8 | (1 << selectCh));
		}
		else if(row == 0 && column == 1)
		{
			KeyParam->EnablePtzCruise8 = (KeyParam->EnablePtzCruise8 & (~(1 << selectCh)));
		}
	}

	
	column = [Verify selectedColumn];
	row = [Verify selectedRow];
	PTZ->Check[selectCh] = column;
	
	column = [StopBit selectedColumn];
	row = [StopBit selectedRow];
	PTZ->StopBit[selectCh] = column;
	
	column = [DataBit selectedColumn];
	row = [DataBit selectedRow];
	if(column == 0 && row == 0)
	{
		PTZ->DataBit[selectCh] = 3;
	}
	else if(column == 1 && row == 0)
	{
		PTZ->DataBit[selectCh] = 2;
	}
	else if(column == 2 && row == 0)
	{
		PTZ->DataBit[selectCh] = 1;
	}
	else if(column == 3 && row == 0)
	{
		PTZ->DataBit[selectCh] = 0;
	}
	
	column = [BaudRate selectedColumn];
	row = [BaudRate selectedRow];
	PTZ->Baudrate[selectCh] = column;
	
	column = [DvrProtocol selectedColumn];
	row = [DvrProtocol selectedRow];
	PTZ->Protocol[selectCh] = column;
	
	PTZ->Number[selectCh] = [Address indexOfSelectedItem] + 1;
}

- (void)SwitchChannel:(id)sender {
	int idx;
	DvrConfig *cfg = kt_dvrMgr_getCurDvr();
	NSLog(@"switch channel");
	idx = [Channel indexOfSelectedItem];
	NSLog(@"index of selected item: %d", idx);
	if(idx != selectCh)
	{
		[self getPTZ];
	}
	selectCh = idx;
	
	if(cfg->channelNum == 16)
	{
		NSLog(@"1xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if(KeyParam->EnablePtzCruise16 & (1<<selectCh)) /*On*/
		{
			[AutoCruise selectCellAtRow:0 column:0];
		}
		else
		{
			[AutoCruise selectCellAtRow:0 column:1]; /*Off*/
		}
		
	}
	else
	{
		NSLog(@"2xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		if(KeyParam->EnablePtzCruise8 & (1 << selectCh))
		{
			[AutoCruise selectCellAtRow:0 column:0];
		}
		else
		{
			[AutoCruise selectCellAtRow:0 column:1];
		}
		
	}
	
	/*updata ui*/
	[DvrProtocol selectCellAtRow:0 column:PTZ->Protocol[selectCh]];
	[BaudRate selectCellAtRow:0 column:PTZ->Baudrate[selectCh]];
	if(PTZ->DataBit[selectCh] == 0)
	{
		[DataBit selectCellAtRow:0 column:3];
	}
	else if(PTZ->DataBit[selectCh] == 1)
	{
		[DataBit selectCellAtRow:0 column:2];
	}
	else if(PTZ->DataBit[selectCh] == 2)
	{
		[DataBit selectCellAtRow:0 column:1];
	}
	else if(PTZ->DataBit[selectCh] == 3)
	{
		[DataBit selectCellAtRow:0 column:0];
	}
	[StopBit selectCellAtRow:0 column:PTZ->StopBit[selectCh]];
	[Verify selectCellAtRow:0 column:PTZ->Check[selectCh]];
	[Address selectItemAtIndex:(PTZ->Number[selectCh] - 1)];
}

- (void)dealloc {
	NSLog(@"DvrSettingPtz dealloc");
	
	[super dealloc];
}

@end

@implementation DvrSettingNetwork

- (void)awakeFromNib {
	NSLog(@"DvrSettingNetwork awakeFromNib");
	Network = NULL;
	DDNS = NULL;
	IEClient = NULL;
	
	[self InitLanguage];
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(golbalOpen)
	{
		str = [[NSString alloc] initWithUTF8String:golbalOpen];
		NSCell *cell = (NSCell*)[DDNSEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(golbalClose)
	{
		str = [[NSString alloc] initWithUTF8String:golbalClose];
		NSCell *cell = (NSCell*)[DDNSEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(settingNetworkType)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkType];
		[labelType setStringValue:str];	
		[str release];
	}
	if(settingNetworkTypeStatic)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkTypeStatic];
		NSCell *cell = (NSCell*)[Type cellAtRow:0 column:2];
		[cell setTitle:str];
		[str release];
	}
	if(settingNetworkIPAdd)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkIPAdd];
		[labelIPAddress setStringValue:str];	
		[str release];
	}
	if(settingNetworkSubnetM)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkSubnetM];
		[labelNetMask setStringValue:str];	
		[str release];
	}
	if(settingNetworkGateway)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkGateway];
		[labelGateway setStringValue:str];	
		[str release];
	}
	if(settingNetworkDNS)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkDNS];
		[labelDNS setStringValue:str];	
		[str release];
	}
	if(settingNetworkInternetBt)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkInternetBt];
		[labelInternetBitrate setStringValue:str];	
		[str release];
	}
	if(settingNetworkMediaPort)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkMediaPort];
		[labelMediaPort setStringValue:str];	
		[str release];
	}
	if(settingNetworkWebPort)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkWebPort];
		[labelWebPort setStringValue:str];	
		[str release];
	}
	if(settingNetworkMobilePort)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkMobilePort];
		[labelMobilePort setStringValue:str];	
		[str release];
	}
	if(settingNetworkUserName)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkUserName];
		[labelPPPOEName setStringValue:str];
		[labelDDNSUserName setStringValue:str];
		[str release];
	}
	if(settingNetworkPassword)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkPassword];
		[labelPPPOEPassword setStringValue:str];
		[labelDDNSPassword setStringValue:str];
		[str release];
	}
	if(settingNetworkDDNSService)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkDDNSService];
		[labelDDNSService setStringValue:str];
		[str release];
	}
	if(settingNetworkDDNSHostName)
	{
		str = [[NSString alloc] initWithUTF8String:settingNetworkDDNSHostName];
		[labelDDNSHostName setStringValue:str];
		[str release];
	}
}

- (void)setNetwork:(NetWorkSetting_t*)network andDdns:(DdnsSetting_t*)ddns andIE:(ieclientsetting_t*)ie andMobile:(mobilesetting_t*)mobile{
	Network = network;
	DDNS = ddns;
	IEClient = ie;
	Mobile = mobile;
	
	/*update ui*/
	[Type selectCellAtRow:0 column:Network->NetworkMode];
	
	NSString *tmp = [[NSString alloc] initWithFormat:@"%d.%d.%d.%d", Network->IPAddr[0], Network->IPAddr[1], Network->IPAddr[2], Network->IPAddr[3]];
	[IPAddress setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithFormat:@"%d.%d.%d.%d", Network->NetMask[0], Network->NetMask[1], Network->NetMask[2], Network->NetMask[3]];	
	[SubNetMask setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithFormat:@"%d.%d.%d.%d", Network->GateWay[0], Network->GateWay[1], Network->GateWay[2], Network->GateWay[3]];	
	[GateWay setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithFormat:@"%d.%d.%d.%d", Network->dns[0], Network->dns[1], Network->dns[2], Network->dns[3]];	
	[DNS setStringValue:tmp];
	[tmp release];
	
	[MediaPort setIntValue:Network->Port];
	[WebPort setIntValue:Network->WebPort];
	[MobilePort setIntValue:Mobile->port];
	
	tmp = [[NSString alloc] initWithUTF8String:Network->PPPOEuser];
	[PPPOEName setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:Network->Password];
	[PPPOEPwd setStringValue:tmp];
	[tmp release];
	if(Network->NetworkMode != 1)
	{
		[PPPOEName setEnabled:NO];
		[PPPOEPwd setEnabled:NO];
	}
	
	if(Network->NetworkMode != 2)
	{
		[IPAddress setEnabled:NO];
		[SubNetMask setEnabled:NO];
		[GateWay setEnabled:NO];
		[DNS setEnabled:NO];
	}
	
	if(DDNS->UseDDNS == 0)
	{
		[DDNSEnable selectCellAtRow:0 column:1];
	}
	else if(DDNS->UseDDNS == 1)
	{
		[DDNSEnable selectCellAtRow:0 column:0];
	}
	
	tmp = [[NSString alloc] initWithUTF8String:DDNS->HostName];
	[DDNSHost setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:DDNS->UserName];
	[DDNSUserName setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:DDNS->UserPW];
	[DDNSUserPwd setStringValue:tmp];
	[tmp release];
	if(DDNS->UseDDNS == 0)
	{
		[DDNSService setEnabled:NO];
		[DDNSHost setEnabled:NO];
		[DDNSUserName setEnabled:NO];
		[DDNSUserPwd setEnabled:NO];
	}
	else
	{
		[DDNSService selectItemAtIndex:DDNS->ServerAddr[0]];
	}

	if(IEClient->BitRate == 128)
	{
		[InternetBitrate selectItemAtIndex:0];
	}
	else if(IEClient->BitRate == 192)
	{
		[InternetBitrate selectItemAtIndex:1];
	}
	else if(IEClient->BitRate == 256)
	{
		[InternetBitrate selectItemAtIndex:2];
	}
	else if(IEClient->BitRate == 384)
	{
		[InternetBitrate selectItemAtIndex:3];
	}
	else if(IEClient->BitRate == 512)
	{
		[InternetBitrate selectItemAtIndex:4];
	}
	else if(IEClient->BitRate == 1024)
	{
		[InternetBitrate selectItemAtIndex:5];
	}
}

- (void)getNetwork {
	NSLog(@"get Network");
	NSString *tmp;
	char *tmpUTF8;	
	int column, row, idx;
	int s1, s2, s3, s4;
	column = 0;
	row = 0;
	
	Network->NetworkMode = [Type selectedColumn];

	Network->Port = [MediaPort intValue];
	Network->WebPort = [WebPort intValue];
	/*mobile port*/
	Mobile->port = [MobilePort intValue];
	
	column = [DDNSEnable selectedColumn];
	row = [DDNSEnable selectedRow];
	if(row == 0 && column == 0)
	{
		DDNS->UseDDNS = 1;
	}
	else if(row == 0 && column == 1)
	{
		DDNS->UseDDNS = 0;
	}

	idx = [InternetBitrate indexOfSelectedItem];
	if(idx == 5)
	{
		IEClient->BitRate = 1024;
	}
	else if(idx == 4)
	{
		IEClient->BitRate = 512;
	}
	else if(idx == 3)
	{
		IEClient->BitRate = 384;
	}
	else if(idx == 2)
	{
		IEClient->BitRate = 256;
	}
	else if(idx == 1)
	{
		IEClient->BitRate = 192;
	}
	else if(idx == 0)
	{
		IEClient->BitRate = 128;
	}
	
	tmp = [DDNSHost stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(DDNS->HostName, tmpUTF8);
	
	tmp = [DDNSUserName stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(DDNS->UserName, tmpUTF8);
	
	tmp = [DDNSUserPwd stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(DDNS->UserPW, tmpUTF8);
	
	//DDNS->ServerAddr[0] = [DDNSService indexOfSelectedItem];
	
	tmp = [PPPOEName stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(Network->PPPOEuser, tmpUTF8);
	
	tmp = [PPPOEPwd stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	strcpy(Network->Password, tmpUTF8);
	
	tmp = [IPAddress stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	sscanf(tmpUTF8, "%d.%d.%d.%d", &s1, &s2, &s3, &s4);
	Network->IPAddr[0] = s1;
	Network->IPAddr[1] = s2;
	Network->IPAddr[2] = s3;
	Network->IPAddr[3] = s4;
	
	tmp = [SubNetMask stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	sscanf(tmpUTF8, "%d.%d.%d.%d", &s1, &s2, &s3, &s4);
	Network->NetMask[0] = s1;
	Network->NetMask[1] = s2;
	Network->NetMask[2] = s3;
	Network->NetMask[3] = s4;
	
	tmp = [GateWay stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	sscanf(tmpUTF8, "%d.%d.%d.%d", &s1, &s2, &s3, &s4);
	Network->GateWay[0] = s1;
	Network->GateWay[1] = s2;
	Network->GateWay[2] = s3;
	Network->GateWay[3] = s4;
	
	tmp = [DNS stringValue];
	tmpUTF8 = (char*)[tmp UTF8String];
	sscanf(tmpUTF8, "%d.%d.%d.%d", &s1, &s2, &s3, &s4);
	Network->dns[0] = s1;
	Network->dns[1] = s2;
	Network->dns[2] = s3;
	Network->dns[3] = s4;
	
}

- (IBAction)DDNSEnable:(id)sender {
	NSLog(@"DDNSEnable");
	DDNS->UseDDNS = 1;
	[DDNSService setEnabled:YES];
	[DDNSHost setEnabled:YES];
	[DDNSUserName setEnabled:YES];
	[DDNSUserPwd setEnabled:YES];
	
}

- (IBAction)DDNSDisable:(id)sender {
	NSLog(@"DDNSDisable");
	DDNS->UseDDNS = 0;
	[DDNSService setEnabled:NO];
	[DDNSHost setEnabled:NO];
	[DDNSUserName setEnabled:NO];
	[DDNSUserPwd setEnabled:NO];
	
}

- (IBAction)UseDHCP:(id)sender {
	NSLog(@"UseDHCP");
	if(Network->NetworkMode == 1)
	{
		[PPPOEName setEnabled:NO];
		[PPPOEPwd setEnabled:NO];
	}
	Network->NetworkMode = 0;
	
	[IPAddress setEnabled:NO];
	[SubNetMask setEnabled:NO];
	[GateWay setEnabled:NO];
	[DNS setEnabled:NO];
}

- (IBAction)UsePPPOE:(id)sender {
	NSLog(@"UsePPPOE");
	if(Network->NetworkMode != 1)
	{
		Network->NetworkMode = 1;
		[PPPOEName setEnabled:YES];
		[PPPOEPwd setEnabled:YES];
	}
	
	[IPAddress setEnabled:NO];
	[SubNetMask setEnabled:NO];
	[GateWay setEnabled:NO];
	[DNS setEnabled:NO];
	
}

- (IBAction)UseStatic:(id)sender {
	NSLog(@"UseStatic");
	if(Network->NetworkMode == 1)
	{
		[PPPOEName setEnabled:NO];
		[PPPOEPwd setEnabled:NO];
	}
	Network->NetworkMode = 2;
	[IPAddress setEnabled:YES];
	[SubNetMask setEnabled:YES];
	[GateWay setEnabled:YES];
	[DNS setEnabled:YES];
	
}

- (void)dealloc {
	NSLog(@"DvrSettingNetwork dealloc");
	
	[super dealloc];
}

@end

@implementation DvrSettingManage

- (void)awakeFromNib {
	NSLog(@"DvrSettingManage awakeFromNib");
	UserPwd = NULL;
	IEClient = NULL;
	
	[self InitLanguage];
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(golbalOpen)
	{
		str = [[NSString alloc] initWithUTF8String:golbalOpen];
		NSCell *cell = (NSCell*)[PasswordEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		cell = (NSCell*)[RemoteEnable cellAtRow:0 column:0];
		[cell setTitle:str];
		[str release];
	}
	if(golbalClose)
	{
		str = [[NSString alloc] initWithUTF8String:golbalClose];
		NSCell *cell = (NSCell*)[PasswordEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		cell = (NSCell*)[RemoteEnable cellAtRow:0 column:1];
		[cell setTitle:str];
		[str release];
	}
	if(settingManageDVRPassword)
	{
		str = [[NSString alloc] initWithUTF8String:settingManageDVRPassword];
		[labelPassword setStringValue:str];	
		[str release];
	}
	if(settingManageIEPassword)
	{
		str = [[NSString alloc] initWithUTF8String:settingManageIEPassword];
		[labelRemote setStringValue:str];	
		[str release];
	}
	if(settingManageUserPassword)
	{
		str = [[NSString alloc] initWithUTF8String:settingManageUserPassword];
		[labelUserPassword1 setStringValue:str];
		[labelUserPassword2 setStringValue:str];
		[str release];
	}
	if(settingManageAdminPassword)
	{
		str = [[NSString alloc] initWithUTF8String:settingManageAdminPassword];
		[labelAdminPassword1 setStringValue:str];
		[labelAdminPassword2 setStringValue:str];
		[str release];
	}
	if(settingManageReEnter)
	{
		str = [[NSString alloc] initWithUTF8String:settingManageReEnter];
		[labelReEnter1 setStringValue:str];
		[labelReEnter2 setStringValue:str];
		[labelReEnter3 setStringValue:str];
		[labelReEnter4 setStringValue:str];
		[str release];
	}
}

- (void)setUserManage:(userpassword_t*)user andIEClient:(ieclientsetting_t*)ie {
	UserPwd = user;
	IEClient = ie;
	
	/*update ui*/
	//[DeviceID setIntValue:UserPwd->DeviceID];
	if(UserPwd->HavePwd == 0)
	{
		[PasswordEnable selectCellAtRow:0 column:1];
	}
	else if(UserPwd->HavePwd == 1)
	{
		[PasswordEnable selectCellAtRow:0 column:0];
	}
	
	NSString *tmp = [[NSString alloc] initWithUTF8String: UserPwd->UserPwd];
	[UserPassword setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:UserPwd->UserPwd2];
	[UserPassword2 setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:UserPwd->AdminPwd];
	[AdminPassword setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:UserPwd->AdminPwd2];
	[AdminPassword2 setStringValue:tmp];
	[tmp release];
	if(UserPwd->HavePwd == 0)
	{
		[UserPassword setEnabled:NO];
		[UserPassword2 setEnabled: NO];
		[AdminPassword setEnabled:NO];
		[AdminPassword2 setEnabled:NO];
	}
	
	if(IEClient->HavePwd == 0)
	{
		[RemoteEnable selectCellAtRow:0 column:1];
	}
	else if(IEClient->HavePwd == 1)
	{
		[RemoteEnable selectCellAtRow:0 column:0];
	}
	
	tmp = [[NSString alloc] initWithUTF8String:IEClient->UserPwd];
	[RemoteUserPwd setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:IEClient->UserPwd];
	[RemoteUserPwd2 setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:IEClient->AdminPwd];
	[RemoteAdminPwd setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:IEClient->AdminPwd];
	[RemoteAdminPwd2 setStringValue:tmp];
	[tmp release];
	if(IEClient->HavePwd == 0)
	{
		[RemoteUserPwd setEnabled:NO];
		[RemoteUserPwd2 setEnabled:NO];
		[RemoteAdminPwd setEnabled:NO];
		[RemoteAdminPwd2 setEnabled:NO];
	}
	
}

- (void)getUserManage {
	NSLog(@"get user manage");
	int column, row;
	NSString *tmp;
	char *tmpUTF8;
	char *tmpUTF82;
	
	column = [RemoteEnable selectedColumn];
	row = [RemoteEnable selectedRow];
	if(column == 0 && row == 0)
	{
		IEClient->HavePwd = 1;
	}
	else
	{
		IEClient->HavePwd = 0;
	}
	if(IEClient->HavePwd == 1)
	{
		tmp = [RemoteUserPwd stringValue];
		tmpUTF8 = (char*)[tmp UTF8String];
		tmp = [RemoteUserPwd2 stringValue];
		tmpUTF82 = (char*)[tmp UTF8String];
		if(strcmp(tmpUTF8, tmpUTF82) == 0)
		{
			NSLog(@"set ie user password");
			strcpy(IEClient->UserPwd, tmpUTF8);
		}
		
		tmp = [RemoteAdminPwd stringValue];
		tmpUTF8 = (char*)[tmp UTF8String];
		tmp = [RemoteAdminPwd2 stringValue];
		tmpUTF82 = (char*)[tmp UTF8String];
		if(strcmp(tmpUTF8, tmpUTF82) == 0)
		{
			NSLog(@"set ie admin password");
			strcpy(IEClient->AdminPwd, tmpUTF8);
		}
	}
	
	column = [PasswordEnable selectedColumn];
	row = [PasswordEnable selectedRow];
	if(column == 0 && row == 0)
	{
		UserPwd->HavePwd = 1;
	}
	else
	{
		UserPwd->HavePwd = 0;
	}
	if(UserPwd->HavePwd == 1)
	{
		tmp = [UserPassword stringValue];
		tmpUTF8 = (char*)[tmp UTF8String];
		strcpy(UserPwd->UserPwd, tmpUTF8);
		tmp = [UserPassword2 stringValue];
		tmpUTF82 = (char*)[tmp UTF8String];
		strcpy(UserPwd->UserPwd2, tmpUTF82);
		
		tmp = [AdminPassword stringValue];
		tmpUTF8 = (char*)[tmp UTF8String];
		strcpy(UserPwd->AdminPwd, tmpUTF8);
		tmp = [AdminPassword2 stringValue];
		tmpUTF82 = (char*)[tmp UTF8String];
		strcpy(UserPwd->AdminPwd2, tmpUTF82);
	}
}

- (IBAction)PasswordVerifyEnable:(id)sender {
	NSLog(@"Password Verify Enable");
	UserPwd->HavePwd = 1;
	[UserPassword setEnabled:YES];
	[UserPassword2 setEnabled:YES];
	[AdminPassword setEnabled:YES];
	[AdminPassword2 setEnabled:YES];	
}

- (IBAction)PasswordVerifyDisable:(id)sender {
	NSLog(@"Password Verify Disable");
	UserPwd->HavePwd = 0;
	[UserPassword setEnabled:NO];
	[UserPassword2 setEnabled: NO];
	[AdminPassword setEnabled:NO];
	[AdminPassword2 setEnabled:NO];	
}

- (IBAction)RemoteVerifyEnable:(id)sender {
	NSLog(@"Remote Verify Enable");
	IEClient->HavePwd = 1;
	[RemoteUserPwd setEnabled:YES];
	[RemoteUserPwd2 setEnabled:YES];
	[RemoteAdminPwd setEnabled:YES];
	[RemoteAdminPwd2 setEnabled:YES];	
}

- (IBAction)RemoteVerifyDisable:(id)sender {
	NSLog(@"Remote Verify Disable");
	IEClient->HavePwd = 0;
	[RemoteUserPwd setEnabled:NO];
	[RemoteUserPwd2 setEnabled:NO];
	[RemoteAdminPwd setEnabled:NO];
	[RemoteAdminPwd2 setEnabled:NO];	
}

- (void)dealloc {
	NSLog(@"DvrSettingManage dealloc");
	
	[super dealloc];
}

@end

@implementation DvrSettingInfo

- (void)awakeFromNib {
	NSLog(@"DvrSettingInfo awakeFromNib");
	DvrState = NULL;
	
	[self InitLanguage];
}

- (void)InitLanguage {
	NSString *str = nil;
	
	if(settingInfoSerialNum)
	{
		str = [[NSString alloc] initWithUTF8String:settingInfoSerialNum];
		[labelSerialNum setStringValue:str];	
		[str release];
	}
	if(settingInfoMACAddress)
	{
		str = [[NSString alloc] initWithUTF8String:settingInfoMACAddress];
		[labelMACAddress setStringValue:str];	
		[str release];
	}
	if(settingInfoVersion)
	{
		str = [[NSString alloc] initWithUTF8String:settingInfoVersion];
		[labelSoftwareVersion setStringValue:str];	
		[str release];
	}
	if(settingInfoHDDState)
	{
		str = [[NSString alloc] initWithUTF8String:settingInfoHDDState];
		[labelHDDState setStringValue:str];	
		[str release];
	}
	if(settingInfoHDD)
	{
		str = [[NSString alloc] initWithUTF8String:settingInfoHDD];
		[labelHDD setStringValue:str];	
		[str release];
	}
}

- (void)setDvrState:(stateparameter_t*)state andKeyParam:(keyparameter_t*)key {
	DvrState = state;
	KeyParam = key;
	
	NSString *tmp = [[NSString alloc] initWithUTF8String:DvrState->AppVer];
	[AppVersion setStringValue:tmp];
	[tmp release];
	
	tmp = [[NSString alloc] initWithUTF8String:DvrState->DeviceType];
	[DeviceType setStringValue:tmp];
	[tmp release];
	tmp = [[NSString alloc] initWithUTF8String:DvrState->SerialNum];
	[SerialNum setStringValue:tmp];
	[tmp release];
	
	tmp = [[NSString alloc] initWithFormat:@"%02x-%02x-%02x-%02x-%02x-%02x", KeyParam->MacAddr[0], 
		   KeyParam->MacAddr[1], KeyParam->MacAddr[2], KeyParam->MacAddr[3], KeyParam->MacAddr[4], 
		   KeyParam->MacAddr[5]];
	[MACAddress setStringValue:tmp];
	[tmp release];
	
	tmp = [[NSString alloc] initWithFormat:@"%dG/%dG", (int)(DvrState->HddTotalSize/1000000000), (int)(DvrState->HddFreeSize/1000000000)];
	[HddSize setStringValue:tmp];
	[tmp release];
	
	if(DvrState->HddState == 0) /*no hdd*/
	{
		[HddState setStringValue:@"No HDD"];
	}
	else if(DvrState->HddState == 1) /*not format*/
	{
		[HddState setStringValue:@"Not Format"];
	}
	else if(DvrState->HddState == 2) /*normal*/
	{
		[HddState setStringValue:@"Normal"];
	}
	else if(DvrState->HddState == 3) /*broken*/
	{
		[HddState setStringValue:@"Broken"];
	}
}

- (void)dealloc {
	NSLog(@"DvrSettingInfo dealloc");
		
	[super dealloc];
}

@end

@implementation SettingPanelControl

- (IBAction)showSettingPanel:(id)sender {
	int status;
	int ret;
		
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2)
	{
		ret = kt_dvrMgr_updateCurDvrParam();
		if(ret < 0)
		{
			NSLog(@"xxxxxxxxxxxxxxxxxxxxxxxxxxx get dvr parameters error ");
			/*show dialog*/
			NSAlert* alert = [[NSAlert alloc] init];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert setMessageText:@"Alert"];
			[alert setInformativeText:@"Get DVR parameters error!"];
			[alert addButtonWithTitle:@"OK"];
			//[alert addButtonWithTitle:@"No"];
			NSInteger result = [alert runModal];
			if(result == NSAlertFirstButtonReturn)
			{
				[alert release];
				return;
			}
			[alert release];
			return;
		}
		if(setupWin)
		{
			[setupWin updateSetting];
		}
	
		if(!setupWin)
		{
			NSLog(@"setup window not create");
			setupWin = [[DvrSettingControl alloc] initWithWindowNibName:@"DvrSetting"];
		}
	
		NSLog(@"SettingPanelControl showWindow");
		[setupWin showWindow:self];
	}
	
}

- (void)dealloc {
	NSLog(@"---------->>>>>Setting Panel Control dealloc");
	if(setupWin)
	{
		[setupWin release];
		setupWin = nil;
	}
	[super dealloc];
}

@end


