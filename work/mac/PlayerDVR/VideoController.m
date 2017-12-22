//
//  VideoController.m
//  PlayerDVR
//
//  Created by Kelvin on 3/2/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "VideoController.h"
#import "CustomButton.h"

#include "kt_dvr_manage.h"
#include "kt_core_handler.h"

#include "kt_language_str.h"

@implementation VideoController

@synthesize ptzPointValue = ptzPointValue_;

- (void)playbackExitHandler:(NSNotification*)notification {
	NSLog(@"event playbackExitHandler");
	playbackStatus = NO;
	
	/*start preview*/
	if(previewStatus == NO)
	{
		NSLog(@"start preview");
		[videoPreview enableDrawAllChannel];
		[videoPreview start];
		previewStatus = YES;
	}
	
}

- (void)settingExitHandler:(NSNotification*)notification {
	NSLog(@"event settingExitHandler");
	settingStatus = NO;
}

- (void)notifyInfoHandler:(NSNotification*)notification {
	NSLog(@"event capture finish handler");
	NSDictionary *dict = [notification userInfo];
	NSString *str = [dict objectForKey:@"name"];
	
	[information setStringValue:str];
	//[str release];
	if(infoStatus == NO)
	{
		infoStatus = YES;
		animationTimer = [NSTimer scheduledTimerWithTimeInterval:(2.0) 
							target:self selector:@selector(onTimer) userInfo:nil repeats:NO];
	}
}

- (void)onTimer {
	[information setStringValue:@""];
	[animationTimer invalidate];
	infoStatus = NO;
}

- (void)awakeFromNib {
	NSLog(@"---------------------------VideoController awakeFromNib");
	
	DvrConfig *cfg = kt_dvrMgr_getCurDvr();
	
	if(cfg->userType == 1)
	{
		NSLog(@"normal user");
		[setupBtn setEnabled:NO];
	}
	else if(cfg->userType == 0)
	{
		NSLog(@"super user");
		[setupBtn setEnabled:YES];
	}
	
#if defined(KT_DEBASE_CHANNEL) && (KT_DEBASE_CHANNEL == 1)
	if(cfg->channelNum == 16)// && cfg->netType == 1)
	{
		NSLog(@"set sisteen button unable");
		[sisteenBtn setEnabled:NO];
		[nineBtn setEnabled:NO];
	}
#endif /*KT_DEBASE_CHANNEL*/
	
	recStatus = NO;
	previewStatus = YES;
	playbackStatus = NO;
	settingStatus = NO;
	infoStatus = NO;
	
	settingCtl = nil;
	
	[self InitLanguage];
	
	[[NSNotificationCenter defaultCenter] addObserver:self 
		selector:@selector(playbackExitHandler:) name:@"PlaybackView Exit" object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self 
		selector:@selector(settingExitHandler:) name:@"SettingView Exit" object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self 
		selector:@selector(notifyInfoHandler:) name:@"Notify Information" object:nil];
	
	
	ptzPointValue_ = 1;
	[ptzPointStep setIntValue:1];
	[ptzPointStep setMinValue:1];
	[ptzPointStep setMaxValue:99];
	[ptzPointEdit setIntValue:1];
}

