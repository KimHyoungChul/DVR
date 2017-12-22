//
//  PlaybackSurface.m
//  DVRPlayer
//
//  Created by Kelvin on 3/22/11.
//  Copyright 2011 KangTop. All rights reserved.
//

#import "PlaybackSurface.h"
#include "kt_util.h"
#include "kt_dvr_manage.h"
#include "../NetDVRSDK/NetDvrApi.h"

@implementation PlaybackSurface

- (void)awakeFromNib {
	NSLog(@"awake from nib");
	
	NSRect r = self.bounds;
	printf("x: %f y: %f h: %f w: %f \n", r.origin.x, r.origin.y, r.size.width, r.size.height);
	startFlag = NO;
	reqCapture = NO;
	playback = NULL;
	counter = 0;
	counter2 = 0;
	stepUp = 0;
	updateBar = YES;
	channel = 0;
	streamCtl = UI_STREAM_NOFILESEL; /*normal*/
	onlineFileSize = 0;
	
#if defined(KT_AUDIO_ENABLE) && (KT_AUDIO_ENABLE == 1)
	audioOutput = NULL;
	outputFlag = NO;
#endif /*KT_AUDIO_ENABLE*/
	
	[self createPlayback:0];
	
	curRect = self.bounds;
		
	//animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/60.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];	
}

- (void)setUpdateBar:(BOOL)update {
	updateBar = update;
	counter = 0;
}

- (void)drawRect:(NSRect)rect {
	
	NSRect bounds = self.bounds;
    
    // Draw background gradient
    NSGradient *gradient = [[NSGradient alloc] initWithColorsAndLocations:
							[NSColor colorWithDeviceWhite:0.45f alpha:1.0f], 0.0f,							 
							[NSColor colorWithDeviceWhite:0.49f alpha:1.0f], 0.5f, 
							[NSColor colorWithDeviceWhite:0.50f alpha:1.0f], 0.5f, 	
							[NSColor colorWithDeviceWhite:0.55f alpha:1.0f], 1.0f,							
                            nil];
    
    [gradient drawInRect:bounds angle:90.0f];
    [gradient release];
    
    // Stroke bounds
    [[NSColor blackColor] setStroke];
    [NSBezierPath strokeRect:bounds];	
	
}

- (void)ontimer {
	int precent;
#if defined(KT_AUDIO_ENABLE) && (KT_AUDIO_ENABLE == 1)
	bool result = false;
#endif /*KT_AUDIO_ENABLE*/
	
	/*handle fullscreen or*/
	if(curRect.origin.x != self.bounds.origin.x || curRect.origin.y != self.bounds.origin.y 
	   || curRect.size.width != self.bounds.size.width || curRect.size.height != self.bounds.size.height)
	{
		NSLog(@"isInFullScreenMode");
		curRect = self.bounds;
		NSRect rect = self.bounds;
		stream_playback_setDstSize(playback, (int)(rect.size.width), (int)(rect.size.height));
	}
	
	//NSLog(@"Playback surface ontimer");
	[self displayYUVFrame];
	
#if defined(KT_AUDIO_ENABLE) && (KT_AUDIO_ENABLE == 1)
	if(outputFlag == NO)
	{
		PRC_data_entry_str pEntry = (PRC_data_entry_str)stream_playback_AudioListPop(playback);
		if(pEntry)
		{
			//NSLog(@"pop frame from audio list ");
			if(audioOutput == NULL)
			{
				audioOutput = wavOut_create(1);
				if(audioOutput == NULL)
				{
					NSLog(@"create audio output error");
				}
				else
				{
					result = wavOut_SetPcmParams(audioOutput, 8000, 16, 1, CFRunLoopGetMain());
					if(result != true)
					{
						NSLog(@"init audio queue error");
					}
				}
			}
			stream_playback_AudioPoolPush(playback, (void*)pEntry);
			if(outputFlag == NO)
			{
				wavOut_Play_Samples(audioOutput);
				wavOut_setStreamHandler(audioOutput, playback);
				outputFlag = YES;
			}
		}
	}
#endif /*KT_AUDIO_ENABLE*/
	
	counter++;
	if(counter == 10 && updateBar == YES)
	{
		double theValue;
		double maxX=[processBar bounds].size.width;
		double txx = maxX/1000;
		stepUp = stream_playback_getFilePos(playback);
		if(stepUp <= 1000)
		{
			theValue = [processBar minValue] + (([processBar maxValue] - [processBar minValue]) *
										  (txx*stepUp)/(maxX - 0));
			[processBar setDoubleValue:theValue];
			[processBar setNeedsDisplay:YES];
		}
		else
		{
		}
		
		precent = stream_playback_getDownloadPrecent(playback);
		if(precent <= 1000)
		{
			theValue = [processBar minValue] + (([processBar maxValue] - [processBar minValue]) *
												(txx*precent)/(maxX - 0));
			[processBar setDoubleValue2:theValue];
			[processBar setNeedsDisplay:YES];
		}

		counter = 0;
	}
	if(stepUp == 1000)
		streamCtl = UI_STREAM_FINISH;
	
	counter2++;
	if(counter2 == 80)
	{
		[[NSNotificationCenter defaultCenter]
		 postNotificationName:@"Update Playback Status" 
		 object:nil];
		counter2 = 0;
	}
	
}