- (void)InitLanguage {
	NSLog(@"VideoController InitLanguage");
	NSString *str = nil;
	
	if(previewTitle)
	{
		str = [[NSString alloc] initWithUTF8String:previewTitle];
		[previewWin setTitle:str];
		[str release];
	}
	if(previewVideoControl)
	{
		str = [[NSString alloc] initWithUTF8String:previewVideoControl];
		[labelPreviewCtl setStringValue:str];
		[str release];
	}
	if(previewPTZControl)
	{
		str = [[NSString alloc] initWithUTF8String:previewPTZControl];
		[labelPTZCtl setStringValue:str];
		[str release];
	}
	if(previewPlayback)
	{
		str = [[NSString alloc] initWithUTF8String:previewPlayback];
		[playbackBtn setTitle:str];
		[str release];
	}
	if(previewLogout)
	{
		str = [[NSString alloc] initWithUTF8String:previewLogout];
		[logoutBtn setTitle:str];
		[str release];
	}
	if(previewSetup)
	{
		str = [[NSString alloc] initWithUTF8String:previewSetup];
		[setupBtn setTitle:str];
		[str release];
	}
	if(previewPTZCtlZoom)
	{
		str = [[NSString alloc] initWithUTF8String:previewPTZCtlZoom];
		[labelPTZCtlZoom setStringValue:str];
		[str release];
	}
	if(previewPTZCtlFocus)
	{
		str = [[NSString alloc] initWithUTF8String:previewPTZCtlFocus];
		[labelPTZCtlFocus setStringValue:str];
		[str release];
	}
	if(previewPTZCtlIris)
	{
		str = [[NSString alloc] initWithUTF8String:previewPTZCtlIris];
		[labelPTZCtlIris setStringValue:str];
		[str release];
	}
	if(previewCruiseSet)
	{
		str = [[NSString alloc] initWithUTF8String:previewCruiseSet];
		[ptzCruiseSet setStringValue:str];
		[str release];
	}
	if(previewCruiseStart)
	{
		str = [[NSString alloc] initWithUTF8String:previewCruiseStart];
		[ptzCruiseStart setStringValue:str];
		[str release];
	}
	if(previewCruiseStop)
	{
		str = [[NSString alloc] initWithUTF8String:previewCruiseStop];
		[ptzCruiseStop setStringValue:str];
		[str release];
	}
	if(previewCruiseRecall)
	{
		str = [[NSString alloc] initWithUTF8String:previewCruiseRecall];
		[ptzCruiseRecall setStringValue:str];
		[str release];
	}
	if(previewCruiseLoad)
	{
		str = [[NSString alloc] initWithUTF8String:previewCruiseLoad];
		[ptzCruiseLoad setStringValue:str];
		[str release];
	}
	if(previewCruiseSave)
	{
		str = [[NSString alloc] initWithUTF8String:previewCruiseSave];
		[ptzCruiseSave setStringValue:str];
		[str release];
	}
	if(previewCruiseClear)
	{
		str = [[NSString alloc] initWithUTF8String:previewCruiseClear];
		[ptzCruiseClear setStringValue:str];
		[str release];
	}
}

- (IBAction)Logout:(id)sender {
	int status;
	NSLog(@"Logout");

	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO && settingStatus == NO) /*clicked*/
	{
		[videoPreview logout];

		/*logout and show login window*/
		if(!add_window)
		{
			add_window = [[NSWindowController alloc] initWithWindowNibName:@"MainMenu"];
		}
		NSWindow *wnd = [add_window window];

		if(![wnd isVisible])
		{
			NSLog(@"show login window");
			[wnd makeKeyAndOrderFront:sender];
			[previewWin close];

		}
	}
}

- (IBAction)dvrSetup:(id)sender {
	int status;
	int ret;
	NSLog(@"dvrSetup");
	status = [(CustomButton*)sender getBtnStatus];

	if(status == 2)// && playbackStatus == NO) /*clicked*/
	{
		NSLog(@"do dvr setup");
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
		
		
		if(settingCtl)
		{
			[settingCtl updateSetting];
		}
		if(settingCtl == nil)
		{
			//settingCtl = [[DvrSettingControl alloc] init];
			settingCtl = [[DvrSettingControl alloc] initWithWindowNibName:@"DvrSetting"];
		}

		[settingCtl showWindow:self];
		
		/*
		if(!settingCtl)//这个是你加入的的controller里面的数据成员 
			settingCtl = [[DvrSettingControl alloc] initWithWindowNibName:@"DvrSetting"];//DvrSetting是nib文件名,不用将后缀名都打上去 
		
		NSWindow *wnd = [settingCtl window]; 
		if (![wnd isVisible])  
		{ 
			[wnd makeKeyAndOrderFront: sender]; 
		}  
		else  
		{ 
			[settingCtl showWindow: sender]; 
		} 
		*/
		
		settingStatus = YES;
	}
}

- (IBAction)StartVideo:(id)sender {
	NSLog(@"StartVideo");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[videoPreview start];
	}
}

- (IBAction)StopVideo:(id)sender {
	NSLog(@"StopVideo");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[videoPreview stop];
	}
}

- (IBAction)oneChannel:(id)sender {
	NSLog(@"one channel display");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[videoPreview setGridType: ONE_GRID];
	}
}

- (IBAction)testButton:(id)sender {
    NSLog(@"testButton");
}

- (IBAction)fourChannel:(id)sender {
	NSLog(@"four channel display");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[videoPreview setGridType:FOUR_GRID];
	}
}

- (IBAction)nineChannel:(id)sender {
	NSLog(@"nine channel display");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[videoPreview setGridType:NINE_GRID];
	}
	
}

- (IBAction)sisteenChannel:(id)sender {
	NSLog(@"sisteen channel display");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2) /*clicked*/
	{
		[videoPreview setGridType:SISTEEN_GRID];
	}
	
}

- (IBAction)imgCapture:(id)sender {
	int status;
	DvrConfig *dvrCfg = NULL;
	NSLog(@"stop record");
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	if(dvrCfg)
	{
		kt_core_createDvrHome((char*)dvrCfg->serverAddr);
	}
	
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		NSLog(@"btn clicked");
		[videoPreview focusCapture];
	}
	
}

- (IBAction)doRecord:(id)sender {
	NSLog(@"do record");
	int status;
	DvrConfig *dvrCfg = NULL;
	
	dvrCfg = kt_dvrMgr_getCurDvr();
	if(dvrCfg)
	{
		kt_core_createDvrHome((char*)dvrCfg->serverAddr);
	}
	
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		if(recStatus == NO)
		{
			NSLog(@"start Rec");
			[videoPreview recordAllChannel];
			recStatus = YES;
		}
		else if(recStatus == YES)
		{
			NSLog(@"stop Rec");
			[videoPreview stopRecordAllChannel];
			recStatus = NO;
		}
	}
}

- (IBAction)switchPreview:(id)sender {
	NSLog(@"switch preview");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		NSLog(@"do switch preview");
		if(previewStatus == YES)
		{
			/*stop rec and preview*/
			[videoPreview stopRecordAllChannel];
			recStatus = NO;
			NSLog(@"stop preview");
			[videoPreview stop];
			[videoPreview disableDrawAllChannel];
			previewStatus = NO;
		}
		else
		{
			/*start preview*/
			NSLog(@"start preview");
			[videoPreview enableDrawAllChannel];
			[videoPreview start];
			previewStatus = YES;
		}

	}
}

- (IBAction)startPlayback:(id)sender {
	NSLog(@"start Playback");
	NSWindow *wnd;
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		NSLog(@"btn clicked");
		if(previewStatus == YES)
		{
			/*stop rec and preview*/
			[videoPreview stopRecordAllChannel];
			recStatus = NO;
			NSLog(@"stop preview");
			[videoPreview stop];
			[videoPreview disableDrawAllChannel];
			previewStatus = NO;
		}
		
		if(playbackWin == nil)
		{
			playbackWin = [[NSWindowController alloc] initWithWindowNibName:@"VideoPlayback"];
		}
		wnd = [playbackWin window];
		
		if(![wnd isVisible])
		{
			[wnd makeKeyAndOrderFront:sender];
		}
		[wnd release];
		playbackStatus = YES;
		
	}
	else if(status == 1) /*pressed*/
	{
		NSLog(@"btn pressed");
	}
}

- (IBAction)ptzUp:(id)sender {
	NSLog(@"ptzUP");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzUp];
	}
	
}

- (IBAction)ptzDown:(id)sender {
	NSLog(@"ptzDown");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzDown];
	}
	
}

- (IBAction)ptzLeft:(id)sender {
	NSLog(@"ptzLeft");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzLeft];
	}
	
}

- (IBAction)ptzRight:(id)sender {
	NSLog(@"ptzRight");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzRight];
	}
		
}

- (IBAction)ptzZoomin:(id)sender {
	NSLog(@"ptzZoomin");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzZoomin];
	}
	
}

- (IBAction)ptzZoomout:(id)sender {
	NSLog(@"ptzZoomout");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzZoomout];
	}
	
}

- (IBAction)ptzFocusFar:(id)sender {
	NSLog(@"ptzFocusFar");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzFocusFar];
	}
	
}

- (IBAction)ptzFocusNear:(id)sender {
	NSLog(@"ptzFocusNear");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzFocusNear];
	}
	
}