- (void)setDownloadProcess:(BOOL)flag {
	[processBar setEnableValue2:flag];
}

- (void)setDownloadPrecent:(int)precent {
	stream_playback_setDownloadPrecent(playback, precent);
}

- (void)setDownloadFileSize:(int)size {
	//stream_playback_setDownloadFileSize(playback, size);
	onlineFileSize = size;
}

- (int)createPlayback:(int)type {
	NSLog(@"------>create playback");
	if(type == 0)
	{
		playback = stream_playback_init(STREAM_LOCAL);
	}
	else if(type == 1)
	{
		playback = stream_playback_init(STREAM_ONLINE);
	}
	
	if(playback != NULL)
	{
		stream_playback_setup(playback, 0);
	}
	else
	{
		return -1;
	}
	
	curRect = self.bounds;
	NSRect rect = self.bounds;
	stream_playback_setDstSize(playback, (int)(rect.size.width), (int)(rect.size.height));
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0/60.0) target:self selector:@selector(ontimer) userInfo:nil repeats:YES];	

	if(type == 1)
	{
		//stream_playback_setDownloadFileSize(playback, onlineFileSize);
	}
		
	return 0;
}

- (void)viewDidEndLiveResize {
	NSLog(@"viewDidEndLiveResize");
	curRect = self.bounds;
	NSRect rect = self.bounds;
	stream_playback_setDstSize(playback, (int)(rect.size.width), (int)(rect.size.height));
	[super viewDidEndLiveResize];
}

- (void)viewWillStartLiveResize {
	NSLog(@"viewWillStartLiveResize");
	[super viewWillStartLiveResize];
}

- (int)destroyPlayback {
	NSLog(@"------>destroy playback");
	[animationTimer invalidate];
	
#if defined(KT_AUDIO_ENABLE) && (KT_AUDIO_ENABLE == 1)
	if(audioOutput)
	{
		wavOut_delete(audioOutput);
		audioOutput = NULL;
	}
#endif /*KT_AUDIO_ENABLE*/
	
	if(playback)
	{
        DVS_CloseLocalPlay(playback->hPlayUtility);
		stream_playback_stop(playback);
		stream_playback_fini(playback);
		playback = NULL;
	}

	return 0;
}

- (void)setNeedCapture {
	NSLog(@"set capture");
	if(startFlag == NO)
		return ;
	if(streamCtl == UI_STREAM_FINISH)
		return ;
	if(streamCtl == UI_STREAM_STOP)
		return ;
	if(streamCtl == UI_STREAM_PAUSE)
		return ;
	if(streamCtl == UI_STREAM_NOFILESEL)
		return ;
	
	reqCapture = YES;
}