- (IBAction)ptzIRISOpen:(id)sender {
	NSLog(@"ptzIRISOpen");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzIRISOpen];
	}
	
}

- (IBAction)ptzIRISClose:(id)sender {
	NSLog(@"ptzIRISClose");
	int status;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO) /*clicked*/
	{
		[videoPreview focusPtzStop];
	}
	else if(status == 1 && playbackStatus == NO) /*pressed*/
	{
		[videoPreview focusPtzIRISClose];
	}
	
}

- (IBAction)ptzCruisePreSet:(id)sender {
	NSLog(@"ptzCruisePreSet");
	int status;
	int ret = 0;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO)
	{
		//[videoPreview focusPtzCruisePreSet:1];
		NSLog(@"point: %d", [ptzPointStep intValue]);
		ret = [ptzPointStep intValue];
		ret = [videoPreview focusPtzCruisePreSet:ret];
//		NSString *name;
//		if(ret != 0)
//		{
//			name = [[NSString alloc] initWithUTF8String:"Set fail!"];
//			
//		}
//		else
//		{
//			name = [[NSString alloc] initWithUTF8String:"Set success!"];
//		}
//		NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
//		
//		[[NSNotificationCenter defaultCenter]
//		 postNotificationName:@"Notify Information" 
//		 object:nil userInfo:dict];
//		[name release];
	}
}

- (IBAction)ptzCruiseLoad:(id)sender {
	NSLog(@"ptzCruiseLoad");
	int status;
	int ret = 0;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO)
	{
		/*load cruise*/
		ret = [videoPreview focusPtzCruiseLoad:1];
		NSLog(@"ret: %d", ret);
		if(ret > 0)
		{
			[ptzPointStep setIntValue:ret];
			[ptzPointEdit setIntValue:ret];
		}
		else if(ret == 0)
		{
			[ptzPointEdit setIntValue:1];
			[ptzPointStep setIntValue:1];
		}
		else {
//			NSString *name;
//			name = [[NSString alloc] initWithUTF8String:"Load fail!"];
//			NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
//			
//			[[NSNotificationCenter defaultCenter]
//			 postNotificationName:@"Notify Information" 
//			 object:nil userInfo:dict];
//			[name release];
		}
	}
}

- (IBAction)ptzCruiseStart:(id)sender {
	NSLog(@"ptzCruiseLoad");
	int status;
	int ret = 0;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO)
	{
		ret = [ptzPointStep intValue];
		ret = [videoPreview focusPtzCruiseStart:ret];
//		NSString *name;
//		if(ret != 0)
//		{
//			name = [[NSString alloc] initWithUTF8String:"Start fail!"];
//			
//		}
//		else
//		{
//			name = [[NSString alloc] initWithUTF8String:"Start success!"];
//		}
//		NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
//		
//		[[NSNotificationCenter defaultCenter]
//		 postNotificationName:@"Notify Information" 
//		 object:nil userInfo:dict];
//		[name release];
	}
}

- (IBAction)ptzCruiseStop:(id)sender {
	NSLog(@"ptzCruiseStop");
	int status;
	int ret = 0;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO)
	{
		ret = [ptzPointStep intValue];
		ret = [videoPreview focusPtzCruiseStop:ret];
//		NSString *name;
//		if(ret != 0)
//		{
//			name = [[NSString alloc] initWithUTF8String:"Stop fail!"];
//			
//		}
//		else
//		{
//			name = [[NSString alloc] initWithUTF8String:"Stop success!"];
//		}
//		NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
//		
//		[[NSNotificationCenter defaultCenter]
//		 postNotificationName:@"Notify Information" 
//		 object:nil userInfo:dict];
//		[name release];
	}
}

- (IBAction)ptzCruiseRecall:(id)sender {
	NSLog(@"ptzCruiseRecall");
	int status;
	int ret = 0;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO)
	{
		ret = [ptzPointStep intValue];
		ret = [videoPreview focusPtzCruiseRecall:ret];
		NSString *name;
		if(ret != 0)
		{
			name = [[NSString alloc] initWithUTF8String:"Recall fail!"];
			
		}
		else
		{
			name = [[NSString alloc] initWithUTF8String:"Recall success!"];
		}
		NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
		
		[[NSNotificationCenter defaultCenter]
		 postNotificationName:@"Notify Information" 
		 object:nil userInfo:dict];
		[name release];
	}
}