- (int)startPlayback {
	NSLog(@"start playback");
	if(playback)
	{
		if(startFlag)
		{
			NSLog(@"PlaybackSurface stop first");
			/*restart this channel*/
			stream_playback_stop(playback);
		}
		NSLog(@"PlaybackSurface start playback");
		////stream_playback_resetDecode(playback);
		////[self destroyPlayback];
		////[self createPlayback];
		//stream_playback_setFilePos(playback, 0);		
		//stream_playback_start(playback);
  
        UNS32 dwBitMaskChn = 0xffff;   
        stream_playback_setCallBackFunc(playback);
        
        if(playback->type == STREAM_LOCAL)
        {
            DVS_OpenLocalPlay(playback->localOrRemoteVideoFilePath,16,&(playback->hPlayUtility));            
            DVS_SetLocalDataCallBack(playback->cbRealVideoFunc, playback->hPlayUtility, playback);                           
            DVS_SetLocalChannelMask(playback->hPlayUtility,dwBitMaskChn);
            DVS_LocalPlayControl(playback->hPlayUtility, DVS_PLAYCTRL_PLAY);     
        }           
        else 
        {
            DVS_OpenRemotePlay(playback->hDvs, playback->localOrRemoteVideoFilePath, 16, &(playback->hPlayUtility));
            DVS_SetRemoteDataCallBack(playback->cbRealVideoFunc, playback->hPlayUtility, playback);
            DVS_SetRemoteChannelMask(playback->hPlayUtility, dwBitMaskChn);
            DVS_RemotePlayControl(playback->hPlayUtility, DVS_PLAYCTRL_PLAY);
        }
           
		startFlag = YES;
		stepUp = 0;
		streamCtl = UI_STREAM_NORMAL;
	}	
	
	return 0;
}


- (void)CheckPlayState{
    
    DVS_TIME dtTime;
    
    if (playback) {
        if(DVS_GetRemotePlayTime(playback->hPlayUtility, &dtTime)){
            
        }
    }
    
}

- (int)stopPlayback {
	NSLog(@"stop playback");
	if(streamCtl == UI_STREAM_NOFILESEL)
		return 0;
	
	if(playback && startFlag == YES)
	{
		//stream_playback_stop(playback);
        DVS_LocalPlayControl(playback->hPlayUtility, DVS_PLAYCTRL_STOP);  
        
		startFlag = NO;
		streamCtl = UI_STREAM_STOP;
	}
	
	return 0;
}

- (int)normalPlayback {
	NSLog(@"normal playback");
	NSLog(@"stream control: %d", streamCtl);
	if(streamCtl == UI_STREAM_NOFILESEL)
		return 0;
	
	if(playback && startFlag == YES)
	{
		if(streamCtl != UI_STREAM_FINISH)
		{
			//stream_playback_normal(playback);
            DVS_LocalPlayControl(playback->hPlayUtility, DVS_PLAYCTRL_PLAY);
			streamCtl = UI_STREAM_NORMAL;
		}
		else {
			[self startPlayback];
		}
		streamCtl = UI_STREAM_NORMAL;

	}
	if(streamCtl == UI_STREAM_STOP || streamCtl == UI_STREAM_FINISH)
	{
		[self startPlayback];
	}
	return 0;
}

- (int)pausePlayback {
	NSLog(@"pause playback");
	
	if(startFlag == NO)
		return 0;
	if(streamCtl == UI_STREAM_FINISH)
		return 0;
	if(streamCtl == UI_STREAM_STOP)
		return 0;
	if(streamCtl == UI_STREAM_NOFILESEL)
		return 0;
	
	if(playback && startFlag == YES)
	{
		//stream_playback_pause(playback);
        DVS_LocalPlayControl(playback->hPlayUtility, DVS_PLAYCTRL_PAUSE);
		streamCtl = UI_STREAM_PAUSE;
	}
	
	return 0;
}