- (IBAction)ptzCruiseSave:(id)sender {
	NSLog(@"ptzCruiseSave");
	int status;
	int ret = 0;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO)
	{
		ret = [ptzPointStep intValue];
		ret = [videoPreview focusPtzCruiseSave:ret];
		NSString *name;
		if(ret != 0)
		{
			name = [[NSString alloc] initWithUTF8String:"Save fail!"];
			
		}
		else
		{
			name = [[NSString alloc] initWithUTF8String:"Save success!"];
		}
		NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
		
		[[NSNotificationCenter defaultCenter]
		 postNotificationName:@"Notify Information" 
		 object:nil userInfo:dict];
		[name release];
	}
}

- (IBAction)ptzCruiseClear:(id)sender {
	NSLog(@"ptzCruiseClear");
	int status;
	int ret = 0;
	status = [(CustomButton*)sender getBtnStatus];
	if(status == 2 && playbackStatus == NO)
	{
		ret = [ptzPointStep intValue];
		ret = [videoPreview focusPtzCruiseClear:ret];
		if(ret == 0)
		{
			[ptzPointEdit setIntValue:1];
			[ptzPointStep setIntValue:1];
		}
//		NSString *name;
//		if(ret != 0)
//		{
//			name = [[NSString alloc] initWithUTF8String:"Clear fail!"];
//			
//		}
//		else
//		{
//			name = [[NSString alloc] initWithUTF8String:"Clear success!"];
//		}
//		NSDictionary *dict = [NSDictionary dictionaryWithObject:name forKey:@"name"];
//		
//		[[NSNotificationCenter defaultCenter]
//		 postNotificationName:@"Notify Information" 
//		 object:nil userInfo:dict];
//		[name release];
	}
}

- (BOOL)windowShouldClose:(id)window {
	//NSLog(@"panel window should close");
	
	return YES;
}

- (void)dealloc {
	NSLog(@"xxxxxxxxxxxxxxxxxxxxxxxxxx>>>>>>>> VideoController dealloc");
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

@end

@implementation previewDelegate 

- (BOOL)windowShouldClose:(id)window {
	NSLog(@"==================>Preview window should close");
	
	/*popup dialog*/
//	NSAlert* alert = [[NSAlert alloc] init];
//	[alert setAlertStyle:NSWarningAlertStyle];
//	[alert setMessageText:@"Alert"];
//	[alert setInformativeText:@"Are you sure you want to quit?"];
//	[alert addButtonWithTitle:@"Yes"];
//	[alert addButtonWithTitle:@"No"];
//	NSInteger result = [alert runModal];
//	if(result == NSAlertFirstButtonReturn)
//	{
//		[alert release];
//		[videoPreview logout];		
//		return YES;
//	}
//	[alert release];
//
//	return NO;
	
	
	/*not popup dialog*/
	[videoPreview logout];
	
	/*preview window close, the app will terminate.*/
	NSApplication *app = [NSApplication sharedApplication];
	[app terminate:nil];
	
	return YES;
}

@end

@implementation playbackDelegate

- (BOOL)windowShouldClose:(id)window {
	NSLog(@"xxxxxxxxxxxxxxxxxxxxxxx>>>>>playbackDelegat window should close");
	[MediaControl stopTimer];
	
	if(playbackWin)
	{
		[playbackWin release];
		playbackWin = nil;
	}
	
	/*playback window exit notify send*/
	[[NSNotificationCenter defaultCenter]
		postNotificationName:@"PlaybackView Exit" 
		object:nil];
	
	return YES;
}

@end

@implementation settingDelegate

- (BOOL)windowShouldClose:(id)window {
	NSLog(@"xxxxxxxxxxxxxxxxxxxxxxxxxxxxx>>>>>panel window should close");
	
	/*setting window exit notify send*/
	[[NSNotificationCenter defaultCenter]
	 postNotificationName:@"SettingView Exit" 
	 object:nil];
	
	return YES;	
	
}

@end