- (int)fastForwardPlayback {
	NSLog(@"fast forward playback");
	if(startFlag == NO)
		return 0;
	if(streamCtl == UI_STREAM_FINISH)
		return 0;
	if(streamCtl == UI_STREAM_STOP)
		return 0;
	if(streamCtl == UI_STREAM_NOFILESEL)
		return 0;
	
	if(streamCtl != UI_STREAM_FF_2X && streamCtl != UI_STREAM_FF_4X && streamCtl != UI_STREAM_FF_8X)
	{
		//stream_playback_fastForward(playback, 1);
        streamCtl = UI_STREAM_FF_2X;
	}
	else if(streamCtl == UI_STREAM_FF_2X)
	{
		//stream_playback_fastForward(playback, 2);
		streamCtl = UI_STREAM_FF_4X;
	}
	else if(streamCtl == UI_STREAM_FF_4X)
	{
		//stream_playback_fastForward(playback, 3);
		streamCtl = UI_STREAM_FF_8X;
	}
	else if(streamCtl == UI_STREAM_FF_8X)
	{
        //stream_playback_fastForward(playback, 1);
		streamCtl = UI_STREAM_FF_2X;
	}
	
    DVS_LocalPlayControl(playback->hPlayUtility, DVS_PLAYCTRL_FORWARD);

	return 0;
}

- (int)slowForwardPlayback {
	NSLog(@"slow forward playback");
	if(startFlag == NO)
		return 0;
	if(streamCtl == UI_STREAM_FINISH)
		return 0;
	if(streamCtl == UI_STREAM_STOP)
		return 0;
	if(streamCtl == UI_STREAM_NOFILESEL)
		return 0;
	
	if(streamCtl != UI_STREAM_SF_2X && streamCtl != UI_STREAM_SF_4X && streamCtl != UI_STREAM_SF_8X)
	{
		//stream_playback_slowForward(playback, 1);
		streamCtl = UI_STREAM_SF_2X;
	}
	else if(streamCtl == UI_STREAM_SF_2X)
	{
		//stream_playback_slowForward(playback, 2);
		streamCtl = UI_STREAM_SF_4X;
	}
	else if(streamCtl == UI_STREAM_SF_4X)
	{
		//stream_playback_slowForward(playback, 3);
		streamCtl = UI_STREAM_SF_8X;
	}
	else if(streamCtl == UI_STREAM_SF_8X)
	{
		//stream_playback_slowForward(playback, 1);
		streamCtl = UI_STREAM_SF_2X;
	}
	
    DVS_LocalPlayControl(playback->hPlayUtility, DVS_PLAYCTRL_BACKPLAY);
    
	return 0;
}

- (int)stepPlayback {
	NSLog(@"step playback");
	if(startFlag == NO)
		return 0;
	if(streamCtl == UI_STREAM_FINISH)
		return 0;
	if(streamCtl == UI_STREAM_STOP)
		return 0;
	if(streamCtl == UI_STREAM_NOFILESEL)
		return 0;
	
	if(playback)
	{
		//stream_playback_step(playback);
		DVS_LocalPlayControl(playback->hPlayUtility, DVS_PLAYCTRL_SHIFT);
	}
	streamCtl = UI_STREAM_STEP;
	
	return 0;
}

- (int)resetPlaybackRestart:(int)pos {
	NSLog(@"resetPlaybackRestart");
	if(streamCtl == UI_STREAM_NOFILESEL)
		return 0;
	
	if(playback)
	{
		if(startFlag)
		{
			NSLog(@"stop first");
			/*restart this channel*/
			stream_playback_stop(playback);
		}
		NSLog(@"start playback");
		stream_playback_setFilePos(playback, pos);
		stream_playback_start(playback);
		startFlag = YES;
		streamCtl = UI_STREAM_NORMAL;
	}
	
	return 0;
}

- (UIStreamControl)getPlaybackStatus {
	return streamCtl;
}

- (void)setPlaybackFile:(const char*)path andChannel:(int)ch {
	if(playback)
	{
        playback->channel = ch;
        memcpy(playback->localOrRemoteVideoFilePath, path, sizeof(playback->localOrRemoteVideoFilePath));
		//stream_playback_setFilePath(playback, (UPSint8*)path);
	}
	channel = ch;
}

- (void)displayYUVFrame {
	RGB_Entry *pEntry = NULL;
	NSRect rect = self.bounds;	
	
	pEntry = stream_playback_listPop2(playback);
	if(pEntry == NULL)
	{
		return;
	}
	
	if(pEntry != NULL)
	{
		if(pEntry->width == ((int)(rect.size.width)) && pEntry->height == ((int)(rect.size.height)))
		{
			[self displayFrame: pEntry];
		}
		stream_playback_poolPush2(playback, pEntry);
	}	
	
}

- (void)displayFrame:(RGB_Entry *)pEntry {
	RGB_Render *pRender = NULL;
	
	if(pEntry == NULL)
	{
		return ;
	}
	
	pRender = (RGB_Render*)pEntry->data;
	
	NSRect rect = self.bounds;	
	
	[self lockFocus];
	
	CGColorSpaceRef rgbColorspace = CGColorSpaceCreateDeviceRGB();	
	CGContextRef srcCtx = CGBitmapContextCreate(pRender->outData, rect.size.width, rect.size.height, 8, 4*(rect.size.width), rgbColorspace, kCGImageAlphaNoneSkipFirst);
	CGColorSpaceRelease(rgbColorspace);
	CGImageRef srcCtx_img = CGBitmapContextCreateImage(srcCtx);
	
	/*screen capture*/
	if(reqCapture == YES)
	{
		char *homePath = getenv("HOME");
		char capPath[128] = {0};
		char capName[128] = {0};
		DvrConfig *dvrCfg = NULL;
		dvrCfg = kt_dvrMgr_getCurDvr();
		sprintf(capPath, "%s/DvrRecord/%s/capture", homePath, dvrCfg->serverAddr);
		//NSString *path = [@"~/Pictures" stringByExpandingTildeInPath];
		
		struct tm *curTime = kt_util_getLocalTime();
		printf("data: %d%d%d time: %d:%d:%d \n", (1900 + curTime->tm_year), (1 + curTime->tm_mon), 
			   curTime->tm_mday, (curTime->tm_hour + 8), curTime->tm_min, curTime->tm_sec);
		//NSString *name = [[NSString alloc] initWithFormat:@"%@/ch%d-%d%02d%02d-%02d%02d%02d.jpeg", path, (channel), (1900+curTime->tm_year), 
		//				  (1+curTime->tm_mon), curTime->tm_mday, (curTime->tm_hour+8), curTime->tm_min, curTime->tm_sec];
		sprintf(capName, "%s/ch%02d-%d%02d%02d-%02d%02d%02d.jpg", capPath, (channel+1), (1900+curTime->tm_year), 
				(1+curTime->tm_mon), curTime->tm_mday, (curTime->tm_hour+8), curTime->tm_min, curTime->tm_sec);
		NSString *name = [[NSString alloc] initWithUTF8String:(const char*)capName];		
		
		NSSize tSize;
		NSRect tRect = self.bounds;
		tSize.width = tRect.size.width;
		tSize.height = tRect.size.height;
		NSBitmapImageRep *img = [[NSBitmapImageRep alloc] initWithCGImage:srcCtx_img];
		//NSArray *representations = [img representations];
		//NSData *bitmapData = [NSBitmapImageRep representationOfImageRepsInArray:representations usingType:NSJPEGFileType properties:nil];
		NSData *jpegData = [img representationUsingType:NSJPEGFileType properties:nil];
		[jpegData writeToFile:name atomically:YES];
		//[jpegData release];
		[name release];
		[img release];
		reqCapture = NO;
		[[NSNotificationCenter defaultCenter]
		 postNotificationName:@"Update Capture Status" 
		 object:nil];
	}	
	
	CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
	CGRect r;
		
	r.origin.x = rect.origin.x;
	r.origin.y = rect.origin.y;
	r.size.width = rect.size.width;
	r.size.height = rect.size.height;
	
	CGContextDrawImage(context, r, srcCtx_img);
	
	CGImageRelease(srcCtx_img);
	CGContextRelease(srcCtx);
	
	[self unlockFocus];
	
	CGContextFlush(context);
}

- (void)dealloc {
	NSLog(@"Playback surface dealloc");
	
	[super dealloc];
}

@end
